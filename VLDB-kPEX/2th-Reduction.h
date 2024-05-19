#ifndef REDUCTION_H
#define REDUCTION_H

#include "Graph.h"

/**
 * @brief second order reduction: CF-CTCP
 */
class Reduction
{
private:
    // the edge in the edge queue is stored as {from_v, edge_id, remove_time}
    struct EdgeForQ_E
    {
        ui u, edge_id;
        ui remove_time;
    };
    // my queue
    class Queue
    {
        ui *q;
        ui hh, tt;

    public:
        Queue(ui max_size) : q(nullptr), hh(1), tt(0)
        {
            q = new ui[max_size + 1];
        }
        ~Queue()
        {
            if (q != nullptr)
            {
                delete[] q;
            }
        }
        void push(ui u)
        {
            q[++tt] = u;
        }
        void pop()
        {
            hh++;
        }
        ui front()
        {
            return q[hh];
        }
        int size()
        {
            return tt + 1 - hh;
        }
    };
    // G-fast: only vertices in G-fast are useful
    Graph &G_fast;
    ui *triangles, *compute_time, *another_edge;
    vector<bool> vertex_removed_from_G_fast, edge_removed_from_G_fast;
    ui n, m;
    ui timestamp;
    // G-slow: this is used to update triangle count
    vector<bool> edge_removed_from_G_slow;
    // cache
    vector<bool> vis;

