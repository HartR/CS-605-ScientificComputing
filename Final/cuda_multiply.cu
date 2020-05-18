/* multiply.cu */
#include <cuda.h>
#include "mycuda.h"
#include <stdio.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <math.h>
 
using namespace std;

 __global__ void __multiply__ (double* a, double* b, double* c, int matrix_a_height, int matrix_a_width_matrix_b_height, int matrix_b_width, int offset, int mat_result_length)
 {
     
      
     int i = blockIdx.y * blockDim.y + threadIdx.y; 
     int j = blockIdx.x * blockDim.x + threadIdx.x;

     //I tried many solutions, but this is the only one that worked. It's not elegent, but it will have to do.
     //The var j keeps track of where on the array we are. Because we can't ensure that the kernel will be called the 
     //Correct number of times as there are elements in the result matrix
     //The offset variable is at the halfpoint of the result matrix size
     //This ensures that the multiplication work will be done halfway on the first node
     //And the second half will be done on the second node
     if(j < mat_result_length/2 && offset == 0) 
     {
          //printf ("TWO: %d is j, offset is%d\n", j, offset); 

         for(int k = 0; k < matrix_a_width_matrix_b_height; k++) 
         {
               c[i * matrix_b_width + j] += a[i * matrix_a_width_matrix_b_height + k] * b[k * matrix_b_width + j];
         }
     }
     else if (j > offset && j < mat_result_length/2 + offset) 
     {
         //printf ("ONE: %d is j, offset is%d\n", j, offset); 
         for(int k = 0; k < matrix_a_width_matrix_b_height; k++) 
         {
               c[i * matrix_b_width + j] += a[i * matrix_a_width_matrix_b_height + k] * b[k * matrix_b_width + j];
         }     
     }
 }

void MatrixMultiplyCuda(double* mat_a, double* mat_b, double* mat_result, int matrix_a_height, int matrix_a_width_matrix_b_height, int matrix_b_width, int host_id)
{
     cudaError_t cudaStatus;
     double* mat_a_device;
     double* mat_b_device;
     double* mat_result_device;

     //figure out ideal thread/block numbers
     //I'm targeting 256 threads, because we found that to be optimal from assignment 4
     int thread_number = 256;
     int block_number = 1;
     int mat_result_length = matrix_a_height*matrix_b_width;
     if(mat_result_length < thread_number)
     {
          thread_number = mat_result_length;
     }
     else if (mat_result_length > thread_number)
     {
          //get the ceiling of the division
          block_number = (mat_result_length + thread_number - 1)/thread_number;
     }
     
     //offset is the halfway point in the result matrix
     int offset = host_id * (mat_result_length)/2;


     cudaMalloc((void**)&mat_a_device, sizeof(double)*matrix_a_height*matrix_a_width_matrix_b_height);
     cudaMalloc((void**)&mat_b_device, sizeof(double)*matrix_a_width_matrix_b_height*matrix_b_width);
     cudaMalloc((void**)&mat_result_device, sizeof(double)*mat_result_length);
     cudaMemcpy(mat_a_device, mat_a, sizeof(double)*matrix_a_height*matrix_a_width_matrix_b_height, cudaMemcpyHostToDevice);
     cudaMemcpy(mat_b_device, mat_b, sizeof(double)*matrix_a_width_matrix_b_height*matrix_b_width, cudaMemcpyHostToDevice);
     cudaMemcpy(mat_result_device, mat_result, sizeof(double)*mat_result_length, cudaMemcpyHostToDevice);


     __multiply__ <<<block_number, thread_number>>> (mat_a_device, mat_b_device, mat_result_device, matrix_a_height, matrix_a_width_matrix_b_height, matrix_b_width, offset, mat_result_length);
     cudaMemcpy(mat_result, mat_result_device, sizeof(double)*mat_result_length, cudaMemcpyDeviceToHost);

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
