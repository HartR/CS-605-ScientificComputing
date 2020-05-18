#include <stdio.h>
#include "mpi.h"
#include "mycuda.h"
#include <stdlib.h>

using namespace std;

double* matrix_a;
double* matrix_b;
double* matrix_result_1;
double* matrix_result_2;
int matrix_a_length;
int matrix_b_length;
int matrix_result_length;
int matrix_a_height, matrix_a_width_matrix_b_height, matrix_b_width;


void PopulateMatrix(double* matrix, int length)
{
    for (int i = 0; i < length; ++i) {
            matrix[i] = ((double)rand() / (RAND_MAX));
    }
}

void PrintMatrix(double* matrix, int outer, int inner, string message)
{
    cout << endl << message << endl;
    for (int i = 0; i < outer; ++i) {
        for (int j = 0; j < inner; ++j) {
            cout << i * inner + j << ": " << "Value:" << matrix[i * inner + j] << " | ";
        }
        cout << endl;
    } 
}

void PrintMatrixLinear(double* matrix, int length, string message)
{
    cout << endl << message << endl;
    for (int i = 0; i < length; i++)
        cout << i << ": " << matrix[i] << ", ";
    cout << endl;
}

//copy the 2nd half of matrix 2 to to matrix 1, making matrix 1 the complete result
void MergeMatrices()
{
    double* merged_matrix = new double[matrix_result_length];
    for(int i = matrix_result_length/2; i < matrix_result_length; i++)
    {
        matrix_result_1[i] = matrix_result_2[i];
    }
}


int main(int argc, char *argv[])
{
    //because tuckoo only has 2 nodes with GPUS, programming this solution for that configuration
    // input dimensions of input matrices: 
    // Matrix A: matrix_a_height  X  matrix_a_width_matrix_b_height 
    // Matrix B: matrix_a_width_matrix_b_height  X  matrix_b_width  
    matrix_a_height = atoi(argv[1]);
    matrix_a_width_matrix_b_height = atoi(argv[2]);
    matrix_b_width = atoi(argv[3]);
    matrix_a_length = matrix_a_height * matrix_a_width_matrix_b_height;
    matrix_b_length = matrix_a_width_matrix_b_height * matrix_b_width;
    matrix_result_length = matrix_a_height * matrix_b_width;
    matrix_a = new double[matrix_a_length];
    matrix_b = new double[matrix_b_length];

    matrix_result_1 = new double[matrix_result_length];
    matrix_result_2 = new double[matrix_result_length];
    PopulateMatrix(matrix_a, matrix_a_length);
    PopulateMatrix(matrix_b, matrix_b_length);
    PrintMatrixLinear(matrix_a, matrix_a_length, "a after populating");


    MPI_Status status;
    int current_node, total_nodes;
    const int sender = 0;
    const int receiver = 1;
    int tag_unused = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &current_node);
    MPI_Comm_size(MPI_COMM_WORLD, &total_nodes);

    if (current_node == sender) // master
    {


        MatrixMultiplyCuda(matrix_a, matrix_b, matrix_result_1, matrix_a_height, matrix_a_width_matrix_b_height, matrix_b_width, current_node);
        PrintMatrixLinear(matrix_result_1, matrix_result_length, "result?");
        MPI_Recv(matrix_result_2, matrix_result_length, MPI_DOUBLE, receiver, tag_unused, MPI_COMM_WORLD, &status);
    }
    
    else if (current_node == receiver)
    {
        MatrixMultiplyCuda(matrix_a, matrix_b, matrix_result_2, matrix_a_height, matrix_a_width_matrix_b_height, matrix_b_width, current_node);
        PrintMatrixLinear(matrix_result_2, matrix_result_length, "is this bad?");
        MPI_Send(matrix_result_2, matrix_result_length, MPI_DOUBLE, sender, tag_unused, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (current_node == sender) // master
    {
        PrintMatrixLinear(matrix_result_2, matrix_a_height * matrix_b_width, "In sender, printing mat res 2");

        PrintMatrixLinear(matrix_result_1, matrix_a_height * matrix_b_width, "In sender, printing mat res 1");
        MergeMatrices();
        PrintMatrixLinear(matrix_result_1, matrix_a_height * matrix_b_width, "merged matrix");

    }
    else if (current_node = receiver) // second node
    {
       PrintMatrixLinear(matrix_result_2, matrix_a_height * matrix_b_width, "In receiver, printing mat res 2");

       PrintMatrixLinear(matrix_result_1, matrix_a_height * matrix_b_width, "In receiver, printing mat res 1");
    }

    MPI_Finalize();

    
    return 0;
}

