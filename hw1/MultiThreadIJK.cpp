#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>
#include <sys/time.h>

using namespace std;

#define DIMENSION 2500
#define SIZE DIMENSION * DIMENSION

        double* matrix1;
        double* matrix2;
        double* output;
        double runtime;
        int num_threads;
        pthread_t* threads;
        int step_i = 0;

void* Multiply(void* arg) 
{ 
    int core = step_i++; 

    for (int i = core * DIMENSION / num_threads; i < (core + 1) * DIMENSION / num_threads; i++)  
        for (int j = 0; j < DIMENSION; j++)  
            for (int k = 0; k < DIMENSION; k++)  
                output[i * DIMENSION + j] += matrix1[i * DIMENSION + k] * matrix2[k * DIMENSION + j]; 
}

class MatrixMultiply
{
    private:


        void InitVars()
        {
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
                int* p; 
                pthread_create(&threads[i], NULL, Multiply, (void*)p); 
            }

            for (int i = 0; i < num_threads; i++)  
                pthread_join(threads[i], NULL);
                            gettimeofday(&end, NULL);
            
            runtime = (end.tv_sec - start.tv_sec) * 1e6;
            runtime = (runtime + (end.tv_usec - start.tv_usec)) * 1e-6;  
            /*
            struct timeval start, end;

            gettimeofday(&start, NULL);
            for (int i = 0; i < DIMENSION; i++) {
                for (int j = 0; j < DIMENSION; j++) {
                    for (int k = 0; k < DIMENSION; k++) {
                        output[i * DIMENSION + j] += matrix1[i * DIMENSION + k] * matrix2[k * DIMENSION + j];
                    }
                    //cout << fixed << setprecision(5) << output[i * matrix_2_width + j] << " ";
                }
                //cout << endl;
            }
            gettimeofday(&end, NULL);
            
            runtime = (end.tv_sec - start.tv_sec) * 1e6;
            runtime = (runtime + (end.tv_usec - start.tv_usec)) * 1e-6;
            */
        }
};

int main()
{
    int num = 32;
    num_threads = num;
    MatrixMultiply matrices;
    //matrices.PrintResult();
    cout << runtime << "done";
    return 0;
}
