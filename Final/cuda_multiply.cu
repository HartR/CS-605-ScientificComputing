/* multiply.cu */
#include <cuda.h>
#include "mycuda.h"
#include <stdio.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <math.h>


 
using namespace std;

 __global__ void __multiply__ (double* a, double* b, double* result, int offset, int half_length)
 {
     int pixel = blockIdx.x * blockDim.x + threadIdx.x;
     if (pixel < half_length)
     {
          printf("\nIn matrix b, current value in result: %f, value at %d: %f, ", result[pixel], pixel, b[pixel]);
          result[pixel] = b[pixel+offset];
     }
     printf("\n\n");
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

void MatrixMultiplyCuda(int host_id)
{
     cudaError_t cudaStatus;
     double* mat_a_device;
     double* mat_b_device;
     double* mat_result_device;
     size_t matrix_size = SIZE*sizeof(double);

     //figure out ideal thread/block numbers
     //I'm using 256 threads, because we found that to be optimal from assignment 4
     int thread_number = 256;
     int block_number = 1;
     if(SIZE < thread_number)
     {
          thread_number = SIZE;
     }
     else if (SIZE > thread_number)
     {
          //get the ceiling of the division
          block_number = (SIZE + thread_number - 1)/thread_number;
     }
     int offset = host_id * (HALF);

     //thread_number*block_number == HALF
     cudaMalloc((void**)&mat_a_device, matrix_size);
     cudaMalloc((void**)&mat_b_device, matrix_size);
     cudaMalloc((void**)&mat_result_device, matrix_size/2);
     cudaMemcpy(mat_a_device, matrix_a, matrix_size, cudaMemcpyHostToDevice);
     cudaMemcpy(mat_b_device, matrix_b, matrix_size, cudaMemcpyHostToDevice);
     doubl* mat_result;
     if(host_id == 0)
          mat_result = matrix_result_1;
     else
          mat_result = matrix_result_2;
     
     cudaMemcpy(mat_result_device, mat_result, matrix_size/2, cudaMemcpyHostToDevice);

     //PrintMatrix(mat_result, sqrt(SIZE), host_id);

     __multiply__ <<<block_number, thread_number>>> (mat_a_device, mat_b_device, mat_result_device, offset, HALF);
     cudaMemcpy(mat_result, mat_result_device, matrix_size/2, cudaMemcpyDeviceToHost);


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
