#ifndef BRANCH_H
#define BRANCH_H

#include "Graph.h"
#include "../Verify/verify.h"

class Branch
{
private:
    using Set = MyBitset;
    Graph_input &G_input;
    int lb;

    // info of the search tree
    int v_just_add;
    vector<int> loss_cnt;
    vector<int> deg;
    AjacentMatrix non_A, A;
    Graph *ptr_g;

    // arrays that can be shared
    vector<int> int_array;
    Set bool_array;
    vector<int> array_N; // N is the input graph size

    // information of log
    ll dfs_cnt;
    double part_PI_time;
    double run_time;
    double fast_reduce_time;
    double core_reduce_time;
    double pivot_select_time;
    double IE_induce_time;
    double matrix_init_time;
    ll S_size;

public:
    set<int> solution;
    Branch(Graph_input &input, int _lb) : G_input(input), lb(_lb), bool_array(input.n),
                                          dfs_cnt(0), run_time(0), fast_reduce_time(0), core_reduce_time(0),
                                          part_PI_time(0), pivot_select_time(0), IE_induce_time(0), S_size(0),
                                          matrix_init_time(0)
    {
    }
    ~Branch() {}

    void print_result()
    {
        lb += G_input.must_contain.size();
        printf("dfs_cnt= %lld\n", dfs_cnt);
        print_module_time("fast reduce", fast_reduce_time);
        print_module_time("partition", part_PI_time);
        print_module_time("core reduce", core_reduce_time);
        print_module_time("IE induce", IE_induce_time);
        puts("");
        print_module_time("pivot select", pivot_select_time);
        print_module_time("matrix init", matrix_init_time);
        printf("average S size: %.2lf  ", S_size * 1.0 / dfs_cnt);
        puts("");
        puts("*************DPBB result*************");
        printf("ground truth= %d , use time= %.4lf s\n", lb, run_time / 1e6);
        if (solution.size())
        {
            G_input.get_ground_truth(solution, true);
            printf("Solution (size= %u):\n", solution.size());
            print_set(solution);
            assert(solution.size() == lb);
        }
        else
            printf("The heuristic solution is the ground truth!\n");
        puts("");
    }

    /**
     * each time we enumerate v_i that must be included into S, and C is the 2-hop neighbors of v_i
     */
    void IE_framework()
    {
        double start_IE = get_system_time_microsecond();
        G_input.init_heap();
        array_N.resize(G_input.n);
        while (G_input.size() > lb)
        {
            double percentage = 1.0 - G_input.size() * 1.0 / G_input.n;
            print_progress_bar(percentage);
            double start_induce = get_system_time_microsecond();

            int u = G_input.get_min_degree_v();
            if (G_input.d[u] + paramK <= lb)
            {
                G_input.remove_v(u);
                continue;
            }

            auto &vis = bool_array;
            vis.clear();
            vis.set(u);

            G_input.induce_to_2hop(vis, u);
            Graph g(vis, G_input.neighbors, array_N);
            matrix_init_time += g.init_time;
            ptr_g = &g;

            int id_u = 0; // the index of u in the new-induced graph g
            for (int v : vis)
                if (v == u)
                    break;
                else
                    id_u++;

            Set S(g.size()), C(g.size());
            S.set(id_u);
            C.flip();
            C.reset(id_u);
            assert(S.size() + C.size() == g.size());

            init_info(id_u, g);

            IE_induce_time += get_system_time_microsecond() - start_induce;

            v_just_add = id_u;
            bnb(S, C);

            G_input.remove_v(u);
        }
        run_time = get_system_time_microsecond() - start_IE;
        print_progress_bar(1.0, true);

        print_result();
    }

    void init_info(int u, Graph &g) // init information for the induced graph of IE
    {
        v_just_add = u;
        loss_cnt.clear();
        loss_cnt.resize(g.size());
        deg.clear();
        deg.resize(g.size());
        non_A = g.adj_matrix;
        non_A.flip();
        A = g.adj_matrix;

        int_array.clear();
        int_array.resize(g.size());
    }

    void update_lb(Set &S)
    {
        int sz = S.size();
        if (sz > lb)
        {
            lb = sz;
            solution.clear();
            for (int v : S)
                solution.insert(v);
            solution = ptr_g->get_ori_vertices(solution);
            assert(solution.size() == lb);
        }
    }

    void update_lb(Set &S, Set &C)
    {
        int sz = S.size() + C.size();
        if (sz > lb)
        {
            lb = sz;
            solution.clear();
            for (int v : S)
                solution.insert(v);
            for (int v : C)
                solution.insert(v);
            solution = ptr_g->get_ori_vertices(solution);
            assert(solution.size() == lb);
            printf("Find a larger plex : %d\n", sz + (int)G_input.must_contain.size());
            fflush(stdout);
        }
    }

