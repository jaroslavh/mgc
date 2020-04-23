#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <cstring>
#include <omp.h>
#include <mpi.h>
#include <deque>
#include <vector>
#include <string>

#define MAX_VALS 160
#define SIZE 50

using namespace std;

float ADJ_MATRIX[160][160] = {};
float BEST_VAL = 100000;

int n = 0; //number of nodes
int k = 0; //average node order
int max_ones = 0; //size of graph partition marked as 1
int tag_work = 0;
int tag_done = 1;
MPI_Status status;
int tmp_arr[SIZE];
float result = 1000000;

void print_array(int* arr, int len)
{
    for (int j = 0; j < len; j++){
     //   if (arr[j] == -1) break;
        cout << arr[j] << " ";
    }
    cout << endl;
}

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
    //cout << "Reading file: " << graph_file << endl;
    string line;
    ifstream in_file(graph_file);
    if (in_file.is_open())
    {
        getline(in_file, line);
        vector<string> x = split(line, ' ');
        n = stoi(x[0]);
        k = stoi(x[1]);
        max_ones = stoi(x[2]);
        //cout << "Parameters are: n=" << n << " k=" << k << " a=" << max_ones << " " << endl; 

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

int * vect2arr(vector<int> &v)
{
    int* arr = new int[SIZE];
    for (int i = 0; i < v.size(); ++i) {
        arr[i] = v[i];
    }
    arr[v.size()] = -1;
    return arr;
}

void get_vals(int arr[], int &ones, int &last, float& val)
{
    int ones_sum = 0;
    int index = -1;
    for (int i = 0; i < n; ++i)
    {
        if (arr[i] == -1) {
            index = i;
            break;
        }
        if (arr[i] == 1) ones_sum += 1;
    }
    if (index == -1) last = n;
    else last = index;
    ones = ones_sum;

    float ret_sum = 0;
    for (int i = 0; i < last-1; ++i) {
        for (int j = 0; j < i; j++) {
            if (arr[i] != arr[j]) ret_sum += ADJ_MATRIX[j][i];
        }
    }
    val = ret_sum;
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
    int sequential_threshold = 8;
    if (prev_ones > max_ones) return;
    if (fill_index - prev_ones > n - max_ones) return;

    float new_val = evaluateNewNode(current_solution, fill_index - 1) + prev_val;
    if (new_val > BEST_VAL) return;

    if (fill_index == n){
        if (new_val < BEST_VAL)
        {
            #pragma omp critical
            {
                BEST_VAL = new_val;
            }
            return;
        }
    }

    current_solution[fill_index] = 0;

    int arr_0[SIZE] = {};
    for (int i = 0; i <= fill_index; ++i) arr_0[i] = current_solution[i];

    if (n - fill_index > sequential_threshold){
        #pragma omp task
        solve(fill_index + 1, arr_0, new_val, prev_ones);
    } else {
        solve(fill_index + 1, arr_0, new_val, prev_ones);
    }

    current_solution[fill_index] = 1;

    int arr_1[SIZE];
    for (int i = 0; i <= fill_index; ++i) arr_1[i] = current_solution[i];

    if (n - fill_index > sequential_threshold){
        #pragma omp task
        solve(fill_index + 1, arr_1, new_val, prev_ones + 1);
    } else {
        solve(fill_index + 1, arr_1, new_val, prev_ones + 1);
    }
    //#pragma omp taskwait
    return;
}

void ompSolve(int *curr_sol)
{
    #pragma omp parallel
    {
        #pragma omp single
        {
            int index, ones;
            float val;
            get_vals(curr_sol, ones, index, val);            
            solve(index, curr_sol, val, ones);
        }
    }
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int proc_num, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_num);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // reading input file
    char* filename = argv[1];
    read_graph_file(filename);

    // MASTER PROCESS
    if (proc_num == 0) {
        // queue population
        deque<vector<int> *> q;
        vector<int> * first = new vector<int>({0});
        vector<int> * second = new vector<int>({1});

        q.push_back(first);
        q.push_back(second);
        while (q.size() < 256) { //index 8
            generate_states(q);
        }

        // initial work distribution
        for (int dest = 1; dest < num_procs; dest++){
            int * send;
            send = vect2arr(*(q.front()));
            delete q.front();
            q.pop_front();
            MPI_Send(send, SIZE, MPI_INT, dest, tag_work, MPI_COMM_WORLD);
            delete send;
        }
        int working_slaves = num_procs - 1;

        // main loop
        while (working_slaves > 0) {
            MPI_Recv(&result, 1,  MPI_DOUBLE, MPI_ANY_SOURCE, tag_done, MPI_COMM_WORLD, &status);
            if (result < BEST_VAL) {
                BEST_VAL = result;
            }
            if (q.size() != 0) {
                int * send;
                send = vect2arr(*(q.front()));
                delete q.front();
                q.pop_front();
                MPI_Send(send, SIZE, MPI_INT, status.MPI_SOURCE, tag_work, MPI_COMM_WORLD);
                delete send;
            } else {
                MPI_Send(&tmp_arr, SIZE, MPI_INT, status.MPI_SOURCE, tag_done, MPI_COMM_WORLD);
                working_slaves--;
            }
        }        

        printf("Result %.3f \n", BEST_VAL);
    //SLAVE PROCESS
    } else {
        while (true) {
            MPI_Recv(&tmp_arr, SIZE, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == tag_done) break;
            else if(status.MPI_TAG == tag_work) {
                ompSolve(tmp_arr);
                MPI_Send(&BEST_VAL, 1, MPI_DOUBLE, 0, tag_done, MPI_COMM_WORLD);
                BEST_VAL = 1000;
            }
        }
    }
    MPI_Finalize();

    return 0;

}
