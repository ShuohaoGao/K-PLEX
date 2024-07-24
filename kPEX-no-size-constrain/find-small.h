#ifndef FIND_SMALL_H
#define FIND_SMALL_H

#include "Graph.h"

/**
 * @brief find maximum k-plex of size at most 2k-2
 */
class Solver_small
{
public:
    Graph g;
    set<ui> solution;
    int k;
    ui *d;
    ui *pstart;
    ui *edge_to;
    Solver_small(string graph_path, const set<ui> &s, int paramK) : solution(s), k(paramK)
    {
        // reload graph
        g.readFromFile(graph_path);
        // sort vertices according to degeneracy order
        g.degeneracy_and_reduce(solution.size(), &solution);
        printf("for small search: n= %u m= %u lb= %u\n", g.n, g.m, solution.size());
        d = g.d;
        pstart = g.pstart;
        edge_to = g.edge_to;
    }
    ~Solver_small() {}

    /**
     * @brief start to search
     */
    void start_search()
    {
        ui n = g.n, m = g.m;
        int lb = solution.size();
        vector<bool> vertex_removed(n);
        queue<ui> q;
        set<ui> C;
        for (ui i = 0; i < n; i++)
            C.insert(i);
        // divide-and-conquer: IE framework
        for (ui u = 0; u < n; u++)
        {
            // search for kplex including vertex u
            set<ui> S{u};
            C.erase(u);
            dfs(S, C);
            lb = solution.size(); // lb may be updated
            if (lb == 2 * k - 2)
                return;
            // remove u
            vertex_removed[u] = 1;
            q.push(u);
            while (q.size())
            {
                ui v = q.front();
                q.pop();
                // update v's neighbor w
                for (ui i = pstart[v]; i < pstart[v + 1]; i++)
                {
                    ui w = edge_to[i];
                    if (vertex_removed[w])
                        continue;
                    d[w]--;
                    if (d[w] + k < lb + 1)
                    {
                        vertex_removed[w] = 1;
                        q.push(w);
                        C.erase(w);
                    }
                }
            }
        }
    }

    /**
     * @brief depth-first search
     */
    void dfs(set<ui> &S, set<ui> &C)
    {
        if (solution.size() == 2 * k - 2) // we find the final solution!
            return;
        vector<ui> removed_vertics;
        // reduction
        bool pruned = fast_reduce(S, C, removed_vertics);
        if (pruned)
            return;
        // kPlexT_reduce(S, C, removed_vertics);
        if (S.size() + C.size() <= solution.size())
        {
            // prune and rollback
            for (ui u : removed_vertics)
                C.insert(u);
            return;
        }
        // branching
        {
            ui pivot = random_branch_vertex(C);
            // branch 1: include pivot
            S.insert(pivot);
            C.erase(pivot);
            if (S.size() > solution.size()) // update lb
            {
                solution.clear();
                printf("larger lb: %u\n", S.size());
                for (ui v : S)
                    solution.insert(g.map_refresh_id[v]);
            }
            dfs(S, C);
            S.erase(pivot);
            // branch 2: exclude pivot
            dfs(S, C);
            C.insert(pivot);
        }
        // rollback
        for (ui u : removed_vertics)
            C.insert(u);
    }

