#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

float adjacency_matrix[160][160] = {};
int n = 0; //number of nodes
int k = 0; //average node order
int a = 0; //size of graph partition

vector<string> split(const string &str, const char &delim) {
    typedef string::const_iterator iter;
    iter beg = str.begin();
    vector<string> tokens;

    while(beg != str.end())
    {
        while ((beg != str.end()) && (*beg == delim)) beg ++;
        iter temp = find(beg, str.end(), delim);
        if(beg != str.end()) tokens.push_back(string(beg, temp));
        beg = temp;
    }
    return tokens;
}

int read_graph_file(char* graph_file)
//reads given graph_file and loads its contents to global adjacency matrix
{
    string line;
    ifstream in_file(graph_file);
    if (in_file.is_open())
    {
        getline(in_file, line);
        vector<string> x = split(line, ' ');
        n = stoi(x[0]);
        k = stoi(x[1]);
        a = stoi(x[2]);
        cout << "Parameters are: n=" << n << " k=" << k << " a=" << a << " " << endl; 

        while(getline(in_file, line))
        {
            vector<string> y = split(line, ' ');
            int i0 = stoi(y[0]);
            int i1 = stoi(y[1]);
            float edge = atof(y[2].c_str());
            adjacency_matrix[i0][i1] = edge;
        }
    }
}

int main(int argc, char* argv[])
{
    char* filename = argv[1];

    read_graph_file(filename);

    return 0;
}