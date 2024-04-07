#include <bits/stdc++.h>
#include <sys/time.h> // gettimeofday
#include <unistd.h>
#include <chrono>
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
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    return static_cast<long long>(microseconds.count());
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

string get_file_name_suffix(string f)
{
    string ret = "";
    bool has_dot = 0;
    for (char ch : f)
    {
        if (ch == '.')
        {
            has_dot = 1;
            ret = "";
        }
        else if (has_dot)
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

string input_file, file_name;
ll n, m;
vector<pii> edges;
ui *pstart, *edge_to, *deg;

void read_graph_mtx()
{
    ifstream in(input_file);
    assert(in.is_open());
    string line;
    do
    {
        getline(in, line);
    } while (line[0] == '%');
    // the first line should be n n m
    {
        stringstream ss(line);
        if (ss >> n >> n >> m)
        {
        }
        else
        {
            cout << input_file << " failed !!!!" << endl;
            exit(1);
        }
    }
    edges.resize(m << 1);
    unordered_map<int, int> v_map;
    int id_v = 0;
    ui idx = 0;
    for (ui i = 0; i < m; i++)
    {
        ui a, b;
        getline(in, line);
        stringstream ss(line);
        if (ss >> a >> b)
        {
            if (a == b)
                continue;
            if (!v_map.count(a))
                v_map[a] = id_v++;
            if (!v_map.count(b))
                v_map[b] = id_v++;
            a = v_map[a], b = v_map[b];
            edges[idx++] = {a, b};
            edges[idx++] = {b, a};
        }
    }
    edges.resize(idx);
    n = id_v;
    unique_pii(edges, n);
    m = edges.size();
    printf("read ok, n=%lld m= %lld\n", n, m);
    fflush(stdout);
}

void read_graph_no_suffix()
{
    freopen(input_file.c_str(), "r", stdin);
    n = read(), m = read();
    edges.resize(m * 2);
    vector<int> v_map(n + 1, -1);
    int id_v = 0;
    ui idx = 0;
    ui j = 0;
    for (ll i = 0; i < m; i++)
    {
        int a = read(), b = read();
        if (v_map[a] == -1)
            v_map[a] = id_v++;
        if (v_map[b] == -1)
            v_map[b] = id_v++;
        a = v_map[a], b = v_map[b];
        assert(a < n && b < n);
        if (a == b)
            continue;
        edges[j++] = {a, b};
        edges[j++] = {b, a};
    }
    edges.resize(j);
    unique_pii(edges, id_v);
    n = id_v;
    m = edges.size();
    printf("read ok, n=%lld m= %lld\n", n, m);
    fflush(stdout);
}

string strip(string &a)
{
    string ret = "";
    bool start = 1;
    for (char ch : a)
    {
        if (start)
        {
            if (ch == ' ' || ch == '\t')
                continue;
            start = 0;
        }
        ret += ch;
    }
    return ret;
}

bool read_two_ints(string &s, ui &a, ui &b)
{
    if (s[0] == ' ' || s[0] == '\t')
    {
        s = strip(s);
    }
    if (s.size() <= 2 || s[0] < '0' || s[0] > '9')
    {
        return false;
    }
    a = 0;
    b = 0;
    bool is_a = 1;
    for (char ch : s)
    {
        if (ch >= '0' && ch <= '9')
        {
            if (is_a)
            {
                a = a * 10 + ch - '0';
            }
            else
            {
                b = b * 10 + ch - '0';
            }
        }
        else
        {
            if (is_a)
                is_a = 0;
            else
            {
                return true;
            }
        }
    }
    if (is_a)
        return false;
    return true;
}

void read_graph_edges()
{
    ifstream in(input_file);
    assert(in.is_open());
    string line;
    do
    {
        getline(in, line);
    } while (line[0] == '%' || line[0] == '#' || line[0] == '/');
    unordered_map<int, int> v_map;
    int id_v = 0;
    do
    {
        ui a, b;
        if (read_two_ints(line, a, b))
        {
            if (!v_map.count(a))
                v_map[a] = id_v++;
            if (!v_map.count(b))
                v_map[b] = id_v++;
            a = v_map[a], b = v_map[b];
            if (a == b)
                continue;
            edges.push_back({a, b});
            edges.push_back({b, a});
        }
    } while (getline(in, line));
    n = id_v;
    unique_pii(edges, n);
    m = edges.size();
    printf("read ok, n=%lld m= %lld\n", n, m);
    fflush(stdout);
}

void read_graph()
{
    string suffix = get_file_name_suffix(input_file);
    if (suffix == "mtx")
    {
        read_graph_mtx();
        return;
    }
    else if (suffix.size() == 0)
    {
        read_graph_no_suffix();
        return;
    }
    else if (suffix == "edges")
    {
        read_graph_edges();
        return;
    }
    freopen(input_file.c_str(), "r", stdin);
    n = read(), m = read();
    edges.resize(m * 2);
    ll j = 0;
    for (ll i = 0; i < m; i++)
    {
        int a = read(), b = read();
        assert(a < n && b < n);
        if (a == b)
            continue;
        edges[j++] = {a, b};
        edges[j++] = {b, a};
    }
    edges.resize(j);
    unique_pii(edges, n);
    m = edges.size();
    printf("read ok, n=%lld m= %lld\n", n, m);
    fflush(stdout);
}

// for KPLEX and kPEX
void dump_bin()
{
    ui *deg = new ui[n];
    memset(deg, 0, sizeof(ui) * n);
    for (auto &h : edges)
        deg[h.x]++;
    ui size_int = sizeof(ui);
    ui v_n = n, v_m = m;
    ui *ne = new ui[edges.size()];
    for (ll i = 0; i < m; i++)
        ne[i] = edges[i].y;
    {
        string name = file_name + ".bin";
        FILE *out = fopen(name.c_str(), "wb");
        assert(out != nullptr);
        fwrite(&size_int, sizeof(ui), 1, out);
        fwrite(&v_n, sizeof(ui), 1, out);
        fwrite(&v_m, sizeof(ui), 1, out);
        fwrite(deg, sizeof(ui), n, out);
        fwrite(ne, sizeof(ui), m, out);
        fflush(out);
        fclose(out);
        cout << name << " bin ok" << endl;
    }
    delete[] deg;
    delete[] ne;
}

int main(int argc, char *argv[])
{
    input_file = string(argv[1]);
    file_name = get_file_name_without_suffix(input_file);
    cout << "File: " << file_name << endl;
    read_graph();
    dump_bin();

    return 0;
}