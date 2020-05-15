/* multiply.cu */
#include <cuda.h>
#include "mycuda.h"
#include <stdio.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
 
 __global__ void __multiply__ (double* a, double* b)
 {
          printf("Hello from block %d, thread %d\n", blockIdx.x, threadIdx.x);
 }
 

void call_me_maybe()
{
     cudaError_t cudaStatus;
     /* ... Load CPU data into GPU buffers  */
     double* mat_a_device, mat_b_device;
     double* mat_a, mat_b;
     int array_length = 1;
     //need to allocate result matrix
     cudaMalloc((void**)&mat_a_device, array_length*sizeof(double));
     cudaMalloc((void**)&mat_b_device, array_length*sizeof(double));
     cudaMemcpy(mat_a_device, &mat_a, array_length*sizeof(double), cudaMemcpyHostToDevice);
     cudaMemcpy(mat_b_device, &mat_b, array_length*sizeof(double), cudaMemcpyHostToDevice);
     __multiply__ <<<5, 5>>> (mat_a_device, mat_b_device);


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
