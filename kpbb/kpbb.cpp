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

void print_heuris_log()
{
    puts("*************Heuristic result*************");
    total_heuris_time = get_system_time_microsecond() - algorithm_start_time;
    printf("list triangles time: %.4lf s, strong reduce time: %.4lf s\n", list_triangle_time / 1e6, strong_reduce_time / 1e6);
    printf("total-heuristic-time= %.4lf s, FastHeuris-time= %.4lf s, StrongHeuris-time= %.4lf s\n",
           total_heuris_time / 1e6, FastHeuris_time / 1e6, StrongHeuris_time / 1e6);
    printf("lb= %d , FastHeuris-lb= %d \n", lb, FastHeuris_lb);
    assert(solution.size() == lb);
    printf("Heuristic solution(size= %d ):\n", (int)solution.size());
    print_set(solution);
    fflush(stdout);

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
        puts("------------------{whole procedure: kpbb}---------------------");
        printf("kpbb time: %.4lf s\n\n", (get_system_time_microsecond() - algorithm_start_time) / 1e6);
        exit(0);
    }
}

/**
 * @brief stage-I: we conduct heuristic and preprocessing stage 
 */
void heuris()
{
    int iteration_cnt = 1;
    int input_n = g.n;
    //     while (1)
    //     {
    // #ifdef NO_SQRT
    //         break;
    // #endif
    //         // degeracy order for the front sqrt(n) vertices, T(n)=O(n)
    //         double start_first_extend = get_system_time_microsecond();
    //         int ret = g.sqrt_degeneracy(&solution);
    //         printf("%d-th sqrt degeneracy lb= %d ,", iteration_cnt++, ret);
    //         print_time(start_first_extend);

    //         if (lb >= ret)
    //             break;
    //         lb = ret;

    //         // weak reduce, T(n)=O(m)
    //         double start_weak_reduce = get_system_time_microsecond();
    //         g.fast_weak_reduce(lb);
    //         g.weak_reduce(lb);
    //         printf("After weak reduce: n= %u , m= %u ,", g.n, g.m);
    //         print_time(start_weak_reduce);

    //         if (lb >= g.n)
    //         {
    //             g.n = 0;
    //             FastHeuris_lb = lb;
    //             FastHeuris_time = get_system_time_microsecond() - algorithm_start_time;
    //             print_heuris_log();
    //             exit(0);
    //         }

    //         // break;
    //     }

    //     iteration_cnt = 1;
    //     while (1) // degeracy order for the whole graph, T(n)=O(m)
    //     {
    //         double start_degeneracy = get_system_time_microsecond();
    //         int degeneracy_lb = g.degeneracyLB(&solution);
    //         printf("%d-th degeneracy order lb= %d ,", iteration_cnt++, degeneracy_lb);
    //         print_time(start_degeneracy);

    //         if (lb >= degeneracy_lb)
    //             break;
    //         lb = degeneracy_lb;

    //         // weak reduce, T(n)=O(m)
    //         double start_weak_reduce = get_system_time_microsecond();
    //         g.weak_reduce(lb);
    //         printf("After weak reduce: n= %u , m= %u ,", g.n, g.m);
    //         print_time(start_weak_reduce);

    //         if (lb >= g.n)
    //         {
    //             g.n = 0;
    //             FastHeuris_lb = lb;
    //             FastHeuris_time = get_system_time_microsecond() - algorithm_start_time;
    //             print_heuris_log();
    //             exit(0);
    //         }

    //         break;
    //     }

    // sqrt degeneracy + weak reduce
    {
        Timer t("FastHeuris");
        lb = g.sqrt_degeneracy(&solution);
        printf("sqrt lb= %d, use time %.4lf s\n", lb, t.get_time()/1e6);
        lb = max(lb, g.degeneracy_and_reduce(lb, &solution));
        printf("After degeneracy and weak reduce, n= %u , m= %u , use time %.4lf s\n", g.n, g.m, t.get_time()/1e6);
        if (lb >= g.n)
        {
            g.n = 0;
            FastHeuris_lb = lb;
            FastHeuris_time = get_system_time_microsecond() - algorithm_start_time;
            print_heuris_log();
            exit(0);
        }
    }

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
            FastHeuris_time = get_system_time_microsecond() - algorithm_start_time;
            print_heuris_log();
            exit(0);
        }
    }

    FastHeuris_time = get_system_time_microsecond() - algorithm_start_time;
    FastHeuris_lb = lb;

    iteration_cnt = 1;
    double time_limit = FastHeuris_time * 3;
    double start_extend_time = get_system_time_microsecond();
    while (1)
    {
        int extend_lb = 0;
        double start_current_iteration = get_system_time_microsecond();
        int extend_times = sqrt(input_n) + 1;
        extend_times = min(extend_times, (int)g.n);
        ui *seq = new ui[g.n];
        g.sort_by_degree(seq);
        // for(int i=0;i<g.n;i++)
        //     seq[i]=i;
        for (ui i = 0; i < extend_times; i++)
        {
            extend_lb = max(extend_lb, g.extend(seq[i], &solution));
            if (extend_lb > lb)
            {
                break;
            }
        }
        delete[] seq;
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
                StrongHeuris_time = get_system_time_microsecond() - start_extend_time;
                print_heuris_log();
                exit(0);
            }
        }
    }

    StrongHeuris_time = get_system_time_microsecond() - start_extend_time;

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

    puts("------------------{whole procedure: kpbb}---------------------");
    printf("kpbb time: %.4lf s\n\n", (get_system_time_microsecond() - algorithm_start_time) / 1e6);

    return 0;
}