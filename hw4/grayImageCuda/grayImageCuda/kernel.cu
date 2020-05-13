
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <filesystem>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

__global__ void MakeGrey(uchar* gpu_color_img, uchar* gpu_grey_img, size_t grey_img_size)
{
    //get position of grey pixel in current CUDA thread
    int pixel = blockIdx.x * blockDim.x + threadIdx.x;
    //there are 3 color pixels per grey pixel
    int gpu_color_pixel = pixel * 3;
    //get the three values from the color picture and assign the corresponding grey value to the grey picture
    gpu_grey_img[pixel] = gpu_color_img[gpu_color_pixel] * 0.11 \
        + gpu_color_img[gpu_color_pixel + 1] * 0.59 + gpu_color_img[gpu_color_pixel + 2] * 0.3;
}

void FreeMemory(uchar* gpu_color_img, uchar* gpu_grey_img)
{
    cudaFree(gpu_color_img);
    cudaFree(gpu_grey_img);
}

cudaError_t CopyAndExecute(const Mat& color_img, const Mat& grey_img, int rows, int columns)
{
    //create variables for use
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaError_t cudaStatus;
    uchar* gpu_color_img;
    uchar* gpu_grey_img;
    float milliseconds = 0;
    size_t color_img_size = color_img.rows * color_img.cols * sizeof(Vec3b);
    size_t grey_img_size = grey_img.rows * grey_img.cols;
    cout << "size of color image: " << color_img_size << endl;
    cout << "size of grey img: " << grey_img_size << endl;

    //allocate memory for the color and grey image on the GPU, then copy them over to device memory
    cudaStatus = cudaMalloc((void**)&gpu_color_img, color_img_size);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed at color image!");
        FreeMemory(gpu_color_img, gpu_grey_img);
    }

    cudaStatus = cudaMalloc((void**)&gpu_grey_img, grey_img_size);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed at grey image!");
        FreeMemory(gpu_color_img, gpu_grey_img);
    }

    cudaStatus = cudaMemcpy(gpu_color_img, color_img.data, color_img_size, cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed at color image!");
        FreeMemory(gpu_color_img, gpu_grey_img);
    }

    cudaStatus = cudaMemcpy(gpu_grey_img, grey_img.data, grey_img_size, cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed at grey image!");
        FreeMemory(gpu_color_img, gpu_grey_img);
    }
    
    //To get many data points, use a nested for loop with a variety of parameters
    int thread_numbers[6] = {32, 64, 128, 256, 512, 1024};
    for (int i = 0; i < 6; i++)
    {
        //ensure that we allocate the correct number of threads 
        int block_number = grey_img_size / thread_numbers[i];
        cudaEventRecord(start);
        MakeGrey << <block_number, thread_numbers[i] >> > (gpu_color_img, gpu_grey_img, grey_img_size);
        cudaEventRecord(stop);

        //record time with parameters
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&milliseconds, start, stop);
        cout << "With " << thread_numbers[i] << " threads "
            << "and "<< block_number << " blocks "
            << "it took " << milliseconds << "milliseconds" << endl;      
    }

    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        FreeMemory(gpu_color_img, gpu_grey_img);
    }

    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        FreeMemory(gpu_color_img, gpu_grey_img);
    }

    cudaStatus = cudaMemcpy(grey_img.data, gpu_grey_img, grey_img_size, cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy to host failed!");
        FreeMemory(gpu_color_img, gpu_grey_img);
    }
    return cudaStatus;
}

int main()
 {
    //Print out NVIDIA device data
    int nDevices;
    cudaGetDeviceCount(&nDevices);
    for (int i = 0; i < nDevices; i++) {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, i);
        cout << "Device Number: " << i << endl;
        cout << "Device name: " << prop.name << endl;
        cout << "Memory Clock Rate (KHz): " << prop.memoryClockRate << endl;
        cout << "Memory Bus Width (bits): " << prop.memoryBusWidth << endl;
        cout << "Max threads per block: " << prop.maxThreadsPerBlock << endl;
        cout << "Warp Size: " << prop.warpSize << endl;
        cout << "Max grid size x, y, z: "
            << prop.maxGridSize[0] << ", "   
            << prop.maxGridSize[1] << ", "
            << prop.maxGridSize[2]
            << endl;
        cout << "Max threads x, y, z: " 
            << prop.maxThreadsDim[0] << ", "
            << prop.maxThreadsDim[1] << ", "
            << prop.maxThreadsDim[2]
            << endl;
    }

    //try with a variety of image sizes
    string images[3] = { "outside-small", "outside-medium", "outside-large"};
    for (int i = 0; i < 3; i++)
    {
        cout << "Converting " << images[i] << endl;
        //use opencv to find the picture and create empty image of the same size
        #ifdef __linux__ 
            string image_path = filesystem::current_path() + images[i] + ".jpg";
        #elif _WIN32
            string image_path = samples::findFile(images[i] + ".jpg");
        #endif
        Mat color_img = imread(image_path, IMREAD_COLOR);
        int rows = color_img.rows;
        int cols = color_img.cols;
        Mat grey_img = Mat::zeros(rows, cols, CV_8UC1);
        if (color_img.empty())
        {
            cout << "Could not read the image: " << image_path << std::endl;
            return 1;
        }

        cudaError_t cudaStatus = CopyAndExecute(color_img, grey_img, rows, cols);
        if (cudaStatus != cudaSuccess) {
            fprintf(stderr, "take_input failed!");
            return 1;
        }

        //save to file system
        imwrite(images[i] + "-greyscale" + ".jpg", grey_img);

        cudaStatus = cudaDeviceReset();
        if (cudaStatus != cudaSuccess) {
            fprintf(stderr, "cudaDeviceReset failed!");
            return 1;
        }
        cout << endl;
    }

    return 0;
}
