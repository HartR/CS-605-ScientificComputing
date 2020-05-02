
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

__global__ void MakeGrey(uchar* gpu_color_img, uchar* gpu_grey_img, int columns)
{
    unsigned int row = blockIdx.y * blockDim.y + threadIdx.y;
    unsigned int col = blockIdx.x * blockDim.x + threadIdx.x;
    int gpu_color_pixel = (row * columns + col) * 3;
    gpu_grey_img[row * columns + col] = gpu_color_img[gpu_color_pixel] * 0.11 \
        + gpu_color_img[gpu_color_pixel + 1] * 0.59 + gpu_color_img[gpu_color_pixel + 2] * 0.3;
}

cudaError_t CopyAndExecute(const Mat& color_img, const Mat& grey_img, int rows, int columns)
{
    uchar* gpu_color_img;
    uchar* gpu_grey_img;
    size_t color_img_size = color_img.rows * color_img.cols * sizeof(Vec3b);
    size_t grey_img_size = grey_img.rows * grey_img.cols * sizeof(uchar);
    cout << "size of color image: " << color_img_size << endl;
    cout << "size of grey img: " << grey_img_size << endl;

    cudaError_t cudaStatus;

    cudaStatus = cudaMalloc((void**)&gpu_color_img, color_img_size);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&gpu_grey_img, grey_img_size);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    // Copy input vectors from host memory to GPU buffers.
    cudaStatus = cudaMemcpy(gpu_color_img, color_img.data, color_img_size, cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    cudaStatus = cudaMemcpy(gpu_grey_img, grey_img.data, grey_img_size, cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    MakeGrey <<<1200,1000>>>(gpu_color_img, gpu_grey_img, columns);

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        goto Error;
    }

    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }

    // Copy output vector from GPU buffer to host memory.
    cudaStatus = cudaMemcpy(grey_img.data, gpu_grey_img, grey_img_size, cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    Error:
        cudaFree(gpu_color_img);
        cudaFree(gpu_grey_img);
    
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

    //get data from color picture and create grey picture
    string image_path = samples::findFile("sunflower.jpg");
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
    
    /*
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            Vec3b color_pixel = color_img.at<Vec3b>(Point(x, y));
            Scalar grey_pixel = grey_img.at<uchar>(Point(x, y));
            grey_pixel = (color_pixel[0] * 0.11 + color_pixel[1] * 0.59 + color_pixel[2] * 0.3);

            grey_img.at<uchar>(Point(x, y)) = grey_pixel.val[0];
        }
    }
    */


    imshow("Display Window", grey_img);
    imwrite("sunflower_greyscale.jpg", grey_img);
    waitKey(0);


    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        return 1;
    }

    return 0;
}
