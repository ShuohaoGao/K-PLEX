#include <bits/stdc++.h>

#define x first
#define y second

using namespace std;

using ll=long long;
using pii=pair<int,int>;
using ui=uint32_t;

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


//==============================================
string input_file, file_name;
ll n,m;
vector<pii> edges;

int read()
{
    int ret=0;
    char ch;
    while((ch=getchar()) <'0' || ch>'9');
    while(ch>='0' && ch<='9')
    {
        ret=ret*10+ch-'0';
        ch=getchar();
    }
    return ret;
}

void read_graph()
{
    freopen(input_file.c_str(), "r", stdin);
    n=read(), m=read();
    edges.resize(m*2);
    for(ll i=0, j=0;i<m;i++)
    {
        int a=read(), b=read();
        assert(a<n && b<n);
        if(a==b) continue;
        edges[j++]={a,b};
        edges[j++]={b,a};
    }
    unique_pii(edges, n);
    m=edges.size();
    printf("read ok, n=%lld m= %lld\n",n,m);
    fflush(stdout);
}

void dump_gap()
{
    string name="./for-gap/"+file_name+".bin";
    FILE* out=fopen(name.c_str(), "wb");
    ui *deg=new ui[n];
    memset(deg, 0, sizeof(ui)*n);
    for(auto &h:edges)
        deg[h.x]++;
    ui size_int=sizeof(ui);
    fwrite(&size_int, sizeof(ui), 1, out);
    ui v_n=n, v_m=m;
    fwrite(&v_n, sizeof(ui), 1, out);
    fwrite(&v_m, sizeof(ui), 1, out);
    ui *ne=new ui[edges.size()];
    for(ll i=0;i<m;i++)
        ne[i]=edges[i].y;
    fwrite(deg, sizeof(ui), n, out);
    fwrite(ne, sizeof(ui), m, out);
    fflush(out);
    fclose(out);
    delete[] deg;
    delete[] ne;
    cout<<name<<" gap ok"<<endl;
}

void dump_DPBB()
{
    string name="./for-DPBB/"+file_name+".out";
    FILE* out=fopen(name.c_str(), "w");
    fprintf(out, "%lld %lld\n", n, m);
    for(auto &h:edges)
        fprintf(out, "%d %d\n", h.x, h.y);
    fflush(out);
    fclose(out);
    cout<<name<<" DPBB ok"<<endl;
}

int main(int argc, char *argv[])
{
    input_file = string(argv[1]);
    file_name=get_file_name_without_suffix(input_file);
    cout<<file_name<<endl;
    read_graph();
    dump_DPBB();
    dump_gap();


    return 0;
}