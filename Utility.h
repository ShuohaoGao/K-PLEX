#ifndef UTILITY_H
#define UTILITY_H

#include <bits/stdc++.h>
#include <sys/time.h> // gettimeofday
#include <unistd.h>
#define x first
#define y second

using namespace std;
using ll = long long;
using ui = unsigned int;
using pii = pair<ui, ui>;

const int INF = 0x3f3f3f3f;

int paramK;
int lb;

double list_triangle_time;

inline ll get_system_time_microsecond()
{
    static struct timeval timestamp = {};
    gettimeofday(&timestamp, NULL);
    return timestamp.tv_sec * 1000000 + timestamp.tv_usec;
    if (0 == gettimeofday(&timestamp, NULL))
        return timestamp.tv_sec * 1000000 + timestamp.tv_usec;
    else
        return 0;
}

string get_file_name(string str)
{
    string ret = "";
    for (char ch : str)
    {
        ret += ch;
        if (ch == '\\' || ch == '/')
            ret = "";
    }
    return ret;
}

string get_file_name_without_suffix(string name)
{
    name = get_file_name(name);
    string ret = "";
    for (char ch : name)
    {
        if (ch == '.')
            break;
        ret += ch;
    }
    return ret;
}

/**
 * faster than std::sort
 * We first sort using "second(y)", then sort using "first(x)"
 * because the countingSort is stable, the final array is sorted in the ascending order
 */
void countingSort(vector<pii> &a, int k)
{
    ui *cnt = new ui[k];
    // fisrt, we sort using y
    memset(cnt, 0, sizeof(ui) * k);
    for (auto &h : a)
        cnt[h.y]++;
    for (int i = 1; i < k; i++)
        cnt[i] += cnt[i - 1];
    vector<pii> out(a.size());
    for (ll i = (ll)a.size() - 1; i >= 0; i--)
        out[cnt[a[i].y] - 1] = a[i], cnt[a[i].y]--;
    // next, we sort using x
    memset(cnt, 0, sizeof(ui) * k);
    for (auto &h : out)
        cnt[h.x]++;
    for (int i = 1; i < k; i++)
        cnt[i] += cnt[i - 1];
    for (ll i = (ll)a.size() - 1; i >= 0; i--)
        a[cnt[out[i].x] - 1] = out[i], cnt[out[i].x]--;
    delete[] cnt;
}

void unique_pii(vector<pii> &a, int n)
{
    countingSort(a, n);
    a.erase(unique(a.begin(), a.end()), a.end());
}

template <typename T>
void unique_vector(vector<T> &a)
{
    sort(a.begin(), a.end());
    a.erase(unique(a.begin(), a.end()), a.end());
}

template <typename T>
bool has(vector<T> &a, T x)
{
    auto it = lower_bound(a.begin(), a.end(), x);
    return it != a.end() && *it == x;
}

template <typename T>
bool has(T *st, T *ed, T x)
{
    auto it = lower_bound(st, ed, x);
    return it != ed && (*it) == x;
}

template <typename T>
int find(vector<T> &a, T x)
{
    return lower_bound(a.begin(), a.end(), x) - a.begin();
}

template <typename T>
int find(T *st, T *ed, T x)
{
    return lower_bound(st, ed, x) - st;
}

template <typename T>
int intersect_count(T *a, T *b, T *l, T *r) // return [a,b) ∩ [l,r)
{
    int ret = 0;
    while (a < b && l < r)
    {
        if ((*a) == (*l))
            ret++, a++, l++;
        else if ((*a) > (*l))
            l++;
        else
            a++;
    }
    return ret;
}

void print_progress_bar(double percentage, bool enter=false)
{
#ifdef NO_PROGRESS_BAR
    return;
#endif
    const int bar_length = 50;
    const char fill_char = '#';
    const char empty_char = '-';
    int filled_length = (int)(percentage * bar_length);
    int empty_length = bar_length - filled_length;
    char buffer[bar_length + 10];
    sprintf(buffer, "\r%.2lf%% [", percentage * 100);
    int len = strlen(buffer);
    for (int i = 0; i < filled_length; i++)
    {
        buffer[len + i] = fill_char;
    }
    for (int i = 0; i < empty_length; i++)
    {
        buffer[len + filled_length + i] = empty_char;
    }
    buffer[bar_length + len] = ']';
    write(STDOUT_FILENO, buffer, bar_length + len + 1);
    fflush(stdout);
    if(enter)
        cout<<endl;
}

template <typename T>
void print_set(set<T> &s)
{
    auto it = s.begin();
    cout << (*it);
    it++;
    while (it != s.end())
    {
        cout << ", " << (*it);
        it++;
    }
    cout << endl;
}

template <typename T>
int countBits(T x)
{
    int count = 0;
    while (x)
    {
        x &= (x - 1);
        count++;
    }
    return count;
}

void print_time(double start)
{
    printf("use time %.4lf s\n", (get_system_time_microsecond() - start) / 1e6);
    fflush(stdout);
}

void print_module_time(string name, double time)
{
    printf("%s-time: %.4lf s  ", name.c_str(), time / 1e6);
}

ui read()
{
    char ch;
    while ((ch = getchar()) < '0' || ch > '9')
        ;
    ui ret = 0;
    while (ch >= '0' && ch <= '9')
    {
        ret = ret * 10 + ch - '0';
        ch = getchar();
    }
    return ret;
}

#endif