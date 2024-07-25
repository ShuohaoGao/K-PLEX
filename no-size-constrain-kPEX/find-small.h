#ifndef FIND_SMALL_H
#define FIND_SMALL_H

#include "Graph.h"

/**
 * @brief find maximum k-plex of size at most 2k-2
 */
class Solver_small
{
public:
    Graph g;      // lb is given
    Graph base_g; // lb=solution.size()
    set<ui> solution;
    int lb;
    int k;
    ui *d;
    ui *pstart;
    ui *edge_to;

    vector<int> S;
    vector<bool> in_S;             // u is in S <=> in_S[u]=1
    vector<int> C, pos_in_C;       // C[i]=u <=> pos_in_C[u] = i
    vector<int> deg;               // degree in G[S+C]
    vector<int> neighbor_cnt_in_S; // |N(u)\cap S|

    Solver_small(string graph_path, const set<ui> &s, int paramK) : solution(s), k(paramK)
    {
        // reload graph
        base_g.readFromFile(graph_path);
        // sort vertices according to degeneracy order
        base_g.degeneracy_and_reduce(solution.size(), &solution);
        printf("for small search: n= %u m= %u lb= %u\n", base_g.n, base_g.m, solution.size());
    }
    ~Solver_small() {}

    /**
     * @brief start to search
     */
    void start_search()
    {
        for (lb = 2 * k - 3; lb >= solution.size(); lb--)
        {
            printf("now given lb= %d ,focus on lb+1 ", lb);
            // aim: verify whether exists kplex of size lb+1
            g = base_g;
            set<ui> temp;
            // heuristic again
            g.degeneracy_and_reduce(lb, &temp);
            printf("n= %u m= %u\n", g.n, g.m);
            assert(temp.size() <= lb + 1);
            if (temp.size() != lb + 1)
                g.strong_heuris(lb, temp, 3e6); // time-limit = 3s
            assert(temp.size() <= lb + 1);
            if (temp.size() > solution.size())
            {
                solution = temp;
                if (lb < solution.size())
                    break;
            }
            if (g.n <= lb)
                continue;
            d = g.d;
            pstart = g.pstart;
            edge_to = g.edge_to;
            ui n = g.n;
            S.resize(0);
            in_S.clear();
            in_S.resize(n);
            C.resize(n);
            pos_in_C.resize(n);
            for (ui i = 0; i < n; i++)
            {
                C[i] = i;
                pos_in_C[i] = i;
            }
            deg.resize(n);
            for (ui u : C)
                deg[u] = d[u];
            neighbor_cnt_in_S.resize(n);
            for (ui u : C)
                neighbor_cnt_in_S[u] = 0;
            bnb();
        }
    }

