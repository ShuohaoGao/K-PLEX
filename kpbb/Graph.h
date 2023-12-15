#ifndef GRAPH_H
#define GRAPH_H

#include "LinearHeap.h"
#include "MyBitset.h"

/**
 * used for heuristic & preprocess
 */
class Graph
{
    bool reduced; // if so, the indices of vertices is mapped so we need to use map_refresh_id

public:
    ui n, m;
    ui *d; // degree
    // edge_to[ pstart[a] ... pstart[a+1] ] is the neighbor of a
    ui *edge_to;               // size = m
    ui *pstart;                // size = n+1
    vector<ui> map_refresh_id; // we need to re-map the reduced graph to {0,1,...,n-1}, thus requiring to record the map
    Graph() : n(0), m(0), d(nullptr), edge_to(nullptr), pstart(nullptr), reduced(false)
    {
    }
    Graph(vector<int> &ids, vector<pii> &edges) : d(nullptr), edge_to(nullptr), pstart(nullptr), reduced(true)
    {
        // unique_vector(edges);
        n = ids.size();
        map_refresh_id.resize(n);
        for (int i = 0; i < n; i++)
            map_refresh_id[i] = ids[i];
        m = edges.size();
        d = new ui[n];
        edge_to = new ui[m];
        pstart = new ui[n + 1];
        ui j = 0;
        for (ui u = 0; u < n; u++)
        {
            pstart[u] = j;
            while (j < m && edges[j].x == u)
            {
                edge_to[j] = edges[j].y;
                j++;
            }
            d[u] = j - pstart[u];
        }
        pstart[n] = j;
        assert(j == m);
    }
    ~Graph()
    {
        if (d != nullptr)
            delete[] d;
        map_refresh_id.clear();
        if (edge_to != nullptr)
            delete[] edge_to;
        if (pstart != nullptr)
            delete[] pstart;
    }
    /**
     * @return whether (a, b) ∈ E
     */
    bool exist_edge(ui a, ui b)
    {
        return has(edge_to + pstart[a], edge_to + pstart[a + 1], b);
    }
    /**
     * @brief read edges from file where the file format can be ".txt" ".mtx" ".bin" ".out" or no suffix name
     *
     * as for ".mtx" or no suffix name, we need to re-map the vertex index to [0, n-1]
     * as for  ".txt" ".bin" ".out", just read
     */
    void readFromFile(string file_path)
    {
        ifstream in(file_path);
        if (!in.is_open())
        {
            printf("Failed to open %s \n", file_path.c_str());
            fflush(stdout);
            exit(1);
        }
        string suffix = get_file_name_suffix(file_path);
        if (suffix == "mtx")
        {
            string line;
            do
            {
                getline(in, line);
            } while (line[0] == '%');
            // the first line should be n n m
            {
                stringstream ss(line);
                ss >> n >> n >> m;
            }
            cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m << " k= " << paramK << endl;
            vector<pii> edges(m << 1);
            vector<int> v_map(n + 1, -1);
            int id_v = 0;
            for (ui i = 0, idx = 0; i < m; i++)
            {
                ui a, b;
                in >> a >> b;
                if (a == b)
                    continue;
                if (v_map[a] == -1)
                    v_map[a] = id_v++;
                if (v_map[b] == -1)
                    v_map[b] = id_v++;
                a = v_map[a], b = v_map[b];
                edges[idx++] = {a, b};
                edges[idx++] = {b, a};
            }
            unique_pii(edges, n);
            d = new ui[n];
            pstart = new ui[n + 1];
            m = edges.size();
            edge_to = new ui[m];
            ui last_v = 0;
            pstart[0] = 0;
            for (ui i = 0; i < m; i++)
            {
                auto &h = edges[i];
                while (h.x != last_v)
                {
                    d[last_v] = i - pstart[last_v];
                    last_v++;
                    pstart[last_v] = i;
                }
                edge_to[i] = h.y;
            }
            d[last_v] = m - pstart[last_v];
            pstart[n] = m;
            while (last_v + 1 < n)
            {
                last_v++;
                pstart[last_v] = m;
            }
        }
        else if (suffix.size() == 0)
        {
            char ch;
            string s;
            in >> ch >> s >> n >> m;
            assert(ch == 'p');
            assert(s == "edge");
            cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m << " k= " << paramK << endl;
            vector<pii> edges(m << 1);
            vector<int> v_map(n + 1, -1);
            int id_v = 0;
            for (ui i = 0, idx = 0; i < m; i++)
            {
                ui a, b;
                in >> ch >> a >> b;
                assert(ch == 'e');
                if (a == b)
                    continue;
                if (v_map[a] == -1)
                    v_map[a] = id_v++;
                if (v_map[b] == -1)
                    v_map[b] = id_v++;
                a = v_map[a], b = v_map[b];
                assert(a != b);
                assert(a < n && b < n);
                edges[idx++] = {a, b};
                edges[idx++] = {b, a};
            }
            unique_pii(edges, n);
            d = new ui[n];
            pstart = new ui[n + 1];
            m = edges.size();
            for (ui i = 0; i + 1 < m; i++)
                assert(edges[i] < edges[i + 1]);
            edge_to = new ui[m];
            ui last_v = 0;
            pstart[0] = 0;
            for (ui i = 0; i < m; i++)
            {
                auto &h = edges[i];
                while (h.x != last_v)
                {
                    assert(h.x > last_v);
                    d[last_v] = i - pstart[last_v];
                    last_v++;
                    pstart[last_v] = i;
                }
                edge_to[i] = h.y;
            }
            d[last_v] = m - pstart[last_v];
            pstart[n] = m;
            while (last_v + 1 < n)
            {
                last_v++;
                pstart[last_v] = m;
            }
        }
        else if (suffix == "bin")
        {
            FILE *in = fopen(file_path.c_str(), "rb");
            if (in == nullptr)
            {
                printf("Failed to open %s \n", file_path.c_str());
                exit(1);
            }
            ui size_int;
            fread(&size_int, sizeof(ui), 1, in);
            if (size_int != sizeof(ui))
            {
                printf("sizeof int is different: graph_file(%u), machine(%u)\n", size_int, (int)sizeof(ui));
                exit(1);
            }
            fread(&n, sizeof(ui), 1, in);
            fread(&m, sizeof(ui), 1, in);
            cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m / 2 << " k= " << paramK << endl;
            d = new ui[n];
            pstart = new ui[n + 1];
            edge_to = new ui[m];
            fread(d, sizeof(ui), n, in);
            fread(edge_to, sizeof(ui), m, in);
            pstart[0] = 0;
            for (ui i = 1; i <= n; i++)
                pstart[i] = pstart[i - 1] + d[i - 1];
        }
        else // default graph file format: n m \n edges
        {
            in >> n >> m;
            cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m << " k= " << paramK << endl;
            vector<pii> edges(m << 1);
            for (ui i = 0, idx = 0; i < m; i++)
            {
                ui a, b;
                in >> a >> b;
                assert(a != b);
                assert(a < n && b < n);
                edges[idx++] = {a, b};
                edges[idx++] = {b, a};
            }
            unique_pii(edges, n);
            d = new ui[n];
            pstart = new ui[n + 1];
            m = edges.size();
            for (ui i = 0; i + 1 < m; i++)
                assert(edges[i] < edges[i + 1]);
            edge_to = new ui[m];
            ui last_v = 0;
            pstart[0] = 0;
            for (ui i = 0; i < m; i++)
            {
                auto &h = edges[i];
                while (h.x != last_v)
                {
                    assert(h.x > last_v);
                    d[last_v] = i - pstart[last_v];
                    last_v++;
                    pstart[last_v] = i;
                }
                edge_to[i] = h.y;
            }
            d[last_v] = m - pstart[last_v];
            pstart[n] = m;
            while (last_v + 1 < n)
            {
                last_v++;
                pstart[last_v] = m;
            }
        }
        map_refresh_id.resize(n);
        for (ui i = 0; i < n; i++)
            map_refresh_id[i] = i;
        printf("Graph init ok\n");
        fflush(stdout);
    }
    /**
     * @brief stage-II: acquire degeneracy order on subgraph
     */
    int degen_on_subgraph(ui range, vector<vector<ui>> &neighbor, vector<ui> &res)
    {
        int *deg = new int[range];
        for (ui i = 0; i < range; i++)
            deg[i] = neighbor[i].size();
        LinearHeap heap(range, range, deg);
        vector<bool> rm(range);
        while (heap.get_min_key() + paramK < heap.sz)
        {
            ui u = heap.get_min_node();
            heap.delete_node(u);
            rm[u] = 1;
            for (int v : neighbor[u])
            {
                if (rm[v])
                    continue;
                heap.decrease(--deg[v], v);
            }
        }
        if (heap.sz > lb)
            for (ui i = 0; i < range; i++)
            {
                if (!rm[i])
                    res.push_back(i);
            }
        delete[] deg;
        return heap.sz;
    }
    /**
     * @brief stage-I: induce a subgraph
     */
    int degen_on_subgraph(ui u, vector<int> &deg_in_g, vector<int> &cnt, vector<bool> &vertex_removed,
                          set<ui> &solution, bool &pruned)
    {
        if (vertex_removed[u])
            return paramK;
        pruned = false;
        vector<ui> candidate; // 2-hops neighbors of u
        // get the subgraph
        // 1. get the neighbors of u, and they should form a (lb+1-2k)-core
        for (ui i = pstart[u]; i < pstart[u + 1]; i++)
        {
            ui v = edge_to[i];
            if (vertex_removed[v])
                continue;
            cnt[v] = 1;
            candidate.push_back(v);
            deg_in_g[v] = 0;
            for (ui j = pstart[v]; j < pstart[v + 1]; j++)
            {
                ui w = edge_to[j];
                if (w >= v)
                    break;
                if (cnt[w] != -1)
                {
                    deg_in_g[v]++;
                    deg_in_g[w]++;
                }
            }
        }
        // reduce N(u) to a (lb+1-2k)-core
        queue<ui> q;
        for (ui w : candidate)
            if (deg_in_g[w] + 2 * paramK < lb + 1)
            {
                q.push(w);
                cnt[w] = -1;
                deg_in_g[w] = 0;
            }
        while (q.size())
        {
            ui v = q.front();
            q.pop();
            for (ui j = pstart[v]; j < pstart[v + 1]; j++)
            {
                ui w = edge_to[j];
                if (cnt[w] != -1)
                {
                    deg_in_g[w]--;
                    if (deg_in_g[w] + 2 * paramK < lb + 1)
                    {
                        cnt[w] = -1;
                        q.push(w);
                        deg_in_g[w] = 0;
                    }
                }
            }
        }
        ui rest_cnt = 0;
        for (ui i = 0; i < candidate.size(); i++)
        {
            ui v = candidate[i];
            if (cnt[v] != -1)
            {
                candidate[rest_cnt++] = v;
            }
        }
        candidate.resize(rest_cnt);
        if (rest_cnt + paramK < lb + 1) // current subgraph can be pruned
        {
            // clear the arrays
            for (ui v : candidate)
            {
                cnt[v] = -1;
                deg_in_g[v] = 0;
            }
            pruned = true;
            return paramK;
        }
        // we then add the 2-hops neighbors
        for (ui v : candidate)
        {
            for (ui j = pstart[v]; j < pstart[v + 1]; j++)
            {
                ui w = edge_to[j];
                if (vertex_removed[w])
                    continue;
                deg_in_g[w]++;
            }
        }
        cnt [u]=1;
        for (ui i = 0; i < rest_cnt; i++)
        {
            ui v = candidate[i];
            deg_in_g[v] = 0;
            for (ui j = pstart[v]; j < pstart[v + 1]; j++)
            {
                ui w = edge_to[j];
                if (deg_in_g[w] >= lb + 3 - 2 * paramK && cnt[w] == -1)
                {
                    cnt[w] = 0;
                    candidate.push_back(w);
                }
                deg_in_g[w] = 0;
            }
        }
        if (candidate.size() + 1 <= lb)
        {
            pruned = true;
            for (ui w : candidate)
                cnt[w] = -1;
            cnt[u]=-1;
            return paramK;
        }
        candidate.push_back(u);
        ui range = candidate.size();
        for (int i = 0; i < range; i++)
            cnt[candidate[i]] = i;
        vector<vector<ui>> neighbor(range);
        // get edges
        for (ui u : candidate)
        {
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                if (cnt[v] != -1)
                {
                    neighbor[cnt[u]].push_back(cnt[v]);
                }
            }
        }
        vector<ui> plex;
        int degen_lb = degen_on_subgraph(range, neighbor, plex);
        if (degen_lb > lb)
        {
            solution.clear();
            for (int u : plex)
                solution.insert(map_refresh_id[candidate[u]]);
        }
        // clear the arrays
        for (ui v : candidate)
        {
            cnt[v] = -1;
            deg_in_g[v] = 0;
        }
        return degen_lb;
    }
    /**
     * @brief acquire a maximal plex containing u in g (g is a 2-hop induced subgraph)
     *
     * @param deg_in_S used for storing the degree of vertices in S; deg_in_S[u]=-1 <==> u is not in S
     * @param deg_in_g used for storing the degree of vertices in g
     * @param cnt used for recording the edge count between a vertex w and S; cnt[u]=-1 <==> u is not in candidate
     * @param vertex_removed some vertices are already removed and we shouldn't use these vertices
     * @param pruned whether we can prune the subgraph by estimating the upper bound
     *
     * @return lb
     */
    int extend(ui u, vector<int> &deg_in_S, vector<int> &deg_in_g, vector<int> &cnt, vector<bool> &vertex_removed,
               set<ui> &solution, bool &pruned)
    {
        if (vertex_removed[u])
            return paramK;
        pruned = false;
        vector<ui> candidate; // 2-hops neighbors of u
        // get the subgraph
        // 1. get the neighbors of u, and they should form a (lb+1-2k)-core
        for (ui i = pstart[u]; i < pstart[u + 1]; i++)
        {
            ui v = edge_to[i];
            if (vertex_removed[v])
                continue;
            cnt[v] = 1;
            candidate.push_back(v);
            deg_in_g[v] = 0;
            for (ui j = pstart[v]; j < pstart[v + 1]; j++)
            {
                ui w = edge_to[j];
                if (w >= v)
                    break;
                if (cnt[w] != -1)
                {
                    deg_in_g[v]++;
                    deg_in_g[w]++;
                }
            }
        }
        // reduce N(u) to a (lb+1-2k)-core
        queue<ui> q;
        for (ui w : candidate)
            if (deg_in_g[w] + 2 * paramK < lb + 1)
            {
                q.push(w);
                cnt[w] = -1;
                deg_in_g[w] = 0;
            }
        while (q.size())
        {
            ui v = q.front();
            q.pop();
            for (ui j = pstart[v]; j < pstart[v + 1]; j++)
            {
                ui w = edge_to[j];
                if (cnt[w] != -1)
                {
                    deg_in_g[w]--;
                    if (deg_in_g[w] + 2 * paramK < lb + 1)
                    {
                        cnt[w] = -1;
                        q.push(w);
                        deg_in_g[w] = 0;
                    }
                }
            }
        }
        ui rest_cnt = 0;
        for (ui i = 0; i < candidate.size(); i++)
        {
            ui v = candidate[i];
            if (cnt[v] != -1)
            {
                candidate[rest_cnt++] = v;
            }
        }
        candidate.resize(rest_cnt);
        if (rest_cnt + paramK < lb + 1) // current subgraph can be pruned
        {
            // clear the arrays
            for (ui v : candidate)
            {
                cnt[v] = -1;
                deg_in_g[v] = 0;
            }
            pruned = true;
            return paramK;
        }
        // we then add the 2-hops neighbors
        for (ui v : candidate)
        {
            for (ui j = pstart[v]; j < pstart[v + 1]; j++)
            {
                ui w = edge_to[j];
                if (vertex_removed[w])
                    continue;
                deg_in_g[w]++;
            }
        }
        cnt[u] = 1; // mark u so that u is in g
        for (ui i = 0; i < rest_cnt; i++)
        {
            ui v = candidate[i];
            deg_in_g[v] = 0;
            for (ui j = pstart[v]; j < pstart[v + 1]; j++)
            {
                ui w = edge_to[j];
                if (deg_in_g[w] >= lb + 3 - 2 * paramK && cnt[w] == -1)
                {
                    cnt[w] = 0;
                    candidate.push_back(w);
                }
                deg_in_g[w] = 0;
            }
        }
        if (candidate.size() + 1 <= lb)
        {
            pruned = true;
            for (ui w : candidate)
                cnt[w] = -1;
            cnt[u]=-1;
            return paramK;
        }
        vector<ui> plex{u};
        deg_in_S[u] = 0;
        // compute deg_in_g[]
        for (ui u : candidate)
        {
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                if (cnt[v] != -1)
                {
                    deg_in_g[u]++;
                }
            }
        }
        cnt[u] = -1;
        // start greedy extending
        ui candidate_size = candidate.size();
        while (candidate_size > 0)
        {
            ui sel_idx = 0, sel_v = candidate[0];
            for (ui i = 1; i < candidate_size; i++)
            {
                ui v = candidate[i];
                if (cnt[v] > cnt[sel_v] || (cnt[v] == cnt[sel_v] && deg_in_g[v] > deg_in_g[sel_v]))
                {
                    sel_idx = i;
                    sel_v = v;
                }
            }
            if (cnt[sel_v] + paramK < plex.size() + 1) // all the rest vertices can not be inserted into plex
            {
                // clear the arrays
                for (ui i = 0; i < candidate_size; i++)
                {
                    ui v = candidate[i];
                    cnt[v] = -1;
                    deg_in_g[v] = 0;
                }
                break;
            }
            swap(candidate[sel_idx], candidate[candidate_size - 1]);
            candidate_size--;
            bool ok = 1;
            // we already make sure :for u in candidate, S+u is plex
            deg_in_S[sel_v] = cnt[sel_v];
            for (ui i = pstart[sel_v]; i < pstart[sel_v + 1]; i++)
            {
                ui v = edge_to[i];
                if (deg_in_S[v] != -1) // v in S
                {
                    deg_in_S[v]++;
                }
                else if (cnt[v] != -1) // v in candidate set
                {
                    cnt[v]++;
                }
            }
            // hereditary proporty based reduction: if S+w is not a plex, then remove w
            for (ui w : plex)
            {
                // including u causes that w can not allow any non-neighbor
                if (deg_in_S[w] + paramK == plex.size() + 1 && !has(edge_to + pstart[sel_v], edge_to + pstart[sel_v + 1], w))
                {
                    // remove all non-neighbors of w from candidate
                    for (ui i = 0; i < candidate_size; i++)
                    {
                        ui v = candidate[i];
                        if (!has(edge_to + pstart[w], edge_to + pstart[w + 1], v))
                        {
                            swap(candidate[i], candidate[candidate_size - 1]);
                            candidate_size--;
                            i--;
                            cnt[v] = -1;
                            deg_in_g[v] = 0;
                        }
                    }
                }
            }
            plex.push_back(sel_v);
            if (deg_in_S[sel_v] + paramK == plex.size()) // sel_v can not allow any non-neighbor
            {
                ui w = sel_v;
                // remove all non-neighbors of w from candidate
                for (int i = 0; i < candidate_size; i++)
                {
                    ui v = candidate[i];
                    if (!has(edge_to + pstart[w], edge_to + pstart[w + 1], v))
                    {
                        swap(candidate[i], candidate[candidate_size - 1]);
                        candidate_size--;
                        i--;
                        cnt[v] = -1;
                        deg_in_g[v] = 0;
                    }
                }
            }
            // remove sel_v from candidate set
            cnt[sel_v] = -1;
            deg_in_g[sel_v] = 0;
        }
        // record the max plex
        if (plex.size() > solution.size())
        {
            solution.clear();
            for (ui v : plex)
                solution.insert(map_refresh_id[v]);
        }
        // clear the array deg_in_S[]
        for (ui u : plex)
            deg_in_S[u] = -1;
        return plex.size();
    }
    /**
     * @brief StrongHeuris
     *
     * @return lb
     */
    int strong_heuris(int lb, set<ui> &solution, double time_limit)
    {
        Timer t;
        int ret = 2 * paramK - 2;
        ret = max(ret, lb);
        ui *seq = new ui[n];
        sort_by_degree(seq);
        // the following arrays are shared for each extending procedure and we need to clear them each time
        vector<int> deg_in_S(n, -1);    // if deg_in_S[u]=-1, then u is not in S
        vector<int> deg_in_g(n, 0);     // g is subgraph induced by the 2-hop-neighbors of u
        vector<int> cnt(n, -1);         // cnt[v] = the edge count between S and v; if cnt[v]=-1, then v is not in candidate set
        vector<bool> vertex_removed(n); // we may infer a vertex u can be removed if UB( u, N_G^{\leq 2}(u) ) <= lb
        // for (ll u = n - 1, enumerate_num = 1; u >= 0; u--, enumerate_num++)
        for (ll i = 0; i < n; i++)
        {
            ll enumerate_num = i + 1;
            ui u = seq[i];
            bool pruned;
            int extend_lb = extend(u, deg_in_S, deg_in_g, cnt, vertex_removed, solution, pruned);
            // int extend_lb = degen_on_subgraph(u, deg_in_g, cnt, vertex_removed, solution, pruned);
            if (extend_lb > ret)
            {
                ret = extend_lb;
                printf("StrongHeuris find a larger plex: %d , already extend %d times\n", extend_lb, enumerate_num);
                break;
            }
            if (pruned)
            {
                vertex_removed[u] = 1;
            }
            // if (t.get_time() > time_limit)
            // {
            //     printf("StrongHeuris is cut off due to time limit, already extend %d times\n", enumerate_num);
            //     break;
            // }
        }
        remove_v(vertex_removed, ret);
        delete[] seq;
        return ret;
    }
    /**
     * @brief a[] = {0,1,2,...,n-1}, we need sort a[] by degree; counting sort O(n) is faster than std::sort O(nlogn)
     *
     * @param descending if so , d[a[0]] >= d[a[1]] >= ... >= d[a[n-1]]; else, d[a[0]] <= d[a[1]] <= ... <= d[a[n-1]]
     */
    void sort_by_degree(ui a[], bool descending = true)
    {
        vector<int> cnt(n, 0);
        if (descending) // key=n-1-deg[i], deg[i]↑, key↓, so the larger degree, the smaller index
        {
            for (ui i = 0; i < n; i++)
                cnt[n - 1 - d[i]]++;
            for (ui i = 1; i < n; i++)
                cnt[i] += cnt[i - 1];
            for (ui i = 0; i < n; i++)
            {
                ui key = n - 1 - d[i];
                a[cnt[key] - 1] = i;
                cnt[key]--;
            }
        }
        else // key=deg[i], deg[i]↑, key↑, so the larger degree, the larger index
        {
            for (ui i = 0; i < n; i++)
                cnt[d[i]]++;
            for (ui i = 1; i < n; i++)
                cnt[i] += cnt[i - 1];
            for (ui i = 0; i < n; i++)
            {
                a[cnt[d[i]] - 1] = i;
                cnt[d[i]]--;
            }
        }
    }
    /**
     * @brief select vertices that must occur in maximum k-plex, and store them in s
     *
     * @param rm rm[u]=1 <==> u in s
     */
    void get_v_must_include(set<ui> &s, vector<bool> &rm)
    {
        set<ui> satisfied;
        for (ui i = 0; i < n; i++)
            if (d[i] + paramK >= n)
                satisfied.insert(i);
        for (ui u : satisfied)
        {
            if (d[u] + 1 == n)
            {
                s.insert(map_refresh_id[u]);
                rm[u] = 1;
                continue;
            }
            ui cnt = 0; // the number of satisfied vertices that are non-neighbor of u
            for (ui v : satisfied)
            {
                if (v != u && !exist_edge(u, v))
                    cnt++;
            }
            if (d[u] + cnt + 1 == n)
            {
                // all non-neighbors of u are in the set 'satisfied' => u must be included
                s.insert(map_refresh_id[u]);
                rm[u] = 1;
            }
        }
    }
    /**
     * @brief remove the vertices that must include, and update the degree of rest vertices
     *
     * @param rm mask of vertices that need to remove
     */
    void remove_v(vector<bool> &rm, int lb)
    {
        ui *q = new ui[n + 1];
        ui hh = 1, tt = 0;
        for (ui i = 0; i < n; i++)
            if (rm[i])
                q[++tt] = i;
        if (tt == 0)
        {
            delete[] q;
            return;
        }
        while (hh <= tt)
        {
            ui u = q[hh++];
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                if (rm[v])
                    continue;
                if (--d[v] + paramK <= lb) // actually, none of the rest vertices can be removed by weak reduce
                    q[++tt] = v, rm[v] = 1;
            }
        }
        ui new_n = 0;
        vector<ui> new_map(n);
        for (ui i = 0; i < n; i++)
            if (!rm[i])
            {
                new_map[new_n] = map_refresh_id[i];
                q[i] = new_n++;
            }
        new_map.resize(new_n);
        map_refresh_id = new_map;
        ui *new_pstart = new ui[new_n + 1];
        ui *new_d = new ui[new_n];
        ui j = 0;
        for (ui i = 0; i < n; i++)
        {
            if (rm[i])
                continue;
            ui u = q[i];
            new_pstart[u] = j;
            for (ui p = pstart[i]; p < pstart[i + 1]; p++)
            {
                ui v = edge_to[p];
                if (!rm[v])
                    edge_to[j++] = q[edge_to[p]];
            }
            new_d[u] = j - new_pstart[u];
        }
        new_pstart[new_n] = j;
        delete[] d;
        delete[] pstart;
        d = new_d;
        pstart = new_pstart;
        if (j * 2 < m)
        {
            ui *new_edge_to = new ui[j];
            memcpy(new_edge_to, edge_to, sizeof(ui) * j);
            delete[] edge_to;
            edge_to = new_edge_to;
        }
        m = j;
        n = new_n;
        delete[] q;
    }
    /**
     * @brief stage-III: acquire a heuristic solution in the sqrt graph
     *
     * @param range the number of vertices in the subgraph where each vertex's id is 0~range-1
     * @param neighbor neighbor[u] is the array of u's neighbors
     * @param d d[u] = neighbor[u].size()
     * @param id id[u] is the true index of u in the input graph, i.e., id[u]∈[0, n-1]
     * @param solution the solution set of heuristic plex
     *
     * @return lb
     */
    int sqrt_degeneracy(ui range, vector<vector<ui>> &neighbor, vector<ui> &d, vector<ui> &id, set<ui> *solution = nullptr)
    {
        LinearHeap heap(range, range, d);
        vector<bool> rm(range, 0);
        while (heap.get_min_key() + paramK < heap.sz)
        {
            ui sel = heap.get_min_node();
            heap.delete_node(sel);
            rm[sel] = 1;
            for (ui v : neighbor[sel])
            {
                if (rm[v])
                    continue;
                heap.decrease(--d[v], v);
            }
        }
        int rest = heap.sz;
        if (solution != nullptr && solution->size() < rest)
        {
            solution->clear();
            if (reduced)
            {
                for (ui i = 0; i < range; i++)
                    if (!rm[i])
                        solution->insert(map_refresh_id[id[i]]);
            }
            else
            {
                for (ui i = 0; i < range; i++)
                    if (!rm[i])
                        solution->insert(id[i]);
            }
        }
        return rest;
    }
    /**
     * @brief stage-II: induce a subgraph with sqrt(m) vertices
     *
     * @param start_u the start vertex when we bfs
     * @param solution the solution set of heuristic plex
     *
     * @return lb
     */
    int sqrt_degeneracy(ui start_u, set<ui> *solution = nullptr)
    {
        ui range = sqrt(n);
        vector<ui> id(range, 0);
        vector<ui> vis(n, 0); // vis[u]!=0 <==> u in subgraph and id[vis[u]-1]=u
        ui cnt = 0;
        vis[start_u] = 1 + cnt;
        id[cnt++] = start_u;
        // bfs
        queue<ui> q;
        q.push(start_u);
        // {
        //     int max_neighbor = range>>4;
        //     auto nei=neighbor(start_u);
        //     sort(nei.begin(), nei.end(), [this](int a,int b){return d[a]>d[b];});
        //     for(int v:nei)
        //     {
        //         if (!vis[v])
        //         {
        //             q.push(v);
        //             vis[v] = 1 + cnt;
        //             id[cnt++] = v;
        //             if (cnt == range || --max_neighbor == 0)
        //                 break;
        //         }
        //     }
        // }
        while (q.size() && cnt < range) // 对于已加入的u，从u邻居的sqrt(n)个邻居中选出最好的3个
        {
            ui u = q.front();
            q.pop();
            int max_neighbor = 3;
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                if (!vis[v])
                {
                    q.push(v);
                    vis[v] = 1 + cnt;
                    id[cnt++] = v;
                    if (cnt == range || --max_neighbor == 0)
                        break;
                }
            }
        }
        for (ui i = 0; i < n && cnt < range; i++) // if bfs didn't find range vertices, we fill the rest
            if (!vis[i])
            {
                vis[i] = 1 + cnt;
                id[cnt++] = i;
            }
        // induce a subgraph with vertices in id[]
        vector<vector<ui>> neighbor(range);
        vector<ui> d(range, 0);
        for (ui u = 0; u < range; u++)
        {
            for (ui i = pstart[id[u]]; i < pstart[id[u] + 1]; i++)
            {
                ui v = edge_to[i];
                if (!vis[v])
                    continue;
                neighbor[u].push_back(vis[v] - 1);
            }
            d[u] = neighbor[u].size();
        }
        return sqrt_degeneracy(range, neighbor, d, id, solution);
    }
    /**
     * @brief stage-I: select a vertex (with max degree) as start vertex
     *
     * @param solution the solution set of heuristic plex
     * @param cnt the number of subgraphs
     *
     * @return lb
     */
    int sqrt_degeneracy(set<ui> *solution = nullptr, ui cnt = 10)
    {
        set<pii> s;
        for (ui i = 0; i < n; i++)
        {
            if (s.size() < cnt)
                s.insert({d[i], i});
            else if (d[i] > s.begin()->x)
            {
                s.erase(s.begin());
                s.insert({d[i], i});
            }
        }
        int ret = paramK;
        for (auto &h : s)
        {
            ret = max(ret, sqrt_degeneracy(h.y, solution));
            // int u = h.y;
            // ll id_v = pstart[u];
            // id_v = (id_v + pstart[u + 1]) / 2;
            // ret = max(ret, sqrt_degeneracy(u, edge_to[id_v], ret - paramK, solution));
        }
        ret = max(ret, first_sqrt_vertices_degeneracy(solution));
        return ret;
    }
    /**
     * @brief bfs  from (u,v)
     */
    int sqrt_degeneracy(ui u, ui v, ui deg_threshold, set<ui> *solution = nullptr)
    {
        ui range = sqrt(n);
        int max_cnt = range - 2;
        vector<int> vis(n, 0);
        vis[u] = vis[v] = 2;
        queue<ui> q;
        q.push(u);
        q.push(v);
        while (q.size() && max_cnt > 0)
        {
            ui w = q.front();
            q.pop();
            for (ui i = pstart[w]; i < pstart[w + 1]; i++)
            {
                ui y = edge_to[i];
                if (deg_threshold > d[y])
                    continue;
                if (++vis[y] == 2)
                {
                    q.push(y);
                    if (--max_cnt == 0)
                        break;
                }
            }
        }
        range -= max_cnt;
        vector<ui> id(range);
        for (ui i = 0, j = 0; i < n; i++)
            if (vis[i] >= 2)
            {
                vis[i] = j;
                id[j++] = i;
            }
            else
                vis[i] = 0;
        vector<vector<ui>> neighbor(range);
        vector<ui> d(range, 0);
        for (ui u = 0; u < range; u++)
        {
            for (ui i = pstart[id[u]]; i < pstart[id[u] + 1]; i++)
            {
                ui v = edge_to[i];
                if (!vis[v])
                    continue;
                neighbor[u].push_back(vis[v]);
            }
            d[u] = neighbor[u].size();
        }
        return sqrt_degeneracy(range, neighbor, d, id, solution);
    }
    /**
     * @brief stage-IV: degeneracy of G[{0,1,...,sqrt(n)}]
     *
     * @param solution the solution set of heuristic plex
     *
     * @return lb
     */
    int first_sqrt_vertices_degeneracy(set<ui> *solution = nullptr)
    {
        ui range = sqrt(m) + 1;
        vector<vector<ui>> neighbor(range);
        vector<ui> d(range, 0), id(range, 0);
        for (ui u = 0; u < range; u++)
        {
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                if (v >= range)
                    break;
                neighbor[u].push_back(v);
            }
            d[u] = neighbor[u].size();
            id[u] = u;
        }
        return sqrt_degeneracy(range, neighbor, d, id, solution);
    }
    /**
     * @brief for u in G, is d[u]+k<=lb, remove u
     *
     * T(n)=O(n+m)
     * after reduction, the graph is re-built so that each vertex's id ∈[0, n-1], the map is stored in map_refresh_id
     */
    void weak_reduce(int lb)
    {
        ui *q = new ui[n + 1]; // queue
        vector<bool> rm(n, 0); // rm[u]=1 <=> u is removed
        ui hh = 1, tt = 0;     // used for queue
        for (ui i = 0; i < n; i++)
            if (d[i] + paramK <= lb)
                q[++tt] = i, rm[i] = 1;
        if (tt == 0) // q is empty
        {
            if (!reduced)
            {
                reduced = 1;
                for (ui i = 0; i < n; i++)
                    map_refresh_id[i] = i;
            }
            delete[] q;
            return;
        }
        while (hh <= tt)
        {
            ui u = q[hh++];
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                if (rm[v])
                    continue;
                if (--d[v] + paramK <= lb)
                    q[++tt] = v, rm[v] = 1;
            }
        }
        // re-build the graph : re-map the id of the rest vertices; the array q[] is recycled to save the map
        ui new_n = 0;
        vector<ui> new_map(n);
        for (ui i = 0; i < n; i++)
            if (!rm[i])
            {
                new_map[new_n] = map_refresh_id[i];
                q[i] = new_n++;
            }
        new_map.resize(new_n);
        map_refresh_id = new_map;
        ui *new_pstart = new ui[new_n + 1];
        ui *new_d = new ui[new_n];
        ui j = 0; // we don't need extra memory to store new-edge_to, just re-use edge_to[]
        for (ui i = 0; i < n; i++)
        {
            if (rm[i])
                continue;
            ui u = q[i];
            new_pstart[u] = j;
            for (ui p = pstart[i]; p < pstart[i + 1]; p++)
            {
                ui v = edge_to[p];
                if (!rm[v])
                    edge_to[j++] = q[edge_to[p]];
            }
            new_d[u] = j - new_pstart[u];
        }
        new_pstart[new_n] = j;
        delete[] d;
        delete[] pstart;
        d = new_d;
        pstart = new_pstart;
        // you can ignore the following
        if (j * 2 < m)
        {
            ui *new_edge_to = new ui[j];
            memcpy(new_edge_to, edge_to, sizeof(ui) * j);
            delete[] edge_to;
            edge_to = new_edge_to;
        }
        m = j;
        n = new_n;
        delete[] q;
    }
    /**
     * @brief degenaracy order to get lb, i.e., each time we remove the vertex with min degree;
     * then we reduce the graph to (lb+1-k)-core
     *
     * @param solution if not NULL, the heuristic result should be stored
     *
     * @return lb
     *
     * T(n)=O(n+m) [actually, the code is O(n+mlogn)]
     */
    int degeneracy_and_reduce(int lb, set<ui> *solution = nullptr)
    {
        vector<bool> rm(n, 0); // rm[u]=1 <==> u is peeled and removed
        // weak reduce
        if (lb > paramK)
        {
            queue<ui> q;
            for (ui i = 0; i < n; i++)
                if (d[i] + paramK <= lb)
                {
                    q.push(i);
                    rm[i] = 1;
                }
            while (q.size())
            {
                ui u = q.front();
                q.pop();
                for (ui i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    ui v = edge_to[i];
                    if (!rm[v])
                    {
                        if (--d[v] + paramK <= lb)
                        {
                            q.push(v);
                            rm[v] = 1;
                        }
                    }
                }
            }
        }
        vector<ui> core(n, 0);
        vector<ui> seq(n, n); // the reverse order of degeneracy order, i.e., v_0 is seq[n-1] while v_{n-1} is seq[0]
        vector<ui> plex;
        ui rest_v_cnt = 0;
        // compute degeneracy order
        {
            ui *pd = d; // this may destroy d[]; however, d[] is useless for us now
            LinearHeap heap(n, n);
            for (ui i = 0; i < n; i++)
                if (!rm[i])
                {
                    heap.insert(pd[i], i);
                }
            if (!heap.sz)
            {
                n = m = 0;
                return lb;
            }
            rest_v_cnt = heap.sz;
            ui max_core = 0;
            // each time we remove the vertex with min degree
            while (heap.get_min_key() + paramK < heap.sz)
            {
                ui u = heap.get_min_node();
                max_core = max(max_core, pd[u]);
                core[u] = max_core;
                heap.delete_node(u);
                seq[heap.sz] = u;
                rm[u] = 1;
                // update the degrees of the rest vertices
                for (ui i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    ui v = edge_to[i];
                    if (!rm[v])
                    {
                        heap.decrease(--pd[v], v);
                    }
                }
            }
            int rest = heap.sz;
            while (heap.sz)
            {
                ui u = heap.get_min_node();
                heap.delete_node(u);
                seq[heap.sz] = u;
                plex.push_back(u);
                max_core = max(max_core, pd[u]);
                core[u] = max_core;
            }
            lb = max(lb, rest);
        }
        // rebuild graph: following degeneracy order
        {
            ui new_n = rest_v_cnt;
            // first order reduction can be done with the information of core[]
            for (ui i = 0; i < rest_v_cnt; i++) // compute the number of rest vertices
            {
                ui u = seq[i];
                if (core[seq[i]] + paramK <= lb)
                {
                    new_n = i;
                    break;
                }
            }
            seq.resize(new_n);
            reverse(seq.begin(), seq.end()); // now seq[] is degeneracy order, v_i is seq[i]
            vector<ui> new_map(new_n);
            vector<ui> q(n, n); // q[seq[u]]=u
            ui most_edge_cnt = 0;
            for (ui i = 0; i < new_n; i++) // store the map of indices of vertices
            {
                ui u = seq[i];
                new_map[i] = map_refresh_id[u];
                q[u] = i;
                ui deg = pstart[u + 1] - pstart[u];
                most_edge_cnt += deg;
            }
            map_refresh_id = new_map;
            ui *new_edge_to = new ui[most_edge_cnt];
            ui *new_pstart = new ui[new_n + 1];
            ui *new_d = new ui[new_n];
            ui new_m = 0;
            // rebuild graph
            for (ui u = 0; u < new_n; u++)
            {
                new_pstart[u] = new_m;
                ui pre_u = seq[u];
                for (ui i = pstart[pre_u]; i < pstart[pre_u + 1]; i++)
                {
                    ui v = edge_to[i];
                    if (q[v] >= n)
                        continue;
                    new_edge_to[new_m++] = q[v];
                }
                new_d[u] = new_m - new_pstart[u];
                // this cause T(n)=O(mlogn), we can use countingSort to improve the complexity, but we choose not
                sort(new_edge_to + new_pstart[u], new_edge_to + new_m);
            }
            new_pstart[new_n] = new_m;
            delete[] pstart;
            delete[] d;
            delete[] edge_to;
            d = new_d;
            pstart = new_pstart;
            edge_to = new_edge_to;
            m = new_m;
            n = new_n;
        }
        // store k-plex
        if (solution != nullptr && solution->size() < plex.size())
        {
            solution->clear();
            if (reduced)
            {
                for (ui i : plex)
                    solution->insert(map_refresh_id[i]);
            }
            else
            {
                for (ui i : plex)
                    solution->insert(i);
            }
        }
        return plex.size();
    }
    /**
     * @brief dump the reduced graph to file; abandoned now
     */
    void dump_to_file(string path, set<ui> *must_contain = nullptr)
    {
        FILE *out = fopen(path.c_str(), "w");
        if (out == nullptr)
        {
            printf("File open failed: %s\n", path.c_str());
            exit(1);
        }
        if (!n)
            m = 0;
        fprintf(out, "%u %u %d\n", n, m, lb);
        for (ui i = 0; i < n; i++)
        {
            for (ui j = pstart[i]; j < pstart[i + 1]; j++)
            {
                ui v = edge_to[j];
                fprintf(out, "%u %u\n", i, v);
            }
        }
        fprintf(out, "\n");
        // dump the map
        for (ui i = 0; i < n; i++)
            fprintf(out, "%u\n", map_refresh_id[i]);

        fprintf(out, "\n\n%d\n", must_contain == nullptr ? 0 : (int)must_contain->size());
        if (must_contain != nullptr)
            for (ui v : (*must_contain))
                fprintf(out, "%u\n", v);
        fclose(out);
    }
};

