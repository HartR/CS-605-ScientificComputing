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
#define NUMTHREADS 8

class Matrices
{
    private:
        double* matrix1;
        double* matrix2;
        double* output;
        double runtime;

        void PopulateMatrices()
        {
                for (int j = 0; j < SIZE; j++)
                {
                    matrix1[j] = ((double)rand() / (RAND_MAX));
                    matrix2[j] = ((double)rand() / (RAND_MAX));
                }
        }

        void AllocateMatrices()
        {
            matrix1 = new double[SIZE];
            matrix2 = new double[SIZE];
            output = new double[SIZE];
        }

    public:
        Matrices()
        {
            AllocateMatrices();
            PopulateMatrices();
            PerformMultiIJK();
            //PrintMatrix(output);
        }

        ~Matrices()
        {
            delete[] matrix1;
            delete[] matrix2;
            delete[] output;
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

        void PerformMultiIJK()
        {
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
        }
};

int main()
{
    Matrices matrices;
    cout << "done";
    return 0;
}
