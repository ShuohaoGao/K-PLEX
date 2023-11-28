#ifndef BRANCH_H
#define BRANCH_H

#include "Graph.h"

class Branch
{
private:
    using Set = MyBitset;
    Graph_reduced &G_input;
    int lb;

    // used for branching rule: select the vertex that must contain
    struct Must_contain_status
    {
        int must_contain_u;
        int v, w;
        bool on;
    } must_contain_status;

    // info of the search tree
    int v_just_add;
    vector<int> loss_cnt;
    vector<int> deg;
    AjacentMatrix non_A, A;
    Graph_adjacent *ptr_g;

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
    int IE_graph_cnt;
    ll IE_graph_size;
    double CTCP_time;
    double C_reduce;
    double ub_try_time;

public:
    set<int> solution;
    Branch(Graph_reduced &input, int _lb) : G_input(input), lb(_lb), bool_array(input.n),
                                            dfs_cnt(0), run_time(0), fast_reduce_time(0), core_reduce_time(0),
                                            part_PI_time(0), pivot_select_time(0), IE_induce_time(0), S_size(0),
                                            matrix_init_time(0), IE_graph_cnt(0), IE_graph_size(0), CTCP_time(0),
                                            C_reduce(0), ub_try_time(0)
    {
    }
    ~Branch() {}

    /**
     * @brief print some logs
     */
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
        print_module_time("CTCP", CTCP_time);
        print_module_time("C-reduce", C_reduce);
        puts("");
        print_module_time("ub-try-time", ub_try_time);
        puts("");
        printf("average g_i size: %.2lf ", IE_graph_size * 1.0 / IE_graph_cnt);
        printf("average S size: %.2lf  ", S_size * 1.0 / dfs_cnt);
        puts("");
        puts("*************bnb result*************");
        printf("ground truth= %d , exact searching use time= %.4lf s\n", lb, run_time / 1e6);
        if (solution.size())
        {
            G_input.get_ground_truth(solution, true);
            // printf("Solution (size= %u):\n", solution.size());
            // print_set(solution);
            assert(solution.size() == lb);
        }
        else
            printf("The heuristic solution is the ground truth!\n");
        // puts("");
    }

    /**
     * @brief each time we enumerate v_i that must be included into S, and C is the 2-hop neighbors of v_i
     * another name: Divide and Conquer framework
     */
    void IE_framework()
    {
        double start_IE = get_system_time_microsecond();
        G_input.init_before_IE();
        CTCP_time += get_system_time_microsecond() - start_IE;
        array_N.resize(G_input.n);
        while (G_input.size() > lb)
        {
            double percentage = 1.0 - G_input.size() * 1.0 / G_input.n;
            print_progress_bar(percentage);
            double start_induce = get_system_time_microsecond();

            int u = G_input.get_min_degree_v();

            auto &vis = bool_array;
            vis.set(u);

            vector<int> vertices_2hops{u};
            G_input.induce_to_2hop(u, vis, vertices_2hops);
            vector<int> &inv = array_N;
            Graph_adjacent g(vis, vertices_2hops, G_input, inv);
            if (vertices_2hops.size() > 2)
            {
                // <==> vis.clear() but this may be too time-consuming, so we clear it as follows
                for (int v : vertices_2hops)
                    vis.reset(v);
            }
            else
            {
                // the reduced graph is stored as adjacent matrix, so O(n) is acceptable
                vis.clear();
            }
            IE_graph_size += g.size();
            IE_graph_cnt++;
            matrix_init_time += g.init_time;
            ptr_g = &g;

            int id_u = inv[u]; // the index of u in the new-induced graph g

            Set S(g.size()), C(g.size());
            S.set(id_u);
            C.flip();
            C.reset(id_u);
            assert(S.size() + C.size() == g.size());

            init_info(id_u, g);

            IE_induce_time += get_system_time_microsecond() - start_induce;

            int pre_lb = lb;
            bnb(S, C);

            double start_CTCP = get_system_time_microsecond();
            G_input.remove_v(u, lb, lb > pre_lb ? true : false);
            CTCP_time += get_system_time_microsecond() - start_CTCP;
        }
        run_time = get_system_time_microsecond() - start_IE;
        print_progress_bar(1.0, true);

        print_result();
    }
    /**
     * @brief init information for the induced graph of IE
     */
    void init_info(int u, Graph_adjacent &g)
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

    /**
     * @brief plex=S
     */
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
            printf("Find a larger plex : %d\n", sz + (int)G_input.must_contain.size());
            fflush(stdout);
        }
    }
    /**
     * @brief plex=S+C
     */
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
     * @brief using reduction rules & acquire degree; mainly based on definition and heredictary property
     * @param g_is_plex serve as return
     * @param S_is_plex serve as return
     */
    void fast_reduction(Set &S, Set &C, bool &g_is_plex, bool &S_is_plex, Must_contain_status &must_contain_status)
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
#ifdef BR3
            if (satisfied_non_neighbor + 2 == tot_non_neighbor && !must_contain_status.on)
            {
                must_contain_status.on = true;
                must_contain_status.must_contain_u = u;
                auto copy = non_neighbor;
                copy &= satisfied;
                non_neighbor ^= copy;
                assert(non_neighbor.size() == 2);
                auto it = non_neighbor.begin();
                must_contain_status.v = *it;
                ++it;
                must_contain_status.w = *it;
            }
