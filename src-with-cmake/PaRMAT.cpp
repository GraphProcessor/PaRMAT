#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <cstring>
#include <cstdlib>
#include <cassert>

#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>

#include "GraphGen_sorted.hpp"
#include "GraphGen_notSorted.hpp"
#include "utils.hpp"
#include "internal_config.hpp"

#include "util/log.h"

std::string out_file_dir;

using namespace std;
using namespace std::chrono;

void openFileToWrite(std::ofstream &input_file, const char *file_name) {
    input_file.open(file_name);
    if (!input_file)
        throw std::runtime_error("Failed to open specified file.");
}

template<typename intT>
intT determine_num_of_CPU_worker_thread(const intT suggested) {
    if (suggested < MIN_CPU_WORKER_THREAD)
        return static_cast<intT>( MIN_CPU_WORKER_THREAD );
    else if (suggested > MAX_CPU_WORKER_THREAD)
        return static_cast<intT>( MAX_CPU_WORKER_THREAD );
    else
        return static_cast<intT>( suggested );
}

double determine_RAM_usage(const double suggested) {
    if (suggested < MIN_RAM_PORTION_USAGE)
        return MIN_RAM_PORTION_USAGE;
    else if (suggested > MAX_RAM_PORTION_USAGE)
        return MAX_RAM_PORTION_USAGE;
    else
        return suggested;
}

