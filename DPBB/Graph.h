#ifndef GRAPH_H
#define GRAPH_H

#include "MyBitset.h"
#include "LinearHeap.h"

class Graph_input
{
public:
    LinearHeap heap;
    MyBitset vertex;
    ll n, m;
    vector<int> vertex_id;
    vector<int> must_contain;
    unordered_map<ll,int> triangles;// the key of edge (u,v) is u*n+v , make sure u<v
    int *d; // degree
    AjacentMatrix A;
    Graph_input() : d(nullptr) {}
    ~Graph_input()
    {
        if (d != nullptr)
        {
            delete[] d;
        }
    }
    void read_from_file(string path)
    {
        FILE *in = fopen(path.c_str(), "r");
        if (in == nullptr)
        {
            printf("File : %s open failed\n", path.c_str());
            exit(1);
        }
        fscanf(in, "%lld %lld %d", &n, &m, &lb);
        printf("File: %s k= %d n= %d m= %d lb= %d\n", get_file_name_without_suffix(path).c_str(), paramK, n, m, lb);
        fflush(stdout);
        if (!n)
        {
            puts("The heuristic solution is the ground truth");
            printf("ground truth= %d , use time= 0.0000 s\n\n", lb);
            exit(0);
        }
        A = AjacentMatrix(n);
        for (ui i = 0; i < m; i++)
        {
            ui a, b;
            fscanf(in, "%u %u", &a, &b);
            if (a < b)
                A.add_edge(a, b);
        }

        vertex_id.resize(n);
        for (int i = 0; i < n; i++)
        {
            if (fscanf(in, "%d", &vertex_id[i]) <= 0)
                vertex_id[i] = i;
        }

        int must_contain_cnt;
        if (fscanf(in, "%d", &must_contain_cnt) <= 0)
        {
            must_contain_cnt = 0;
        }
        must_contain.resize(must_contain_cnt);
        for (int i = 0; i < must_contain_cnt; i++)
            fscanf(in, "%d", &must_contain[i]);
        fclose(in);

        vertex = MyBitset(n);
        vertex.flip();

        d = new int[n];
        for (int i = 0; i < n; i++)
            d[i] = A[i].size();
        printf("Graph init ok\n");
        fflush(stdout);
    }

    void init_heap()
    {
        heap = LinearHeap(n, n, d);
    }

    void init_triangles()
    {
        for(int u=0;u<n;u++)
        {
            for(int v:A[u])
            {
                if(v>=u) break;
                triangles[v*n+u]=A[v].intersect(A[u]);
            }
        }
    }

    void init_before_IE()
    {
        init_triangles();
        init_heap();
    }

    int get_min_degree_v()
    {
        return heap.get_min_node();
    }

    void remove_v(int v)
    {
        vertex.reset(v);
        heap.delete_node(v);
        for (int u : A[v])
            if (vertex.test(u))
            {
                d[u]--;
                heap.decrease(d[u], u);
                A[u].reset(v);
            }
        CTCP(v);
        A[v].clear();
    }

    void CTCP(int v=-1)
    {
        // for(int u:A[v])
        // {
        //     for(int w:A[v])
        //     {
        //         if(w>=u) break;
        //         if(!A[u][w]) continue;
        //         if(--triangles[w*n+u]+2*paramK <= lb)
        //         {
                    
        //         }
        //     }
        // }
    }

    void induce_to_2hop(MyBitset &vis, int v)
    {
        for (int u : A[v])
        {
            assert(vertex[u]);
            if (!vis[u])
                vis.set(u);
            vis |= A[u];
        }
    }

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

    int size()
    {
        return vertex.size();
    }
};

class Graph
{
public:
    double init_time;
    AjacentMatrix adj_matrix;
    int n;
    vector<int> vertex_id;
    Graph() : init_time(0) {}
    // induce graph
    Graph(MyBitset &V_mask, AjacentMatrix &A, vector<int> &inv)
    {
        for (int i : V_mask)
        {
            vertex_id.push_back(i);
        }
        n = vertex_id.size();

        adj_matrix = AjacentMatrix(n);

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
    bool exist_edge(int a, int b)
    {
        return adj_matrix[a][b];
    }
    set<int> get_ori_vertices(set<int> &s)
    {
        set<int> ret;
        for (int v : s)
            ret.insert(vertex_id[v]);
        return ret;
    }

    int size()
    {
        return n;
    }
};

#endif