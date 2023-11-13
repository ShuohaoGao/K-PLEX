#ifndef GRAPH_H
#define GRAPH_H

#include "LinearHeap.h"
#include "SegTree.h"

class Graph
{
    bool reduced;

public:
    ui n, m;
    ui *d;
    // edge_to[ pstart[a] ... pstart[a+1] ] is the neighbor of a
    ui *edge_to;
    ui *pstart;
    unordered_map<ui, ui> map_refresh_id; // we need to re-map the reduced graph to {0,1,...,n-1}, thus requiring to record the map
    Graph() : n(0), m(0), d(nullptr), edge_to(nullptr), pstart(nullptr), reduced(false)
    {
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
    bool exist_edge(ui a, ui b)
    {
        return has(edge_to + pstart[a], edge_to + pstart[a + 1], b);
    }
    void readFromFile(string file_path)
    {
        ifstream in(file_path);
        if (!in.is_open())
        {
            printf("Failed to open %s \n", file_path.c_str());
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
            fread(&n, sizeof(ui), 1, in);
            fread(&m, sizeof(ui), 1, in);
            d = new ui[n];
            pstart = new ui[n + 1];
            edge_to = new ui[m];
            fread(d, sizeof(ui), n, in);
            fread(edge_to, sizeof(ui), m, in);
            pstart[0] = 0;
            for (ui i = 1; i < n; i++)
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

        printf("Graph init ok\n");
        fflush(stdout);
    }
    // return a maximal plex containing u
    int extend(ui u, set<ui> *solution = nullptr, set<ui> *must_contain = nullptr)
    {
        SegTree tr(n, d, edge_to + pstart[u], d[u]);
        set<ui> s{u};
        tr.remove(u);
        vector<ui> deg(n, 0);
        vector<bool> rm(n, 0);
        rm[u] = 1;
        for (ui I = 0; I + 1 < n; I++) // enumerate the rest n-1 vertices
        {
            ui v = tr.get();
            deg[v] = tr.get_cnt();
            tr.remove(v);
            rm[v] = 1;
            if (deg[v] + paramK < s.size() + 1)
                break;
            ui *l = edge_to + pstart[v], *r = edge_to + pstart[v + 1];
            for (ui w : s)
            {
                if (has(l, r, w))
                    deg[w]++;
            }
            // check whether v can be inserted into s
            if (s.size() >= paramK)
            {
                bool ok = 1;
                for (ui w : s)
                    if (deg[w] + paramK < s.size() + 1)
                    {
                        ok = 0;
                        break;
                    }
                ok &= deg[v] + paramK >= s.size() + 1;
                if (!ok)
                {
                    for (ui w : s)
                    {
                        if (exist_edge(v, w))
                            deg[w]--;
                    }
                    continue;
                }
            }
            s.insert(v);
            for (ui i = pstart[v]; i < pstart[v + 1]; i++)
            {
                ui w = edge_to[i];
                if (rm[w])
                    continue;
                tr.add(w);
            }
        }
        // update current solution
        if (solution != nullptr)
        {
            if (must_contain == nullptr && solution->size() < s.size())
            {
                solution->clear();
                for (ui v : s)
                    solution->insert(map_refresh_id[v]);
            }
            else if (must_contain != nullptr && must_contain->size() + s.size() > solution->size())
            {
                solution->clear();
                for (ui v : s)
                    solution->insert(map_refresh_id[v]);
                for (ui v : (*must_contain))
                    solution->insert(v);
            }
        }
        return s.size();
    }
    bool is_plex(set<ui> &s)
    {
        map<ui, ui> deg;
        for (ui u : s)
        {
            deg[u] = 0;
            for (ui v : s)
                if (exist_edge(u, v))
                    deg[u]++;
            if (deg[u] + paramK < s.size())
                return false;
        }
        return true;
    }
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
            ui cnt = 0;
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
            else if (d[u] + cnt + 2 == n)
            {
                s.insert(map_refresh_id[u]);
                rm[u] = 1;
                // for (ui i = 0; i < n; i++)
                // {
                //     if (i != u && d[i] + paramK < n && !exist_edge(u, i))
                //     {
                //         rm[i] = 1;
                //         break;
                //     }
                // }
            }
        }
    }
    void remove_v_must_include(vector<bool> &rm, int lb)
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
                if (--d[v] + paramK <= lb)
                    q[++tt] = v, rm[v] = 1;
            }
        }
        ui new_n = 0;
        if (reduced)
        {
            unordered_map<ui, ui> new_map;
            for (ui i = 0; i < n; i++)
                if (!rm[i])
                {
                    new_map[new_n] = map_refresh_id[i];
                    q[i] = new_n++;
                }
            map_refresh_id = new_map;
        }
        else
        {
            for (ui i = 0; i < n; i++)
                if (!rm[i])
                {
                    map_refresh_id[new_n] = i;
                    q[i] = new_n++;
                }
            reduced = true;
        }
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
    // stage-III: acquire a heuristic solution in the sqrt graph
    int sqrt_degeneracy(ui range, vector<vector<ui>> &neighbor, vector<ui> &deg, vector<ui> &id, set<ui> *solution = nullptr)
    {
        LinearHeap heap(range, range, deg);
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
                heap.decrease(--deg[v], v);
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
    // stage-II: induce a subgraph with sqrt(m) vertices
    int sqrt_degeneracy(ui start_u, set<ui> *solution = nullptr)
    {
        ui range = sqrt(m);
        vector<ui> id(range, 0);
        vector<ui> vis(n, 0);
        ui cnt = 0;
        vis[start_u] = 1 + cnt;
        id[cnt++] = start_u;
        queue<ui> q;
        q.push(start_u);
        while (q.size() && cnt < range)
        {
            ui u = q.front();
            q.pop();
            int max_neighbor = range >> 2;
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
        for (ui i = 0; i < n && cnt < range; i++)
            if (!vis[i])
            {
                vis[i] = 1 + cnt;
                id[cnt++] = i;
            }
        vector<vector<ui>> neighbor(range);
        vector<ui> deg(range, 0);
        for (ui u = 0; u < range; u++)
        {
            for (ui i = pstart[id[u]]; i < pstart[id[u] + 1]; i++)
            {
                ui v = edge_to[i];
                if (!vis[v])
                    continue;
                neighbor[u].push_back(vis[v] - 1);
            }
            deg[u] = neighbor[u].size();
        }
        return sqrt_degeneracy(range, neighbor, deg, id, solution);
    }
    // stage-I: select a vertex as start vertex
    int sqrt_degeneracy(set<ui> *solution = nullptr, ui cnt = 5)
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
            ret = max(ret, sqrt_degeneracy(h.y, solution));
        return ret;
    }
    vector<ui> neighbor(ui u)
    {
        vector<ui> ret;
        for (ui i = pstart[u]; i < pstart[u + 1]; i++)
        {
            ui v = edge_to[i];
            ret.push_back(v);
        }
        return ret;
    }
    void weak_reduce(int lb)
    {
        ui *q = new ui[n + 1];
        vector<bool> rm(n, 0); // rm[u]=1 <=> u is removed
        ui hh = 1, tt = 0;
        for (ui i = 0; i < n; i++)
            if (d[i] + paramK <= lb)
                q[++tt] = i, rm[i] = 1;
        if (tt == 0)
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
        // re-build the graph : re-map the id of the rest vertices
        ui new_n = 0;
        if (reduced)
        {
            unordered_map<ui, ui> new_map;
            for (ui i = 0; i < n; i++)
                if (!rm[i])
                {
                    new_map[new_n] = map_refresh_id[i];
                    q[i] = new_n++;
                }
            map_refresh_id = new_map;
        }
        else
        {
            for (ui i = 0; i < n; i++)
                if (!rm[i])
                {
                    map_refresh_id[new_n] = i;
                    q[i] = new_n++;
                }
            reduced = true;
        }
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
    int degeneracyLB(set<ui> *solution = nullptr)
    {
        vector<bool> rm(n, 0);
        int *pd = new int[n];
        memcpy(pd, d, sizeof(int) * n);
        LinearHeap heap(n, n, pd);
        while (heap.get_min_key() + paramK < heap.sz)
        {
            ui u = heap.get_min_node();
            assert(u < n);
            heap.delete_node(u);
            rm[u] = 1;
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                assert(v < n);
                if (!rm[v])
                {
                    assert(pd[v] > 0);
                    heap.decrease(--pd[v], v);
                }
            }
        }
        delete[] pd;
        if (solution != nullptr && solution->size() < heap.sz)
        {
            solution->clear();
            for (ui i = 0; i < n; i++)
                if (!rm[i])
                    solution->insert(map_refresh_id[i]);
        }
        return heap.sz;
    }
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

#endif