    /**
     * @brief reduce G-fast to (lb+1-k)-core
     *
     * @param q_v the vertices that must remove
     * @param q_e the edges removed should be pushed to queue
     */
    void core_reduce(queue<ui> &q_v, int lb, vector<EdgeForQ_E> &q_e)
    {
        ui *pstart = G_fast.pstart, *edge_to = G_fast.edge_to;
        ui *d = G_fast.d;
        while (q_v.size())
        {
            ui u = q_v.front();
            q_v.pop();
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                if (edge_removed_from_G_fast[i])
                    continue;
                ui v = edge_to[i];
                if (!vertex_removed_from_G_fast[v])
                {
                    d[v]--;
                    if (d[v] + paramK == lb)
                    {
                        q_v.push(v);
                        vertex_removed_from_G_fast[v] = 1;
                    }
                    else
                        q_e.push_back({u, i, timestamp});
                }
            }
        }
    }

    /**
     * @brief remove the edge(u,v) and reduce G-fast to (lb+1-k)-core immediately
     *
     * @param u from
     * @param v to
     * @param edge_id make sure that edge_to[edge_id] = v
     * @param q_e the edges removed should be pushed to queue
     */
    void remove_edge_from_G_fast(ui u, ui v, ui edge_id, int lb, vector<EdgeForQ_E> &q_e)
    {
        q_e.push_back({u, edge_id, ++timestamp});
        edge_removed_from_G_fast[edge_id] = 1;
        ui another_edge_id = another_edge[edge_id];
        edge_removed_from_G_fast[another_edge_id] = 1;
        queue<ui> q;
        if (--G_fast.d[u] + paramK == lb)
        {
            q.push(u);
            vertex_removed_from_G_fast[u] = 1;
        }
        if (--G_fast.d[v] + paramK == lb)
        {
            q.push(v);
            vertex_removed_from_G_fast[v] = 1;
        }
        if (q.size())
        {
            core_reduce(q, lb, q_e);
        }
    }

    /**
     * @brief reduce G-fast to (lb+1-k)-core for the first round: we don't consider updating triangles count
     *
     * @param q_v the vertices that must remove
     *
     * @return the number of vertices we remove
     */
    ui core_reduce(Queue &q_v, int lb)
    {
        ui *pstart = G_fast.pstart, *edge_to = G_fast.edge_to;
        ui *d = G_fast.d;
        ui cnt = 0;
        while (q_v.size())
        {
            ui u = q_v.front();
            q_v.pop();
            cnt++;
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                if (edge_removed_from_G_fast[i])
                    continue;
                ui v = edge_to[i];
                if (!vertex_removed_from_G_fast[v])
                {
                    d[v]--;
                    if (d[v] + paramK == lb)
                    {
                        q_v.push(v);
                        vertex_removed_from_G_fast[v] = 1;
                    }
                }
            }
        }
        return cnt;
    }

    /**
     * @brief refresh the graph which can fast the computation of counting triangles
     *
     * @param u the next vertex that we are going to enumerate its edges
     *
     * @return the index of u in the new graph(if u is not in new graph, return the next vertex of u)
     */
    ui rebuild_graph_for_first_round(ui u)
    {
        ui *pstart = G_fast.pstart, *edge_to = G_fast.edge_to;
        ui *d = G_fast.d;
        ui *id_map = new ui[n];
        ui new_n = 0;
        vector<ui> map_refresh_id(n);
        for (ui i = 0; i < n; i++)
        {
            if (!vertex_removed_from_G_fast[i])
            {
                map_refresh_id[new_n] = G_fast.map_refresh_id[i];
                id_map[i] = new_n++;
            }
        }
        ui new_idx_u = new_n;
        for (ui i = u; i < n; i++)
            if (!vertex_removed_from_G_fast[i])
            {
                new_idx_u = id_map[i];
                break;
            }
        map_refresh_id.resize(new_n);
        G_fast.map_refresh_id = map_refresh_id;
        ui new_m = 0;
        ui *new_pstart = new ui[new_n + 1];
        for (ui u = 0; u < n; u++)
        {
            if (vertex_removed_from_G_fast[u])
                continue;
            new_pstart[id_map[u]] = new_m;
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                if (edge_removed_from_G_fast[i])
                    continue;
                ui v = edge_to[i];
                if (vertex_removed_from_G_fast[v])
                    continue;
                edge_to[new_m++] = id_map[v];
            }
        }
        new_pstart[new_n] = new_m;
        ui *new_d = new ui[new_n];
        for (ui i = 0; i < new_n; i++)
            new_d[i] = new_pstart[i + 1] - new_pstart[i];
        delete[] pstart;
        delete[] d;
        delete[] id_map;
        G_fast.pstart = new_pstart;
        G_fast.d = new_d;
        n = G_fast.n = new_n;
        m = G_fast.m = new_m;
        edge_removed_from_G_fast.clear();
        edge_removed_from_G_fast.resize(m, 0);
        vertex_removed_from_G_fast.clear();
        vertex_removed_from_G_fast.resize(n, 0);
        vis.resize(n, 0);
        return new_idx_u;
    }

    /**
     * @brief for the first round, we think almost each edge can be removed
     * so we compute a near-exact triangle count(slightly greater than the fact) and judge if it can be removed
     *
     * @param q_v a queue that can be shared
     * @param start_u for v<start_u, we don't consider the edges related to v
     */
    void first_round_reduce(int lb, Queue &q_v, ui start_u = 0)
    {
        ui *pstart = G_fast.pstart, *edge_to = G_fast.edge_to;
        ui *d = G_fast.d;
        Timer t;
        ui remove_vertex_cnt = 0;
        for (ui u = start_u; u < n; u++)
        {
            if (vertex_removed_from_G_fast[u])
                continue;
            if (d[u] + 1 == n) // u is connected to all vertices
                continue;
            // cache: record the neighbor of u(like a hash operation)
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                if (edge_removed_from_G_fast[i])
                    continue;
                ui v = edge_to[i];
                if (vertex_removed_from_G_fast[v])
                    continue;
                vis[v] = 1;
            }
            // enmerate the neighbor of u, and each edge we compute only once
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                if (edge_removed_from_G_fast[i])
                    continue;
                ui v = edge_to[i];
                if (vertex_removed_from_G_fast[v])
                    continue;
                if (v >= u) // make sure v<u in degeneracy order
                    break;
                ui edge_cnt = 0;
                // compute the triangle count of (u,v)
                for (ui j = pstart[v]; j < pstart[v + 1]; j++)
                {
                    if (edge_removed_from_G_fast[j])
                        continue;
                    ui w = edge_to[j];
                    if (vertex_removed_from_G_fast[w])
                        continue;
                    if (vis[w]) // w is a common neighbor
                        edge_cnt++;
                }
                if (edge_cnt + 2 * paramK <= lb) // remove (u,v)
                {
                    edge_removed_from_G_fast[i] = true;
                    ui another_edge_id = find(edge_to + pstart[v], edge_to + pstart[v + 1], u) + pstart[v];
                    edge_removed_from_G_fast[another_edge_id] = true;
                    if (--d[u] + paramK == lb)
                    {
                        q_v.push(u);
                        vertex_removed_from_G_fast[u] = true;
                    }
                    if (--d[v] + paramK == lb)
                    {
                        q_v.push(v);
                        vertex_removed_from_G_fast[v] = true;
                    }
                    if (q_v.size())
                    {
                        remove_vertex_cnt += core_reduce(q_v, lb);
                        if (vertex_removed_from_G_fast[u])
                            break;
                    }
                    // note that v is not a neighbor of u now
                    vis[v] = 0;
                }
            }
            // clear the cache
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                vis[v] = 0;
            }
            // shrink current graph to boost the computation
            if (remove_vertex_cnt * 4 >= n)
            {
                ui new_start_u = rebuild_graph_for_first_round(u + 1);
                list_triangle_time += t.get_time();
                first_round_reduce(lb, q_v, new_start_u);
                return;
            }
        }
        rebuild_graph_for_first_round(0);
        list_triangle_time += t.get_time();
    }

