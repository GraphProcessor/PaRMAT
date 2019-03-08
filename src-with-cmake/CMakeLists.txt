cmake_minimum_required(VERSION 3.6)
project(src)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

find_package(Threads REQUIRED)

add_compile_options(-O3 -Wall -c -fmessage-length=0 -g)
add_executable(PaRMAT
        capacity_controller.hpp
        Edge.cpp
        Edge.hpp
        GraphGen_notSorted.cpp
        GraphGen_notSorted.hpp
        GraphGen_sorted.cpp
        GraphGen_sorted.hpp
        internal_config.hpp
        PaRMAT.cpp
        Square.cpp
        Square.hpp
        threadsafe_queue.hpp
        utils.cpp
        utils.hpp)

target_link_libraries(PaRMAT PRIVATE ${CMAKE_THREAD_LIBS_INIT})

add_executable(play play/play.cpp Edge.cpp)
