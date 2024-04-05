# Code for Maximum  k-Plex Searching.

Written by [***Shuohao Gao***](https://shuohaogao.github.io/) in **HITSZ**.\
If you find bugs or meet problems, just feel free to contact us.

# Graph data format:
Two kinds of graph formats are supported, and you can find the details in ***Graph::readFromFile*** in [**kPEX/Graph.h**](./kPEX/Graph.h).

## 1. *.out or *.txt
first line: 
```n m```\
next $m$ lines are undirected edges: ```a b```\
Note that  $0 \leq a,b \leq n-1$ & you need to make sure that self-loops are removed (multi-edges are allowed).

We provide  two example graphs in [data/edges/](./data/edges/).

## 2. *.bin
binary graph \
first $4$ Bytes: **sizeof(uint32_t)**, which should be $4$\
then $4$ Bytes: $n$\
then $4$ Bytes: $2\times m$\
then $4\times n$ Bytes: the degree $d_G(\cdot)$ of $n$ vertices\
then: $n$ parts ($2m\times 4$ Bytes in total), each part has $d_G(u)$ integers which are the neighbors of $u$ ***in ascending order***

We provide an example of binary graph file  in [data/bin/](./data/bin/).

# Usage
The whole procedure for searching Maximum K-Plex is located at [kPEX/](./kPEX/). 

## compile
```
g++ -std=c++11 -O3 -w main.cpp -o kPEX -DNDEBUG -DNO_PROGRESS_BAR
```

or

```
make
```

Note that we add a macro definition in the compile command: \
***-DNO_PROGRESS_BAR*** will disable the progress bar;\
we recommend to use this definition when you use batch commands. 

## run
```
./kPEX graph_path k
```

## an example
```
cd kPEX
make
./kPEX ../data/bin/brock200-2.bin 2
./kPEX ../data/edges/email-Eu-core.out 2
```


#### We offer an executable program:
```kPEX/kPEX```  can be executed in Ubuntu 20.04\