/**
 * base class : can be implemented using adj-matrix or adj-list
 * @brief this is a base class representing the reduced graph which serves for IE (or DC)
 */
class Graph_reduced
{
public:
    LinearHeap heap; // used for degeneracy & IE
    MyBitset vertex;
    ll n, m;
    vector<int> vertex_id; // for u in this, vertex_id[u] in G_input
    vector<int> must_contain;
    unordered_map<ll, int> triangles_nn; // the key of edge (u,v) is u*n+v , make sure u<v
    int *d;                              // degree
    AdjacentMatrix A;
    // shared memory for CTCP
    vector<bool> bool_array_n_n, bool_array_m, bool_array_n;
    int *pstart, *edge_to, *triangles_m;
    vector<bool> edge_removed;
    Graph_reduced() : d(nullptr), pstart(nullptr), edge_to(nullptr), triangles_m(nullptr)
    {
    }
    /**
     * @return edge number
     */
    int get_m()
    {
        int ret;
        for (int v : vertex)
        {
            ret += d[v];
        }
        return ret >> 1;
    }
    /**
     * @brief prepare for IE & degeneracy
     */
    void init_heap()
    {
        heap = LinearHeap(n, n, d);
    }
    /**
     * @brief compute the number of triangles for each edge
     */
    virtual void init_triangles()
    {
    }
    /**
     * @brief prepare some thing
     */
    virtual void init_before_IE()
    {
    }
    /**
     * @return the vertex with min degree
     */
    int get_min_degree_v()
    {
        int ret = heap.get_min_node();
        return ret;
    }
    /**
     * @brief after searching subgraph g_i, we exclude v_i and update the number of triangles of edges related to v_i
     * @param v the vertex we need to remove, i.e., v_i
     * @param lb_changed if so, we need to check each edge whether it can be reduced
     */
    void remove_v(int v, int lb, bool lb_changed)
    {
        CTCP(lb, v);
        if (lb_changed)
        {
            CTCP(lb);
        }
    }
    /**
     * @brief inspired by Lijun Chang
     * @param v if v==-1, called for lb increment; else, remove v
     */
    virtual void CTCP(int lb, int v = -1)
    {
    }
    /**
     * given a vertex v, induce the 2-hop neighbor of v
     * @param vis stores the 2-hop neighbor of v
     * @param vertices stores the 2-hop neighbor of v
     */
    virtual void induce_to_2hop(int v, MyBitset &vis, vector<int> &vertices)
    {
    }
    /**
     * given a vertex v, induce the 2-hop neighbor of v; In addition, we will reduce it(according to Chang)
     * @param vis stores the 2-hop neighbor of v
     * @param vertices stores the 2-hop neighbor of v
     * @param deg used for storing the degree of the subgraph, which should be all 0;
     *              also it can store the neighbor count; don't forget to clear it
     *
     * @return if this subgraph can be pruned, then we set vis[v]=0, i.e., vis.size()=0
     */
    virtual void induce_to_2hop_and_reduce(int v, MyBitset &vis, vector<int> &vertices, vector<int> &deg, int lb)
    {
    }
    /**
     * @brief obtain the ground-truth: the maximum k-plex
     * @param s the max plex we found (without considering the vertices that must include)
     *          so we need to add must_contain to s
     * @param trans_id whether we need to transform the index
     */
    void get_ground_truth(set<int> &s, bool trans_id)
    {
        if (trans_id)
        {
            set<int> temp;
            for (int v : s)
                temp.insert(vertex_id[v]);
            s = temp;
        }
        s.insert(must_contain.begin(), must_contain.end());
    }
    /**
     * @return the number of vertices now
     */
    int size()
    {
        return vertex.size();
    }
    /**
     * @return whether the graph is stored as matrix
     */
    bool is_matrix()
    {
        return edge_to == nullptr;
    }
};

