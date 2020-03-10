#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

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

float evaluateCut(int arr[], int len)
{
    float ret_sum = 0;
    int i, j;
    for (i = 1; i < len; i++)
    {
        for (j = 0; j < i; j++)
        {
            if (arr[i] != arr[j]) ret_sum += ADJ_MATRIX[j][i];
        }
    }
    return ret_sum;
}

void solve(int fill_index, int* current_solution)
// i is the index to be filled in
{
    ++RECURSION_CALLS;
    int curr_ones = sumOccurences(current_solution, 1, fill_index);
    if (curr_ones > max_ones) return;
    if (fill_index - curr_ones > n - max_ones) return;

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
    solve(new_index, current_solution);
    current_solution[fill_index] = 1;
    solve(new_index, current_solution);
    return;
}

int main(int argc, char* argv[])
{
    //TESTS
    int test_arr[10] = {0, 1, 0, 1, 0, 0, 0, 0, 0, 1};
    assert(sumOccurences(test_arr, 1, 10) == 3);
    assert(sumOccurences(test_arr, 0, 10) == 7);

    n = 2;
    ADJ_MATRIX[0][1] = 1;
    ADJ_MATRIX[1][0] = -1;
    ADJ_MATRIX[1][1] = -10;
    ADJ_MATRIX[0][0] = -10;
    int cutA[2] = {1, 0};
    int cutB[2] = {0, 0};
    assert(evaluateCut(cutA, 2) == 1);
    assert(evaluateCut(cutB, 2) == 0);
    
    //SOLUTION
    char* filename = argv[1];

    read_graph_file(filename);
    int arr[n] = {};
    PTR = arr;
    solve(0, arr);


    cout << "======================================" << endl;
    cout << "Minimal cut: " << BEST_VAL << endl;
    cout << "Recursion calls: " << RECURSION_CALLS << endl;
    return 0;
}