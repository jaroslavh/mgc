#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <cstring>
#include <omp.h>
#include <deque>
#include <vector>

#define MAX_VALS 160

using namespace std;

float ADJ_MATRIX[160][160] = {};
float BEST_VAL = 100000;
vector<int> * BEST_SOLUTION = NULL;

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

int sumOccurences(vector<int> &array, int val)
{
    int ret_sum = 0;
    for (int i = 0; i < array.size(); ++i)
    {
        //assert(array[i] < 2);
        //assert(array[i] >= 0);
        if (array[i] == val) ret_sum += 1;
    }
    return ret_sum;
}

void print_vector(vector<int> &arr)
{
    for (auto j = arr.begin(); j != arr.end(); j++){
        cout << *j << " ";
    }
    cout << endl;
}

float evaluateCutMinus(vector<int> &arr)
{
    float ret_sum = 0;
    for (int i = 0; i < arr.size() - 1; ++i) {
        for (int j = 0; j < i; j++) {
            if (arr[i] != arr[j]) ret_sum += ADJ_MATRIX[j][i];
        }
    }
    return ret_sum;
}

float evaluateNewNode(vector<int> &arr)
{
    float ret_sum = 0;
    int new_index = arr.size() -1;
    for (int i = 0; i < new_index; ++i)
    {
        if (arr[i] != arr[new_index]) ret_sum += ADJ_MATRIX[i][new_index];
    }
    return ret_sum;
}

void solve(vector<int> &cur, float prev_val, int prev_ones)
{
    if (prev_ones > max_ones) { //more ones than allowed
        delete &cur;
        return;
    }
    if (cur.size() - prev_ones > n - max_ones) { //more zeros than allowed
        delete &cur;
        return;
    }

    float new_val = evaluateNewNode(cur) + prev_val;
    if (new_val >= BEST_VAL) {
        delete &cur;
        return;
    }

    if (cur.size() == n){
        #pragma omp critical
        {
            BEST_VAL = new_val;
            delete BEST_SOLUTION;
            BEST_SOLUTION = new vector<int> (cur.begin(), cur.end());
        }
        delete &cur;
        return;
    }

    vector<int> * sol0 = new vector<int>(cur.begin(), cur.end());
    vector<int> * sol1 = new vector<int>(cur.begin(), cur.end());
    sol0->push_back(0);
    sol1->push_back(1);
    delete &cur;
        
    solve(*sol0, new_val, prev_ones);
    solve(*sol1, new_val, prev_ones + 1);
    return;
}

void generate_states(deque<vector<int>*> &q)
{
    vector<int> * working = q.front();
    vector<int> * zero = new vector<int>(*working);
    zero->push_back(0);
    q.push_back(zero);
    vector<int> * one = new vector<int>(*working);
    one->push_back(1);
    q.push_back(one);
    q.pop_front();
    delete working;
}

int main(int argc, char* argv[])
{
    // reading input filgge
    char* filename = argv[1];
    read_graph_file(filename);

    deque<vector<int> *> q;
    vector<int> * first = new vector<int>({0});
    vector<int> * second = new vector<int>({1});

    q.push_back(first);
    q.push_back(second);
    while (q.size() < 256) { //index 8
        generate_states(q);
    }

    vector<vector<int> *> vect(q.begin(), q.end());
    int vect_size = vect.size();
    #pragma omp parallel for
    for (int i = 0; i < vect_size; ++i) {
        float val = evaluateCutMinus(*vect[i]);
        int ones = sumOccurences(*vect[i], 1);
        solve(*vect[i], val, ones);
    }
    // printing result
    cout << "======================================" << endl;
    cout << "Minimal cut: " << BEST_VAL << endl;
    print_vector(*BEST_SOLUTION);
    return 0;
}