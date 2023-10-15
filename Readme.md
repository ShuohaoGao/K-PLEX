# Code for Maximum  k-Plex Searching.

Written by ***Shuohao Gao*** in **HITSZ**.


# Graph data format:
first line: 
```n m```

next $m$ lines are undirected edges: ```a b```

Note that  $0 \leq a,b \leq n-1$ & you need to make sure that self-loops are removed (multi-edges are allowed).

# Usage
**MDPS** has two stages: *heuristic stage (preprocessing stage)* and *exact search stage*.

If you just need a relative large instead of strictly maximum  k-plex, then **DPHeuris** is suitable due to its high accuracy and little time cost.

### 1. DPHeuris
compile command need at least c++11:
```
cd DPHeuris
g++ -std=c++11 -O3 -w MDP-pre.cpp -o MDP-pre
./MDP-pre input_graph_file_path k
```
reduced graph is dumped to ```./reduced_graph/```

### 2. DPBB
The **DPBB** procedure is based on the output of **DPHeuris**\
Compile command need at least c++11:
```
cd DPBB
g++ -std=c++11 -O3 -w MDP-bb.cpp -o MDP-bb -DNO_PROGRESS_BAR -DNDEBUG
./MDP-bb reduced_graph_file_path k 
```
Note that we add two macro definitions in the compile command: \
***-DNO_PROGRESS_BAR*** will disable the progress bar; we recommend to use this definition when you use batch commands. \
***-DNDEBUG*** will disable the *assert()*, making the program faster.

The format of ***reduced_graph_file***:
first line: ```n m lb```;\
next $m$ lines are edges: ```a b```;\
next $n$ lines are $v_i$, i.e., a vertex in the reduced graph whose id is $j$ is the vertex $v_j$ in the intial graph.

then ```cnt```, which is the count of vertices that must be included. \
next $cnt$ lines are $v_i$, i.e., a vertex id that must be included.


# Example
```data/email-Eu-core.out``` is an example graph

### DPHeuris:
```
./MDP-pre ../data/email-Eu-core.out 2
```
### DPBB:
```
./MDP-bb ../reduced_graph/email-Eu-core_K=2.out 2
```
#### More details of usage example are shown in ```DPHeuris/pre.sh``` and ```DPBB/run.sh```, the running results are shown in ```DPHeuris/log/pre_log.txt``` and ```DPBB/log/run_log.txt```

We offer two kinds of executable programs:\
```DPHeuris/MDP-pre``` and ```DPBB/MDP-bb``` can be executed in Ubuntu 20.04\
```DPHeuris/MDP-pre.exe``` and ```DPBB/MDP-bb.exe``` can be executed in Win11 and Win10( note that we use *gettimeofday(&timestamp, NULL)* to compute the time cost, which may be incorrect in Windows)
