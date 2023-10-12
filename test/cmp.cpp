#include "../Utility.h"

string input_file, file_name;
vector<vector<bool>> g;
int n, m;

void readFromFile(string file)
{
    freopen(file.c_str(), "r", stdin);
    cin >> n >> m >> lb;
    g.resize(n);
    for (auto &s : g)
        s.resize(n);
    for (int i = 0; i < m; i++)
    {
        int a, b;
        cin >> a >> b;
        g[a][b] = g[b][a] = 1;
    }
}

int erfen(set<int> &P)
{
    map<int, int> d;
    for (int u : P)
    {
        for (int v : P)
            if (g[u][v])
                d[u]++;
    }
    if (P.size() <= paramK)
        return P.size();
    vector<int> cnt(n, 0);
    for (int u : P)
        cnt[d[u]]++;
    for (int i = P.size(); i >= 0; i--)
        cnt[i] += cnt[i + 1];
    for (int i = P.size(); i >= 0; i--)
    {
        if (cnt[i - paramK] >= i)
        {
            return i;
        }
        assert(i > paramK);
    }
    return 0;
}

int partUB(set<int> &S)
{
    map<int, set<int>> pi;
    for (int i = 0; i < n; i++)
    {
        if (S.count(i))
            continue;
        int sel = n;
        for (int v : S)
            if (!g[i][v])
            {
                sel = v;
                break;
            }
        pi[sel].insert(i);
    }
    set<int> pi0(pi[n]);
    for (int v : S)
        pi0.insert(v);
    int ret = erfen(pi0);
    map<int, int> d;
    for (int u : S)
    {
        for (int v : S)
            if (g[u][v])
                d[u]++;
    }
    for (int v : S)
        ret += min((int)pi[v].size(), paramK - (int)S.size() + d[v]);
    cout<<"  erfen: ";
    if(ret<=lb)
        cout<<"1"<<endl;
    else cout<<"0"<<endl;
    return ret;
}

int reduce(set<int> &s, int cnt)
{
    map<int, int> d;
    queue<int> q;
    map<int, bool> rm;
    for (int u : s)
    {
        for (int v : s)
            if (g[u][v])
                d[u]++;
        if (d[u] + paramK < cnt)
        {
            q.push(u);
            rm[u] = 1;
        }
    }
    while (q.size())
    {
        int u = q.front();
        q.pop();
        for (int i = 0; i < n; i++)
            if (g[u][i] && !rm[i])
            {
                if (--d[i] + paramK < cnt)
                {
                    q.push(i);
                    rm[i] = 1;
                }
            }
    }
    for (auto h : rm)
        s.erase(h.x);
    return s.size();
}

void reduce_ub(set<int> &S)
{
    map<int, set<int>> pi;
    for (int i = 0; i < n; i++)
    {
        if (S.count(i))
            continue;
        int sel = n;
        for (int v : S)
            if (!g[i][v])
            {
                sel = v;
                break;
            }
        pi[sel].insert(i);
    }
    int cnt = lb + 1 - S.size();
    map<int, int> d;
    for (int u : S)
    {
        for (int v : S)
            if (g[u][v])
                d[u]++;
    }
    for (int v : S)
        cnt -= min((int)pi[v].size(), paramK - (int)S.size() + d[v]);
    reduce(pi[n], cnt);
    if(pi[n].size()<cnt)
        cout<<"reduce: 1  ";
    else cout<<"reduce: 0  ";
}

void cmp(set<int> S)
{
    reduce_ub(S);
    partUB(S);
}

int main(int argc, char *argv[])
{
    // input_file = string(argv[1]);
    input_file = "..\\reduced_graph\\WikiTalk_K=5.out";
    file_name = get_file_name_without_suffix(input_file);
    cout << file_name << endl;
    // paramK=atoi(argv[2]);
    paramK = 5;

    readFromFile(input_file);
    cout << n << ' ' << lb << endl;

    int cnt = 200;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            // for (int k = j + 1; k < n; k++)
            {
                if (--cnt <= 0)
                    break;
                set<int> S{i, j};
                cmp(S);
            }
        }
    }

    return 0;
}