#include <stdio.h>
#include "mpi.h"
#include "mycuda.h"
#include <stdlib.h>


#define DIMENSION 2
#define SIZE DIMENSION * DIMENSION

/* global */
int a[DIMENSION][DIMENSION] = {
    { 1, 2 },
    { 3, 4 }
};

int b[DIMENSION][DIMENSION] = {
    { 5, 6 },
    { 7, 8 }
};

int c[DIMENSION][DIMENSION];

void PopulateMatrix(double* matrix)
{
        for (int j = 0; j < SIZE; j++)
        {
            matrix[j] = ((double)rand() / (RAND_MAX));
        }
}

int main(int argc, char *argv[])
{
    
    size_t mat_a_size = DIMENSION*DIMENSION*sizeof(double);
    size_t mat_b_size = DIMENSION*DIMENSION*sizeof(double);
    double* matrix_a = (double *) malloc(sizeof(mat_a_size));
    double* matrix_b = (double *) malloc(sizeof(mat_b_size));
    PopulateMatrix(matrix_a);
    PopulateMatrix(matrix_b);


    call_me_maybe(matrix1, matrix2, DIMENSION*DIMENSION);
  MPI_Status status;
  int me,p;
  int i,j,k;
 

  /* Start up MPI */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &me);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  printf("me=%d, p=%d", me, p);

  /* Data distribution */
  if (me == 0) // master
  {
      // assume p = 2
      for (i=1; i<p; i++)
      {
          printf("send to  %d with data from: %d and size:%d \n", i, (i)*DIMENSION/p, DIMENSION*DIMENSION/p);
          MPI_Send(&a[i * DIMENSION / p][0], DIMENSION * DIMENSION / p, MPI_INT, i, 0, MPI_COMM_WORLD);
          MPI_Send(b, DIMENSION * DIMENSION, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
  }
  else
  {
      printf("Recv from %d with data from: %d and size:%d \n", 0, (me)*DIMENSION/p, DIMENSION*DIMENSION/p);
      MPI_Recv(&a[me * DIMENSION / p][0], DIMENSION * DIMENSION / p, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
      MPI_Recv(b, DIMENSION * DIMENSION, MPI_INT, i, 0, MPI_COMM_WORLD, 0);

  }
  /* Computation */
  for(i=me * DIMENSION / p; i<(me+1) * DIMENSION/p; ++i)
  {
      for (j=0; j < DIMENSION; j++)
      {
          c[i][j] = 0;
          for (k=0; k<DIMENSION; k++)
          {
              c[i][j] += a[i][k] * b[k][j];
          }
      }
  }

  /* Result gathering */
  if (me != 0 )
  {
      MPI_Send(&c[(me) * DIMENSION/p][0], DIMENSION*DIMENSION/p, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  else
  {
      for (i=1; i<p; i++)
      {
          MPI_Recv(&c[i * DIMENSION/p][0], DIMENSION * DIMENSION / p, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
      }
  }

  MPI_Barrier(MPI_COMM_WORLD);


  /* print the matrix */
  if (me == 0)
  {
      for (i=0; i<DIMENSION; i++) {
          printf("\n\t| ");
          for (j=0; j<DIMENSION; j++)
              printf("%2d ", c[i][j]);
          printf("|");
      }
  }


  /* Quit */

  MPI_Finalize();
  return 0;
}

