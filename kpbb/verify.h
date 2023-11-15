#ifndef VERIFIER_H
#define VERIFIER_H

#include "Utility.h"

template <typename T>
class Verifier_undirected_kPlex
{
    set<T> s;
    map<int, set<int>> neighbors;

public:
    Verifier_undirected_kPlex(set<T> _s, string file_path):s(_s)
    {
        FILE *f = fopen(file_path.c_str(), "r");
        if (f == nullptr)
        {
            printf("File: %s open failed!\n", file_path.c_str());
            exit(1);
        }
        int n, m;
        fscanf(f, "%d%d", &n, &m);
        while (m--)
        {
            int a, b;
            fscanf(f, "%d%d", &a, &b);
            if (!s.count(a) || !s.count(b))
                continue;
            neighbors[a].insert(b);
            neighbors[b].insert(a);
        }
        fclose(f);
    }
    int get_k()
    {
        size_t k=s.size();
        for(T u:s)
            k=min(k, neighbors[u].size());
        return s.size()-k;
    }
    bool is_correct(int k)
    {
        for (T u : s)
        {
            if (neighbors[u].size() + k < s.size() )
                return false;
        }
        return true;
    }
};

#endif