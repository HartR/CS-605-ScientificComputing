#include <stdio.h>
#include "mpi.h"
#include "mycuda.h"
#include <stdlib.h>


#define N 2
#define SIZE N*N

using namespace std;

/* global */
int a[N][N] = {
    { 1, 2 },
    { 3, 4 }
};

int b[N][N] = {
    { 5, 6 },
    { 7, 8 }
};

int c[N][N];

void PopulateMatrices(double* matrix_a, double* matrix_b)
{
    for (int j = 0; j < SIZE; j++)
    {
        matrix_a[j] = ((double)rand() / (RAND_MAX));
        matrix_b[j] = ((double)rand() / (RAND_MAX));
    }
}

void PrintMatrix(double* matrix)
{
    for (int row = 0; row < N; row++)
    {
        for (int column = 0; column < N; column++)
            cout << matrix[N * row + column] << " ";
        cout << endl;
    }  
}


int main(int argc, char *argv[])
{
    
    double* matrix_a = new double[SIZE];
    double* matrix_b = new double[SIZE];
    PopulateMatrices(matrix_a, matrix_b);
    //PrintMatrix(matrix_a);


    MPI_Status status;
    int me,p;
    int i,j,k;
 

  /* Start up MPI */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &me);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

    MatrixMultiplyCuda(matrix_a, matrix_b, SIZE);

  printf("me=%d, p=%d", me, p);

  /* Data distribution */
  if (me == 0) // master
  {
      // assume p = 2
      for (i=1; i<p; i++)
      {
          printf("send to  %d with data from: %d and size:%d \n", i, (i)*N/p, N*N/p);
          MPI_Send(&a[i * N / p][0], N * N / p, MPI_INT, i, 0, MPI_COMM_WORLD);
          MPI_Send(b, N * N, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
  }
  else
  {
      printf("Recv from %d with data from: %d and size:%d \n", 0, (me)*N/p, N*N/p);
      MPI_Recv(&a[me * N / p][0], N * N / p, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
      MPI_Recv(b, N * N, MPI_INT, i, 0, MPI_COMM_WORLD, 0);

  }
  /* Computation */
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

  /* Result gathering */
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


  /* print the matrix */
  if (me == 0)
  {
      for (i=0; i<N; i++) {
          printf("\n\t| ");
          for (j=0; j<N; j++)
              printf("%2d ", c[i][j]);
          printf("|");
      }
  }


  /* Quit */

  MPI_Finalize();
  return 0;
}

