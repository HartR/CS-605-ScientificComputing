#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h> 
#include <sys/time.h> 

using namespace std;

#define DIMENSION 3200
#define SIZE DIMENSION * DIMENSION

void PopulateMatrices(double *matrix1, double *matrix2)
{
    for (int j = 0; j < SIZE; j++)
    {
        matrix1[j] = ((double)rand() / (RAND_MAX));
        matrix2[j] = ((double)rand() / (RAND_MAX));
    }
}

void Deallocate()
{

}

void PerformIJK()
{
    //initialize variables
    //struct timeval start, end;
    vector<float> run_times;



    //initialize matrix1 with values between 0-1


    /*
    //perform ijk operation and time it
    gettimeofday(&start, NULL);
    for (int i = 0; i < matrix_1_height; i++) {
        for (int j = 0; j < matrix_2_width; j++) {
            for (int k = 0; k < matrix_1_width_matrix_2_height; k++) {
                output[i * matrix_2_width + j] += matrix1[i * matrix_1_width_matrix_2_height + k] * matrix2[k * matrix_2_width + j];
            }
            //cout << fixed << setprecision(5) << output[i * matrix_2_width + j] << " ";
        }
        //cout << endl;
    }
    gettimeofday(&end, NULL);

    //print resultant matrix to stdout
    for (int j = 0; j < matrix_1_height; j++)
    {
        for (int k = 0; k < matrix_2_width; k++)
        {
            cout << fixed << setprecision(5) << output[j * matrix_2_width + k] << " ";
        }
        cout << endl;
    }
    cout << endl;
    */

    //delete matrix1;
    delete[] matrix1;

    //delete matrix2;
    delete[] matrix2;

    //delete output;
    delete[] output;
    /*
    double ijk_runtime = 0.0;
    ijk_runtime = (end.tv_sec - start.tv_sec) * 1e6;
    ijk_runtime = (ijk_runtime + (end.tv_usec - start.tv_usec)) * 1e-6;
    run_times.push_back(ijk_runtime);
    return run_times;
    */
}

int main()
{
    //allocate heap for matrix1
    double* matrix1 = new double[SIZE];

    //allocate heap for matrix2
    double* matrix2 = new double[SIZE];

    //allocate heap for result
    double* output = new double[SIZE];
    
    PopulateMatrices(matrix1, matrix2);
    int numThreads = 1;
    cin >> numThreads;
    cout << "Num threads: " << numThreads;
    PerformIJK();
    /*
    for (uint i = 0; i < run_times.size(); i++)
    {
        cout << "Runtime for matrix #" << i << ": " << fixed << setprecision(5) << run_times[i] << endl;
    }
    cout << "Done" << endl;
    */
    return 0;
}
