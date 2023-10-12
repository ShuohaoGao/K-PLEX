#include <fstream>
#include <sstream>
#include <dirent.h>
#include "../Utility.h"

using namespace std;

void processFile(const string &filePath)
{
    cout<<filePath<<endl;
    FILE* in=fopen(filePath.c_str(), "r");
    int n,m,lb;
    fscanf(in, "%d%d%d",&n, &m, &lb);
    if(!n)
    {
        fclose(in);
        return;
    }
    set<pii> edges;
    while(m--)
    {
        ui a,b;
        fscanf(in, "%u %u",&a, &b);
        edges.insert({a,b});
        edges.insert({b,a});
    }
    vector<ui> idx(n, 1);
    for(int i=0;i<n;i++)
        fscanf(in, "%u", &idx[i]);
    fclose(in);
    FILE* out=fopen(filePath.c_str(), "w");
    fprintf(out, "%d %d %d\n", n, (int)edges.size(), lb);
    for(auto h:edges)
        fprintf(out, "%u %u\n",h.x, h.y);
    fprintf(out, "\n");
    for(ui x:idx)
        fprintf(out, "%u\n", x);
    fflush(out);
    fclose(out);
}

int main()
{
    string directoryPath = "../reduced_graph/";

    DIR *directory = opendir(directoryPath.c_str());
    if (directory)
    {
        struct dirent *entry;
        while ((entry = readdir(directory)) != nullptr)
        {
            string fileName = entry->d_name;
            if (fileName != "." && fileName != "..")
            {
                string filePath = directoryPath + fileName;
                processFile(filePath);
            }
        }
        closedir(directory);
    }
    else
    {
        cerr << "无法打开目录 " << directoryPath << endl;
    }

    return 0;
}
