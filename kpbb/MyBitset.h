#ifndef MY_BITSET_H
#define MY_BITSET_H

#include "Utility.h"

/**
 * @brief bitset, which supports & | bit-operation in O(n/64).
 * we provide an iterator too
 */
class MyBitset
{
public:
    int n; // the size of buff is n+1
    int m; // valid size in last int
    int capacity;
    int range;
    uint64_t *buf;

    MyBitset() : n(0), m(0), capacity(0), buf(nullptr) {}

    MyBitset(int _range) : range(_range), n(_range >> 6), m(_range & 63), capacity(_range)
    {
        buf = new uint64_t[n + 1];
        memset(buf, 0, sizeof(uint64_t) * (n + 1));
    }

    MyBitset(const MyBitset &other) : range(other.range), n(other.n), m(other.m), capacity(other.capacity)
    {
        buf = new uint64_t[n + 1];
        memcpy(buf, other.buf, sizeof(uint64_t) * (n + 1));
    }

    MyBitset &operator=(const MyBitset &other)
    {
        if (this == &other)
            return *this;
        if (buf != nullptr)
        {
            delete[] buf;
            buf = nullptr;
        }
        range = other.range;
        n = other.n;
        m = other.m;
        capacity = other.capacity;
        buf = new uint64_t[n + 1];
        memcpy(buf, other.buf, sizeof(uint64_t) * (n + 1));
        return *this;
    }

    ~MyBitset()
    {
        if (buf != nullptr)
        {
            delete[] buf;
            buf = nullptr;
        }
    }

    void clear()
    {
        memset(buf, 0, sizeof(uint64_t) * (n + 1));
    }

    void flip()
    {
        for (int i = 0; i < n; ++i)
            buf[i] = ~buf[i];
        buf[n] ^= (1ULL << m) - 1;
    }

    void set(int x)
    {
        assert(x < capacity);
        assert(!test(x));
        buf[x >> 6] |= 1ULL << (x & 63);
    }

    void reset(int x)
    {
        assert(x < capacity);
        assert(test(x));
        buf[x >> 6] &= ~(1ULL << (x & 63));
    }

    bool test(int x) const
    {
        assert(x < capacity);
        return (buf[x >> 6] >> (x & 63)) & 1ULL;
    }

    bool has(int x) const
    {
        assert(x < capacity);
        return (buf[x >> 6] >> (x & 63)) & 1ULL;
    }

    bool empty() const
    {
        return std::all_of(buf, buf + n + 1, [](uint64_t val)
                           { return val == 0ULL; });
    }

    void operator&=(const MyBitset &other)
    {
        assert(n == other.n);
        std::transform(buf, buf + n + 1, other.buf, buf, std::bit_and<uint64_t>());
    }

    void operator|=(const MyBitset &other)
    {
        assert(n == other.n);
        std::transform(buf, buf + n + 1, other.buf, buf, std::bit_or<uint64_t>());
    }

    void operator^=(const MyBitset &other)
    {
        assert(n == other.n);
        std::transform(buf, buf + n + 1, other.buf, buf, std::bit_xor<uint64_t>());
    }

    bool operator[](int x) const
    {
        assert(x < capacity);
        return (buf[x >> 6] >> (x & 63)) & 1ULL;
    }

    int size()
    {
        return std::accumulate(buf, buf + n + 1, 0, [](int sum, uint64_t val)
                               { return sum + __builtin_popcountll(val); });
        //    { return sum + countBits(val); });
    }

    int intersect(const MyBitset &other)
    {
        return std::inner_product(buf, buf + n + 1, other.buf, 0, std::plus<int>(), [](uint64_t a, uint64_t b)
                                  { return __builtin_popcountll(a & b); });
        //   { return countBits(a & b); });
    }

    int intersect(const MyBitset &a, const MyBitset &b)
    {
        int ret = 0;
        for (int i = 0; i <= n; i++)
            ret += __builtin_popcountll((buf[i] & a.buf[i]) & b.buf[i]);
        return ret;
    }

    class Iterator
    {
    private:
        int index;
        const MyBitset &bitset;

    public:
        Iterator(int idx, const MyBitset &bs) : index(idx), bitset(bs)
        {
        }

        Iterator &operator++()
        {
            if (index < bitset.capacity - 1)
            {
                index++;
                int block_index = index >> 6;
                uint64_t mask = bitset.buf[block_index] >> (index & 63);
                while (mask == 0 && (block_index) < bitset.n)
                {
                    mask = bitset.buf[++block_index];
                    index = block_index << 6; // Move to the next block of 64 bits
                }
                if (mask != 0)
                {
                    index += __builtin_ctzll(mask); // Find the next set bit using __builtin_ctzll
                }
                else
                    index = bitset.capacity;
            }
            else
            {
                index = bitset.capacity;
            }
            return *this;
        }

        int operator*() const
        {
            return index;
        }

        bool operator!=(const Iterator &other) const
        {
            return index != other.index;
        }
    };

    Iterator begin() const
    {
        int idx = 0;
        int blockIndex = 0;
        uint64_t mask = buf[blockIndex];
        while (mask == 0 && blockIndex < n)
        {
            mask = buf[++blockIndex];
            idx += 64; // Move to the next block of 64 bits
        }
        if (mask != 0)
        {
            idx += __builtin_ctzll(mask); // Find the first set bit using __builtin_ctzll
        }
        else
            idx = capacity;
        return Iterator(idx, *this);
    }

    Iterator end() const
    {
        return Iterator(capacity, *this);
    }
};

/**
 * @brief use n*n bits to store a graph
 */
class AjacentMatrix
{
public:
    int n;
    vector<MyBitset> A;

    AjacentMatrix() {}
    AjacentMatrix(int _n) : n(_n), A(_n, MyBitset(_n))
    {
        // for (auto &s : A)
        //     s = MyBitset(n);
    }
    ~AjacentMatrix() {}

    AjacentMatrix &operator=(const AjacentMatrix &other)
    {
        n = other.n;
        A = other.A;
        return *this;
    }

    void add_edge(int a, int b)
    {
        A[a].set(b);
        A[b].set(a);
    }

    bool exist_edge(int a, int b)
    {
        return A[a][b];
    }

    MyBitset &operator[](int x)
    {
        return A[x];
    }

    MyBitset &neighbors(int u)
    {
        return A[u];
    }

    int degree(int u)
    {
        return A[u].size();
    }

    void flip()
    {
        for (auto &s : A)
            s.flip();
    }
};

#endif