int main(int argc, char **argv) {

    std::string usage =
            "\tRequired command line arguments:\n\
		-Number of edges. E.g., -nEdges 1021\n\
		-NUmber of vertices. E.g., -nVertices 51\n\
	Additional arguments:\n\
		-Output file (default: out.txt). E.g., -output myout.txt\n\
		-RMAT a parameter (default: 0.45). E.g., -a 0.42\n\
		-RMAT b parameter (default: 0.22). E.g., -b 0.42\n\
		-RMAT c parameter (default: 0.22). E.g., -c 0.42\n\
		-Number of worker CPU threads (default: queried/1). E.g., -threads 4\n\
		-Output should be sorted based on source index (default: not sorted). To sort: -sorted\n\
		-Allow edge to self (default:yes). To disable: -noEdgeToSelf\n\
		-Allow duplicate edges (default:yes). To disable: -noDuplicateEdges\n\
		-Will the graph be directed (default:yes). To make it undirected: -undirected\n\
		-Usage of available system memory (default: 0.5 which means up to half of available RAM may be requested). E.g., -memUsage 0.9";


    std::ofstream outf;
    unsigned long long nEdges = 0, nVertices = 0;
    double a = 0.45, b = 0.22, c = 0.22;
    unsigned int nCPUWorkerThreads = 0;
    bool sorted = false;
    double RAM_usage = 0.5;
    bool allowEdgeToSelf = true;
    bool allowDuplicateEdges = true;
    bool directedGraph = true;
    unsigned long long standardCapacity = 0;

    try {

        // Getting required input parameters.
        for (int iii = 1; iii < argc; ++iii) {
            if (!strcmp(argv[iii], "-nEdges") && iii != argc - 1 /*is not the last one*/)
                nEdges = std::stoull(std::string(argv[iii + 1]));
            else if (!strcmp(argv[iii], "-nVertices") && iii != argc - 1 /*is not the last one*/)
                nVertices = std::stoull(std::string(argv[iii + 1]));
            else if (!strcmp(argv[iii], "-output") && iii != argc - 1 /*is not the last one*/)
                openFileToWrite(outf, argv[iii + 1]);
            else if (!strcmp(argv[iii], "-outbin") && iii != argc - 1 /*is not the last one*/)
                out_file_dir = std::string(argv[iii + 1]);
            else if (!strcmp(argv[iii], "-a") && iii != argc - 1 /*is not the last one*/)
                a = std::stod(std::string(argv[iii + 1]));
            else if (!strcmp(argv[iii], "-b") && iii != argc - 1 /*is not the last one*/)
                b = std::stod(std::string(argv[iii + 1]));
            else if (!strcmp(argv[iii], "-c") && iii != argc - 1 /*is not the last one*/)
                c = std::stod(std::string(argv[iii + 1]));
            else if (!strcmp(argv[iii], "-threads") && iii != argc - 1 /*is not the last one*/)
                nCPUWorkerThreads = determine_num_of_CPU_worker_thread(std::stoul(std::string(argv[iii + 1])));
            else if (!strcmp(argv[iii], "-sorted"))
                sorted = true;
            else if (!strcmp(argv[iii], "-memUsage") && iii != argc - 1 /*is not the last one*/)
                RAM_usage = determine_RAM_usage(std::stod(std::string(argv[iii + 1])));
            else if (!strcmp(argv[iii], "-noEdgeToSelf"))
                allowEdgeToSelf = false;
            else if (!strcmp(argv[iii], "-noDuplicateEdges"))
                allowDuplicateEdges = false;
            else if (!strcmp(argv[iii], "-undirected"))
                directedGraph = false;
        }

        if (out_file_dir.size() == 0) {
            out_file_dir = ".";
        }

        if (nVertices == 0 || nEdges == 0 || nEdges >= nVertices * nVertices)
            throw std::runtime_error("Number of edges or number of vertices are not specified (correctly).");

        if (!outf.is_open())
            openFileToWrite(outf, "out.txt");

        // Avoiding very small regions which may cause incorrect results.
        if (nEdges < 10000)
            nCPUWorkerThreads = 1;

        if (nCPUWorkerThreads == 0)    // If number of concurrent threads haven't specified by the user,
            nCPUWorkerThreads = std::max(1, static_cast<int>(std::thread::hardware_concurrency()) -
                                            1);    // try to manage their numbers automatically. If cannot determine, go single-threaded.

        // Print the info.
        std::cout << "Requested graph will have " << nEdges << " edges and " << nVertices << " vertices." << "\n" <<
                  "Its a, b, and c parameters will be respectively " << a << ", " << b << ", and " << c << "." << "\n"
                  <<
                  "There can be up to " << nCPUWorkerThreads << " worker CPU thread(s) making the graph." << "\n" <<
                  "The graph will" << (sorted ? " " : " NOT ") << "necessarily be sorted." << "\n" <<
                  "Up to about " << (RAM_usage * 100.0) << " percent of RAM can be used by this program." << "\n" <<
                  "Specified graph may" << (allowEdgeToSelf ? " " : " NOT ")
                  << "contain edges that have same source and destination index." << "\n" <<
                  "Specified graph may" << (allowDuplicateEdges ? " " : " NOT ") << "contain duplicate edges." << "\n"
                  <<
                  "Specified graph will be " << (directedGraph ? "DIRECTED." : "UNDIRECTED.") << "\n";

        auto totalSystemRAM = static_cast<unsigned long long>(getTotalSystemMemory());    // In bytes.
        auto availableSystemRAM = calculateAvailableRAM(totalSystemRAM, RAM_usage);    // In bytes.

        standardCapacity =
                availableSystemRAM / (2 * nCPUWorkerThreads * sizeof(Edge)); // 2 can count for vector's effect.
        std::cout << "Each thread capacity is " << standardCapacity << " edges." << "\n";

    }
    catch (const std::exception &strException) {
        std::cerr << "Initialization Error: " << strException.what() << "\n";
        std::cerr << "Usage: " << usage << std::endl << "Exiting." << std::endl;
        return (EXIT_FAILURE);
    }
    catch (...) {
        std::cerr << "An exception has occurred during the initialization." << "\n" << "Exiting." << std::endl;
        return (EXIT_FAILURE);
    }

    try {

        // Start the work.
        --nVertices;
#define FILE_PRIVILEGE (0644)
        int outFileFD = open((out_file_dir + "/undir_edge_list.bin").c_str(), O_RDWR | O_CREAT, FILE_PRIVILEGE);
        size_t max_size = static_cast<size_t >(20) * 1024 * 1024 * 1024 + 4096;
        ftruncate(outFileFD, max_size);
        static_assert(sizeof(size_t) == 8, "");
        static_assert(sizeof(Edge) == 8, "");
        auto *mmap_meta_cnt_ = (size_t *) mmap(nullptr, sizeof(size_t),
                                               PROT_READ | PROT_WRITE, MAP_SHARED, outFileFD, 0);
        *mmap_meta_cnt_ = 0;
        auto *mmap_edges = static_cast<Edge *>(mmap(nullptr, max_size,
                                                    PROT_READ | PROT_WRITE, MAP_SHARED, outFileFD, 0)) + 1;

        auto clk_beg = high_resolution_clock::now();
        auto fOutcome = sorted ?
                        GraphGen_sorted::GenerateGraph(nEdges, nVertices, a, b, c, nCPUWorkerThreads, outf,
                                                       outFileFD, *mmap_meta_cnt_, mmap_edges,
                                                       standardCapacity, allowEdgeToSelf,
                                                       allowDuplicateEdges, directedGraph) :
                        GraphGen_notSorted::GenerateGraph(nEdges, nVertices, a, b, c, nCPUWorkerThreads, outf,
                                                          outFileFD, *mmap_meta_cnt_, mmap_edges,
                                                          standardCapacity, allowEdgeToSelf,
                                                          allowDuplicateEdges,
                                                          directedGraph);
        auto clk_end = high_resolution_clock::now();
        log_info("total time: %.3lf s",
                 duration_cast<milliseconds>(clk_end - clk_beg).count() / 1000.0);

        if (fOutcome == EXIT_FAILURE) {
            std::cerr << "Exiting." << std::endl;
            return (EXIT_FAILURE);
        }


        std::cerr << "Edge#:" << *mmap_meta_cnt_ << std::endl;

        ftruncate(outFileFD, static_cast<size_t>(*mmap_meta_cnt_) * sizeof(int32_t) * 2 + sizeof(size_t));
        munmap(mmap_edges, max_size);
        munmap(mmap_meta_cnt_, sizeof(size_t));
        close(outFileFD);
        std::cout << "Done." << std::endl;
        return (EXIT_SUCCESS);

    }
    catch (const std::exception &strException) {
        std::cerr << "Error: " << strException.what() << "\n" << "Exiting." << std::endl;
        return (EXIT_FAILURE);
    }
    catch (...) {
        std::cerr << "An exception has occurred during the graph generation." << "\n" << "Exiting." << std::endl;
        return (EXIT_FAILURE);
    }

}
