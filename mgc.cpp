#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <assert.h>

#define MAX_VALS 160

using namespace std;

float ADJ_MATRIX[160][160] = {};
array<int, 160> BEST_ARR = {};
float BEST_VAL = 100000;
int RECURSION_CALLS = 0;
array<int, MAX_VALS> WORK = {};

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

int sumOccurences(const array<int, MAX_VALS> array, const int val, const int last)
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

void print_array(array<int, MAX_VALS> arr, int len)
{
    for (int j = 0; j < len; j++){
        cout << arr[j] << " ";
    }
    cout << endl;

}

float evaluateCut(const array<int, MAX_VALS> arr, const int len)
{
    float ret_sum = 0;
    for (int i = 0; i < len; ++i)
    {
        for (int j = 0; j < len; ++j)
        {
            if (j < i && arr[i] != arr[j]) ret_sum += ADJ_MATRIX[j][i];
        }

    }
    return ret_sum;
}

void solve(int fill_index, array<int, MAX_VALS> &current_solution)
// i is the index to be filled in
{
    ++RECURSION_CALLS;
    if (fill_index == n){
        float val = evaluateCut(current_solution, n);
        if (val < BEST_VAL)
        {
            BEST_VAL = val;
            for (int i = 0; i < n; ++i)
            {
                BEST_ARR[i] = current_solution[i];
            }
            return;
        }
    }
    if (evaluateCut(current_solution, fill_index) > BEST_VAL) return;
    current_solution[fill_index] = 0;
    int new_index = fill_index + 1;
    if (sumOccurences(current_solution, 0, new_index) <= n - max_ones) solve(new_index, current_solution);
    current_solution[fill_index] = 1;
    if (sumOccurences(current_solution, 1, new_index) <= max_ones) solve(new_index, current_solution);
    return;
}

int main(int argc, char* argv[])
{
    //TESTS
    array<int, MAX_VALS> test_arr = {0, 1, 0, 1, 0, 0, 0, 0, 0, 1};
    assert(sumOccurences(test_arr, 1, 10) == 3);
    assert(sumOccurences(test_arr, 0, 10) == 7);

    n = 2;
    ADJ_MATRIX[0][1] = 1;
    ADJ_MATRIX[1][0] = -1;
    ADJ_MATRIX[1][1] = -10;
    ADJ_MATRIX[0][0] = -10;
    array<int, MAX_VALS> cutA = {1, 0};
    array<int, MAX_VALS> cutB = {0, 0};
    assert(evaluateCut(cutA, 2) == 1);
    assert(evaluateCut(cutB, 2) == 0);
    
    //SOLUTION
    char* filename = argv[1];

    read_graph_file(filename);
    solve(0, WORK);


    cout << "======================================" << endl;
    cout << "Minimal cut: " << BEST_VAL << endl;
    cout << "Recursion calls: " << RECURSION_CALLS << endl;
    return 0;
}