    /**
     * @brief recursive branch-and-bound procedure: aim to find largest DPlex containing S in G[S+C]
     */
    void bnb()
    {
        if (S.size() > solution.size()) // update solution
        {
            solution.clear();
            for (ui v : S)
                solution.insert(g.map_refresh_id[v]);
            assert(solution.size() == S.size());
            printf("larger lb= %u \n", solution.size());
        }
        if (solution.size() > lb)
            return;

        vector<int> vertex_removed; // store the vertices we removed in current branch
        reduce_C(vertex_removed);
        kPlexT_reduce(vertex_removed);

        // int ub = S.size() + C.size();
        int ub = get_upper_bound();
        if (ub <= lb)
        {
            rollback(vertex_removed);
            return;
        }
        if (ub == lb + 1)
        {
            int sel = -1;
            for (ui v : S)
                if (deg[v] + k == ub)
                {
                    sel = v;
                    break;
                }
            if (sel != -1)
            {
                vector<int> temp;
                for (ui i = pstart[sel]; i < pstart[sel + 1]; i++)
                {
                    ui v = edge_to[i];
                    if (pos_in_C[v] != -1)
                    {
                        move_from_C_to_S(v);
                        temp.push_back(v);
                    }
                }
                if (temp.size())
                {
                    if (S_is_KPlex())
                        bnb();
                    for (int i = temp.size() - 1; i >= 0; i--)
                    {
                        ui v = temp[i];
                        move_from_S_to_C(v);
                    }
                    rollback(vertex_removed);
                    return;
                }
            }
        }

        int pivot = select_pivot(); // pivot in C
        assert(pivot != -1);
        int V_sz = S.size() + C.size();
        if (deg[pivot] + k >= V_sz) // check if S+C is DPlex
        {
            bool ok = 1;
            for (int v : S)
            {
                if (deg[v] + k < V_sz)
                {
                    ok = 0;
                    break;
                }
            }
            if (ok)
            {
                // update solution
                if (S.size() + C.size() > solution.size())
                {
                    solution.clear();
                    for (ui v : S)
                        solution.insert(g.map_refresh_id[v]);
                    for (ui v : C)
                        solution.insert(g.map_refresh_id[v]);
                    assert(solution.size() == S.size() + C.size());
                    printf("larger lb= %u \n", solution.size());
                }
                rollback(vertex_removed);
                return;
            }
        }

        // generate sub-branches
        // branch 1: add pivot to S
        move_from_C_to_S(pivot);
        if (S_is_KPlex())
            bnb();
        move_from_S_to_C(pivot);

        // branch 2: remove pivot from C
        remove_from_C(pivot);
        bnb();
        add_to_C(pivot);

        // add the removed vertices to C
        rollback(vertex_removed);
    }

    /**
     * @brief ub
     */
    int get_upper_bound()
    {
        int ret = S.size() + C.size();
        for (int v : S)
            ret = min(ret, deg[v] + k);
        return ret;
    }

    /**
     * @brief select a vertex with minimum degree in G[S+C]
     * @return pivot in C
     */
    int select_pivot()
    {
        assert(C.size());
        int sel = -1, min_d = INF;
        for (int u : C)
        {
            if (deg[u] < min_d)
            {
                sel = u;
                min_d = deg[u];
            }
        }
        return sel;
    }

    /**
     * @brief for the vertices removed in current branch, we need to rollback before return
     */
    void rollback(vector<int> &vertex_removed)
    {
        for (int u : vertex_removed)
            add_to_C(u);
    }

    /**
     * @brief reduce u from C if 1) S+u is not a k-plex or 2) d(u)+k<=lb
     */
    void reduce_C(vector<int> &vertex_removed)
    {
        for (int i = 0; i < C.size();)
        {
            int u = C[i];
            bool removed = 0;
            if (neighbor_cnt_in_S[u] + k < S.size() + 1)
                removed = 1;
            if (deg[u] + k <= lb)
                removed = 1;
            if (removed)
            {
                remove_from_C(u);
                vertex_removed.push_back(u);
                continue;
            }
            i++;
        }
    }

    /**
     * @return whether G[S] is a k-plex
     */
    bool S_is_KPlex()
    {
        for (ui v : S)
        {
            if (neighbor_cnt_in_S[v] + k < S.size())
                return false;
        }
        return true;
    }

    /**
     * @brief move a vertex u from S to C, and update the related array
     */
    void move_from_C_to_S(int u)
    {
        // for (int u : C)
        // {
        //     int pos = pos_in_C[u];
        //     assert(pos != -1);
        //     assert(C[pos] == u);
        // }
        int pos = pos_in_C[u];
        assert(!in_S[u] && pos != -1);
        assert(C[pos] == u);
        pos_in_C[C.back()] = pos;
        swap(C[C.size() - 1], C[pos]);
        C.pop_back();
        pos_in_C[u] = -1;
        in_S[u] = 1;
        S.push_back(u);
        for (ui i = pstart[u]; i < pstart[u + 1]; i++)
        {
            ui v = edge_to[i];
            neighbor_cnt_in_S[v]++;
        }
        // for (int u : C)
        // {
        //     int pos = pos_in_C[u];
        //     assert(pos != -1);
        //     assert(C[pos] == u);
        // }
    }

