#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;

// GLOBAL VARIABLES
std::string window_name = "Edge Map";
const int max_lowThreshold = 100;
int lowThreshold = 0;
const int kernel_size = 7;

Mat src, src_gray;
Mat dst, detected_edges;

static void CannyThreshold(int pos, void *user)
{
    cvtColor(src, src_gray, COLOR_BGR2GRAY);
    GaussianBlur(src_gray, dst, Size(kernel_size, kernel_size), 0, 0);
    Canny(dst, detected_edges, lowThreshold, max_lowThreshold);
    dst = Scalar::all(0);
    src.copyTo(dst, detected_edges);
    imshow(window_name, dst);
}

// MAIN
int main(int argc, char **argv)
{
    /*   struct userdata
     {
         int th;
     } data;
 */
    src = imread("../street_scene.png", IMREAD_COLOR);
    dst.create(src.size(), src.type());

    namedWindow(window_name, WINDOW_AUTOSIZE);
    createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

    CannyThreshold(0, 0);

    waitKey(0);
    return 0;
}
