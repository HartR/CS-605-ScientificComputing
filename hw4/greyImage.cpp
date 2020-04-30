#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

//using namespace cv;
//! [includes]

int main()
{
    String image_path = "/home/605/russell/hw4/sunflower.jpg";
    Mat img = imread(image_path, IMREAD_COLOR);
    return 0;
}