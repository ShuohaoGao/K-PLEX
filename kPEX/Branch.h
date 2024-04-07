#ifndef BRANCH_H
#define BRANCH_H

#include "Graph.h"
#include "2th-Reduction.h"

class Branch
{
private:
    using Set = MyBitset;
    Graph_reduced &G_input;
    int lb;

    // info of the search tree
    int v_just_add;
    vector<int> loss_cnt;
    vector<int> deg;
    vector<int> one_loss_non_neighbor_cnt;
    vector<int> que; // queue
    Set one_loss_vertices_in_C;
    AdjacentMatrix non_A, A;
    Graph_adjacent *ptr_g;
    // todo, prepare Pi_0 for lookahead, where Pi_0 is reduced by one-loss partition

    // arrays that can be shared
    vector<int> array_n; // n is the size of subgraph g_i
    Set bool_array;
    vector<int> array_N; // N is the input graph size
    vector<int> array1_N;

    // information of log
    ll dfs_cnt;
    ll leaf_cnt;
    double part_PI_time;
    double run_time;
    double fast_reduce_time;
    double core_reduce_time;
    double pivot_select_time;
    double IE_induce_time;
    double matrix_init_time;
    double higher_order_reduce_time;
    ll S_size;
    ll S_size_when_pruned;
    int IE_graph_cnt;
    ll IE_graph_size;
    double CTCP_time;
    double C_reduce_time;
    double lookahead_time;
    double lookahead_vertex_time;
    double lookahead_edge_time;
    ll subgraph_pruned_cnt, subgraph_search_cnt;
    map<int, int> counter;
    double stage1;

public:
    set<int> solution;
    Branch(Graph_reduced &input, int _lb) : G_input(input), lb(_lb), bool_array(input.n),
                                            dfs_cnt(0), run_time(0), fast_reduce_time(0), core_reduce_time(0),
                                            part_PI_time(0), pivot_select_time(0), IE_induce_time(0), S_size(0),
                                            matrix_init_time(0), IE_graph_cnt(0), IE_graph_size(0), CTCP_time(0),
                                            higher_order_reduce_time(0),
                                            C_reduce_time(0), lookahead_time(0), lookahead_vertex_time(0), lookahead_edge_time(0),
                                            leaf_cnt(0), S_size_when_pruned(0), subgraph_pruned_cnt(0),
                                            subgraph_search_cnt(0), stage1(0)
    {
    }
    ~Branch() {}
    /**
     * @brief print some logs
     */
    void print_result()
    {
        printf("dfs_cnt= %lld\n", dfs_cnt);
        print_module_time("fast reduce", fast_reduce_time);
        print_module_time("partition", part_PI_time);
        print_module_time("core reduce", core_reduce_time);
        print_module_time("IE induce", IE_induce_time);
        puts("");
        print_module_time("pivot select", pivot_select_time);
        print_module_time("matrix init", matrix_init_time);
        print_module_time("CTCP", CTCP_time);
        print_module_time("C-reduce", C_reduce_time);
        puts("");
        print_module_time("look-ahead", lookahead_time);
        print_module_time("look-ahead-vertex", lookahead_vertex_time);
        print_module_time("look-ahead-edge", lookahead_edge_time);
        print_module_time("high-order-reduce", higher_order_reduce_time);
        print_module_time("stage1", stage1);
        // print_module_time("stage2", stage2);
        puts("");
        printf("average g_i size: %.2lf ", IE_graph_size * 1.0 / IE_graph_cnt);
        printf("average S size: %.2lf  ", S_size * 1.0 / dfs_cnt);
        printf("average S size when pruned: %.2lf  ", S_size_when_pruned * 1.0 / leaf_cnt);
        printf("g_i pruned: %lld g_i searched: %lld ", subgraph_pruned_cnt, subgraph_search_cnt);
        puts("");
        puts("*************bnb result*************");
        printf("ground truth= %d , exact searching use time= %.4lf s\n", lb, run_time / 1e6);
        if (solution.size())
        {
            G_input.get_ground_truth(solution, true);
            assert(solution.size() == lb);
        }
        else
            printf("The heuristic solution is the ground truth!\n");
        for (auto &h : counter)
            cout << h.x << ' ' << h.y << endl;
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
        array1_N.resize(G_input.n, 0);
        while (G_input.size() > lb)
        {
            double percentage = 1.0 - G_input.size() * 1.0 / G_input.n;
            print_progress_bar(percentage);
            double start_induce = get_system_time_microsecond();

            int u = G_input.get_min_degree_v();
            int previous_lb = lb;

            auto &vis = bool_array;
            vis.set(u);

            vector<int> vertices_2hops{u};
            G_input.induce_to_2hop_and_reduce(u, vis, vertices_2hops, array1_N, lb);
            vector<pii> edges;
            int id_u = CTCP_for_g_i(u, vis, vertices_2hops, array_N, edges, lb);
            if (id_u != -1) // this subgraph is not pruned: begin bnb
            {
                subgraph_search_cnt++;
                vector<int> &inv = array_N;
                // Graph_adjacent g(vis, vertices_2hops, G_input, inv);
                Graph_adjacent g(vertices_2hops, edges);
                IE_induce_time += get_system_time_microsecond() - start_induce;
                IE_graph_size += g.size();
                IE_graph_cnt++;
                matrix_init_time += g.init_time;
                ptr_g = &g;

                // int id_u = inv[u]; // the index of u in the new-induced graph g
                {
                    // higher order reduction
                    Timer tt;
                    g.edge_reduction(id_u, lb);
                    higher_order_reduce_time += tt.get_time();
                }

                Set S(g.size()), C(g.size());
                S.set(id_u);
                C.flip();
                C.reset(id_u);
                init_info(id_u, g);
                v_just_add = id_u;
                bnb(S, C); // BRB_Rec in paper
            }
            else
            {
                IE_induce_time += get_system_time_microsecond() - start_induce;
                subgraph_pruned_cnt++;
            }

            double start_CTCP = get_system_time_microsecond();
            G_input.remove_v(u, lb, lb > previous_lb ? true : false);
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
        array_n.clear();
        array_n.resize(g.size());
        one_loss_vertices_in_C = Set(g.size());
        one_loss_non_neighbor_cnt.resize(g.size());
        que.resize(g.size());
    }
    /**
     * @return the index of v in the subgraph
     */
    int CTCP_for_g_i(int v, MyBitset &V_mask, vector<int> &vertices, vector<int> &inv, vector<pii> &edges, int lb)
    {
        if (!V_mask[v]) // the subgraph is already pruned due to core-reduction for N(v) and N^2(v)
            return -1;
        auto &g = G_input;
        sort(vertices.begin(), vertices.end());
        for (int i = 0; i < (int)vertices.size(); i++)
            inv[vertices[i]] = i;
        for (int u : vertices)
        {
            for (int i = g.pstart[u]; i < g.pstart[u + 1]; i++)
            {
                if (g.edge_removed[i])
                    continue;
                int v = g.edge_to[i];
                if (!V_mask[v])
                    continue;
                edges.push_back({inv[u], inv[v]});
            }
        }
        Graph g_i(vertices, edges);
        if (paramK > 10)
        {
            g_i.weak_reduce(lb);
            if (g_i.n > lb)
            {
                Reduction reduce(&g_i);
                reduce.strong_reduce(lb);
            }
        }
        // clear the mask
        for (int u : vertices)
        {
            assert(V_mask[u]);
            V_mask.reset(u);
        }
        if (g_i.n > lb)
        {
            // current subgraph can not be pruned and we need to search it,
            // so we prepare edges[] and vertices[] to build graph using adj-matrix
            int ret = -1;
            for (int i = 0; i < g_i.n; i++)
                if (vertices[i] == v)
                {
                    ret = i;
                    break;
                }
            if (ret != -1) // v is still in the subgraph
            {
                if (g_i.m != edges.size())
                {
                    edges.clear();
                    vertices.resize(g_i.n);
                    for (ui u = 0; u < g_i.n; u++)
                    {
                        vertices[u] = g_i.map_refresh_id[u];
                        for (ui i = g_i.pstart[u]; i < g_i.pstart[u + 1]; i++)
                        {
                            ui v = g_i.edge_to[i];
                            edges.push_back({u, v});
                        }
                    }
                }
                return ret;
            }
        }
        // subgraph is pruned
        return -1;
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
            printf("Find a larger plex : %d\n", sz);
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
            printf("Find a larger plex : %d\n", sz);
            fflush(stdout);
        }
    }
    /**
     * @brief compute loss_cnt[] & remove u if S+u is not a plex
     * @param S_is_plex serve as return
     */
    void compute_loss_cnt(Set &S, Set &C, bool &S_is_plex)
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
        S_is_plex = true;
        update_lb(S);
        one_loss_vertices_in_C.clear();
        for (int u : C)
        {
            loss_cnt[u] = non_A[u].intersect(S); // u∈C, delta[u] = the number of non-neighbors of u in S
            if (loss_cnt[u] >= paramK)           // u has at least k non-neighbors in S, so u can be removed
                C.reset(u);
            else if (loss_cnt[u] == 1)
                one_loss_vertices_in_C.set(u);
        }
    }
    /**
     * @brief using reduction rules & acquire degree; mainly based on definition and heredictary property
     * @param g_is_plex serve as return
     * @param S_is_plex serve as return
     */
    void fast_reduction(Set &S, Set &C, bool &g_is_plex, bool &S_is_plex)
    {
        if (v_just_add != -1) // only if S changed, we can update loss_cnt[]
        {
            compute_loss_cnt(S, C, S_is_plex);
            if (!S_is_plex)
                return;
        }
        else
            one_loss_vertices_in_C &= C;
        S_is_plex = true;
        // compute degree of subgraph S∪C
        auto V = C;
        V |= S;
        g_is_plex = 1;
        int sz = V.size();
        // u is k-satisfied <==> deg[u]+k >= n
        // u is C_near-satisfied <==> deg[u]+k+1 >= n and u in C
        Set satisfied(S.range);
        Set C_near_satisfied(S.range);
        int S_size = S.size();
        for (int v : V)
        {
            int neighbor_in_C = A[v].intersect(C);
            // weak reduce: if d[v] + k <= lb, then remove v
            // note that if we change $sz$, it may affect the later process when finding vertices must include;
            // to avoid bugs, we choose not to decrease $sz$
            if (S[v])
            {
                if (S_size + neighbor_in_C + paramK - loss_cnt[v] <= lb)
                {
                    S_is_plex = false;
                    return;
                }
            }
            else
            {
                // v is in C
                if (S_size + neighbor_in_C + paramK - loss_cnt[v] <= lb)
                {
                    V.reset(v);
                    C.reset(v);
                    continue;
                }
            }
            deg[v] = S_size - loss_cnt[v] + neighbor_in_C;
            // deg[v] = A[v].intersect(V);
            if (deg[v] + paramK >= sz)
            {
                satisfied.set(v);
                if (C[v])
                    C_near_satisfied.set(v);
            }
            else
            {
                g_is_plex = false;
                if (deg[v] + paramK + 1 == sz && C[v])
                    C_near_satisfied.set(v);
            }
        }
        if (g_is_plex)
            return;
        // now we consider the vertices that must be included
        int satis_cnt = satisfied.size();
        bool S_changed = false;
        for (int u : C_near_satisfied)
        {
            bool must_include = 0;
            // case 1
            if (deg[u] + 2 >= sz)
            {
                must_include = 1;
            }
            else
            {
                // we consider the non-neighbors of u (excluding u itself)
                int satisfied_non_neighbor = satisfied.intersect(non_A[u]);
                int tot_non_neighbor = non_A[u].intersect(V);
                if (satisfied[u])
                {
                    satisfied_non_neighbor--;
                }
                tot_non_neighbor--;
                // case 2: u is satisfied and all non-neighbors of u are satisfied
                if (satisfied_non_neighbor == tot_non_neighbor)
                {
                    if (satisfied[u])
                        must_include = 1;
                }
                if (loss_cnt[u] > 0)
                    continue;
                // case 3: deg[u]>=n-k-1 and only one non-neighbor of u is un-satisfied
                else if (satisfied_non_neighbor + 1 == tot_non_neighbor)
                {
                    must_include = 1;
                }
                // case 4: deg[u]>=n-k-1 and all the un-satisfied non-neighbors form an independent vertex set
                else
                {
                    auto non_neighbor = V;
                    non_neighbor &= non_A[u];
                    auto &un_satisfied_non_neighbor = non_neighbor;
                    un_satisfied_non_neighbor.sub(satisfied);
                    // assert(un_satisfied_non_neighbor.size() >= 2);
                    bool is_independent = true;
                    for (int a : un_satisfied_non_neighbor)
                    {
                        for (int b : un_satisfied_non_neighbor)
                        {
                            if (b >= a)
                                break;
                            if (A[a][b])
                            {
                                is_independent = false;
                                break;
                            }
                        }
                        if (!is_independent)
                            break;
                    }
                    if (is_independent)
                        must_include = 1;
                }
            }
            if (must_include)
            {
                S.set(u);
                C.reset(u);
                S_changed = true;
                v_just_add = u;
                break; // each time we only include one vertex that must be included
            }
        }
        // S is changed, so we need to re-compute loss_cnt[] and deg[]
        if (S_changed)
        {
            fast_reduction(S, C, g_is_plex, S_is_plex);
        }
    }
    bool core_reduction_for_g(Set &S, Set &C)
    {
        auto V = S;
        V |= C;
        core_reduction(V, lb + 1);
        if (S.intersect(V) != S.size())
        {
            return true;
        }
        C &= V;
        return false;
    }
    /**
     * @brief Branch-aNd-Bound on subgraph g_i
     * i.e., BRB_Rec in paper
     */
    void bnb(Set &S, Set &C)
    {
        dfs_cnt++;
        S_size += S.size();

        // reduction rules
        Timer start_fast_reduce;
        bool S_is_plex, g_is_plex;
        fast_reduction(S, C, g_is_plex, S_is_plex); // existing techniques
        fast_reduce_time += start_fast_reduce.get_time();

        if (!S_is_plex)
            return;
        if (g_is_plex)
        {
            update_lb(S, C);
            return;
        }

        // AltRB: bounding & stronger reduction (our novel method)
        vector<pii> edges_removed;
        int ub = get_UB(S, C, edges_removed);
        if (ub <= lb)
        {
            leaf_cnt++;
            S_size_when_pruned += S.size();
            return;
        }

        // select pivot to generate 2 branches
        Timer look;
        if (paramK > 15)
        {
            lookahead_edge(S, C, edges_removed);
            if (core_reduction_for_g(S, C))
            {
                // rollback
                for (auto &h : edges_removed)
                {
                    int u = h.x, v = h.y;
                    A.add_edge(u, v);
                    non_A.remove_edge(u, v);
                }
                return;
            }
        }
        if (paramK > 5)
        {
            // look ahead: if UB(S+u, C-u)<=lb, then remove u; we select the vertex with min ub as pivot
            lookahead_vertex(S, C);
        }
        lookahead_time += look.get_time();

        Timer select_timer;
        int pivot = -1;
        pivot = select_pivot_vertex_with_min_degree(C);
        pivot_select_time += select_timer.get_time();

        if (pivot == -1)
        {
            // rollback
            for (auto &h : edges_removed)
            {
                int u = h.x, v = h.y;
                A.add_edge(u, v);
                non_A.remove_edge(u, v);
            }
            return;
        }

        generate_sub_branches(S, C, pivot);

        // rollback
        for (auto &h : edges_removed)
        {
            int u = h.x, v = h.y;
            A.add_edge(u, v);
            non_A.remove_edge(u, v);
        }
    }
    void generate_sub_branches(Set &S, Set &C, int pivot)
    {
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
            v_just_add = pivot;
            bnb(S, C);
        }
    }
    /**
     * @brief reduce P to (cnt-k)-core, namely P need to provide at least cnt vertices
     */
    void core_reduction(Set &P, int cnt)
    {
        // if |P|<cnt, then P must be reduce to empty; if cnt<=k, then we can't reduce any vertex
        if (cnt <= paramK || P.size() < cnt)
            return;
        double start_core_reduce = get_system_time_microsecond();
        auto &deg = array_n; // we reuse the array to decrease time cost
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
     * @brief reduce P to (cnt-k)-core, namely P need to provide at least $cnt$ vertices
     *  for u in Pi_0, we should have UB(S+u, C-u) = |S|+1+(lb+1-cnt-|S|)+|N(u)\cap P|+k-1-|S\N(u)| >= lb+1
     *  <=> |N(u)\cap P| >= cnt+|S\N(u)|-k
     *  i.e., we remove u from Pi_0 if u has less than $cnt-k+loss_cnt[u]$ neighbors in Pi_0
     * @param C if we remove u from P, then we remove it from C too
     */
    void reduce_Pi_0(Set &P, int cnt, Set &C)
    {
        // if |P|<cnt, then P must be reduce to empty; if cnt<=k, then we can't reduce any vertex
        if (cnt <= paramK || P.size() < cnt)
            return;
        double start_core_reduce = get_system_time_microsecond();
        auto &deg = array_n; // we reuse the array to decrease time cost
        for (int u : P)
        {
            int d = A[u].intersect(P);
            if (d < cnt + loss_cnt[u] - paramK)
            {
                for (int v : P)
                {
                    if (v == u)
                        break;
                    if (A[u][v])
                        deg[v]--;
                }
                P.reset(u);
                C.reset(u);
            }
            else
                deg[u] = d;
        }
        int hh = 0, tt = -1;
        auto &q = que; // faster than std::queue
        for (int u : P)
            if (deg[u] < cnt + loss_cnt[u] - paramK)
            {
                q[++tt] = u;
            }
        while (hh <= tt)
        {
            int u = q[hh++];
            P.reset(u);
            C.reset(u);
            for (int v : P)
            {
                if (A[u][v])
                {
                    --deg[v];
                    if (deg[v] + 1 == cnt + loss_cnt[v] - paramK)
                    {
                        q[++tt] = v;
                    }
                }
            }
        }
        core_reduce_time += get_system_time_microsecond() - start_core_reduce;
    }
    /**
     * @brief reduce an edge (u,v) if u,v in C and UB(S+u+v, C-u-v)<=lb
     *
     * @param edges_removed we record the edges we remove in order to rollback when backtrack
     */
    void lookahead_edge(Set &S, Set &C, vector<pii> &edges_removed)
    {
        return;
        Timer t;
        int S_sz = S.size();
        for (int u : C)
        {
            auto N_u = C;
            N_u &= A[u];
            int allow_u = C.size() - N_u.size() - 1;
            allow_u = min(allow_u, paramK - 1 - loss_cnt[u]);
            for (int v : N_u)
            {
                if (v >= u)
                    break;
                int common_neighbor_cnt = N_u.intersect(A[v]);
                int ub = S_sz + common_neighbor_cnt + allow_u + min(paramK - 1 - loss_cnt[v], N_u.size() - common_neighbor_cnt) + 2;
                if (ub <= lb)
                {
                    edges_removed.push_back({u, v});
                    A.remove_edge(u, v);
                    N_u.reset(v);
                    non_A.add_edge(u, v);
                }
            }
        }
        lookahead_edge_time += t.get_time();
    }
    /**
     * @brief reduce a vertex (u,v) if u in C and UB(S+u, C-u)<=lb;
     * bounding method: select v in S and u in C, ub=|N(v) \cap N(u)| + (k-|S+u\N(v)|) + (k-|S+u\N(u)|) + |S+u|
     */
    void lookahead_vertex(Set &S, Set &C)
    {
        Timer t;
        int v = v_just_add;
        if (v == -1)
        {
            for (int u : S)
            {
                if (v == -1 || loss_cnt[u] > loss_cnt[v])
                {
                    v = u;
                }
            }
        }
        auto N_v = A[v];
        N_v &= C;
        int S_sz = S.size();
        if (N_v.size() > paramK - S.size())
        {
            for (int u : C)
            {
                int common_neighbor = N_v.intersect(A[u]);
                int loss_v = loss_cnt[v] + (!A[v][u]);
                int loss_u = loss_cnt[u] + 1;
                int ub = common_neighbor + S_sz + 1 + (paramK - loss_v) + (paramK - loss_u);
                if (ub <= lb)
                {
                    C.reset(u);
                    if (N_v[u])
                        N_v.reset(u);
                }
            }
        }
        else
        {
            // in this case, the partition of Pi_u will be useless
            if (N_v.size() + S_sz + paramK - loss_cnt[v] <= lb)
            {
                C.clear();
                return;
            }
        }
        lookahead_vertex_time += t.get_time();
    }
    /**
     * @brief partition ub without reduction rules
     */
    int only_part_UB(Set &S, Set &C, int u = -1)
    {
        Timer part_timer;
        auto &loss = deg;
        for (int v : S)
            loss[v] = non_A[v].intersect(S);
        auto copy_S = S;
        auto copy_C = C;
        int S_sz = S.size();
        int ub = S_sz;
        // we will utilize Pi_u
        if (u != -1)
        {
            int sz = non_A[u].intersect(copy_C);
            int cnt = paramK - loss[u];
            copy_S.reset(u);
            ub += cnt;
            copy_C &= A[u];
        }
        for (int v : copy_S)
        {
            one_loss_non_neighbor_cnt[v] = non_A[v].intersect(one_loss_vertices_in_C);
        }
        while (copy_S.size())
        {
            int sel = -1, size = 0, ub_cnt = 0;
            for (int v : copy_S)
            {
                int sz = non_A[v].intersect(copy_C);
                int cnt = (paramK - loss[v]);
                if (sz <= cnt) // Pi_i is useless
                {
                    copy_S.reset(v);
                }
                else
                {
                    // int one_loss_cnt = non_A[v].intersect(one_loss_vertices_in_C);
                    int one_loss_cnt = one_loss_non_neighbor_cnt[v];
                    if (one_loss_cnt >= cnt)
                    {
                        sel = v;
                        ub_cnt = cnt;
                        break;
                    }
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
        part_PI_time += part_timer.get_time();
        // now copy_C = Pi_0
        auto &Pi_0 = copy_C;
        int ret = ub + Pi_0.size();
        if (u == -1 && ret == lb + 1 && Pi_0.size())
        {
            S |= Pi_0;
            C ^= Pi_0;
            Timer start_fast_reduce;
            bool S_is_plex, g_is_plex;
            v_just_add = *Pi_0.begin();
            fast_reduction(S, C, g_is_plex, S_is_plex);
            fast_reduce_time += start_fast_reduce.get_time();

            if (!S_is_plex)
                return lb;
            if (g_is_plex)
            {
                update_lb(S, C);
                return lb;
            }
        }
        return ret;
    }
    /**
     * @brief AltRB in paper, including ComputeUB and Partition
     * we partition C to |S| sets: Pi_0, Pi_1, ..., Pi_|S|; Pi_0 is C_R and the rest are C_L
     * we will also reduce unpromissing vertices from C
     * @param edges_removed we record the edges we remove in order to rollback when backtrack
     * @return ub
     */
    int bound_and_reduce(Set &S, Set &C)
    {
        Timer part_timer;
        int initial_C_size = C.size();
        auto copy_S = S;
        auto copy_C = C;
        int S_sz = S.size();
        int ub = S_sz;
        Set useful_S(S.range); // for v in useful_S, Pi_v is generated
        // DisePUB: Partition
        while (copy_S.size())
        {
            int sel = -1, size = 0, allow = 0;
            for (int v : copy_S)
            {
                int sz = non_A[v].intersect(copy_C);
                int allow_v = (paramK - loss_cnt[v]);
                if (sz <= allow_v) // Pi_i is useless
                {
                    copy_S.reset(v);
                }
                else
                {
                    if (sel == -1 || size * allow_v < sz * allow) // (size/ub_cnt)<(sz/cnt)
                    {
                        sel = v;
                        size = sz;
                        allow = allow_v;
                    }
                }
            }
            if (sel == -1)
                break;
            if (lb + 1 > ub && size * 1.0 / allow <= copy_C.size() * 1.0 / (lb + 1 - ub))
                break;
            copy_S.reset(sel);
            useful_S.set(sel);
            ub += allow;
            copy_C &= A[sel]; // remove the non-neighbors of sel
        }
        part_PI_time += part_timer.get_time();

        // now copy_C = Pi_0
        auto &Pi_0 = copy_C;
#ifndef NO_REFINE_BOUND
        reduce_Pi_0(Pi_0, lb + 1 - ub, C);
#else
        // no core-reduce & no revocation
        for (int v : S)
        {
            if (useful_S[v])
                continue;
            ub += min(Pi_0.intersect(non_A[v]), paramK - loss_cnt[v]);
            Pi_0 &= A[v];
        }
        copy_S.clear();
#endif
        int ret = ub + Pi_0.size();
        if (ret <= lb) // pruned
            return ret;

#ifndef NO_RR
        // RR1
        // assume we need to include at least $h$ vertices from S+Pi_0; $h$=lb+1-(ub-|S|);
        // then for u∈Pi_i, u must has at least $h-k+1$ neighbors from S+Pi_0
        if (paramK > 5)
        {
            Timer t;
            int LB_Pi_0 = lb + 1 - ub;
            if (LB_Pi_0 > paramK - 1)
            {
                auto temp = C;
                temp ^= Pi_0; // temp = C - Pi_0 = Pi_i
                // u∈Pi_i, then u has at least (lb+1-sigma_(min(|Pi_i|, k-|S\N(v_i)|))-(k-1) = neighbor_cnt) neighbors in Pi_0∪S
                for (int u : temp)
                {
                    int threshold = LB_Pi_0 - (paramK - 1 - loss_cnt[u]);
                    if (A[u].intersect(Pi_0) < threshold)
                        C.reset(u);
                }
            }
            C_reduce_time += t.get_time();
        }
#endif
        while (copy_S.size())
        {
            int sel = -1, size = 0, allow = 0;
            for (int v : copy_S)
            {
                int sz = non_A[v].intersect(copy_C);
                int allow_v = (paramK - loss_cnt[v]);
                if (sz <= allow_v) // Pi_i is useless
                {
                    copy_S.reset(v);
                }
                else
                {
                    if (sel == -1 || size * allow_v < sz * allow) // (size/ub_cnt)<(sz/cnt)
                    {
                        sel = v;
                        size = sz;
                        allow = allow_v;
                    }
                }
            }
            if (sel == -1)
                break;
            copy_S.reset(sel);
            useful_S.set(sel);
            ub += allow;
            copy_C &= A[sel]; // remove the non-neighbors of sel
        }
        ret = ub + Pi_0.size();
        if (ret <= lb)
            return ret;
#ifndef NO_RR
        // RR1
        // lookahead: for u in C, if UB(S+u, C-u) <= lb, then remove u
        {
            Timer t;
            int Pi_0_size = Pi_0.size();
            int ub_Pi_I = ub - S_sz;
            for (int u : C)
            {
                int neighbor_cnt = Pi_0.intersect(A[u]);
                int non_neighbor_cnt = Pi_0_size - neighbor_cnt;
                if (Pi_0[u])
                {
                    int ub_u = S_sz + 1 + neighbor_cnt + min(non_neighbor_cnt - 1, paramK - 1 - loss_cnt[u]) + ub_Pi_I;
                    if (ub_u <= lb)
                    {
                        C.reset(u);
                        Pi_0.reset(u);
                        Pi_0_size--;
                        ret--;
                        if (ret <= lb)
                            return ret;
                    }
                    else
                    {
                        ub_u -= useful_S.intersect(non_A[u]);
                        if (ub_u <= lb)
                        {
                            C.reset(u);
                            Pi_0.reset(u);
                            Pi_0_size--;
                            ret--;
                        }
                    }
                }
                else
                {
                    int ub_u = S_sz + 1 + neighbor_cnt + min(non_neighbor_cnt, paramK - 1 - loss_cnt[u]) + ub_Pi_I - 1;
                    if (ub_u <= lb)
                    {
                        C.reset(u);
                    }
                    else
                    {
                        ub_u = ub_u + 1 - useful_S.intersect(non_A[u]);
                        if (ub_u <= lb)
                            C.reset(u);
                    }
                }
            }
            C_reduce_time += t.get_time();
        }
        // RR2
        if (ret == lb + 1 && Pi_0.size())
        {
            S |= Pi_0;
            C ^= Pi_0;
            Timer start_fast_reduce;
            bool S_is_plex, g_is_plex;
            v_just_add = *Pi_0.begin();
            fast_reduction(S, C, g_is_plex, S_is_plex);
            fast_reduce_time += start_fast_reduce.get_time();

            if (!S_is_plex)
                return lb;
            if (g_is_plex)
            {
                update_lb(S, C);
                return lb;
            }
        }
#endif
        if (C.size() < initial_C_size) // goto next iteration
            return bound_and_reduce(S, C);
        return ret;
    }
    /**
     * @brief reduction-and-bound framework, i.e., AltRB or SeqRB
     * @param edges_removed we record the edges we remove in order to rollback when backtrack
     * @return UB(S, C)
     */
    inline int get_UB(Set &S, Set &C, vector<pii> &edges_removed)
    {
        int ub = bound_and_reduce(S, C); // AltRB
        if (ub <= lb)
            return ub;
        ub = only_part_UB(S, C); // just bounding without AltRB
        return ub;
    }
    /**
     * @return the vertex with minimum degree
     */
    int select_pivot_vertex_with_min_degree(Set &C)
    {
        int sel = -1;
        for (int u : C)
        {
            if (loss_cnt[u] == paramK - 1)
                return u;
            if (sel == -1 || deg[u] < deg[sel] || deg[u] == deg[sel] && loss_cnt[sel] < loss_cnt[u])
                sel = u;
        }
        return sel;
    }
};

#endif