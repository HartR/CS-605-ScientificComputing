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
    // input dimensions of input matrices: matrix_1_height X matrix_1_width_matrix_2_height matrix (A)
    // matrix_1_width_matrix_2_height X matrix_2_width matrix (B)    
    int matrix_1_height, matrix_1_width_matrix_2_height, matrix_2_width;
    matrix_1_height = atoi(argv[1]);
    matrix_1_width_matrix_2_height = atoi(argv[2]);
    matrix_2_width = atoi(argv[3]);
    matrix_a_length = matrix_1_height*matrix_1_width_matrix_2_height;
    matrix_b_length = matrix_1_width_matrix_2_height*matrix_2_width;
    matrix_a = new double[matrix_a_length];
    matrix_b = new double[matrix_b_length];
    matrix_result_1 = new double[matrix_1_height*matrix_2_width];
    matrix_result_2 = new double[matrix_1_height*matrix_2_width];
    PopulateMatrix(matrix_a, matrix_1_height, matrix_1_width_matrix_2_height);
    PopulateMatrix(matrix_b, matrix_1_width_matrix_2_height, matrix_2_width);


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
        MatrixMultiplyCuda(matrix_a, matrix_b, matrix_result_1, matrix_1_height, matrix_1_width_matrix_2_height, matrix_2_width, current_node);
        MPI_Recv(matrix_result_2, matrix_1_height*matrix_2_width, MPI_DOUBLE, receiver, tag_unused, MPI_COMM_WORLD, &status);
    }
    else if (current_node == receiver) // master
    {
        MatrixMultiplyCuda(matrix_a, matrix_b, matrix_result_2, matrix_1_height, matrix_1_width_matrix_2_height, matrix_2_width, current_node);
        MPI_Send(matrix_result_2, matrix_1_height*matrix_2_width, MPI_DOUBLE, sender, tag_unused, MPI_COMM_WORLD);
    }
    /*
    MPI_Datatype offset_mpi_vector;
    \jm hyg6]matrix_1_height  x6PI_Type_vector( 2, 2, 4, MPI_DOUBLE, &offset_mpi_vector);
    MPI_Type_commit(&offset_mpi_vector);
    */

    MPI_Barrier(MPI_COMM_WORLD);
    //printf("me=%d, p=%d", me, p);

    //because tuckoo only has 2 nodes with GPUS, programming this solution for that
    /* Data distribution */
    if (current_node == sender) // master
    {
        PrintMatrix(matrix_result_2, matrix_1_height, matrix_2_width, "In sender, printing mat res 2");

        PrintMatrix(matrix_result_1, matrix_1_height, matrix_2_width, "In sender, printing mat res 1");
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
            printf("send to  %d with data from: %d and size:%d \matrix_1_width_matrix_2_height", i, (i)*matrix_1_width_matrix_2_height/p, matrix_1_width_matrix_2_height*matrix_1_width_matrix_2_height/p);
            MPI_Send(&a[i * matrix_1_width_matrix_2_height / p][0], matrix_1_width_matrix_2_height * matrix_1_width_matrix_2_height / p, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(b, matrix_1_width_matrix_2_height * matrix_1_width_matrix_2_height, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        */
    }
    else if (current_node = receiver) // second node
    {
       PrintMatrix(matrix_result_2, matrix_1_height, matrix_2_width, "In receiver, printing mat res 2");

       PrintMatrix(matrix_result_1, matrix_1_height, matrix_2_width, "In receiver, printing mat res 1");

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
        printf("Recv from %d with data from: %d and size:%d \matrix_1_width_matrix_2_height", 0, (me)*matrix_1_width_matrix_2_height/p, matrix_1_width_matrix_2_height*matrix_1_width_matrix_2_height/p);
        MPI_Recv(&a[me * matrix_1_width_matrix_2_height / p][0], matrix_1_width_matrix_2_height * matrix_1_width_matrix_2_height / p, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
        MPI_Recv(b, matrix_1_width_matrix_2_height * matrix_1_width_matrix_2_height, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
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
    for(i=me * matrix_1_width_matrix_2_height / p; i<(me+1) * matrix_1_width_matrix_2_height/p; ++i)
    {
        for (j=0; j < matrix_1_width_matrix_2_height; j++)
        {
            c[i][j] = 0;
            for (matrix_2_width=0; matrix_2_width<matrix_1_width_matrix_2_height; matrix_2_width++)
            {
                c[i][j] += a[i][matrix_2_width] * b[matrix_2_width][j];
            }
        }
    }

    // Result gathering 
    if (me != 0 )
    {
        MPI_Send(&c[(me) * matrix_1_width_matrix_2_height/p][0], matrix_1_width_matrix_2_height*matrix_1_width_matrix_2_height/p, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        for (i=1; i<p; i++)
        {
            MPI_Recv(&c[i * matrix_1_width_matrix_2_height/p][0], matrix_1_width_matrix_2_height * matrix_1_width_matrix_2_height / p, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);


    // print the matrix 
    if (me == 0)
    {
        for (i=0; i<matrix_1_width_matrix_2_height; i++) {
            printf("\matrix_1_width_matrix_2_height\t| ");
            for (j=0; j<matrix_1_width_matrix_2_height; j++)
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

