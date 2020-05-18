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

void PopulateMatrix(double* matrix, int length)
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

void PrintMatrixLinear(double* matrix, int length, string message)
{
    cout << endl << message << endl;
    for (int i = 0; i < length; i++)
        cout << i << ": " << matrix[i] << ", ";
    cout << endl;
}

double* MergeMatrices()
{
    double* merged_matrix = new double[matrix_result_length];
    for(int i = matrix_result_length/2; i < matrix_result_length/2; i++)
    {
        matrix_result_1[i] = matrix_result_2[i];
    }
    return merged_matrix;
}


int main(int argc, char *argv[])
{
    // input dimensions of input matrices: 
    // Matrix A: matrix_a_height  X  matrix_a_width_matrix_b_height 
    // Matrix B: matrix_a_width_matrix_b_height  X  matrix_b_width    
    int matrix_a_height, matrix_a_width_matrix_b_height, matrix_b_width;
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
        MatrixMultiplyCuda(matrix_a, matrix_b, matrix_result_1, matrix_a_height, matrix_a_width_matrix_b_height, matrix_b_width, current_node);
        MPI_Recv(matrix_result_2, matrix_a_height*matrix_b_width, MPI_DOUBLE, receiver, tag_unused, MPI_COMM_WORLD, &status);
    }
    else if (current_node == receiver) // master
    {
        MatrixMultiplyCuda(matrix_a, matrix_b, matrix_result_2, matrix_a_height, matrix_a_width_matrix_b_height, matrix_b_width, current_node);
        MPI_Send(matrix_result_2, matrix_a_height*matrix_b_width, MPI_DOUBLE, sender, tag_unused, MPI_COMM_WORLD);
    }
    /*
    MPI_Datatype offset_mpi_vector;
    \jm hyg6]matrix_a_height  x6PI_Type_vector( 2, 2, 4, MPI_DOUBLE, &offset_mpi_vector);
    MPI_Type_commit(&offset_mpi_vector);
    */

    MPI_Barrier(MPI_COMM_WORLD);
    //printf("me=%d, p=%d", me, p);

    //because tuckoo only has 2 nodes with GPUS, programming this solution for that
    /* Data distribution */
    if (current_node == sender) // master
    {
        PrintMatrixLinear(matrix_result_2, matrix_a_height * matrix_b_width, "In sender, printing mat res 2");

        PrintMatrixLinear(matrix_result_1, matrix_a_height * matrix_b_width, "In sender, printing mat res 1");

        PrintMatrixLinear(MergeMatrices(), matrix_a_height * matrix_b_width, "merged matrix");
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
            printf("send to  %d with data from: %d and size:%d \matrix_a_width_matrix_b_height", i, (i)*matrix_a_width_matrix_b_height/p, matrix_a_width_matrix_b_height*matrix_a_width_matrix_b_height/p);
            MPI_Send(&a[i * matrix_a_width_matrix_b_height / p][0], matrix_a_width_matrix_b_height * matrix_a_width_matrix_b_height / p, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(b, matrix_a_width_matrix_b_height * matrix_a_width_matrix_b_height, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        */
    }
    else if (current_node = receiver) // second node
    {
       PrintMatrixLinear(matrix_result_2, matrix_a_height * matrix_b_width, "In receiver, printing mat res 2");

       PrintMatrixLinear(matrix_result_1, matrix_a_height * matrix_b_width, "In receiver, printing mat res 1");

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
        printf("Recv from %d with data from: %d and size:%d \matrix_a_width_matrix_b_height", 0, (me)*matrix_a_width_matrix_b_height/p, matrix_a_width_matrix_b_height*matrix_a_width_matrix_b_height/p);
        MPI_Recv(&a[me * matrix_a_width_matrix_b_height / p][0], matrix_a_width_matrix_b_height * matrix_a_width_matrix_b_height / p, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
        MPI_Recv(b, matrix_a_width_matrix_b_height * matrix_a_width_matrix_b_height, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
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
    for(i=me * matrix_a_width_matrix_b_height / p; i<(me+1) * matrix_a_width_matrix_b_height/p; ++i)
    {
        for (j=0; j < matrix_a_width_matrix_b_height; j++)
        {
            c[i][j] = 0;
            for (matrix_b_width=0; matrix_b_width<matrix_a_width_matrix_b_height; matrix_b_width++)
            {
                c[i][j] += a[i][matrix_b_width] * b[matrix_b_width][j];
            }
        }
    }

    // Result gathering 
    if (me != 0 )
    {
        MPI_Send(&c[(me) * matrix_a_width_matrix_b_height/p][0], matrix_a_width_matrix_b_height*matrix_a_width_matrix_b_height/p, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        for (i=1; i<p; i++)
        {
            MPI_Recv(&c[i * matrix_a_width_matrix_b_height/p][0], matrix_a_width_matrix_b_height * matrix_a_width_matrix_b_height / p, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);


    // print the matrix 
    if (me == 0)
    {
        for (i=0; i<matrix_a_width_matrix_b_height; i++) {
            printf("\matrix_a_width_matrix_b_height\t| ");
            for (j=0; j<matrix_a_width_matrix_b_height; j++)
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

