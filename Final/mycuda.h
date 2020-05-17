#ifndef HEADERFILE_H
#define HEADERFILE_H

#define DIMENSION 6
#define SIZE DIMENSION*DIMENSION
#define HALF SIZE/2

double* matrix_a = new double[SIZE];
double* matrix_b = new double[SIZE];
double* matrix_result_1 = new double[HALF];
double* matrix_result_2 = new double[HALF];

void MatrixMultiplyCuda(double* mat_a, double* mat_b, double* matrix_result, int array_length, int host_id);

#endif
