#ifndef FIND_SMALL_H
#define FIND_SMALL_H

#include "Graph.h"

/**
 * @brief find maximum k-plex of size at most 2k-2
 */
class Solver_small
{
    Graph g;
    set<ui> solution;
    int k;
    // some cache/buffer arrays
    vector<int> int_array_size_n; // default value: -1
    Solver_small(string graph_path, const set<ui> &s, int paramK) : solution(s), k(paramK)
    {
        // reload graph
        g.readFromFile(graph_path);
        // sort vertices according to degeneracy order
        g.degeneracy_and_reduce(solution.size(), &solution);
        int_array_size_n.resize(g.n, -1);
    }
    ~Solver_small() {}

    /**
     * @brief start to search
     */
    void start_search()
    {
        ui n = g.n, m = g.m;
        ui *d = g.d;
        ui *pstart = g.pstart;
        ui *edge_to = g.edge_to;
        int lb = solution.size();
        vector<bool> vertex_removed(n);
        queue<ui> q;
        // divide-and-conquer: IE framework
        for (ui u = 0; u < n; u++)
        {
            // search for kplex including vertex u
            sub_solver(u, vertex_removed);
            lb = solution.size(); // lb may be updated
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
                    }
                }
            }
        }
    }

    /**
     * @brief search for kplex including vertex u
     * @param vertex_removed some vertices are already removed so that they can not be used
     */
    void sub_solver(ui u, const vector<bool> &vertex_removed)
    {
        ui *d = g.d;
        ui *pstart = g.pstart;
        ui *edge_to = g.edge_to;

        // dist[v] => distance from u to v
        auto &dist = int_array_size_n;
        // reduce unpromising vertices based on diameter
        int max_diameter = 0;
        dist[u] = 0;
        vector<ui> promising_vertices{u};
        {
            // bfs
            int hh = 0, tt = 0; // serve as queue
            while (hh <= tt)
            {
                ui v = promising_vertices[hh]; // pop from queue
                hh++;
                if (dist[v] >= max_diameter)
                    break;
                for (ui i = pstart[v]; i < pstart[v + 1]; i++)
                {
                    ui w = edge_to[i];
                    if (dist[w] == -1)
                    {
                        dist[w] = dist[v] + 1;
                        promising_vertices.push_back(w); // push into queue
                        tt++;
                        assert(tt + 1 == promising_vertices.size());
                    }
                }
            }
        }
        // clear buffer vis[]
        for (ui v : promising_vertices)
            dist[v] = -1;
    }
};

#endif