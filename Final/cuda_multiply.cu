/* multiply.cu */
#include <cuda.h>
#include "mycuda.h"
#include <stdio.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <math.h>

#define BLOCK_SIZE 16
 
using namespace std;

 __global__ void __multiply__ (double* a, double* b, double* c, int matrix_1_height, int matrix_1_width_matrix_2_height, int matrix_2_width, int offset)
 {
     int i = blockIdx.y * blockDim.y + threadIdx.y; 
     int j = blockIdx.x * blockDim.x + threadIdx.x;
     int sum = 0;
     if( j < matrix_2_width && i < matrix_1_height) 
     {
         for(int k = 0; k < matrix_1_width_matrix_2_height; k++) 
         {
               c[i * matrix_2_width + j] += a[i * matrix_1_width_matrix_2_height + matrix_2_width] * b[matrix_2_width * matrix_2_width + j];
               //printf("\ni is %d, a is %f, b is %f", i, a[i * matrix_1_width_matrix_2_height + i], b[i * matrix_2_width + j]);
         }
         printf("sum is %f\n", sum);
         c[i * matrix_2_width + j] = sum;
         printf("\n\n");
         //printf("\matrix_1_width_matrix_2_height At location %d, in c, assigned value %f, sum is %f, value of a is %f, val of b is %f", i * matrix_2_width + j + offset, c[i * matrix_2_width + j + offset], a[i], b[i]);

     }
     /*int pixel = blockIdx.x * blockDim.x + threadIdx.x;
     if (pixel < half_length)
     {
          printf("\nIn matrix b, current value in result: %f, value at %d: %f, ", result[pixel], pixel, b[pixel]);
          result[pixel] = b[pixel+offset];
     }
     printf("\matrix_1_width_matrix_2_height\matrix_1_width_matrix_2_height");*/
 }

 /*
 void PrintMatrix(double* matrix, int matrix_1_width_matrix_2_height, int p)
{
     printf("\nPrinting in CUDA on host %d \matrix_1_width_matrix_2_height", p);
    for (int i = 0; i < matrix_1_width_matrix_2_height; i++)
    {
        for (int column = 0; column < matrix_1_width_matrix_2_height; column++)
            printf("%f ", matrix[matrix_1_width_matrix_2_height * i + column]);
        printf("\matrix_1_width_matrix_2_height");
    }  
}
 */

void MatrixMultiplyCuda(double* mat_a, double* mat_b, double* mat_result, int matrix_1_height, int matrix_1_width_matrix_2_height, int matrix_2_width, int host_id)
{
     cudaError_t cudaStatus;
     double* mat_a_device;
     double* mat_b_device;
     double* mat_result_device;

     //figure out ideal thread/block numbers
     //I'matrix_1_height using 256 threads, because we found that to be optimal from assignment 4
     /*int thread_number = 256;
     int block_number = 1;
     int array_length = matrix_1_height*matrix_2_width;
     if(array_length < thread_number)
     {
          thread_number = array_length;
     }
     else if (array_length > thread_number)
     {
          //get the ceiling of the division
          block_number = (array_length + thread_number - 1)/thread_number;
     }
     int offset = host_id * ((matrix_1_height*matrix_2_width)/2);*/

     //thread_number*block_number == array_length/2
     cudaMalloc((void**)&mat_a_device, sizeof(double)*matrix_1_height*matrix_1_width_matrix_2_height);
     cudaMalloc((void**)&mat_b_device, sizeof(double)*matrix_1_width_matrix_2_height*matrix_2_width);
     cudaMalloc((void**)&mat_result_device, sizeof(double)*matrix_1_height*matrix_2_width);
     cudaMemcpy(mat_a_device, mat_a, sizeof(int)*matrix_1_height*matrix_1_width_matrix_2_height, cudaMemcpyHostToDevice);
     cudaMemcpy(mat_b_device, mat_b, sizeof(int)*matrix_1_width_matrix_2_height*matrix_2_width, cudaMemcpyHostToDevice);
     cudaMemcpy(mat_result_device, mat_result, sizeof(double)*matrix_1_height*matrix_2_width, cudaMemcpyHostToDevice);
     //PrintMatrix(mat_result, sqrt(array_length), host_id);

     unsigned int grid_rows = (matrix_1_height + BLOCK_SIZE - 1) / BLOCK_SIZE;
     unsigned int grid_cols = (matrix_2_width + BLOCK_SIZE - 1) / BLOCK_SIZE;
     dim3 dimGrid(grid_cols, grid_rows);
     dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);

     __multiply__ <<<dimGrid, dimBlock>>> (mat_a_device, mat_b_device, mat_result_device, matrix_1_height, matrix_1_width_matrix_2_height, matrix_2_width, offset);
     cudaMemcpy(mat_result, mat_result_device, sizeof(double)*matrix_1_height*matrix_2_width, cudaMemcpyDeviceToHost);

     mat_result[0] = 9999;
     mat_result[1] = 7777;
     cudaStatus = cudaGetLastError();
     if (cudaStatus != cudaSuccess) {
          fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
     }

     cudaStatus = cudaDeviceSynchronize();
     if (cudaStatus != cudaSuccess) {
          fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
     }

     cudaFree(mat_a_device);
     cudaFree(mat_b_device);
     cudaFree(mat_result_device);


     cudaStatus = cudaDeviceReset();
     if (cudaStatus != cudaSuccess) {
          fprintf(stderr, "cudaDeviceReset failed!");
     }     /* ... Transfer data from GPU to CPU */
}
