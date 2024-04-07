# Codes and Datasets of $kPEX$ for Maximum  k-Plex Searching.

Quick start:
```
cd kPEX
make
./kPEX graph_path k
```



## A. Datasets
All datasets can be downloaded from [**Network Repository**](https://networkrepository.com/index.php) and [**2nd-DIMACS**](http://archive.dimacs.rutgers.edu/pub/challenge/graph/). Note that 78 graphs in **2nd-DIMACS** can also be found in **Network Repository**, and we provide the rest 2 graphs ([*p-hat300-1*](./data/p-hat300-1.mtx) and [*p-hat300-2*](./data/p-hat300-2.mtx)) in [*data/*](./data/).

The information list of 664 graphs is shown in [*datasets-list.json*](./data/datasets-list.json), containing 584 graphs from **Network Repository** and 80 graphs from **2nd-DIMACS**. This json file also reports the statistics of each graph, i.e., the number of vertices $n$, the number of edges $m$, and the degeneracy $\delta$.

## B. Input graph data format:
Two kinds of graph formats are supported as follows (our *kPEX* relies on the *suffix name* of input files).

### Format 1: *.out or *.txt
First line: 
```n m```\
Next $m$ lines are undirected edges: ```a b```\
Note that  $0 \leq a,b \leq n-1$ and you need to make sure that self-loops are removed (multi-edges are allowed because $kPEX$ will remove multi-edges).

We provide  two example graphs in [*data/edges/*](./data/edges/).

### Format 2: *.bin
binary graph \
first $4$ Bytes: **sizeof(uint32_t)**, which should be $4$\
then $4$ Bytes: $n$\
then $4$ Bytes: $2\times m$\
then $4\times n$ Bytes: the degree $d_G(\cdot)$ of $n$ vertices\
then: $n$ parts ($2m\times 4$ Bytes in total), each part has $d_G(u)$ integers which are the neighbors of $u$ ***in ascending order***

We provide some examples of binary graph file  in [*data/bin/*](./data/bin/), *corresponding to the representative graphs G1-G10* in our paper.

More details about reading graphs from files can be found in ***Graph::readFromFile*** in [**kPEX/Graph.h**](./kPEX/Graph.h).


### Transform *.txt to *.bin
We provide [*trans.cpp*](./data/trans-graph-from-char-to-bin/trans.cpp) to convert text files to binary files.\
Usage:
```
g++ -O3 -std=c++11 trans.cpp -o tran
./tran text_graph_file_path
```

## C. Algorithm $kPEX$
The whole procedure for searching Maximum K-Plex is located at [*kPEX/*](./kPEX/). 

### 1. Compile
```
g++ -std=c++11 -O3 -w main.cpp -o kPEX -DNDEBUG -DNO_PROGRESS_BAR
```
or
```
make
```

Note that we add a macro definition in the compile command: \
***-DNO_PROGRESS_BAR*** will disable the progress bar;\
we recommend to add this definition when you use batch commands. 

### 2. Run
```
./kPEX graph_path k
```

### 3. An example
```
cd kPEX
make
./kPEX ../data/bin/brock200-2.bin 2
./kPEX ../data/edges/soc-BlogCatalog-ASU.txt 5
```


### 4. We offer an executable program:
[*kPEX*](./kPEX/kPEX)  can be executed on Ubuntu 20.04


### 5. About log
If there is no $k$-plexes larger than $2k-2$, then our $kPEX$ will report log as follows.
```
***We can't find a plex larger than 2k-2!! The following is a heuristic solution.
```

### 6. About major components in codes
***AltRB*** corresponds to [Branch.h::int bound_and_reduce(Set &S, Set &C)](./kPEX/Branch.h);\
***KPHeuris*** corresponds to [*main.cpp::void heuris()*](./kPEX/main.cpp)\
***CF-CTCP*** corresponds to [*2th-Reduction.h*](./kPEX/2th-Reduction.h)