#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>
#include <sys/time.h>

using namespace std;

#define DIMENSION 3200
#define SIZE DIMENSION * DIMENSION

double* matrix1;
double* matrix2;
double* output;
double runtime;
int num_threads;
pthread_t* threads;
int thread_iterator;

void* Multiply(void* arg) 
{ 
    thread_iterator++; 
    for (int i = thread_iterator * DIMENSION / num_threads; i < (thread_iterator + 1) * DIMENSION / num_threads; i++)
    {  
        for (int j = 0; j < DIMENSION; j++)  
        {
            for (int k = 0; k < DIMENSION; k++)  
                output[i * DIMENSION + j] += matrix1[i * DIMENSION + k] * matrix2[k * DIMENSION + j]; 
        }
    }
}

class MatrixMultiply
{
    private:


        void InitVars()
        {
            thread_iterator = 0;
            threads = new pthread_t[num_threads];
            matrix1 = new double[SIZE];
            matrix2 = new double[SIZE];
            output = new double[SIZE];
        }

        //For use in debugging
        void PrintMatrixVerbose(double* matrix)
        {
            for (int row = 0; row < DIMENSION; row++)
            {
                for (int column = 0; column < DIMENSION; column++)
                    cout << "Index:" << DIMENSION * row + column << ", " << "Value:" << matrix[DIMENSION * row + column] << " | ";
                cout << endl;
            }  
        }

        void PrintMatrix(double* matrix)
        {
            for (int row = 0; row < DIMENSION; row++)
            {
                for (int column = 0; column < DIMENSION; column++)
                    cout << matrix[DIMENSION * row + column] << " ";
                cout << endl;
            }  
        }

    public:

        void ResetResult()
        {
            delete[] output;
            output = new double[SIZE];
        }

        MatrixMultiply()
        {
            InitVars();
            PopulateMatrices();
            PerformMultiIJK();
        }

        ~MatrixMultiply()
        {
            delete[] matrix1;
            delete[] matrix2;
            delete[] output;
        }

        void PopulateMatrices()
        {
                for (int j = 0; j < SIZE; j++)
                {
                    matrix1[j] = ((double)rand() / (RAND_MAX));
                    matrix2[j] = ((double)rand() / (RAND_MAX));
                }
        }

        void PrintA()
        {
            PrintMatrix(matrix1);
        }

        void PrintB()
        {
            PrintMatrix(matrix2);
        }

        void PrintResult()
        {
            PrintMatrix(output);
        }
 

        void PerformMultiIJK()
        {
            struct timeval start, end;

            gettimeofday(&start, NULL);

            for (int i = 0; i < num_threads; i++) { 
                void* threadpointer; 
                pthread_create(&threads[i], NULL, Multiply, threadpointer); 
            }

            for (int i = 0; i < num_threads; i++)  
                pthread_join(threads[i], NULL);
            
            gettimeofday(&end, NULL);
            
            runtime = (end.tv_sec - start.tv_sec) * 1e6;
            runtime = (runtime + (end.tv_usec - start.tv_usec)) * 1e-6; 
        }
};

int main(int argc,char* argv[])
{
    //input number of threads as command line argument
    num_threads = atoi(argv[1]);
    MatrixMultiply matrices;
    //matrices.PrintResult();
    cout << "run time: " << runtime << endl;
    return 0;
}
