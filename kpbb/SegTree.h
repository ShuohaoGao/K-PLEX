#ifndef SEGTREE_H
#define SEGTREE_H

#include "Utility.h"

// segment tree: modify a point / query an interval in O(log n)
class SegTree
{
    struct node
    {
        ll v; // neighbor_cnt* n + degree
        int id;
    };
    node *tr;
    int *leaf;
    int *val;
    int n;
    inline void pushup(int u)
    {
        auto &fa = tr[u], &l = tr[u << 1], &r = tr[u << 1 | 1];
        if (l.v > r.v)
            fa.v = l.v, fa.id = l.id;
        else
            fa.v = r.v, fa.id = r.id;
    }
    void build(int u, int l, int r)
    {
        tr[u] = {val[l], l};
        if (l == r)
        {
            leaf[l] = u;
            return;
        }
        int mid = (l + r) >> 1;
        build(u << 1, l, mid);
        build(u << 1 | 1, mid + 1, r);
        pushup(u);
    }

public:
    SegTree(ui _n, ui *deg, ui *neighbor, ui cnt) : n(_n)
    {
        tr = new node[n * 4];
        leaf = new int[n];
        val = new int[n];
        for (ui i = 0; i < n; i++)
            val[i] = deg[i];
        for (ui i = 0; i < cnt; i++)
            val[neighbor[i]] += n;
        build(1, 0, n - 1);
        delete[] val;
        val = nullptr;
    }
    ~SegTree()
    {
        if (tr != nullptr)
            delete[] tr;
        if (leaf != nullptr)
            delete[] leaf;
        if (val != nullptr)
            delete[] val;
    }
    void remove(int p)
    {
        p = leaf[p];
        tr[p].v = -INF;
        while (p > 1)
        {
            p >>= 1;
            pushup(p);
        }
    }
    void add(int p) // val[p]+=n
    {
        p = leaf[p];
        tr[p].v += n;
        while (p > 1)
        {
            p >>= 1;
            pushup(p);
        }
    }
    ui get()
    {
        return tr[1].id;
    }
    ui get_cnt()
    {
        return tr[1].v/n;
    }
};

#endif