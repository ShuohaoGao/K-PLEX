#include "Graph.h"
#include "Branch.h"

#ifdef enable_CTCP // the existing heuristic method Degen uses CTCP
#include "2th-Reduction-CTCP.h"
#else // use CF-CTCP
#include "2th-Reduction.h"
#endif

#include "find-small.h"

string file_path;
Graph g;
set<ui> solution;
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
        // printf("***We can't find a plex larger than 2k-2!! The following is a heuristic solution.\n");
        // if (solution.size())
        // {
        //     print_set(solution);
        // }

        /**
         * We can't find a plex larger than 2k-2!
         * but we have already got a heuristic solution
         * Then we continue to search maximum k-plex of size > lb = solution.size()
         */
        Timer t("small-search");
        if (solution.size() < 2 * paramK - 2)
        {
            Solver_small solver(file_path, solution, paramK);
            solver.start_search();
            if (solver.solution.size() > solution.size())
                solution = solver.solution;
        }
        printf("The size is smaller than 2k-1!\n");
        t.print_time();
        assert(solution.size() <= 2 * paramK - 2);
        printf("Maximum solution(size= %d ):\n", (int)solution.size());
        print_set(solution);
        fflush(stdout);
        return;
    }
#ifndef NO_DUMP
    // if defined NO_DUMP, then we do not output the detailed solution
    printf("Maximum solution(size= %d ):\n", (int)solution.size());
    print_set(solution);
#endif // NO_DUMP
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

    if (g.n <= lb)
    {
        // printf("The heuristic solution is the ground truth!\n");
        print_solution();
        puts("------------------{whole procedure: kPEX}---------------------");
        printf("ground truth= %u , kPEX time: %.4lf s\n\n", solution.size(), (get_system_time_microsecond() - algorithm_start_time) / 1e6);
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

    // strong reduce: CF-CTCP
    {
        Timer start_strong_reduce;
        Reduction reduce(&g);
        ui pre_n = g.n;
        reduce.strong_reduce(lb);
        printf("Afer strong reduce, n= %u , m= %u, use time %.4lf s\n", g.n, g.m / 2, start_strong_reduce.get_time() / 1e6);
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
    double time_limit = FastHeuris_time;
    time_limit = max(time_limit, 0.5 * 1e6);
    if (paramK >= 15)
    {
        time_limit = max(time_limit, 1e3 * 1e6);
    }
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
        lb = extend_lb;
        g.weak_reduce(lb);

        // strong reduce
        {
            Timer start_strong_reduce;
            Reduction reduce(&g);
            ui pre_n = g.n;
            reduce.strong_reduce(lb);
            printf("Afer strong reduce, n= %u , m= %u, use time %.4lf s\n", g.n, g.m / 2, start_strong_reduce.get_time() / 1e6);
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
 * @brief the newly proprosed heuris in kPlexT
 */
void EgoHeuris()
{
    Timer t("ego-degen");
    lb = g.ego_degen(&solution);
    g.weak_reduce(lb);
    t.print_time();
    // strong reduce
    {
        Timer start_strong_reduce;
        Reduction reduce(&g);
        ui pre_n = g.n;
        reduce.strong_reduce(lb);
        printf("Afer strong reduce, n= %u , m= %u, use time %.4lf s\n", g.n, g.m / 2, start_strong_reduce.get_time() / 1e6);
        strong_reduce_time += start_strong_reduce.get_time();
        if (lb >= g.n)
        {
            g.n = 0;
            print_heuris_log();
            exit(0);
        }
    }
}

/**
 * @brief stage-I: we conduct heuristic and preprocessing stage
 * i.e., KPHeuris
 */
void heuris()
{
    input_n = g.n;

#ifdef EGO
    EgoHeuris();
    return;
#endif

    FastHeuris();
    StrongHeuris(); // generate n subgraphs and compute larger lb
}

/**
 * @brief branch and bound searching
 */
void bnb()
{
    Graph_reduced *G;
    G = new Graph_reduced_adjacent_list(g);
    Branch branch(*G, lb);
    branch.IE_framework();                        // generate n subgraphs
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
        printf("2 params are required !!! \n");
        printf("usage: ./kPEX graph_path k\n");
        exit(1);
    }
    file_path = string(argv[1]);
    paramK = atoi(argv[2]);
    g.readFromFile(file_path);

    algorithm_start_time = get_system_time_microsecond();

    // KPHeuris
    puts("------------------{start KPHeuris}---------------------");
    Timer prepro("heuristic and preprocess");
    heuris();
    prepro.print_time();
    print_heuris_log();

    // recursive branch and bound
    puts("------------------{start BRB_Rec}---------------------");
    bnb();

    print_solution();

    puts("------------------{whole procedure: kPEX}---------------------");
    printf("ground truth= %u , kPEX time: %.4lf s\n\n", solution.size(), (get_system_time_microsecond() - algorithm_start_time) / 1e6);

    return 0;
}