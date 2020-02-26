#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h> 
#include <sys/time.h> 

using namespace std;

vector<float> PerformIJK(string filename)
{
    //initialize variables
    int num_loops;
    string input_filename = string(filename);
    ifstream file;
    file.open(input_filename.c_str());
    file >> num_loops;
    struct timeval start, end;
    vector<float> run_times;

    for (int num_loop_iterator = 0; num_loop_iterator < num_loops; num_loop_iterator++)
    {
        //get relevant info from file header and further initialize variables
        int matrix_1_height = 0;
        file >> matrix_1_height;
        int matrix_1_width_matrix_2_height = 0;
        file >> matrix_1_width_matrix_2_height;
        int matrix_2_width = 0;
        file >> matrix_2_width;

        //allocate heap for matrix1
        float* matrix1 = new float[matrix_1_width_matrix_2_height * matrix_1_height];

        //allocate heap for matrix2
        float* matrix2 = new float[matrix_2_width * matrix_1_width_matrix_2_height];

        //allocate heap for result
        float* output = new float[matrix_1_height * matrix_2_width];

        //load matrix1 into memory
        for (int j = 0; j < matrix_1_height; j++)
        {
            for (int k = 0; k < matrix_1_width_matrix_2_height; k++)
            {
                file >> matrix1[j * matrix_1_width_matrix_2_height + k];
                //cout << j * matrix_1_width_matrix_2_height + k << ": " <<matrix1[j * matrix_1_width_matrix_2_height + k] << " ";
            }
            //cout << endl;
        }
        //cout << endl;
        
        //load matrix2 into memory
        for (int j = 0; j < matrix_1_width_matrix_2_height; j++)
        {
            for (int k = 0; k < matrix_2_width; k++)
            {
                file >> matrix2[j * matrix_2_width + k];
                //cout << j * matrix_2_width + k << ": " << matrix2[j * matrix_2_width + k] << " ";;
            }
            //cout << endl;
        }

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
        
        //delete matrix1;
        delete[] matrix1;

        //delete matrix2;
        delete[] matrix2;

        //delete output;
        delete[] output;

        double ijk_runtime = 0.0;
        ijk_runtime = (end.tv_sec - start.tv_sec) * 1e6;
        ijk_runtime = (ijk_runtime + (end.tv_usec - start.tv_usec)) * 1e-6; 
        run_times.push_back(ijk_runtime);
    }
    return run_times;
}

int main()
{
    vector<float> run_times = PerformIJK("input2.txt");
    for (uint i = 0; i < run_times.size(); i++)
    {
        cout << "Runtime for matrix #" << i << ": " << fixed << setprecision(5) << run_times[i] << endl;
    }
    cout << "Done" << endl;
    return 0;
}
