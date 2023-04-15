#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    string path = "../street_scene.png";
    Mat src = imread(path, IMREAD_COLOR);
    Mat gray, canny, blur;

    if (!src.data)
    {
        cout << " No such " << path << " found!" << endl;
        return -1;
    }

    cvtColor(src, gray, COLOR_BGR2GRAY);

    // Thresholding
    Mat thImg;
    threshold(gray, thImg, 240, 255, THRESH_BINARY);

    /* string cannydet = "Canny detection";
     namedWindow(cannydet, WINDOW_KEEPRATIO);
     imshow(cannydet, canny);
  */

    /* Ptr<LineSegmentDetector> det;
    det = createLineSegmentDetector();

    Mat lines;
    det->detect(canny, lines);
    det->drawSegments(src, lines);
    imshow("Result", src);
    */

    Mat morph_image;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(thImg, morph_image, MORPH_CLOSE, kernel);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(morph_image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); i++)
    {
        Scalar color = Scalar(0, 0, 255);
        drawContours(src, contours, (int)i, color, 2, LINE_8, hierarchy, 0);
    }
    imshow("Contours", src);

    waitKey(0);

    return 0;
}