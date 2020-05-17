#include <stdio.h>
#include "mpi.h"
#include "mycuda.h"
#include <stdlib.h>

#define DIMENSION 6
#define SIZE DIMENSION*DIMENSION
#define HALF SIZE/2

using namespace std;

double* matrix_a;
double* matrix_b;
double* matrix_result_1;
double* matrix_result_2;
int matrix_a_length;
int matrix_b_length;
int matrix_result_length;
//double* merged_matrix;

void PopulateMatrix(double* matrix, int outer, int inner)
{
    for (int i = 0; i < outer; ++i) {
        for (int j = 0; j < inner; ++j) {
            matrix[i * inner + j] = ((double)rand() / (RAND_MAX));
        }
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

void PrintHalf(double* matrix)
{
    cout << endl;
    for (int i = 0; i < HALF; i++)
        cout << i << ": " << matrix[i] << ", ";
    cout << endl;
}

double* MergeMatrices()
{
    double* merged_matrix = new double[SIZE];
    for(int i = 0; i < HALF; i++)
    {
        merged_matrix[i] = matrix_result_1[i];
        merged_matrix[i+HALF] = matrix_result_2[i];
    }
    return merged_matrix;
}


int main(int argc, char *argv[])
{
    // input dimensions of input matrices: m X n matrix (A)
    // n X k matrix (B)    
    int m, n, k;
    m = atoi(argv[1]);
    n = atoi(argv[2]);
    k = atoi(argv[3]);
    matrix_a_length = m*n;
    matrix_b_length = n*k;
    matrix_a = new double[matrix_a_length];
    matrix_b = new double[matrix_b_length];
    matrix_result_1 = new double[m*k];
    matrix_result_2 = new double[m*k];
    //PrintMatrix(matrix_result, "matrix result before");
    //PrintMatrix(matrix_b, "matrix b before");


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
        PopulateMatrix(matrix_a, m, n);
        PrintMatrix(matrix_a, m, n, "In sender, printing mat a");
        matrix_a[0] = 9999;
        MPI_Send(matrix_a, matrix_a_length, MPI_DOUBLE, receiver, tag_unused, MPI_COMM_WORLD);
        MPI_Recv(matrix_b, matrix_b_length, MPI_DOUBLE, receiver, tag_unused, MPI_COMM_WORLD, &status);
    }
    else if (current_node == receiver) // master
    {
        PopulateMatrix(matrix_b, n, k);
        PrintMatrix(matrix_b, n, k, "In receiver, printing mat b");
        matrix_a[0] = 7777;
        MPI_Send(matrix_b, matrix_b_length, MPI_DOUBLE, sender, tag_unused, MPI_COMM_WORLD);
        MPI_Recv(matrix_a, matrix_a_length, MPI_DOUBLE, sender, tag_unused, MPI_COMM_WORLD, &status);
    }
    /*
    MPI_Datatype offset_mpi_vector;
    \jm hyg6]m  x6PI_Type_vector( 2, 2, 4, MPI_DOUBLE, &offset_mpi_vector);
    MPI_Type_commit(&offset_mpi_vector);
    */

    MPI_Barrier(MPI_COMM_WORLD);
    //printf("me=%d, p=%d", me, p);

    //because tuckoo only has 2 nodes with GPUS, programming this solution for that
    /* Data distribution */
    if (current_node == sender) // master
    {
        PrintMatrix(matrix_b, n, k, "In sender, printing mat b");
        //PrintMatrix(matrix_result, "before first multiply?");
        //MatrixMultiplyCuda(matrix_a, matrix_b, matrix_result_1, SIZE, current_node);

        //PrintHalf(matrix_result_1);

        //MPI_Recv(matrix_result_2, HALF, MPI_DOUBLE, receiver, tag_unused, MPI_COMM_WORLD, &status);

        //PrintMatrix(matrix_result, "hope this works?");

        /*
        MPI_Send(matrix_a, SIZE, MPI_DOUBLE, receiver, tag_unused, MPI_COMM_WORLD);
        MPI_Send(matrix_b, SIZE, MPI_DOUBLE, receiver, tag_unused, MPI_COMM_WORLD);
        MPI_Send(matrix_result, SIZE, MPI_DOUBLE, receiver, tag_unused, MPI_COMM_WORLD);*/

        /*
        // assume p = 2
        for (i=1; i<p; i++)
        {
            printf("send to  %d with data from: %d and size:%d \n", i, (i)*N/p, N*N/p);
            MPI_Send(&a[i * N / p][0], N * N / p, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(b, N * N, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        */
    }
    else if (current_node = receiver) // second node
    {
        PrintMatrix(matrix_a, m, n, "In receiver, printing mat a");

        /*
        MPI_Recv(matrix_a, SIZE, MPI_DOUBLE, sender, tag_unused, MPI_COMM_WORLD, &status);
        MPI_Recv(matrix_b, SIZE, MPI_DOUBLE, sender, tag_unused, MPI_COMM_WORLD, &status);
        MPI_Recv(matrix_result, SIZE, MPI_DOUBLE, sender, tag_unused, MPI_COMM_WORLD, &status);
                PrintMatrix(matrix_result, "before second");*/

        //MatrixMultiplyCuda(matrix_a, matrix_b, matrix_result_2, SIZE, current_node);

        //PrintHalf(matrix_result_2);

        //MatrixMultiplyCuda(matrix_a, matrix_b, matrix_result, SIZE, current_node);
        //MPI_Send(matrix_result_2, HALF, MPI_DOUBLE, sender, tag_unused, MPI_COMM_WORLD);

        //PrintMatrix(matrix_result, "did it work?");
        /*
        printf("Recv from %d with data from: %d and size:%d \n", 0, (me)*N/p, N*N/p);
        MPI_Recv(&a[me * N / p][0], N * N / p, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
        MPI_Recv(b, N * N, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
        */

    }
    //send matrix result from node 2 back to original node
    /*MPI_Barrier(MPI_COMM_WORLD);

    // Result gathering 
    if (current_node = receiver)
    {
        MPI_Send(matrix_result_2, HALF, MPI_DOUBLE, sender, tag_unused, MPI_COMM_WORLD);
    }
    else if (current_node = sender) // second node
    {
        MPI_Recv(matrix_result_2, HALF, MPI_DOUBLE, receiver, tag_unused, MPI_COMM_WORLD, &status);
    }*/

/*
    MPI_Barrier(MPI_COMM_WORLD);

    if(current_node == sender)
    {
        double* matrix_results_combined = MergeMatrices();

        PrintMatrix(MergeMatrices(), "final");
        delete[] matrix_results_combined;
    }*/

    //PrintMatrix(matrix_result, "did it work please");
    //PrintHalf(matrix_result_1);

    //PrintHalf(matrix_result_2);
    //copy(matrix_result_1, SIZE, matrix_result);
    //copy(matrix_result_2, SIZE, matrix_result + SIZE/2);


    /* Computation */
    /*
    for(i=me * N / p; i<(me+1) * N/p; ++i)
    {
        for (j=0; j < N; j++)
        {
            c[i][j] = 0;
            for (k=0; k<N; k++)
            {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    // Result gathering 
    if (me != 0 )
    {
        MPI_Send(&c[(me) * N/p][0], N*N/p, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        for (i=1; i<p; i++)
        {
            MPI_Recv(&c[i * N/p][0], N * N / p, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);


    // print the matrix 
    if (me == 0)
    {
        for (i=0; i<N; i++) {
            printf("\n\t| ");
            for (j=0; j<N; j++)
                printf("%2d ", c[i][j]);
            printf("|");
        }
    }
    */


    /* Quit */

 //   MPI_Type_free(&offset_mpi_vector);

    MPI_Finalize();

    
    return 0;
}

