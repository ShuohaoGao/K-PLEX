#include "Graph.h"
#include "2th-Reduction.h"
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

void print_solution()
{
    if (solution.size() < 2 * paramK - 1)
    {
        printf("***We can't find a plex larger than 2k-1!! The following is a heuristic solution.\n");
    }
#ifndef NO_DUMP
    printf("Maximum solution(size= %d ):\n", (int)solution.size());
    print_set(solution);
#endif
    fflush(stdout);
}

void print_heuris_log()
{
    puts("*************Heuristic result*************");
    total_heuris_time = get_system_time_microsecond() - algorithm_start_time;
    printf("list triangles time: %.4lf s, strong reduce time: %.4lf s\n", list_triangle_time / 1e6, strong_reduce_time / 1e6);
    printf("total-heuristic-time= %.4lf s, FastHeuris-time= %.4lf s, StrongHeuris-time= %.4lf s\n",
           total_heuris_time / 1e6, FastHeuris_time / 1e6, StrongHeuris_time / 1e6);
    printf("lb= %d , FastHeuris-lb= %d \n", lb, FastHeuris_lb);
    if (solution.size() >= 2 * paramK - 1)
        assert(solution.size() == lb);

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
        print_solution();
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
        lb = max(lb, 2 * paramK - 2); // we only care the solutions with at least 2k-1 vertices
        lb = max(lb, g.degeneracy_and_reduce(lb, &solution));
        printf("After degeneracy and weak reduce, n= %u , m= %u , lb= %d , use time %.4lf s\n", g.n, g.m / 2, lb, t.get_time() / 1e6);
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

    // if core-graph is dense, we can peel some vertices that must be included, which can not be reduced by CTCP
    if (0.1 * g.n * g.n < g.m)
    {
        vector<bool> rm(g.n);
        for (ui i = 0; i < g.n; i++)
        {
            if (g.d[i] + 1 == g.n)
            {
                rm[i] = 1;
                must_contain.insert(g.map_refresh_id[i]);
            }
        }
        if (must_contain.size())
        {
            lb -= must_contain.size();
            g.remove_v(rm, lb);
        }
    }

    // strong reduce: CF-CTCP
    {
        Timer start_strong_reduce;
        Reduction reduce(&g);
        ui pre_n = g.n;
        reduce.strong_reduce(lb);
        printf("Afer CF-CTCP, n= %u , m= %u, use time %.4lf s\n", g.n, g.m / 2, start_strong_reduce.get_time() / 1e6);
        strong_reduce_time += start_strong_reduce.get_time();

        if (lb >= g.n)
        {
            g.n = 0;
            FastHeuris_time = t.get_time();
            lb += must_contain.size();
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
    double time_limit = FastHeuris_time;
    time_limit = max(time_limit, 0.5 * 1e6);
    Timer t_extend("StrongHeuris");
    while (1)
    {
#ifdef NO_STRONG_HEURIS
        break;
#endif
        int extend_lb = 0;
        if (solution.size() < 2 * paramK - 2) // this means we probably find no larger plex
        {
            break;
        }
        extend_lb = g.strong_heuris(lb, solution, time_limit);
        printf("%dth-StrongHeuris lb= %d\n", iteration_cnt++, extend_lb);
        if (extend_lb <= lb)
            break;
        solution.insert(must_contain.begin(), must_contain.end());
        lb = extend_lb;
        g.weak_reduce(lb);

        // strong reduce
        {
            Timer start_strong_reduce;
            Reduction reduce(&g);
            ui pre_n = g.n;
            reduce.strong_reduce(lb);
            printf("Afer CF-CTCP, n= %u , m= %u, use time %.4lf s\n", g.n, g.m / 2, start_strong_reduce.get_time() / 1e6);
            strong_reduce_time += start_strong_reduce.get_time();

            if (lb >= g.n)
            {
                g.n = 0;
                lb += must_contain.size();
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
    lb += must_contain.size();
    while (1)
    {
        vector<bool> rm(g.n, 0);
        ui pre_sz = must_contain.size();
        g.get_v_must_include(must_contain, rm);
        if (pre_sz == must_contain.size())
            break;
        printf("must contain: %u\n", must_contain.size());
        g.remove_v(rm, lb - (int)must_contain.size());
        break;
    }
}

/**
 * @brief branch and bound searching
 */
void bnb()
{
    Graph_reduced *G;
    G = new Graph_reduced_adjacent_list(g, must_contain);
    Branch branch(*G, lb - G->must_contain.size());
    branch.IE_framework();
    if (solution.size() < branch.solution.size()) // record the max plex
    {
        solution.clear();
        for (int v : branch.solution)
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
    int temp = paramK;
    g.readFromFile(file_path);

    algorithm_start_time = get_system_time_microsecond();

    Timer prepro("heuristic and preprocess");
    heuris();
    prepro.print_time();
    print_heuris_log();

    puts("------------------{start bnb}---------------------");

    // branch and bound
    bnb();

    print_solution();
    puts("------------------{whole procedure: kpbb}---------------------");
    printf("ground truth= %u , kpbb time: %.4lf s\n\n", solution.size(), (get_system_time_microsecond() - algorithm_start_time) / 1e6);

    return 0;
}