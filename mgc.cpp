#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

using namespace std;

float ADJ_MATRIX[160][160] = {};
int BEST_ARR[160] = {};
float BEST_VAL = 100000;

int n = 0; //number of nodes
int k = 0; //average node order
int max_ones = 0; //size of graph partition marked as 1


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
        max_ones = stoi(x[2]);
        cout << "Parameters are: n=" << n << " k=" << k << " a=" << max_ones << " " << endl; 

        while(getline(in_file, line))
        {
            vector<string> y = split(line, ' ');
            int i0 = stoi(y[0]);
            int i1 = stoi(y[1]);
            float edge = atof(y[2].c_str());
            ADJ_MATRIX[i0][i1] = edge;
        }
    }
}

int sumOccurences(const int* array, const int val, const int last)
{
    int ret_sum = 0;
    for (int i = 0; i < last; ++i)
    {
        assert(array[i] < 2);
        assert(array[i] >= 0);
        if (array[i] == val) ret_sum += 1;
    }
    return ret_sum;
}

float evaluateCut(const int* array)
{
    float ret_sum = 0;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (j < i && array[i] != array[j]) ret_sum += ADJ_MATRIX[i][j];
        }

    }
    return ret_sum;
}

void solve(int i, int* current_solution)
// i is the index to be filled in
{
    if (i == n){
        int val;
        val = evaluateCut(current_solution);
    }
    if (sumOccurences(current_solution, 1, i) > max_ones || \
        sumOccurences(current_solution, 0, i) > n - max_ones)
        {
            return;
        }
}

int main(int argc, char* argv[])
{
    //TESTS
    int test_arr[10] = {0, 1, 0, 1, 0, 0, 0, 0, 0, 1};
    assert(sumOccurences(test_arr, 1, 10) == 3);
    assert(sumOccurences(test_arr, 0, 10) == 7);

    n = 2;
    ADJ_MATRIX[0][1] = -1;
    ADJ_MATRIX[1][0] = 1;
    ADJ_MATRIX[1][1] = -10;
    ADJ_MATRIX[0][0] = -10;
    int cutA[2] = {1, 0};
    int cutB[2] = {0, 0};
    assert(evaluateCut(cutA) == 1);
    assert(evaluateCut(cutB) == 0);
    
    //SOLUTION
    char* filename = argv[1];

    read_graph_file(filename);



    

    return 0;
}