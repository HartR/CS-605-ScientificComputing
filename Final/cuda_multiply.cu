/* multiply.cu */
#include <cuda.h>
#include "mycuda.h"
#include <stdio.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
 
 __global__ void __multiply__ (double* a, double* b)
 {
          fprintf("Hello from block %d, thread %d\n", blockIdx.x, threadIdx.x);
 }
 
void FreeMemory(double* mat_a_device, double* mat_b_device)
{
     cudaFree(mat_a_device);
     cudaFree(mat_b_device);
}

void call_me_maybe(size_t a_size, size_t b_size, double* mat_a, double* mat_b)
{
     cudaError_t cudaStatus;
     /* ... Load CPU data into GPU buffers  */
     double* mat_a_device;
     double* mat_b_device;
     //need to allocate result matrix
     cudaMalloc((void**)&mat_a_device, a_size);
     cudaMalloc((void**)&mat_b_device, b_size);
     cudaMemcpy(mat_a_device, mat_a, a_size, cudaMemcpyHostToDevice);
     cudaMemcpy(mat_b_device, mat_b, b_size, cudaMemcpyHostToDevice);
     __multiply__ <<<5, 5>>> (mat_a_device, mat_b_device);


     cudaGetLastError();

     cudaDeviceSynchronize();

     FreeMemory(mat_a_device, mat_b_device);
     cudaDeviceReset();    /* ... Transfer data from GPU to CPU */
}