    /**
     * @brief include a vertex u from C to S, and update the related array
     */
    void move_from_S_to_C(int u)
    {
        assert(S.back() == u && in_S[u]);
        S.pop_back();
        in_S[u] = 0;
        assert(pos_in_C[u] == -1);
        pos_in_C[u] = C.size();
        C.push_back(u);
        for (ui i = pstart[u]; i < pstart[u + 1]; i++)
        {
            ui v = edge_to[i];
            neighbor_cnt_in_S[v]--;
        }
    }

    /**
     * @brief remove a vertex u from C, and update the related degree
     */
    void remove_from_C(int u)
    {
        int pos = pos_in_C[u];
        assert(pos != -1);
        assert(C[pos] == u);
        pos_in_C[C.back()] = pos;
        swap(C[C.size() - 1], C[pos]);
        C.pop_back();
        for (ui i = pstart[u]; i < pstart[u + 1]; i++)
        {
            ui v = edge_to[i];
            deg[v]--;
        }
        pos_in_C[u] = -1;
    }

    /**
     * @brief add a vertex u to C, and update the related degree
     */
    void add_to_C(int u)
    {
        assert(!in_S[u] && pos_in_C[u] == -1);
        pos_in_C[u] = C.size();
        C.push_back(u);
        deg[u] = 0;
        neighbor_cnt_in_S[u] = 0;
        for (ui i = pstart[u]; i < pstart[u + 1]; i++)
        {
            ui v = edge_to[i];
            if (in_S[v] || pos_in_C[v] != -1)
            {
                deg[u]++;
                deg[v]++;
                if (in_S[v])
                    neighbor_cnt_in_S[u]++;
            }
        }
    }

    /**
     * @brief Alg.Reduce in kPlexT
     */
    void kPlexT_reduce(vector<int> &vertex_removed)
    {
        if (S.size() <= 1)
            return;
        vector<int> S2;
        int V_sz = S.size() + C.size();
        for (int u : S)
            if (deg[u] + k < V_sz)
                S2.push_back(u);
        int sup_S2 = 0;
        for (int u : S2)
            sup_S2 += k - (S.size() - neighbor_cnt_in_S[u]);
        unordered_map<int, int> non_neighbor_cnt_in_S2;
        for (ui u : C)
        {
            int cnt = 0;
            for (ui v : S2)
                if (!g.exist_edge(u, v))
                    cnt++;
            non_neighbor_cnt_in_S2[u] = cnt;
        }
        vector<int> non_decreasing_order_C = C;
        sort(non_decreasing_order_C.begin(), non_decreasing_order_C.end(),
             [&non_neighbor_cnt_in_S2](int a, int b)
             { return non_neighbor_cnt_in_S2[a] < non_neighbor_cnt_in_S2[b]; });
        for (ui i = 1; i < C.size(); i++)
        {
            int a = non_decreasing_order_C[i - 1];
            int b = non_decreasing_order_C[i];
            assert(non_neighbor_cnt_in_S2[a] <= non_neighbor_cnt_in_S2[b]);
        }
        for (ui i = 0; i < C.size();)
        {
            ui v = C[i];
            int ub = S.size() + 1;
            int nnv = S.size() - neighbor_cnt_in_S[v];
            int sup = sup_S2 - non_neighbor_cnt_in_S2[v];
            for (ui w : non_decreasing_order_C)
            {
                if (w == v)
                    continue;
                if (non_neighbor_cnt_in_S2[w] > sup || ub > lb)
                    break;
                if (pos_in_C[w] == -1)
                    continue;
                if (!g.exist_edge(v, w))
                {
                    if (nnv >= k - 1)
                        continue;
                    nnv++;
                }
                sup -= non_neighbor_cnt_in_S2[w];
                ub++;
            }
            if (ub <= lb)
            {
                remove_from_C(v);
                vertex_removed.push_back(v);
                continue;
            }
            i++;
        }
    }
};

#endif