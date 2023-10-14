#include "Graph.h"
#include "2th-Reduction.h"
#include "../Verify/verify.h"

string file_path;
Graph g;
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

void print_final_log()
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
        exit(1);
    }
    file_path = string(argv[1]);
    paramK = atoi(argv[2]);
    g.readFromFile(file_path);

    algorithm_start_time = get_system_time_microsecond();

    int iteration_cnt = 1;
    while (1)
    {
        // degeracy order for the front sqrt(n) vertices, T(n)=O(n)
        double start_first_extend = get_system_time_microsecond();
        int ret = g.sqrt_degeneracy(&solution);
        printf("%d-th sqrt degeneracy lb= %d ,", iteration_cnt++, ret);
        print_time(start_first_extend);

        if (lb >= ret)
            break;
        lb = ret;

        // weak reduce, T(n)=O(m)
        double start_weak_reduce = get_system_time_microsecond();
        g.weak_reduce(lb);
        printf("After weak reduce: n= %d , ", g.n);
        print_time(start_weak_reduce);

        if (lb >= g.n)
        {
            g.n = 0;
            FastHeuris_lb = lb;
            FastHeuris_time = get_system_time_microsecond() - algorithm_start_time;
            print_final_log();
            return 0;
        }

        break;
    }

    iteration_cnt = 1;
    while (1) // degeracy order for the whole graph, T(n)=O(m)
    {
        double start_degeneracy = get_system_time_microsecond();
        int degeneracy_lb = g.degeneracyLB(&solution);
        printf("%d-th degeneracy order lb= %d ,", iteration_cnt++, degeneracy_lb);
        print_time(start_degeneracy);

        if (lb >= degeneracy_lb)
            break;
        lb = degeneracy_lb;

        // weak reduce, T(n)=O(m)
        double start_weak_reduce = get_system_time_microsecond();
        g.weak_reduce(lb);
        printf("After weak reduce: n= %d , ", g.n);
        print_time(start_weak_reduce);

        if (lb >= g.n)
        {
            g.n = 0;
            FastHeuris_lb = lb;
            FastHeuris_time = get_system_time_microsecond() - algorithm_start_time;
            print_final_log();
            return 0;
        }

        break;
    }

    // strong reduce
    {
        double start_strong_reduce = get_system_time_microsecond();
        Reduction reduce(&g);
        ui pre_n = g.n;
        reduce.strong_reduce(lb);
        strong_reduce_time += get_system_time_microsecond() - start_strong_reduce;

        if (lb >= g.n)
        {
            g.n = 0;
            FastHeuris_time = get_system_time_microsecond() - algorithm_start_time;
            print_final_log();
            return 0;
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
        for (ui i = 0; i < g.n; i++)
        {
            // to avoid that StrongHeuris cost too much time, we set a time limit
            if(get_system_time_microsecond() - start_current_iteration > time_limit) break;
            extend_lb = max(extend_lb, g.extend(i, &solution));
            if (extend_lb > lb)
            {
                break;
            }
        }
        printf("%dth-StrongHeuris lb= %d\n", iteration_cnt++, extend_lb);
        if (extend_lb <= lb)
            break;
        lb = extend_lb;

        // strong reduce
        {
            double start_strong_reduce = get_system_time_microsecond();
            Reduction reduce(&g);
            ui pre_n = g.n;
            reduce.strong_reduce(lb);
            strong_reduce_time += get_system_time_microsecond() - start_strong_reduce;

            if (lb >= g.n)
            {
                g.n = 0;
                StrongHeuris_time = get_system_time_microsecond() - start_extend_time;
                print_final_log();
                return 0;
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
        printf("must contain: %u\n",must_contain.size());
        g.remove_v_must_include(rm, lb - (int)must_contain.size()); // we conduct weak reduce too

        // strong reduce
        {
            double start_strong_reduce = get_system_time_microsecond();
            Reduction reduce(&g);
            ui pre_n = g.n;
            reduce.strong_reduce(lb - (int)must_contain.size());
            strong_reduce_time += get_system_time_microsecond() - start_strong_reduce;
            printf("Remove must contain & strong reduce: n= %u\n",g.n);

            if (lb >= g.n + (int)must_contain.size())
            {
                g.n = 0;
                break;
            }
        }

        if (lb >= g.n + (int)must_contain.size())
        {
            g.n = 0;
            break;
        }

        while (1)
        {
            int extend_lb = 0;
            double start_current_iteration = get_system_time_microsecond();
            for (ui i = 0; i < g.n; i++)
            {
                extend_lb = max(extend_lb, g.extend(i, &solution, &must_contain));
                if (extend_lb + (int)must_contain.size() > lb)
                {
                    break;
                }
            }
            if (extend_lb + (int)must_contain.size() <= lb)
                break;
            lb = extend_lb + (int)must_contain.size();

            // strong reduce
            {
                double start_strong_reduce = get_system_time_microsecond();
                Reduction reduce(&g);
                ui pre_n = g.n;
                reduce.strong_reduce(lb - (int)must_contain.size());
                strong_reduce_time += get_system_time_microsecond() - start_strong_reduce;

                if (lb >= g.n + (int)must_contain.size())
                {
                    g.n = 0;
                    print_final_log();
                    return 0;
                }
            }
        }
    }
    print_final_log();

    return 0;
}