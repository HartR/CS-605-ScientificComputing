#ifndef HEADERFILE_H
#define HEADERFILE_H

#define DIMENSION 6
#define SIZE DIMENSION*DIMENSION
#define HALF SIZE/2

double* matrix_a;
double* matrix_b;
double* matrix_result_1;
double* matrix_result_2;

void MatrixMultiplyCuda(int host_id);

#endif
