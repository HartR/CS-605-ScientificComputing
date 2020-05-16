/* multiply.cu */
#include <cuda.h>
#include "mycuda.h"
#include <stdio.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"


 
using namespace std;

 __global__ void __multiply__ (double* a, double* b)
 {
     int pixel = blockIdx.x * blockDim.x + threadIdx.x;
     printf("Value at %d: %f, ", pixel, a[pixel]);

 }

 void PrintMatrix(double* matrix, int N)
{
     printf("\nPrinting in CUDA\n");
    for (int row = 0; row < N; row++)
    {
        for (int column = 0; column < N; column++)
            printf("%f ", matrix[N * row + column]);
        printf("\n");
    }  
}
 

void MatrixMultiplyCuda(double* mat_a, double* mat_b, int array_length)
{
     PrintMatrix(mat_a, array_length);
     cudaError_t cudaStatus;
     /* ... Load CPU data into GPU buffers  */
     double* mat_a_device;
     double* mat_b_device;
     //need to allocate result matrix
     cudaMalloc((void**)&mat_a_device, array_length*sizeof(double));
     cudaMalloc((void**)&mat_b_device, array_length*sizeof(double));
     cudaMemcpy(mat_a_device, &mat_a, array_length*sizeof(double), cudaMemcpyHostToDevice);
     cudaMemcpy(mat_b_device, &mat_b, array_length*sizeof(double), cudaMemcpyHostToDevice);
     __multiply__ <<<2, 2>>> (mat_a_device, mat_b_device);


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

     cudaStatus = cudaDeviceReset();
     if (cudaStatus != cudaSuccess) {
          fprintf(stderr, "cudaDeviceReset failed!");
     }     /* ... Transfer data from GPU to CPU */
}
