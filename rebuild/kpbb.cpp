#include "Graph.h"
#include "2th-Reduction.h"
#include "../Verify/verify.h"

string file_path;
set<ui> solution;
set<ui> must_contain;
double algorithm_start_time, total_time;
double FastHeuris_time;
double StrongHeuris_time;
double strong_reduce_time;
int FastHeuris_lb;

void print_solution()
{
    printf("Heuristic solution(size= %d ):\n", (int)solution.size());
    print_set(solution);
}

void print_final_log(Graph &g)
{
    total_time = get_system_time_microsecond() - algorithm_start_time;
    printf("list triangles time: %.4lf s, strong reduce time: %.4lf s\n", list_triangle_time / 1e6, strong_reduce_time / 1e6);
    puts("****Heuristic result****");
    printf("total-time= %.4lf s, FastHeuris-time= %.4lf s, StrongHeuris-time= %.4lf s\n", total_time / 1e6, FastHeuris_time / 1e6, StrongHeuris_time / 1e6);
    printf("lb= %d , FastHeuris-lb= %d \n", lb, FastHeuris_lb);
    printf("reduced graph: n= %u m= %u\n", g.n, g.n ? g.m : 0);
    assert(solution.size() == lb);
    print_solution();
    fflush(stdout);

    char out_file[500];
    sprintf(out_file, "../reduced_graph/%s_K=%d.out", get_file_name_without_suffix(file_path).c_str(), paramK);
    g.dump_to_file(string(out_file), &must_contain);

#ifdef VERIFY
    Verifier_undirected_kPlex<ui> verifier(solution, file_path);
    if (!verifier.is_correct(paramK))
        printf("Wrong: the solution is not a plex!!!\n");
    else
        printf("The solution is verified: it is a plex\n");
#endif // VERIFY
    puts("");
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("3 params are required !!! \n");
        puts("usage: ./kpbb graph_file k");
        exit(1);
    }
    file_path = string(argv[1]);
    paramK = atoi(argv[2]);
    Graph g;

    return 0;
}