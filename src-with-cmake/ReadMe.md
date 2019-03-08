## Usage

* sorted and noDuplicateEdges is not satisfied. (do not know why currently)

```zsh

./PaRMAT -nVertices 100000 -nEdges 1000000 -output myout.txt -threads 4  -noEdgeToSelf -noDuplicateEdges -undirected -memUsage 0.5
```


## First-Version Copy

first-version md5sums for files

```
621f145dc66d4dd0386ea64b3d11c007  Edge.cpp
bab276ac9fb5814b8096a281f759dffb  Edge.hpp
c97a3ee51a5fd35b16b6676974c83e9a  GraphGen_notSorted.cpp
be6a4b028465f4e0e93b1c9393f0a36f  GraphGen_notSorted.hpp
f1d3cc07d0ef3bae429b13e437cd23f0  GraphGen_sorted.cpp
e053f6d030ccbb502265261e6f72fa53  GraphGen_sorted.hpp
d06e08dc2be0fc3492ef4de01036b453  internal_config.hpp
a66c4059fc79f585692301e4964cd8c2  PaRMAT.cpp
13cd40517ed9df9abcd9efe68c0c4828  Square.cpp
65884fb6679bfd94183cca2fcdb38ec8  Square.hpp
21af649685a06e1e4a931bf219d4d74f  threadsafe_queue.hpp
8170ebcee97fbfd24c944f5512188aa6  utils.cpp
7740d44f9cdfb8863986b9bd20d51a77  utils.hpp

621f145dc66d4dd0386ea64b3d11c007  ../src-with-cmake/Edge.cpp
bab276ac9fb5814b8096a281f759dffb  ../src-with-cmake/Edge.hpp
c97a3ee51a5fd35b16b6676974c83e9a  ../src-with-cmake/GraphGen_notSorted.cpp
be6a4b028465f4e0e93b1c9393f0a36f  ../src-with-cmake/GraphGen_notSorted.hpp
f1d3cc07d0ef3bae429b13e437cd23f0  ../src-with-cmake/GraphGen_sorted.cpp
e053f6d030ccbb502265261e6f72fa53  ../src-with-cmake/GraphGen_sorted.hpp
d06e08dc2be0fc3492ef4de01036b453  ../src-with-cmake/internal_config.hpp
a66c4059fc79f585692301e4964cd8c2  ../src-with-cmake/PaRMAT.cpp
13cd40517ed9df9abcd9efe68c0c4828  ../src-with-cmake/Square.cpp
65884fb6679bfd94183cca2fcdb38ec8  ../src-with-cmake/Square.hpp
21af649685a06e1e4a931bf219d4d74f  ../src-with-cmake/threadsafe_queue.hpp
8170ebcee97fbfd24c944f5512188aa6  ../src-with-cmake/utils.cpp
7740d44f9cdfb8863986b9bd20d51a77  ../src-with-cmake/utils.hpp
```