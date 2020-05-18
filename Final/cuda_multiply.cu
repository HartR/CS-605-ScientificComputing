/* multiply.cu */
#include <cuda.h>
#include "mycuda.h"
#include <stdio.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <math.h>

#define BLOCK_SIZE 16
 
using namespace std;

 __global__ void __multiply__ (double* a, double* b, double* c, int m, int n, int k, int offset)
 {
     
      
     int row = blockIdx.y * blockDim.y + threadIdx.y; 
     int col = blockIdx.x * blockDim.x + threadIdx.x;
     int sum = 0;
     if( col < k && row < m) 
     {
         for(int i = 0; i < n; i++) 
         {
             sum += a[row * n + i] * b[i * k + col];
         }
         c[row * k + col] = sum;
     }
     /*
     if(i ==0 && j==0)
     {
          printf("\n a in cuda, with offset %d \n", offset);
          for (int i = 0; i < matrix_a_height * matrix_a_width_matrix_b_height; i++)
               printf("%d: %f, ", i, a[i]);
          printf("\n");

          printf("\n b in cuda, with offset %d \n", offset);
          for (int i = 0; i < matrix_a_width_matrix_b_height * matrix_b_width; i++)
               printf("%d: %f, ", i, b[i]);
          printf("\n");
     }
     */



     
     /*int pixel = blockIdx.x * blockDim.x + threadIdx.x;
     if (pixel < half_length)
     {
          printf("\nIn matrix b, current value in result: %f, value at %d: %f, ", result[pixel], pixel, b[pixel]);
          result[pixel] = b[pixel+offset];
     }
     printf("\matrix_a_width_matrix_b_height\matrix_a_width_matrix_b_height");*/
 }

 


void MatrixMultiplyCuda(double* mat_a, double* mat_b, double* mat_result, int matrix_a_height, int matrix_a_width_matrix_b_height, int matrix_b_width, int host_id)
{
     cudaError_t cudaStatus;
     double* mat_a_device;
     double* mat_b_device;
     double* mat_result_device;

     //figure out ideal thread/block numbers
     //I'matrix_a_height using 256 threads, because we found that to be optimal from assignment 4
     /*int thread_number = 256;
     int block_number = 1;
     int array_length = matrix_a_height*matrix_b_width;
     if(array_length < thread_number)
     {
          thread_number = SIZE;
     }
     else if (SIZE > thread_number)
     {
          //get the ceiling of the division
          block_number = (array_length + thread_number - 1)/thread_number;
     }*/
     int offset = host_id * (matrix_a_height*matrix_b_width)/2;

     //thread_number*block_number == array_length/2
     cudaMalloc((void**)&mat_a_device, sizeof(double)*matrix_a_height*matrix_a_width_matrix_b_height);
     cudaMalloc((void**)&mat_b_device, sizeof(double)*matrix_a_width_matrix_b_height*matrix_b_width);
     cudaMalloc((void**)&mat_result_device, sizeof(double)*matrix_a_height*matrix_b_width);
     cudaMemcpy(mat_a_device, mat_a, sizeof(double)*matrix_a_height*matrix_a_width_matrix_b_height, cudaMemcpyHostToDevice);
     cudaMemcpy(mat_b_device, mat_b, sizeof(double)*matrix_a_width_matrix_b_height*matrix_b_width, cudaMemcpyHostToDevice);
     cudaMemcpy(mat_result_device, mat_result, sizeof(double)*matrix_a_height*matrix_b_width, cudaMemcpyHostToDevice);
     //PrintMatrix(mat_result, sqrt(array_length), host_id);

     unsigned int grid_rows = (matrix_a_height + BLOCK_SIZE - 1) / BLOCK_SIZE;
     unsigned int grid_cols = (matrix_b_width + BLOCK_SIZE - 1) / BLOCK_SIZE;
     dim3 dimGrid(grid_cols, grid_rows);
     dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);

     __multiply__ <<<dimGrid, dimBlock>>> (mat_a_device, mat_b_device, mat_result_device, matrix_a_height, matrix_a_width_matrix_b_height, matrix_b_width, offset);
     cudaMemcpy(mat_result, mat_result_device, sizeof(double)*matrix_a_height*matrix_b_width, cudaMemcpyDeviceToHost);

     printf("\n result in buda before, with offset %d \n", offset);
     for (int i = 0; i < matrix_a_height * matrix_a_width_matrix_b_height; i++)
          printf("%d: %f, ", i, mat_result[i]);
     printf("\n");
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
