/* multiply.cu */
#include <cuda.h>
#include "mycuda.h"
#include <stdio.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <math.h>


 
using namespace std;

 __global__ void __multiply__ (double* a, double* b, double* result, int offset)
 {
     int pixel = blockIdx.x * blockDim.x + threadIdx.x;
     pixel += offset;
     printf("\nIn matrix b, current value in result: %f, value at %d: %f, ", result[pixel], pixel, b[pixel]);
     result[pixel] = b[pixel];

 }

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
 

void MatrixMultiplyCuda(double* mat_a, double* mat_b, double* mat_result, int array_length, int host_id)
{
     cudaError_t cudaStatus;
     /* ... Load CPU data into GPU buffers  */
     double* mat_a_device;
     double* mat_b_device;
     double* mat_result_device;
     size_t matrix_size = array_length*sizeof(double);
     //I'm using 256 threads, because we found that to be optimal from assignment 4
     int thread_number = 256;
     int block_number;

     if(array_length > thread_number)
     {
          thread_number = array_length;
          block_number = 1;
     }
     else
     {
          block_number = array_length/thread_number;
     }
     int offset = host_id * (array_length/2);
     //thread_number*block_number == array_length/2
     cudaMalloc((void**)&mat_a_device, matrix_size);
     cudaMalloc((void**)&mat_b_device, matrix_size);
     cudaMalloc((void**)&mat_result_device, matrix_size);
     cudaMemcpy(mat_a_device, mat_a, matrix_size, cudaMemcpyHostToDevice);
     cudaMemcpy(mat_b_device, mat_b, matrix_size, cudaMemcpyHostToDevice);
     cudaMemcpy(mat_result_device, mat_result, matrix_size, cudaMemcpyHostToDevice);
     PrintMatrix(mat_result, sqrt(array_length), host_id);

     __multiply__ <<<3, 3>>> (mat_a_device, mat_b_device, mat_result_device, offset);
     cudaMemcpy(mat_result, mat_result_device, matrix_size, cudaMemcpyDeviceToHost);


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