    /**
     * using reduction rules & acquire degree
     */
    void fast_reduction(Set &S, Set &C, bool &g_is_plex, bool &S_is_plex)
    {
        if (v_just_add != -1) // only if S changed, we can update loss_cnt[]
        {
            for (int v : S)
            {
                loss_cnt[v] = non_A[v].intersect(S); // v∈S, delta[v] = the number of non-neighbors of v in S
                if (loss_cnt[v] > paramK)
                {
                    S_is_plex = false;
                    return;
                }
                else if (loss_cnt[v] == paramK) // v has k non-neighbors in S, so all non-neighbors of v can be removed
                    C &= A[v];
            }
            update_lb(S);
            for (int u : C)
            {
                loss_cnt[u] = non_A[u].intersect(S); // u∈C, delta[u] = the number of non-neighbors of u in S
                if (loss_cnt[u] >= paramK)           // u has at least k non-neighbors in S, so u can be removed
                    C.reset(u);
            }
        }
        S_is_plex = true;
        // compute degree of subgraph S∪C
        auto V = C;
        V |= S;
        g_is_plex = 1;
        int sz = V.size();
        Set satisfied(S.range);
        for (int v : V)
        {
            deg[v] = A[v].intersect(V);
            if (deg[v] + paramK >= sz)
            {
                // if (C[v])
                satisfied.set(v);
            }
            else
                g_is_plex = false;
        }
        if (g_is_plex)
            return;
        // now we consider the vertices that must be included:
        //  if deg[u]>=n-k and each neighbor v of u satisfies: deg[v]>=n-k, then we must include u
        int satis_cnt = satisfied.size();
        auto C_satisfied = satisfied;
        C_satisfied &= C;
        bool S_changed = false;
        for (int u : C_satisfied)
        {
            auto non_neighbor = non_A[u];
            non_neighbor &= V;
            int satisfied_non_neighbor = non_neighbor.intersect(satisfied);
            int tot_non_neighbor = non_neighbor.size();
            if (satisfied_non_neighbor == tot_non_neighbor)
            {
                S.set(u);
                C.reset(u);
                S_changed = true;
            }
            else if (satisfied_non_neighbor + 1 == tot_non_neighbor) // if only one of the non-neighbors of u is un-satisfied (denoted by w), then u must be included
            {
                auto copy = non_neighbor;
                copy &= satisfied;
                non_neighbor ^= copy;
                assert(non_neighbor.size() == 1);
                int w = *non_neighbor.begin();
                if (!S[w])
                {
                    assert(!A[u][w]);
                    assert(!satisfied[w]);
                    S.set(u);
                    C.reset(u);
                    S_changed = true;
                }
            }
        }
        // S is changed, so we need to re-compute loss_cnt[]; but this won't cause any reduction
        if (S_changed)
        {
            for (int v : S)
            {
                loss_cnt[v] = non_A[v].intersect(S); // v∈S, delta[v] = the number of non-neighbors of v in S
            }
            update_lb(S);
            for (int u : C)
            {
                loss_cnt[u] = non_A[u].intersect(S); // u∈C, delta[u] = the number of non-neighbors of u in S
            }
        }
    }

    /**
     * Branch-aNd-Bound
     */
    void bnb(Set &S, Set &C)
    {
        dfs_cnt++;
        S_size += S.size();
        assert(S.intersect(C) == 0);
        // reduction rules
        double start_fast_reduce = get_system_time_microsecond();
        bool S_is_plex, g_is_plex;
        fast_reduction(S, C, g_is_plex, S_is_plex);
        if (!S_is_plex)
            return;
        if (g_is_plex)
        {
            update_lb(S, C);
            return;
        }
        fast_reduce_time += get_system_time_microsecond() - start_fast_reduce;

        // bounding & stronger reduction
        int ub = get_UB(S, C);
        if (ub <= lb)
            return;

        // select pivot to generate 2 branches
        double start_select_time = get_system_time_microsecond();
        int pivot = select_pivot_vertex_with_min_degree(C);
        if (pivot == -1)
            return;
        pivot_select_time += get_system_time_microsecond() - start_select_time;

        {
            auto new_S = S, new_C = C;
            // branch 1: remove pivot
            new_C.reset(pivot);
            v_just_add = -1;
            bnb(new_S, new_C);
        }

        {
            // branch 2: include pivot
            S.set(pivot);
            C.reset(pivot);
            // new_S.set(pivot);
            v_just_add = pivot;
            // bnb(new_S, new_C);
            bnb(S, C);
        }
    }

    // reduce P to (cnt-k)-core, namely P need to provide at least cnt vertices
    void core_reduction(Set &P, int cnt)
    {
        // if |P|<cnt, then P must be reduce to empty; if cnt<=k, then we can't reduce any vertex
        if (P.size() < cnt || cnt <= paramK)
            return;
        double start_core_reduce = get_system_time_microsecond();
        auto &deg = int_array; // we reuse the array to decrease time cost
        for (int u : P)
        {
            int d = A[u].intersect(P);
            if (d + paramK < cnt)
            {
                for (int v : P)
                {
                    if (v == u)
                        break;
                    if (A[u][v])
                        deg[v]--;
                }
                P.reset(u);
            }
            else
                deg[u] = d;
        }
        queue<int> q;
        for (int u : P)
            if (deg[u] + paramK < cnt)
                q.push(u);
        while (q.size())
        {
            int u = q.front();
            q.pop();
            P.reset(u);
            for (int v : P)
            {
                if (A[u][v])
                {
                    if (--deg[v] + paramK + 1 == cnt)
                    {
                        q.push(v);
                    }
                }
            }
        }
        core_reduce_time += get_system_time_microsecond() - start_core_reduce;
    }

