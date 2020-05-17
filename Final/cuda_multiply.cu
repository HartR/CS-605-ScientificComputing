/* multiply.cu */
#include <cuda.h>
#include "mycuda.h"
#include <stdio.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <math.h>


 
using namespace std;

 __global__ void __multiply__ (double* a, double* b, double* c, int m, int n, int k, int offset)
 {
     int row = blockIdx.y * blockDim.y + threadIdx.y; 
     int col = blockIdx.x * blockDim.x + threadIdx.x;
     int sum = 0;
     if( col < k && row < m) 
     {
         for(int i = offset; i < n/2; i++) 
         {
             sum += a[row * n + i] * b[i * k + col];
             printf("\ni is %d, a is %f, b is %f", i, a[row * n + i], b[i * k + col]);
         }
         c[row * k + col + offset] = sum;
         printf("\n\n");
         //printf("\n At location %d, in c, assigned value %f, sum is %f, value of a is %f, val of b is %f", row * k + col + offset, c[row * k + col + offset], a[i], b[i]);

     }
     printf("\n\n");
     /*int pixel = blockIdx.x * blockDim.x + threadIdx.x;
     if (pixel < half_length)
     {
          printf("\nIn matrix b, current value in result: %f, value at %d: %f, ", result[pixel], pixel, b[pixel]);
          result[pixel] = b[pixel+offset];
     }
     printf("\n\n");*/
 }

 /*
 void PrintMatrix(double* matrix, int N, int p)
{
     printf("\nPrinting in CUDA on host %d \n", p);
    for (int row = 0; row < N; row++)
    {
        for (int column = 0; column < N; column++)
            printf("%f ", matrix[N * row + column]);
        printf("\n");
    }  
}
 */

void MatrixMultiplyCuda(double* mat_a, double* mat_b, double* mat_result, int m, int n, int k, int host_id)
{
     cudaError_t cudaStatus;
     double* mat_a_device;
     double* mat_b_device;
     double* mat_result_device;

     //figure out ideal thread/block numbers
     //I'm using 256 threads, because we found that to be optimal from assignment 4
     int thread_number = 256;
     int block_number = 1;
     int array_length = m*k;
     if(array_length < thread_number)
     {
          thread_number = array_length;
     }
     else if (array_length > thread_number)
     {
          //get the ceiling of the division
          block_number = (array_length + thread_number - 1)/thread_number;
     }
     int offset = host_id * ((m*k)/2);

     //thread_number*block_number == array_length/2
     cudaMalloc((void**)&mat_a_device, sizeof(double)*m*n);
     cudaMalloc((void**)&mat_b_device, sizeof(double)*n*k);
     cudaMalloc((void**)&mat_result_device, sizeof(double)*m*k);
     cudaMemcpy(mat_a_device, mat_a, sizeof(int)*m*n, cudaMemcpyHostToDevice);
     cudaMemcpy(mat_b_device, mat_b, sizeof(int)*n*k, cudaMemcpyHostToDevice);
     cudaMemcpy(mat_result_device, mat_result, sizeof(double)*m*k, cudaMemcpyHostToDevice);
     //PrintMatrix(mat_result, sqrt(array_length), host_id);

     __multiply__ <<<block_number, thread_number>>> (mat_a_device, mat_b_device, mat_result_device, m, n, k, offset);
     cudaMemcpy(mat_result, mat_result_device, sizeof(double)*m*k, cudaMemcpyDeviceToHost);


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
