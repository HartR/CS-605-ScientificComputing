#ifndef HEADERFILE_H
#define HEADERFILE_H

#define DIMENSION 6
#define SIZE DIMENSION*DIMENSION
#define HALF SIZE/2

extern double* matrix_a;
extern double* matrix_b;
extern double* matrix_result_1;
extern double* matrix_result_2;

void MatrixMultiplyCuda(int host_id);

#endif