    int erfen_UB(Set &P)
    {
        int sz = P.size();
        if (sz <= paramK)
            return sz;
        vector<int> cnt(sz + paramK, 0);
        for (int u : P)
        {
            int d = A[u].intersect(P);
            cnt[d + paramK]++;
        }
        int sum = 0; // the number of vertices that pseudo-degree is not less than ub, i.e., sum is suffix-sum
        for (int ub = sz + paramK - 1; ub >= paramK; ub--)
        {
            sum += cnt[ub];
            if (sum >= ub)
            {
                return ub;
            }
        }
        // should not reach here
        assert(false);
        return -1;
    }

    /**
     * we partition C to |S| sets: Pi_0, Pi_1, ..., Pi_|S|
     * @return ub
     * we will also reduce unpromissing vertices from C
     */
    int get_part_UB(Set &S, Set &C)
    {
        double start_part = get_system_time_microsecond();
        auto copy_S = S;
        auto copy_C = C;
        int ub = S.size();
        while (copy_S.size())
        {
            int sel = -1, size = 0, ub_cnt = 0;
            for (int v : copy_S)
            {
                auto non_neighbor = non_A[v];
                non_neighbor &= copy_C;
                int sz = non_neighbor.size();
                int cnt = (paramK - loss_cnt[v]);
                if (sz <= cnt) // Pi_i is useless
                {
                    copy_S.reset(v);
                }
                else
                {
                    if (sel == -1 || size * cnt < sz * ub_cnt) // (size/ub_cnt)<(sz/cnt)
                    {
                        sel = v;
                        size = sz;
                        ub_cnt = cnt;
                        // break;
                    }
                }
            }
            if (sel == -1)
                break;
            copy_S.reset(sel);
            ub += ub_cnt;
            copy_C &= A[sel]; // remove the non-neighbors of sel
        }
        part_PI_time += get_system_time_microsecond() - start_part;
#ifndef NO_CORE_REDUCTION
        // now copy_C = Pi_0
        core_reduction(copy_C, lb + 1 - ub);
        int ret = ub + copy_C.size();
        if (ret <= lb) // pruned
            return ret;
        // //er fen UB will not cause due to core-reduce, but it will decrease ub indeed
        // {
        //     ret -= copy_C.size();
        //     ret += erfen_UB(copy_C);
        // }
        // assume we need to include at least $h$ vertices from S+Pi_0; $h$=lb+1-(ub-|S|);
        // then for u∈Pi_i, u must has at least $h-k+1$ neighbors from S+Pi_0
        int neighbor_cnt = lb + 1 - (ub - S.size()) - (paramK - 1);
        if (neighbor_cnt > 0)
        {
            auto S_Pi0 = S;
            S_Pi0 |= copy_C; // S+Pi_0
            auto temp = C;
            temp ^= copy_C; // temp = C - Pi_0 = Pi_i
            // u∈Pi_i, then u has at least (lb+1-sigma_(min(|Pi_i|, k-|S\N(v_i)|))-(k-1) = neighbor_cnt) neighbors in Pi_0∪S
            for (int u : temp)
            {
                if (A[u].intersect(S_Pi0) < neighbor_cnt)
                    C.reset(u);
            }
        }
#endif
        return ret;
    }

    int get_UB(Set &S, Set &C)
    {
        return get_part_UB(S, C);
    }

    /**
     * @return the vertex with minimum degree
     */
    int select_pivot_vertex_with_min_degree(Set &C)
    {
        int sel = -1;
        for (int u : C)
            if (sel == -1 || deg[u] < deg[sel])
                sel = u;
        return sel;
    }

    /**
     * @return the vertex with minimum degree
     */
    int select_pivot_vertex_with_max_degree(Set &C)
    {
        int sel = -1;
        for (int u : C)
            if (sel == -1 || deg[u] > deg[sel])
                sel = u;
        return sel;
    }

    /**
     * @return the vertex with maximum loss & minimum degree
     */
    int select_pivot_vertex_with_max_loss(Set &C)
    {
        int sel = -1;
        for (int u : C)
            if (sel == -1 || loss_cnt[u] > loss_cnt[sel] || (loss_cnt[u] == loss_cnt[sel] && deg[u] < deg[sel]))
                sel = u;
        return sel;
    }

    /**
     * abandoned due to bad performance
     */
    int get_UB_degree(Set &S, Set &C)
    {
        int ub = S.size() + C.size();
        for (int v : S)
            ub = min(ub, deg[v] + paramK);
        return ub;
    }
};

#endif