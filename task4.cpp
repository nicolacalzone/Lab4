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

    Mat hsv;
    cvtColor(src, hsv, COLOR_BGR2HSV);

    Scalar lower_yellow = Scalar(20, 90, 100);
    Scalar upper_yellow = Scalar(30, 255, 255);

    Mat mask;
    inRange(hsv, lower_yellow, upper_yellow, mask); // mask for yellow

    Mat res;
    bitwise_and(src, src, res, mask = mask); // bitwise between orginal and mask

    imshow("Yellow detection", res);

    ///////

    cvtColor(res, res, COLOR_BGR2GRAY);

    vector<Vec3f> circles;
    HoughCircles(res, circles, HOUGH_GRADIENT, 2,
                 res.rows / 16,
                 100, 30, 1, 10); // if last parameter != 10 it detects more circles

    for (size_t i = 0; i < circles.size(); i++)
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        circle(src, center, radius, Scalar(0, 0, 255), 3);
    }

    imshow("Signal detection", src);
    waitKey(0);

    return 0;
}