/**
 * used for IE; maybe not dense enough for using adjacent matrix
 * when g_i is searched and we exclude v_i, we need to invoke CTCP
 * note that this class can be merged with Graph; but we choose not to merge just for module
 */
class Graph_reduced_adjacent_list : public Graph_reduced
{
public:
    Graph_reduced_adjacent_list() : Graph_reduced() {}
    /**
     * init graph after stage-I(preprocessing)
     * @param g reduced graph
     * @param must the vertex set that must include because each vertex in it will occur in a maximum k-plex
     */
    Graph_reduced_adjacent_list(Graph &g, set<ui> &must) : Graph_reduced()
    {
        n = g.n;
        m = g.m;
        edge_removed.resize(m);
        printf("reduced graph n= %d m= %d lb= %d\n", n, m / 2, lb);
        pstart = new int[n + 1];
        edge_to = new int[m];
        d = new int[n];
        for (ui i = 0; i < n; i++)
        {
            pstart[i] = g.pstart[i];
            d[i] = g.pstart[i + 1] - g.pstart[i];
            for (ui j = g.pstart[i]; j < g.pstart[i + 1]; j++)
            {
                edge_to[j] = g.edge_to[j];
            }
        }
        pstart[n] = g.pstart[n];
        vertex_id.resize(n);
        for (int i = 0; i < n; i++)
        {
            vertex_id[i] = g.map_refresh_id[i];
        }
        for (int u : must)
            must_contain.push_back(u);
        vertex = MyBitset(n);
        vertex.flip();
        // printf("Graph for bnb init ok\n");
        fflush(stdout);
    }
    ~Graph_reduced_adjacent_list()
    {
        if (d != nullptr)
        {
            delete[] d;
            d = nullptr;
        }
        if (edge_to != nullptr)
        {
            delete[] edge_to;
            edge_to = nullptr;
        }
        if (pstart != nullptr)
        {
            delete[] pstart;
            pstart = nullptr;
        }
        if (triangles_m != nullptr)
        {
            delete[] triangles_m;
            triangles_m = nullptr;
        }
    }
    /**
     * @brief compute the number of triangles for each edge
     */
    void init_triangles()
    {
        triangles_m = new int[m];
        MyBitset mask(n);
        for (int u = 0; u < n; u++)
        {
            for (int i = pstart[u]; i < pstart[u + 1]; i++)
                mask.set(edge_to[i]);
            for (int i = pstart[u]; i < pstart[u + 1]; i++)
            {
                int v = edge_to[i];
                if (d[v] > d[u])
                    continue;
                int cnt = 0;
                for (int j = pstart[v]; j < pstart[v + 1]; j++)
                {
                    int w = edge_to[j];
                    if (mask[w]) // w is common neighbor of u,v
                        cnt++;
                }
                triangles_m[i] = cnt;
            }
            for (int i = pstart[u]; i < pstart[u + 1]; i++)
                mask.reset(edge_to[i]);
        }
        for (int u = 0; u < n; u++)
        {
            for (int i = pstart[u]; i < pstart[u + 1]; i++)
            {
                int v = edge_to[i];
                if (d[v] <= d[u])
                    continue;
                triangles_m[i] = triangles_m[find(edge_to + pstart[v], edge_to + pstart[v + 1], u) + pstart[v]];
            }
        }
    }
    /**
     * @brief prepare some thing
     */
    void init_before_IE()
    {
        init_triangles();
        init_heap();
        bool_array_n.resize(n);
        bool_array_m.resize(m);
    }
    /**
     * @brief inspired by Lijun Chang
     * @param v if v==-1, called for lb increment; else, remove v
     */
    void CTCP(int lb, int v = -1)
    {
        queue<pii> q_edges; // an edge is stored as (edge_id, from)
        queue<int> q_vertex;
        vector<bool> &in_queue_e = bool_array_m; //(u,v) is already pushed into queue if in_queue_e[edge_id]=1
        vector<bool> &in_queue_v = bool_array_n; // a vertex u is already pushed into queue if in_queue_v[u]=1
        // CTCP is called because lb updated
        if (v == -1)
        {
            for (int u : vertex)
            {
                for (int i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    if (edge_removed[i])
                        continue;
                    int v = edge_to[i];
                    if (!vertex[v])
                        continue;
                    if (triangles_m[i] + paramK * 2 <= lb)
                    {
                        if (u < v)
                            q_edges.push({i, u});
                        in_queue_e[i] = true;
                    }
                }
                if (d[u] + paramK <= lb)
                {
                    q_vertex.push(u);
                    in_queue_v[u] = 1;
                }
            }
        }
        else // CTCP is called because IE removed a vertex
        {
            q_vertex.push(v);
            in_queue_v[v] = 1;
        }
        while (q_edges.size() || q_vertex.size())
        {
            while (q_edges.size())
            {
                auto edge = q_edges.front();
                q_edges.pop();
                int edge_id = edge.x, u = edge.y, v = edge_to[edge_id];
                edge_removed[edge_id] = 1;
                int another_edge_id = find(edge_to + pstart[v], edge_to + pstart[v + 1], u) + pstart[v];
                edge_removed[another_edge_id] = 1;
                if (--d[u] + paramK <= lb && !in_queue_v[u])
                {
                    q_vertex.push(u);
                    in_queue_v[u] = 1;
                }
                heap.decrease(d[u], u);
                if (--d[v] + paramK <= lb && !in_queue_v[v])
                {
                    q_vertex.push(v);
                    in_queue_v[v] = 1;
                }
                heap.decrease(d[v], v);
                int *a = edge_to + pstart[u], *b = edge_to + pstart[u + 1];
                int *l = edge_to + pstart[v], *r = edge_to + pstart[v + 1];
                // update the info of other edge triangles
                while (a < b && l < r)
                {
                    // note that we need to consider a situation where an edge is in the q_edges but not removed yet
                    if (edge_removed[a - edge_to] || in_queue_v[*a])
                    {
                        a++;
                        continue;
                    }
                    if (edge_removed[l - edge_to] || in_queue_v[*l])
                    {
                        l++;
                        continue;
                    }
                    if (*a == *l)
                    {
                        int w = *a; // w is the common neighbor of u,v
                        int id_uw = a - edge_to;
                        int id_wu = find(edge_to + pstart[w], edge_to + pstart[w + 1], u) + pstart[w];
                        assert(!edge_removed[id_wu]);
                        if (!in_queue_e[id_uw])
                        {
                            assert(!in_queue_e[id_wu]);
                            --triangles_m[id_uw];
                            --triangles_m[id_wu];
                            if (triangles_m[id_uw] + paramK * 2 <= lb)
                            {
                                in_queue_e[id_uw] = in_queue_e[id_wu] = 1;
                                q_edges.push({id_uw, u});
                            }
                        }
                        ui id_vw = l - edge_to;
                        ui id_wv = find(edge_to + pstart[w], edge_to + pstart[w + 1], v) + pstart[w];
                        if (!in_queue_e[id_vw])
                        {
                            assert(!in_queue_e[id_wv]);
                            --triangles_m[id_vw];
                            --triangles_m[id_wv];
                            if (triangles_m[id_vw] + paramK * 2 <= lb)
                            {
                                in_queue_e[id_vw] = in_queue_e[id_wv] = 1;
                                q_edges.push({id_vw, v});
                            }
                        }
                        a++;
                        l++;
                    }
                    else if (*a < *l)
                        a++;
                    else
                        l++;
                }
            }
            if (q_vertex.size())
            {
                ui u = q_vertex.front();
                q_vertex.pop();
                for (ui i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    if (in_queue_e[i])
                        continue;
                    ui v = edge_to[i];
                    if (in_queue_v[v])
                        continue;
                    if (--d[v] + paramK <= lb)
                    {
                        in_queue_v[v] = 1;
                        q_vertex.push(v);
                    }
                    heap.decrease(d[v], v);
                }
                // update the triangles containing u
                for (ui i = pstart[u]; i < pstart[u + 1]; i++)
                {
                    if (in_queue_e[i])
                        continue;
                    int v = edge_to[i];
                    if (in_queue_v[v])
                        continue;
                    int *st = edge_to + pstart[v], *ed = edge_to + pstart[v + 1];
                    for (ui j = i + 1; j < pstart[u + 1]; j++)
                    {
                        if (in_queue_e[j])
                            continue;
                        int w = edge_to[j]; // v w are neighbors of u
                        if (in_queue_v[w])
                            continue;
                        if (has(st, ed, w)) // v is connected to w
                        {
                            ui id_vw = find(st, ed, w) + pstart[v];
                            if (in_queue_e[id_vw])
                                continue;
                            ui id_wv = find(edge_to + pstart[w], edge_to + pstart[w + 1], v) + pstart[w];
                            --triangles_m[id_wv];
                            --triangles_m[id_vw];
                            if (triangles_m[id_vw] + 2 * paramK <= lb)
                            {
                                in_queue_e[id_vw] = in_queue_e[id_wv] = 1;
                                q_edges.push({id_vw, v});
                            }
                        }
                    }
                }
                vertex.reset(u);
                heap.delete_node(u);
            }
        }
    }
    /**
     * given a vertex v, induce the 2-hop neighbor of v
     * @param vis stores the 2-hop neighbor of v
     * @param vertices stores the 2-hop neighbor of v
     */
    void induce_to_2hop(int v, MyBitset &vis, vector<int> &vertices)
    {
        for (int i = pstart[v]; i < pstart[v + 1]; i++)
        {
            if (edge_removed[i])
                continue;
            int a = edge_to[i];
            if (!vertex[a])
                continue;
            if (!vis[a])
            {
                vis.set(a);
                vertices.push_back(a);
            }
            for (int j = pstart[a]; j < pstart[a + 1]; j++)
            {
                if (edge_removed[j])
                    continue;
                int b = edge_to[j];
                if (!vertex[b])
                    continue;
                if (!vis[b])
                {
                    vis.set(b);
                    vertices.push_back(b);
                }
            }
        }
    }
    /**
     * given a vertex v, induce the 2-hop neighbor of v; In addition, we will reduce it(according to Chang)
     * @param vis stores the 2-hop neighbor of v
     * @param vertices stores the 2-hop neighbor of v
     * @param deg used for storing the degree of the subgraph, which should be all 0;
     *              also it can store the neighbor count; don't forget to clear it
     *
     * @return if this subgraph can be pruned, then we set vis[v]=0, i.e., vis.size()=0
     */
    void induce_to_2hop_and_reduce(int v, MyBitset &vis, vector<int> &vertices, vector<int> &deg, int lb)
    {
        vis.reset(v);
        // get G[N(v)] and compute degree
        for (int i = pstart[v]; i < pstart[v + 1]; i++)
        {
            if (edge_removed[i])
                continue;
            int a = edge_to[i];
            if (!vertex[a])
                continue;
            vis.set(a);
            vertices.push_back(a);
            // update the deg
            for (int j = pstart[a]; j < pstart[a + 1]; j++)
            {
                int b = edge_to[j];
                if (b > a)
                    break;
                if (!vis[b])
                    continue;
                deg[a]++;
                deg[b]++;
            }
        }
        // reduce G[N(v)] to (lb+1-2k)-core
        {
            queue<int> q;
            for (int i = 1; i < (int)vertices.size(); i++)
            {
                int u = vertices[i];
                if (deg[u] < lb + 1 - 2 * paramK)
                {
                    vis.reset(u);
                    q.push(u);
                }
            }
            if (vertices.size() - 1 + paramK - q.size() <= lb) // |N(v)| + k <= lb
            {
                for (int u : vertices) // clear the arrays
                {
                    deg[u] = 0;
                    if (vis[u])
                        vis.reset(u);
                }
                return;
            }
            while (q.size())
            {
                int a = q.front();
                q.pop();
                for (int j = pstart[a]; j < pstart[a + 1]; j++)
                {
                    int b = edge_to[j];
                    if (!vis[b])
                        continue;
                    if (--deg[b] < lb + 1 - 2 * paramK)
                    {
                        vis.reset(b);
                        q.push(b);
                    }
                }
            }
            // store the rest of N(v)
            int cnt = 0;
            for (int i = 1; i < (int)vertices.size(); i++)
            {
                int u = vertices[i];
                if (vis[u])
                {
                    vertices[++cnt] = u;
                }
                deg[u] = 0; // clear deg[]
            }
            vertices.resize(cnt + 1);
            if (cnt < lb + 1 - paramK) // current subgraph can be pruned
            {
                // clear the vis[]
                for (int i = 1; i <= cnt; i++)
                {
                    int u = vertices[i];
                    vis.reset(u);
                }
                assert(!vis[v]);
                return;
            }
        }
        vis.set(v);
        int cnt = vertices.size() - 1; // count of |N(v)|
        // next, we add 2-hops neighbors, for u in N_G^2(v), u should have at least lb+3-2k neighbors in N(v)
        {
            auto &neighbor_cnt = deg;
            for (int i = 1; i <= cnt; i++)
            {
                int a = vertices[i];
                for (int j = pstart[a]; j < pstart[a + 1]; j++)
                {
                    if (edge_removed[j])
                        continue;
                    int b = edge_to[j];
                    if (!vertex[b])
                        continue;
                    neighbor_cnt[b]++;
                }
            }
            for (int i = 1; i <= cnt; i++)
            {
                int a = vertices[i];
                neighbor_cnt[a] = 0;
                for (int j = pstart[a]; j < pstart[a + 1]; j++)
                {
                    int b = edge_to[j];
                    if (neighbor_cnt[b] >= lb + 3 - 2 * paramK && !vis[b])
                    {
                        vis.set(b);
                        vertices.push_back(b);
                    }
                    neighbor_cnt[b] = 0;
                }
            }
        }
        if (vertices.size() <= lb)
        {
            for (int u : vertices)
                if (vis[u])
                    vis.reset(u);
            assert(!vis[v]);
            return;
        }
        else
        {
            // reduce g_i to a (lb+1-k)-core
            for (int a : vertices)
            {
                for (int j = pstart[a]; j < pstart[a + 1]; j++)
                {
                    int b = edge_to[j];
                    if (b >= a)
                        break;
                    if (vis[b])
                    {
                        deg[a]++, deg[b]++;
                    }
                }
            }
            queue<int> q;
            for (int a : vertices)
            {
                if (deg[a] + paramK <= lb)
                {
                    q.push(a);
                    vis.reset(a);
                }
            }
            while (q.size())
            {
                int a = q.front();
                q.pop();
                deg[a] = 0;
                for (int j = pstart[a]; j < pstart[a + 1]; j++)
                {
                    int b = edge_to[j];
                    if (!vis[b])
                        continue;
                    if (--deg[b] + paramK <= lb)
                    {
                        vis.reset(b);
                        q.push(b);
                    }
                }
            }
            int cnt = 0;
            for (int a : vertices)
            {
                if (vis[a])
                    vertices[cnt++] = a, deg[a] = 0;
            }
            vertices.resize(cnt);
            if (!vis[v] || cnt <= lb)
            {
                for (int a : vertices)
                    vis.reset(a);
                assert(!vis[v]);
            }
        }
    }
};