public:
    Reduction(Graph *_g) : G_fast(*_g), triangles(nullptr), compute_time(nullptr), another_edge(nullptr),
                           timestamp(0)
    {
        n = G_fast.n;
        m = G_fast.m;
        vis.resize(n, 0);
    }
    ~Reduction()
    {
        vector<ui *> ptrs{triangles, compute_time, another_edge};
        for (auto p : ptrs)
            if (p != nullptr)
            {
                delete[] p;
            }
    }
    /**
     * the entrance of our second-order reduction
     */
    void strong_reduce(int lb)
    {
        // the first round, we brutely remove edges (of course, core-reduce is always first)
        bool continue_first_round;
        do
        {
            ui previous_m = m, previous_n = n;
            vertex_removed_from_G_fast.resize(n, 0);
            edge_removed_from_G_fast.resize(m, 0);
            Queue q_v(n);
            first_round_reduce(lb, q_v);
            if (previous_m == m) // none of the edges can be reduced
            {
                return;
            }
            continue_first_round = previous_m * 0.8 > m || previous_n * 0.8 > n;
        } while (continue_first_round);
        // prepare for CF-CTCP
        ui *pstart = G_fast.pstart, *edge_to = G_fast.edge_to;
        triangles = new ui[m];
        compute_time = new ui[m];
        another_edge = new ui[m];
        for (ui u = 0; u < n; u++)
        {
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                if (v >= u)
                    break;
                ui another_edge_id = find(edge_to + pstart[v], edge_to + pstart[v + 1], u) + pstart[v];
                another_edge[i] = another_edge_id;
                another_edge[another_edge_id] = i;
            }
        }
        edge_removed_from_G_slow.resize(m, 0);
        CF_CTCP(lb);
    }
    void rebuild_graph()
    {
        ui *pstart = G_fast.pstart, *edge_to = G_fast.edge_to;
        ui *d = G_fast.d;
        ui *id_map = new ui[n];
        ui new_n = 0;
        vector<ui> map_refresh_id(n);
        for (ui i = 0; i < n; i++)
        {
            if (!vertex_removed_from_G_fast[i])
            {
                map_refresh_id[new_n] = G_fast.map_refresh_id[i];
                id_map[i] = new_n++;
            }
        }
        map_refresh_id.resize(new_n);
        G_fast.map_refresh_id = map_refresh_id;
        ui new_m = 0;
        ui *new_pstart = new ui[new_n + 1];
        for (ui u = 0; u < n; u++)
        {
            if (vertex_removed_from_G_fast[u])
                continue;
            new_pstart[id_map[u]] = new_m;
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                if (edge_removed_from_G_fast[i])
                    continue;
                ui v = edge_to[i];
                if (vertex_removed_from_G_fast[v])
                    continue;
                edge_to[new_m++] = id_map[v];
            }
        }
        new_pstart[new_n] = new_m;
        ui *new_d = new ui[new_n];
        for (ui i = 0; i < new_n; i++)
            new_d[i] = new_pstart[i + 1] - new_pstart[i];
        delete[] pstart;
        delete[] d;
        delete[] id_map;
        G_fast.pstart = new_pstart;
        G_fast.d = new_d;
        n = G_fast.n = new_n;
        m = G_fast.m = new_m;
        for (ui u = 0; u < n; u++)
        {
            for (ui i = G_fast.pstart[u]; i < G_fast.pstart[u + 1]; i++)
            {
                ui v = G_fast.edge_to[i];
            }
        }
    }
    void CF_CTCP(int lb)
    {
        vector<EdgeForQ_E> q_e;
        ui *pstart = G_fast.pstart, *edge_to = G_fast.edge_to;
        ui *d = G_fast.d;
        Timer t;
        // list triangles and reduce G-fast to (lb+1-k)-core whenever we can
        for (ui u = 0; u < n; u++)
        {
            if (vertex_removed_from_G_fast[u])
                continue;
            // cache: record the neighbor of u(like a hash operation)
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                if (edge_removed_from_G_fast[i])
                    continue;
                ui v = edge_to[i];
                if (vertex_removed_from_G_fast[v])
                    continue;
                vis[v] = 1;
            }
            // enmerate the neighbor of u, and each edge we compute only once
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                if (edge_removed_from_G_fast[i])
                    continue;
                ui v = edge_to[i];
                if (vertex_removed_from_G_fast[v])
                    continue;
                if (v >= u)
                    break;
                ui edge_cnt = 0;
                // compute the triangle count of (u,v)
                for (ui j = pstart[v]; j < pstart[v + 1]; j++)
                {
                    if (edge_removed_from_G_fast[j])
                        continue;
                    ui w = edge_to[j];
                    if (vertex_removed_from_G_fast[w])
                        continue;
                    if (vis[w]) // w is a common neighbor
                        edge_cnt++;
                }
                if (edge_cnt + 2 * paramK <= lb) // remove (u,v)
                {
                    remove_edge_from_G_fast(u, v, i, lb, q_e);
                    if (vertex_removed_from_G_fast[u])
                        break;
                    else // note that the neighbors of u are changed so we need to update vis
                    {
                        vis[v] = 0;
                    }
                }
                else
                {
                    triangles[i] = edge_cnt;
                    compute_time[i] = ++timestamp;
                    ui another_edge_id = another_edge[i];
                    triangles[another_edge_id] = edge_cnt;
                    compute_time[another_edge_id] = timestamp;
                }
            }
            // clear the cache
            for (ui i = pstart[u]; i < pstart[u + 1]; i++)
            {
                ui v = edge_to[i];
                vis[v] = 0;
            }
        }
        list_triangle_time += t.get_time();
        if (!q_e.size())
            return;
        // update the information of triangle count exactly
        ui front_idx = 0;
        while (front_idx < q_e.size())
        {
            // auto h = q_e.front(); q_e.pop();
            auto &h = q_e[front_idx++];
            ui u = h.u, edge_id = h.edge_id, v = edge_to[edge_id], time = h.remove_time;
            edge_removed_from_G_slow[edge_id] = 1;
            ui another_edge_id = another_edge[edge_id];
            edge_removed_from_G_slow[another_edge_id] = 1;
            if (vertex_removed_from_G_fast[u] && vertex_removed_from_G_fast[v])
                continue;
            if (!vertex_removed_from_G_fast[u])
            {
                // i for G-fast and j for G-slow
                for (ui i = pstart[u], j = pstart[v]; i < pstart[u + 1] && j < pstart[v + 1];)
                {
                    if (edge_removed_from_G_fast[i] || vertex_removed_from_G_fast[edge_to[i]])
                    {
                        i++;
                        continue;
                    }
                    if (edge_removed_from_G_slow[j])
                    {
                        j++;
                        continue;
                    }
                    if (edge_to[i] < edge_to[j])
                    {
                        i++;
                        continue;
                    }
                    else if (edge_to[i] > edge_to[j])
                    {
                        j++;
                        continue;
                    }
                    else // now we find a w in N_{G_f}(u) \cap N_{G_s}(v)
                    {
                        if (compute_time[i] > time) // the removing of (u,v) will not affect (u,w) because \Delta(u,w) is computed after removing (u,v)
                        {
                            i++;
                            j++;
                            continue;
                        }
                        ui w = edge_to[i];
                        if (--triangles[i] + 2 * paramK <= lb)
                        {
                            remove_edge_from_G_fast(u, w, i, lb, q_e);
                            if (vertex_removed_from_G_fast[u])
                                break;
                        }
                        else
                        {
                            ui another_edge_id = another_edge[i];
                            triangles[another_edge_id]--;
                        }
                        i++;
                        j++;
                    }
                }
            }

            if (!vertex_removed_from_G_fast[v])
            {
                // i for G-fast and j for G-slow
                for (ui i = pstart[v], j = pstart[u]; i < pstart[v + 1] && j < pstart[u + 1];)
                {
                    if (edge_removed_from_G_fast[i] || vertex_removed_from_G_fast[edge_to[i]])
                    {
                        i++;
                        continue;
                    }
                    if (edge_removed_from_G_slow[j])
                    {
                        j++;
                        continue;
                    }
                    if (edge_to[i] < edge_to[j])
                    {
                        i++;
                        continue;
                    }
                    else if (edge_to[i] > edge_to[j])
                    {
                        j++;
                        continue;
                    }
                    else // now we find a w in N_{G_f}(v) \cap N_{G_s}(u)
                    {
                        if (compute_time[i] >= time) // the removing of (u,v) will not affect (v,w) because \Delta(v,w) is computed after removing (u,v)
                        {
                            i++;
                            j++;
                            continue;
                        }
                        ui w = edge_to[i];
                        if (--triangles[i] + 2 * paramK <= lb)
                        {
                            remove_edge_from_G_fast(v, w, i, lb, q_e);
                            if (vertex_removed_from_G_fast[v])
                                break;
                        }
                        else
                        {
                            ui another_edge_id = another_edge[i];
                            triangles[another_edge_id]--;
                        }
                        i++;
                        j++;
                    }
                }
            }
        }
        rebuild_graph();
    }
};

#endif