#endif
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
     * @brief Branch-aNd-Bound on subgraph g_i
     */
    void bnb(Set &S, Set &C)
    {
        dfs_cnt++;
        S_size += S.size();
        assert(S.intersect(C) == 0);
        // reduction rules
        double start_fast_reduce = get_system_time_microsecond();
        bool S_is_plex, g_is_plex;
        must_contain_status.on = false;
        fast_reduction(S, C, g_is_plex, S_is_plex, must_contain_status);
        if (!S_is_plex)
            return;
        if (g_is_plex)
        {
            update_lb(S, C);
            return;
        }
        fast_reduce_time += get_system_time_microsecond() - start_fast_reduce;

#ifdef BR3
        if (must_contain_status.on)
        {
            // cout<<"!!!!!!!"<<endl;
            auto new_S = S, new_C = C;
            int u = must_contain_status.must_contain_u, v = must_contain_status.v, w = must_contain_status.w;
            v_just_add = -1;
            if (!new_S[v])
            {
                new_S.set(v);
                new_C.reset(v);
                v_just_add = v;
            }
            if (!new_S[w])
            {
                new_S.set(w);
                new_C.reset(w);
                v_just_add = w;
            }
            new_C.reset(u);
            // branch1: excluding u and including both {v,w}
            bnb(new_S, new_C);

            S.set(u);
            C.reset(u);
            v_just_add = u;
            // branch2: including u
            bnb(S, C);
            return;
        }
#endif

        // bounding & stronger reduction
        int pivot1;
        int ub = get_UB(S, C, &pivot1);
        if (ub <= lb)
            return;

        // select pivot to generate 2 branches
        double start_select_time = get_system_time_microsecond();
        // int pivot = select_pivot_vertex_with_min_degree(C);
        int pivot = pivot1;
#ifdef NO_LOOKAHEAD
        pivot = select_pivot_vertex_with_min_degree(C);
#endif

#ifdef DEGREE_BRANCH
        pivot = select_pivot_vertex_with_min_degree(C);
#endif
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

    /**
     * @brief extend S to a maximal k-plex in G[S\cup C]
     * by greedily select a best vertex to include
     */
    void extend(Set S, Set C)
    {
        while (C.size())
        {
            int sel = -1, nc = 0;
            for (int u : C)
            {
                int cnt = S.intersect(A[u]);
                if (sel == -1 || (cnt > nc) || (cnt == nc && deg[u] > deg[sel]))
                {
                    sel = u;
                    nc = cnt;
                }
            }
            if (nc + paramK < S.size() + 1)
                break;
            C.reset(sel);
            for (int v : S)
            {
                if (!A[sel][v] && non_A[v].intersect(S) == paramK - 1)
                {
                    C &= A[v];
                }
            }
            S.set(sel);
        }
        update_lb(S);
    }

    void degenerate(Set &S, Set &C)
    {
        auto V = S;
        V |= C;
        int sz = V.size();
        while (1)
        {
            int sel = -1;
            int d = sz;
            for (int u : V)
            {
                int d_u = A[u].intersect(V);
                if (sel == -1 || (d_u < d))
                {
                    sel = u;
                    d = d_u;
                }
            }
            if (d + paramK >= sz)
                break;
            V.reset(sel);
            sz--;
        }
        update_lb(V);
    }

    /**
     * @brief reduce P to (cnt-k)-core, namely P need to provide at least cnt vertices
     */
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

    /**
     * @return ub of P
     * if P has a plex of size = x, then P must have at least x vertices whose degrees >= x+k
     * conclusion: this ub can be replaced by core-reduction for Pi_0
     */
    int binary_UB(Set &P)
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
     * @return the ub of P by partitioning P into several independent sets
     * conclusion: useless
     */
    int color_UB(Set &P)
    {
        int sz = P.size();
        if (sz <= paramK)
            return sz;
        vector<Set> colors(1, Set(P.range)); // in the begin, there is only one color
        for (int u : P)
        {
            bool ok = 0;
            for (auto &c : colors)
            {
                if (A[u].intersect(c) > 0)
                    continue;
                ok = 1;
                c.set(u);
                break;
            }
            if (!ok) // we need to create a new color
            {
                Set temp(P.range);
                temp.set(u);
                colors.push_back(temp);
            }
        }
        int ret = 0;
        for (auto &s : colors)
            ret += min(s.size(), paramK);
        return ret;
    }

    /**
     * @brief we partition C to |S| sets: Pi_0, Pi_1, ..., Pi_|S|; we will also reduce unpromissing vertices from C
     * @param pivot if not NULL, pivot should be stored as the vertex in C with min ub
     * @return ub
     */
    int get_part_UB(Set &S, Set &C, int *pivot = nullptr)
    {
        Timer part_timer;
        auto copy_S = S;
        auto copy_C = C;
        int S_sz = S.size();
        int ub = S_sz;
        Set useful_S(S.range);
        while (copy_S.size())
        {
            int sel = -1, size = 0, ub_cnt = 0;
            for (int v : copy_S)
            {
                int sz = non_A[v].intersect(copy_C);
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
            useful_S.set(sel);
            ub += ub_cnt;
            copy_C &= A[sel]; // remove the non-neighbors of sel
        }
        part_PI_time += part_timer.get_time();

        // now copy_C = Pi_0
        auto &Pi_0 = copy_C;
#ifndef NO_CORE_REDUCTION
        core_reduction(Pi_0, lb + 1 - ub);
#endif
        int ret = ub + Pi_0.size();
        if (ret <= lb) // pruned
            return ret;

#ifndef NO_REDUCE_PI_I
        // assume we need to include at least $h$ vertices from S+Pi_0; $h$=lb+1-(ub-|S|);
        // then for u∈Pi_i, u must has at least $h-k+1$ neighbors from S+Pi_0
        {
            Timer t;
            int neighbor_cnt = lb + 1 - (ub - S_sz) - (paramK - 1);
            if (neighbor_cnt > 0)
            {
                auto S_Pi0 = S;
                S_Pi0 |= Pi_0; // S+Pi_0
                auto temp = C;
                temp ^= Pi_0; // temp = C - Pi_0 = Pi_i
                // u∈Pi_i, then u has at least (lb+1-sigma_(min(|Pi_i|, k-|S\N(v_i)|))-(k-1) = neighbor_cnt) neighbors in Pi_0∪S
                for (int u : temp)
                {
                    if (A[u].intersect(S_Pi0) < neighbor_cnt)
                        C.reset(u);
                }
            }
            C_reduce += t.get_time();
        }
#endif

#ifndef NO_LOOKAHEAD
        {
            Timer t;
            int piv = -1, min_ub = INF;
            // for u in C, if UB(S+u, C-u)<=lb , remove u
            for (int u : C)
            {
                int sub = non_A[u].intersect(Pi_0); // Pi_u get $sub$ vertices from Pi_0
                // due to u is included to S, loss_cnt[v] will increase, decreasing ub;
                // decre<=loss_cnt[u] because not all v in S has corresponding Pi_v(i.e., Pi_v is useless for ub so we include Pi_v to Pi_0)
                int decre = useful_S.intersect(non_A[u]);
                int ub_u = ret - sub + (paramK - loss_cnt[u] - 1) + 1 - decre; // ub_u=UB(S+u, C)>=UB(S+u,C-u)
                if (ub_u <= lb)
                {
                    C.reset(u);
                    if (Pi_0[u])
                    {
                        ret--;
                        Pi_0.reset(u);
                        if (ret <= lb)
                            return ret;
                    }
                }
                else // for select a branching pivot
                {
                    if (min_ub > ub_u)
                    {
                        min_ub = ub_u;
                        piv = u;
                    }
                    else if (min_ub == ub_u && (deg[u] < deg[piv]))
                    {
                        piv = u;
                    }
                }
            }
            if (pivot != nullptr)
            {
                *pivot = piv;
            }
            ub_try_time += t.get_time();
        }
#endif
        return ret;
    }

    /**
     * @param pivot if not NULL, we should select a pivot vertex in it
     * @return UB(S, C)
     */
    int get_UB(Set &S, Set &C, int *pivot = nullptr)
    {
        return get_part_UB(S, C, pivot);
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
     * @return the vertex with minimum degree in the non-neighbors of v
     */
    int select_pivot_vertex_in_non_neighbor(int v, Set &C)
    {
        int sel = -1;
        for (int u : C)
            if (non_A[v][u])
                if (sel == -1 || deg[u] < deg[sel])
                    sel = u;
        if (sel == -1)
            sel = select_pivot_vertex_with_min_degree(C);
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
     * @brief abandoned due to bad performance
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