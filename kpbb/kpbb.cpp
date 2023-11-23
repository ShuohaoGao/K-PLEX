#include "Graph.h"
#include "2th-Reduction.h"
#include "verify.h"
#include "Branch.h"

string file_path;
Graph g;
set<ui> solution;
set<ui> must_contain;
double algorithm_start_time, total_heuris_time;
double FastHeuris_time;
double StrongHeuris_time;
double strong_reduce_time;
int FastHeuris_lb;
int input_n;

void print_heuris_log()
{
    puts("*************Heuristic result*************");
    total_heuris_time = get_system_time_microsecond() - algorithm_start_time;
    printf("list triangles time: %.4lf s, strong reduce time: %.4lf s\n", list_triangle_time / 1e6, strong_reduce_time / 1e6);
    printf("total-heuristic-time= %.4lf s, FastHeuris-time= %.4lf s, StrongHeuris-time= %.4lf s\n",
           total_heuris_time / 1e6, FastHeuris_time / 1e6, StrongHeuris_time / 1e6);
    printf("lb= %d , FastHeuris-lb= %d \n", lb, FastHeuris_lb);
    if (solution.size() < 2 * paramK - 1)
    {
        printf("there is no such a plex size >= 2k - 1  !!\n");
    }
    else
    {
        assert(solution.size() == lb);
    }

#ifdef VERIFY
    Verifier_undirected_kPlex<ui> verifier(solution, file_path);
    if (!verifier.is_correct(paramK))
        printf("Wrong: the solution is not a plex!!!\n");
    else
        printf("The solution is verified: it is a plex\n");
#endif // VERIFY

    if (g.n + must_contain.size() <= lb)
    {
        printf("The heuristic solution is the ground truth!\n");
        printf("Maximum solution(size= %d ):\n", (int)solution.size());
        print_set(solution);
        fflush(stdout);
        puts("------------------{whole procedure: kpbb}---------------------");
        printf("ground truth= %u , kpbb time: %.4lf s\n\n", solution.size(), (get_system_time_microsecond() - algorithm_start_time) / 1e6);
        exit(0);
    }
}

/**
 * @brief the heuristic stage without StrongHeuris
 */
void FastHeuris()
{
    Timer t("FastHeuris");
    // degeneracy + weak reduce
    {
#ifndef NO_SQRT
        lb = g.sqrt_degeneracy(&solution);
        printf("sqrt lb= %d, use time %.4lf s\n", lb, t.get_time() / 1e6);
#endif
        lb = max(lb, 2 * paramK - 2);
        lb = max(lb, g.degeneracy_and_reduce(lb, &solution));
        printf("After degeneracy and weak reduce, n= %u , m= %u , use time %.4lf s\n", g.n, g.m, t.get_time() / 1e6);
        if (lb >= g.n)
        {
            g.n = 0;
            FastHeuris_lb = lb;
            FastHeuris_time = t.get_time();
            print_heuris_log();
            exit(0);
        }
    }
    FastHeuris_lb = lb;

    // strong reduce
    {
        Timer start_strong_reduce;
        Reduction reduce(&g);
        ui pre_n = g.n;
        reduce.strong_reduce(lb);
        printf("Afer CTCP, n= %u , m= %u, use time %.4lf s\n", g.n, g.m, start_strong_reduce.get_time() / 1e6);
        strong_reduce_time += start_strong_reduce.get_time();

        if (lb >= g.n)
        {
            g.n = 0;
            FastHeuris_time = t.get_time();
            print_heuris_log();
            exit(0);
        }
    }
    FastHeuris_time = t.get_time();
}

/**
 * @brief we make more efforts to get better lb and then reduce graph
 */
void StrongHeuris()
{
    int iteration_cnt = 1;
    double time_limit = FastHeuris_time * 3;
    Timer t_extend("StrongHeuris");
    while (1)
    {
        int extend_lb = 0;
        int extend_times = sqrt(input_n) + 1;
        extend_lb = g.strong_heuris(lb, extend_times, &solution, time_limit);
        printf("%dth-StrongHeuris lb= %d\n", iteration_cnt++, extend_lb);
        if (extend_lb <= lb)
            break;
        lb = extend_lb;

        // strong reduce
        {
            Timer start_strong_reduce;
            Reduction reduce(&g);
            ui pre_n = g.n;
            reduce.strong_reduce(lb);
            printf("Afer CTCP, n= %u , m= %u, use time %.4lf s\n", g.n, g.m, start_strong_reduce.get_time() / 1e6);
            strong_reduce_time += start_strong_reduce.get_time();

            if (lb >= g.n)
            {
                g.n = 0;
                StrongHeuris_time = t_extend.get_time();
                print_heuris_log();
                exit(0);
            }
        }
    }
    StrongHeuris_time = t_extend.get_time();
}

/**
 * @brief stage-I: we conduct heuristic and preprocessing stage
 */
void heuris()
{
    input_n = g.n;

    FastHeuris();
    StrongHeuris();

    // next, we will remove the vertices that must occur in the solution
    while (1)
    {
        vector<bool> rm(g.n, 0);
        ui pre_sz = must_contain.size();
        g.get_v_must_include(must_contain, rm);
        if (pre_sz == must_contain.size())
            break;
        printf("must contain: %u\n", must_contain.size());
        g.remove_v_must_include(rm, lb - (int)must_contain.size());
        break;
    }
}

/**
 * @brief branch and bound searching
 */
void bnb()
{
    Graph_reduced *G;
    if (g.m * 1.0 / g.n > g.n * 1.0 / 64) // dense graph
    {
        G = new Graph_reduced_adjacent_matrix(g, must_contain);
    }
    else // sparse graph
    {
        G = new Graph_reduced_adjacent_list(g, must_contain);
    }
    Branch branch(*G, lb - G->must_contain.size());
    branch.IE_framework();
    if(solution.size() < branch.solution.size())
    {
        solution.clear();
        for(int v:branch.solution)
            solution.insert(v);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("3 params are required !!! \n");
        exit(1);
    }
    file_path = string(argv[1]);
    paramK = atoi(argv[2]);
    g.readFromFile(file_path);

    algorithm_start_time = get_system_time_microsecond();

    Timer prepro("heuristic and preprocess");
    heuris();
    prepro.print_time();
    print_heuris_log();

    puts("------------------{start bnb}---------------------");

    // branch and bound
    bnb();

    printf("Maximum solution(size= %d ):\n", (int)solution.size());
    print_set(solution);
    fflush(stdout);
    puts("------------------{whole procedure: kpbb}---------------------");
    printf("ground truth= %u , kpbb time: %.4lf s\n\n", solution.size(), (get_system_time_microsecond() - algorithm_start_time) / 1e6);

    return 0;
}