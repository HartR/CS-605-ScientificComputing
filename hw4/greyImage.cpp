#include <opencv2/core.hpp>

using namespace cv;
//! [includes]

int main()
{
    std::string image_path = "/home/605/russell/hw4/sunflower.jpg";
    Mat img = imread(image_path, IMREAD_COLOR);
    return 0;
}