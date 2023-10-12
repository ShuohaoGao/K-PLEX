#include "Branch.h"

string file_path;
Graph_input G;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("3 params are required !!! \n");
        exit(1);
    }
    file_path = string(argv[1]);
    paramK = atoi(argv[2]);
    G.read_from_file(file_path);

    Branch branch(G, lb-G.must_contain.size());
    branch.IE_framework();

    return 0;
}