    /**
     * @brief using Alg.Reduce in kPlexT
     * @param removed_vertics vertices removed in this func will be pushed into this array
     */
    void kPlexT_reduce(set<ui> &S, set<ui> &C, vector<ui> &removed_vertics)
    {
        vector<ui> S2;
        ui size_V = S.size() + C.size();
        for (ui u : S)
        {
            ui neighbor_cnt = 0;
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                if (S.count(v) || C.count(v))
                    neighbor_cnt++;
            }
            if (neighbor_cnt + k < size_V)
                S2.push_back(u);
        }
        int sup_S2 = 0;
        for (ui u : S2)
        {
            int non_neighbor_in_S = 0;
            for (ui v : S)
            {
                if (v == u)
                    continue;
                if (!g.exist_edge(u, v))
                    non_neighbor_in_S++;
            }
            sup_S2 += k - 1 - non_neighbor_in_S;
        }
        // compute non_neighbor_in_S2
        unordered_map<ui, int> non_neighbor_in_S2;
        {
            unordered_map<ui, int> neighbor_in_S2;
            for (ui u : S2)
            {
                for (ui i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    ui v = edge_to[i];
                    neighbor_in_S2[v]++;
                }
            }
            for (ui u : C)
                non_neighbor_in_S2[u] = S2.size() - neighbor_in_S2[u];
        }
        // sort C
        vector<ui> C_non_decreasing_order;
        for (ui u : C)
            C_non_decreasing_order.push_back(u);
        sort(C_non_decreasing_order.begin(), C_non_decreasing_order.end(),
             [&non_neighbor_in_S2](ui a, ui b)
             { return non_neighbor_in_S2[a] < non_neighbor_in_S2[b]; });
        // compute non_neighbor_in_S
        unordered_map<ui, int> non_neighbor_in_S;
        {
            unordered_map<ui, int> neighbor_in_S;
            for (ui u : S)
            {
                for (ui i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    ui v = edge_to[i];
                    neighbor_in_S[v]++;
                }
            }
            for (ui u : C)
                non_neighbor_in_S[u] = S.size() - neighbor_in_S[u];
        }
        // reduce: lookahead
        set<ui> removed;
        for (ui v : C)
        {
            int ub = S.size() + 1;
            int nnv = non_neighbor_in_S[v];
            assert(nnv < k);
            int sup = sup_S2 - non_neighbor_in_S2[v];
            for (ui w : C_non_decreasing_order)
            {
                if (w == v)
                    continue;
                if (non_neighbor_in_S2[w] > sup || ub > solution.size())
                    break;
                if (removed.count(w))
                    continue;
                if (!g.exist_edge(v, w))
                {
                    if (nnv >= k - 1)
                        continue;
                    nnv++;
                }
                sup -= non_neighbor_in_S2[w];
                ub++;
            }
            if (ub <= solution.size())
            {
                removed.insert(v);
            }
        }
        for (ui v : removed)
        {
            C.erase(v);
            removed_vertics.push_back(v);
        }
    }

    /**
     * @brief randomly select a vertex from C
     */
    ui random_branch_vertex(set<ui> &C)
    {
        for (ui u : C)
        {
            if (rand() & 4 == 0)
                return u;
        }
        return *C.begin();
    }

    /**
     * @brief reduce vertices based on degree
     * @param removed_vertics vertices removed in this func will be pushed into this array
     * @return true if current branch can be pruned
     */
    bool fast_reduce(set<ui> &S, set<ui> &C, vector<ui> &removed_vertics)
    {
        int lb = solution.size();
        // based on core-reduce
        {
            // store degrees of vertices
            unordered_map<ui, int> deg;
            // V = S+C
            set<ui> V = C;
            V.insert(S.begin(), S.end());
            // compute degree
            for (ui u : V)
            {
                for (ui i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    ui v = edge_to[i];
                    if (v >= u) // each edge is considered once
                        break;
                    if (V.count(v))
                    {
                        deg[u]++;
                        deg[v]++;
                    }
                }
            }
            queue<ui> q;
            for (ui u : V)
            {
                if (deg[u] + k <= lb)
                {
                    q.push(u);
                }
            }
            while (q.size())
            {
                ui u = q.front();
                q.pop();
                if (!V.count(u))
                    continue;
                V.erase(u);
                for (ui i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    ui v = edge_to[i];
                    if (V.count(v))
                    {
                        deg[v]--;
                        if (deg[v] + k <= lb)
                        {
                            q.push(v);
                        }
                    }
                }
            }
            for (ui u : S)
                if (!V.count(u)) // pruned
                    return true;
            // C = C \cap V
            for (ui u : C)
            {
                if (V.count(u))
                    continue;
                q.push(u);
            }
            while (q.size())
            {
                ui u = q.front();
                q.pop();
                C.erase(u);
                removed_vertics.push_back(u);
            }
        }

        // based on hereditary
        {
            unordered_map<ui, int> neighbor_cnt_in_S;
            for (ui u : S)
            {
                for (ui i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    ui v = edge_to[i];
                    neighbor_cnt_in_S[v]++;
                }
            }
            for (ui u : S)
            {
                if (neighbor_cnt_in_S[u] + k == S.size())
                {
                    // remove non-neighbors of u
                    ui state = removed_vertics.size();
                    for (ui v : C)
                        if (!g.exist_edge(u, v))
                            removed_vertics.push_back(v);
                    for (ui i = state; i < removed_vertics.size(); i++)
                        C.erase(removed_vertics[i]);
                }
            }
            {
                ui state = removed_vertics.size();
                for (ui u : C)
                    if (neighbor_cnt_in_S[u] + k < S.size() + 1)
                        removed_vertics.push_back(u);
                for (ui i = state; i < removed_vertics.size(); i++)
                    C.erase(removed_vertics[i]);
            }
        }
        return false;
    }
};

#endif