#include "../Utility.h"

const int N=500;
int n,m;
vector<vector<bool>> g;
set<int> S,P;
// unordered_map<int,int> common_neighbor;

int erfen(map<int,int> &d)
{
    if(P.size()<=paramK)
        return P.size();
    vector<int> cnt(n, 0);
    for(int u:P)
        cnt[d[u]]++;
    for(int i=P.size();i>=0;i--)
        cnt[i]+=cnt[i+1];
    for(int i=P.size();i>=0;i--)
    {
        if(cnt[i-paramK] >= i)
        {
            return i;
        }
        assert(i>paramK);
    }
    return 0;
}

int get_ub_P()
{
    map<int,int> d;
    for(int u:P)
    {
        for(int v:P)
            if(g[u][v])
                d[u]++;
    }
    int ret=erfen(d);
    return ret;
}

void weak_reduce(map<int,int> &d, int lb)
{
    queue<int> q;
    vector<bool> rm(n);
    for(int u:S)
        if(d[u] + paramK< lb)
        {
            q.push(u);
            rm[u]=1;
        }
    while(q.size())
    {
        int u=q.front();
        q.pop();
        for(int v:S)
        {
            if(rm[v]) continue;
            if(g[u][v])
            {
                if(--d[v] +paramK < lb)
                {
                    q.push(v);
                    rm[v]=1;
                }
            }
        }
    }
    for(int i=0;i<n;i++)
        if(rm[i])
            S.erase(i);
}

void reduce_S(int lb)
{
    map<int,int> d;
    for(int u:S)
    {
        for(int v:S)
            if(g[u][v])
                d[u]++;
    }
    weak_reduce(d, lb);
    vector<vector<int>> cn(n);
    for(auto &s:cn)
        s.resize(n, 0);
    for(int u:S)
    {
        for(int v:S)
        {
            if(!g[u][v]) continue;
            if(v>=u)
                break;
            else 
            {
                for(int w:S)
                {
                    if(g[u][w] && g[v][w])
                        cn[u][v]++;
                }
                if(cn[u][v] + 2*paramK < lb)
                {
                    d[u]--, d[v]--;
                }
            }
        }
    }
    weak_reduce(d, lb);
}

void reduce_P(int lb)
{
    map<int,int> cnt;
    set<int> rm;
    for(int u:P)
    {
        for(int v:S)
            if(g[u][v])
                cnt[u]++;
        if(cnt[u]+paramK-2 < lb)
        {
            rm.insert(u);
        }
    }
    for(int u:rm)
        P.erase(u);
}

int select_u()
{
    map<int,int> cnt, d;
    for(int u:S)
    {
        for(int v:P)
            if(!g[u][v])
                cnt[u]++;
        for(int v=0;v<n;v++)
            if(g[u][v])
                d[u]++;
    }
    int sel=*S.begin();
    for(int u:S)
    {
        if(cnt[u]>cnt[sel] || cnt[u]==cnt[sel] && d[u]<d[sel])
            sel=u;
    }
    return sel;
}

int main()
{
    paramK=2;
    string file="..\\reduced_graph\\WikiTalk_K=2.out";
    freopen(file.c_str(), "r", stdin);
    cin>>n>>m>>lb;
    g.resize(n);
    for(auto &s:g)
        s.resize(n);
    for(int i=0;i<m;i++)
    {
        int a,b;
        cin>>a>>b;
        g[a][b]=g[b][a]=1;
    }
    for(int i=0;i<n;i++)
        S.insert(i);


    vector<bool> rm(n, 0);
    while(S.size()>lb)
    {
        P.clear();
        int u=select_u();
        rm[u]=1;
        S.erase(u);
        for(int v:S)
            if(!g[u][v])
                P.insert(v);
        for(int v:P)
            S.erase(v);
        int pre_S=S.size();
        int pre_P=P.size();
        reduce_S(lb-paramK+1);
        reduce_P(lb-paramK+1);
        cout<<pre_S<<' '<<S.size()<<"  "<<pre_P<<' '<<P.size()<<"   "<<(int)S.size()+(int)P.size()-lb<<endl;
        for(int i=0;i<n;i++)
            if(!rm[i])
                S.insert(i);
    }
    


    return 0;
}