/**
 * used for IE; maybe not dense enough for using adjacent matrix
 * when g_i is searched and we exclude v_i, we need to invoke CTCP
 * note that this class can be merged with Graph; but we choose not to merge just for module
 */
class Graph_reduced_adjacent_matrix : public Graph_reduced
{
public:
    Graph_reduced_adjacent_matrix() : Graph_reduced() {}
    /**
     * init graph after stage-I(preprocessing)
     * @param g reduced graph
     * @param must the vertex set that must include because each vertex in it will occur in a maximum k-plex
     */
    Graph_reduced_adjacent_matrix(Graph &g, set<ui> &must) : Graph_reduced()
    {
        n = g.n;
        m = g.m;
        printf("reduced graph n= %d m= %d lb= %d\n", n, m / 2, lb);
        A = AdjacentMatrix(n);
        for (ui i = 0; i < n; i++)
        {
            for (ui j = g.pstart[i]; j < g.pstart[i + 1]; j++)
            {
                ui v = g.edge_to[j];
                if (i < v)
                    A.add_edge(i, v);
            }
        }
        vertex_id.resize(n);
        for (int i = 0; i < n; i++)
        {
            vertex_id[i] = g.map_refresh_id[i];
        }
        for (int u : must)
            must_contain.push_back(u);
        vertex = MyBitset(n);
        vertex.flip();

        d = new int[n];
        for (int i = 0; i < n; i++)
            d[i] = A[i].size();
        // printf("Graph for bnb init ok\n");
        fflush(stdout);
    }
    ~Graph_reduced_adjacent_matrix()
    {
        if (d != nullptr)
        {
            delete[] d;
        }
    }
    /**
     * @brief compute the number of triangles for each edge
     */
    void init_triangles()
    {
        for (int u = 0; u < n; u++)
        {
            for (int v : A[u])
            {
                if (v >= u)
                    break;
                triangles_nn[v * n + u] = A[v].intersect(A[u]);
            }
        }
    }
    /**
     * @brief prepare some thing
     */
    void init_before_IE()
    {
        init_triangles();
        init_heap();
        bool_array_n.resize(n);
        bool_array_n_n.resize(n * n);
    }
    /**
     * @brief inspired by Lijun Chang
     * @param v if v==-1, called for lb increment; else, remove v
     */
    void CTCP(int lb, int v = -1)
    {
        queue<pii> q_edges; // an edge is stored as (u,v) where u<v
        queue<int> q_vertex;
        vector<bool> &in_queue_e = bool_array_n_n; //(u,v) is already pushed into queue if in_queue_e[u*n+v]=1
        vector<bool> &in_queue_v = bool_array_n;   // a vertex u is already pushed into queue if in_queue_v[u]=1
        // CTCP is called because lb updated
        if (v == -1)
        {
            for (int u : vertex)
            {
                for (int v : A[u])
                {
                    if (v >= u)
                        break;
                    if (triangles_nn[v * n + u] + paramK * 2 <= lb)
                    {
                        q_edges.push({v, u});
                        in_queue_e[v * n + u] = true;
                    }
                }
                if (d[u] + paramK <= lb)
                {
                    q_vertex.push(u);
                    in_queue_v[u] = 1;
                }
            }
        }
        else // CTCP is called because IE removed a vertex
        {
            q_vertex.push(v);
            in_queue_v[v] = 1;
        }
        while (q_edges.size() || q_vertex.size())
        {
            while (q_edges.size())
            {
                auto edge = q_edges.front();
                q_edges.pop();
                int u = edge.x, v = edge.y; // u<v
                assert(u < v);
                assert(A[u][v] && A[v][u]);
                // remove this edge and update the degree
                A[u].reset(v);
                A[v].reset(u);
                heap.decrease(--d[u], u);
                heap.decrease(--d[v], v);
                if (d[u] + paramK <= lb && !in_queue_v[u])
                {
                    q_vertex.push(u);
                    in_queue_v[u] = 1;
                }
                if (d[v] + paramK <= lb && !in_queue_v[v])
                {
                    q_vertex.push(v);
                    in_queue_v[v] = 1;
                }
                // update the number of triangles of other edges
                auto common_neighbor = A[u];
                common_neighbor &= A[v];
                for (int w : common_neighbor)
                {
                    if (in_queue_v[w])
                        continue;
                    ll edge_uw = u < w ? u * n + w : w * n + u;
                    if (!in_queue_e[edge_uw])
                    {
                        if (--triangles_nn[edge_uw] + paramK * 2 <= lb)
                        {
                            pii edge = {u, w};
                            if (w < u)
                                edge = {w, u};
                            q_edges.push(edge);
                            in_queue_e[edge_uw] = 1;
                        }
                    }
                    ll edge_vw = v < w ? v * n + w : w * n + v;
                    if (!in_queue_e[edge_vw])
                    {
                        if (--triangles_nn[edge_vw] + paramK * 2 <= lb)
                        {
                            pii edge = {v, w};
                            if (w < v)
                                edge = {w, v};
                            q_edges.push(edge);
                            in_queue_e[edge_vw] = 1;
                        }
                    }
                }
            }

            if (q_vertex.size())
            {
                int u = q_vertex.front();
                q_vertex.pop();
                // update the degree
                for (int v : A[u])
                {
                    if (in_queue_v[v])
                        continue;
                    ll edge_uv = u < v ? u * n + v : v * n + u;
                    if (in_queue_e[edge_uv])
                        continue;
                    heap.decrease(--d[v], v);
                    if (d[v] + paramK <= lb)
                    {
                        q_vertex.push(v);
                        in_queue_v[v] = 1;
                    }
                    A[v].reset(u); // remove edge (u,v)
                }
                // update the number of triangles of edges
                for (int v : A[u])
                {
                    ll edge_uv = u < v ? u * n + v : v * n + u;
                    assert(!in_queue_e[edge_uv]);
                    if (in_queue_v[v])
                        continue;
                    for (int w : A[u])
                    {
                        if (w == v)
                            break;
                        if (in_queue_v[w])
                            continue;
                        ll edge_uw = u < w ? u * n + w : w * n + u;
                        assert(!in_queue_e[edge_uw]);
                        if (!A[v][w])
                            continue;
                        assert(A[w][v]);
                        assert(w < v);
                        ll edge_vw = w * n + v;
                        assert(!in_queue_e[edge_vw]);
                        if (--triangles_nn[edge_vw] + paramK * 2 <= lb)
                        {
                            pii edge = {w, v};
                            q_edges.push(edge);
                            in_queue_e[edge_vw] = 1;
                        }
                    }
                }
                // remove this vertex
                vertex.reset(u);
                A[u].clear();
                heap.delete_node(u);
            }
        }
    }
    /**
     * given a vertex v, induce the 2-hop neighbor of v
     * @param vis stores the 2-hop neighbor of v
     */
    void induce_to_2hop(int v, MyBitset &vis, vector<int> &vertices)
    {
        for (int u : A[v])
        {
            if (!vis[u])
                vis.set(u);
            vis |= A[u];
        }
    }
    /**
     * given a vertex v, induce the 2-hop neighbor of v; In addition, we will reduce it(according to Chang)
     * @param vis stores the 2-hop neighbor of v
     * @param vertices stores the 2-hop neighbor of v; this is useless when using adj-matrix
     * @param deg used for storing the degree of the subgraph, which should be all 0
     *
     * @return if this subgraph can be pruned, then we set vis[v]=0, i.e., vis.size()=0
     */
    void induce_to_2hop_and_reduce(int v, MyBitset &vis, vector<int> &vertices, vector<int> &deg, int lb)
    {
        vis |= A[v];
        vis.reset(v);
        // get G[N(v)] and compute degree, reduce G[N(v)] to (lb+1-2k)-core
        queue<int> q;
        for (int u : A[v])
        {
            deg[u] = A[u].intersect(A[v]);
            if (deg[u] < lb + 1 - 2 * paramK)
            {
                vis.reset(u);
                q.push(u);
            }
        }
        while (q.size())
        {
            int a = q.front();
            q.pop();
            for (int b : vis)
            {
                if (A[a][b])
                {
                    deg[b]--;
                    if (deg[b] < lb + 1 - 2 * paramK)
                    {
                        vis.reset(b);
                        q.push(b);
                    }
                }
            }
        }
        // now vis stores the rest of N(v)+v
        for (int &x : deg) // clear deg[]
            x = 0;
        if (vis.size() < lb + 1 - paramK) // current subgraph can be pruned
        {
            vis.clear();
            return;
        }
        vis.set(v);
        // next, we add 2-hops neighbors, for u in N_G^2(v), u should have at least lb+3-2k neighbors in N(v)
        auto N_2 = A[v];
        N_2.flip();
        N_2 &= vertex;
        MyBitset temp(vertex.range);
        for (int u : N_2)
        {
            if (A[u].intersect(vis) >= lb + 3 - 2 * paramK)
            {
                temp.set(u);
            }
        }
        vis |= temp;
    }
};

