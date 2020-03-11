#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <cstring>

#define MAX_VALS 160

using namespace std;

float ADJ_MATRIX[160][160] = {};
int BEST_ARR[150] = {};
float BEST_VAL = 100000;
int RECURSION_CALLS = 0;
int *PTR = NULL;

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
    cout << "Reading file: " << graph_file << endl;
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

int sumOccurences(int* array, int val, int last)
{
    int ret_sum = 0;
    for (int i = 0; i < last; ++i)
    {
        //assert(array[i] < 2);
        //assert(array[i] >= 0);
        if (array[i] == val) ret_sum += 1;
    }
    return ret_sum;
}

void print_array(int* arr, int len)
{
    for (int j = 0; j < len; j++){
        cout << arr[j] << " ";
    }
    cout << endl;
}

float evaluateNewNode(int arr[], int new_index)
{
    float ret_sum = 0;
    for (int i = 0; i < new_index; ++i)
    {
        if (arr[i] != arr[new_index]) ret_sum += ADJ_MATRIX[i][new_index];
    }
    return ret_sum;
}

void solve(int fill_index, int* current_solution, float prev_val, int prev_ones)
// i is the index to be filled in
{
    ++RECURSION_CALLS;
    if (prev_ones > max_ones) return;
    if (fill_index - prev_ones > n - max_ones) return;

    float new_val = evaluateNewNode(current_solution, fill_index - 1) + prev_val;
    if (new_val > BEST_VAL) return;

    if (fill_index == n){
        if (new_val < BEST_VAL)
        {
            BEST_VAL = new_val;
            memcpy(BEST_ARR, current_solution, n*sizeof(int));
            return;
        }
    }

    current_solution[fill_index] = 0;
    solve(fill_index + 1, current_solution, new_val, prev_ones);
    current_solution[fill_index] = 1;
    solve(fill_index + 1, current_solution, new_val, prev_ones + 1);
    return;
}

int main(int argc, char* argv[])
{
    //SOLUTION
    char* filename = argv[1];

    read_graph_file(filename);
    int arr[n] = {};
    PTR = arr;
    solve(0, arr, 0, 0);


    cout << "======================================" << endl;
    cout << "Minimal cut: " << BEST_VAL << endl;
    cout << "Recursion calls: " << RECURSION_CALLS << endl;
    return 0;
}