/**
 * used for bnb, i.e., each 2-hop induced subgraph is stored using adjacent matrix
 */
class Graph_adjacent
{
public:
    double init_time; // used for log
    AdjacentMatrix adj_matrix;
    int n;
    vector<int> vertex_id; // for u in this, vertex_id[u] in G_reduced
    Graph_adjacent() : init_time(0) {}
    /**
     * @brief given vertex set V_mask, induce subgraph
     *
     * @param V_mask V_mask[u]=1 <==> u in the subgraph
     * @param vertices the vertex set of induced graph(which can avoid huge cost when reduced graph is too large but 2-hop induced graph is small)
     * @param g reduced graph which use adj-list to store edges
     * @param inv each vertex in subgraph is [0, n-1], so we need to save the origin index
     */
    Graph_adjacent(MyBitset &V_mask, vector<int> &vertices, Graph_reduced &g, vector<int> &inv)
    {
        if (g.is_matrix())
        {
            init_from_A(V_mask, g.A, inv);
        }
        else // init from adj-list
        {
            vertex_id = vertices;
            n = vertex_id.size();

            adj_matrix = AdjacentMatrix(n);

            int id = 0;
            for (int i = 0; i < vertex_id.size(); i++)
                inv[vertex_id[i]] = i;
            Timer t;
            for (int u : vertices)
            {
                for (int i = g.pstart[u]; i < g.pstart[u + 1]; i++)
                {
                    if (g.edge_removed[i])
                        continue;
                    int v = g.edge_to[i];
                    if (v >= u)
                        break;
                    if (!V_mask[v])
                        continue;
                    adj_matrix.add_edge(inv[u], inv[v]);
                }
            }
            init_time = t.get_time();
        }
    }
    Graph_adjacent(vector<int> &vertices, vector<pii> &edges)
    {
        vertex_id = vertices;
        Timer t;
        n = vertex_id.size();
        adj_matrix = AdjacentMatrix(n);
        for (auto &h : edges)
        {
            if (h.x < h.y)
                adj_matrix.add_edge(h.x, h.y);
        }
        init_time = t.get_time();
    }
    Graph_adjacent &operator=(const Graph_adjacent &other)
    {
        if (this == &other)
            return *this;
        init_time = other.init_time;
        adj_matrix = other.adj_matrix;
        n = other.n;
        vertex_id = other.vertex_id;
        return *this;
    }
    /**
     * @brief reduce edges: second & higher order reduction
     */
    void edge_reduction(int v_in_S, int lb)
    {
        // if(paramK <= 5)
        //     return;
        auto &A = adj_matrix;
        bool reduced = false;
        do
        {
            reduced = false;
            for (int u = 0; u < n; u++)
            {
                for (int v : A[u])
                {
                    if (v >= u)
                        break;
                    int common_neighbor_cnt = A[u].intersect(A[v]);
                    if (common_neighbor_cnt + 2 * paramK <= lb)
                    {
                        A[u].reset(v);
                        A[v].reset(u);
                        reduced = true;
                    }
                }
                int deg = A[u].size();
                if (deg + paramK <= lb && deg > 0)
                {
                    for (int v : A[u])
                    {
                        A[v].reset(u);
                    }
                    A[u].clear();
                    reduced = true;
                }
            }
            if (reduced || paramK == 2)
            {
                continue;
            }
            auto &N_v = A[v_in_S];
            for (int u = 0; u < n; u++)
            {
                if (u == v_in_S)
                    continue;
                for (int v : A[u])
                {
                    if (v == v_in_S)
                        continue;
                    if (v >= u)
                        break;
                    int common_neighbor_cnt = N_v.intersect(A[u], A[v]); // common neighbors of {u,v,v_in_S}
                    int loss_cnt = (!A[v_in_S][u]) + (!A[v_in_S][v]);
                    if (common_neighbor_cnt + 3 * paramK - 2 * loss_cnt <= lb)
                    {
                        A[u].reset(v);
                        A[v].reset(u);
                        reduced = true;
                    }
                }
                int deg = A[u].size();
                if (deg + paramK <= lb && deg > 0)
                {
                    for (int v : A[u])
                    {
                        A[v].reset(u);
                    }
                    A[u].clear();
                    reduced = true;
                }
            }
        } while (reduced);
    }
    /**
     * @brief given vertex set V_mask, induce subgraph
     *
     * @param V_mask V_mask[u]=1 <==> u in the subgraph
     * @param inv each vertex in subgraph is [0, n-1], so we need to save the origin index
     */
    void init_from_A(MyBitset &V_mask, AdjacentMatrix &A, vector<int> &inv)
    {
        for (int i : V_mask)
        {
            vertex_id.push_back(i);
        }
        n = vertex_id.size();

        adj_matrix = AdjacentMatrix(n);

        int id = 0;
        for (int i = 0; i < vertex_id.size(); i++)
            inv[vertex_id[i]] = i;
        double start_init = get_system_time_microsecond();
        for (int u : V_mask)
        {
            auto nei = V_mask;
            nei &= A[u];
            for (int v : nei)
            {
                if (v >= u)
                    break;
                adj_matrix.add_edge(inv[u], inv[v]);
            }
        }
        init_time = get_system_time_microsecond() - start_init;
    }
    /**
     * useless, just a demo
     */
    bool exist_edge(int a, int b)
    {
        return adj_matrix[a][b];
    }
    /**
     * @brief all vertices in s are in the subgraph g_i, and we need to know the origin indices of them
     */
    set<int> get_ori_vertices(set<int> &s)
    {
        set<int> ret;
        for (int v : s)
            ret.insert(vertex_id[v]);
        return ret;
    }
    /**
     * @brief obtain the number of vertices
     */
    int size()
    {
        return n;
    }
};

#endif