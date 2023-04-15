#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>

using namespace cv;
using namespace std;

vector<Vec4i> findMaxLines(vector<Vec4i> lines, int numLines);
Mat drawRedLine(Mat img, Point p1, Point p2);
vector<Point> getPointsInRoi(vector<Vec4i> lines, Rect roi);
bool compareLength(Vec4i line1, Vec4i line2);

int main(int argc, char **argv)
{
    string path = "../street_scene.png";
    Mat src = imread(path, IMREAD_COLOR);
    Mat grayImg, whiteMask, cannyImg, blurImg;

    if (!src.data)
    {
        cout << " No such " << path << " found!" << endl;
        return -1;
    }

    cvtColor(src, grayImg, COLOR_BGR2GRAY);
    int min_threshold = 240; // near 255 to detect only white
    threshold(grayImg, whiteMask, min_threshold, 255, THRESH_BINARY);
    GaussianBlur(whiteMask, blurImg, Size(5, 5), 0);
    Canny(blurImg, cannyImg, 50, 150); // edges

    // TAKE ROI
    Point topLeft(460, 120);
    Point bottomRight(800, 340);
    Rect roi(topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);
    Mat result = cannyImg(roi);

    // SORT LINES
    vector<Vec4i> lines;
    sort(lines.begin(), lines.end(), compareLength);
    cout << "lenght of lines is " << lines.size() << endl;

    // HOUGH
    HoughLinesP(cannyImg, lines, 1, CV_PI / 180, 50, 50, 10);

    // ROI --> arr
    vector<Point> arr = getPointsInRoi(lines, roi);

    // int linesToFind = 2;
    // if (!lines.empty())
    // lines = findMaxLines(lines, linesToFind);

    if (!lines.empty()) // draws strongest lines on src
    {
        /* for (auto line : lines)
        {
            cv::line(src,
                     Point(line[0], line[1]),
                     Point(line[2], line[3]),
                     Scalar(0, 0, 255),
                     2);
        } */

        for (int i = 0; i < 2 && i < lines.size(); i++)
        {
            Vec4i line = lines[i];
            cv::line(src,
                     Point(line[0], line[1]),
                     Point(line[2], line[3]),
                     Scalar(0, 0, 255),
                     2);
        }
    }

    // DRAW EVERY ROW
    for (int i = 0; i < 2 && i < lines.size(); i++)
    {
        auto line = lines[i];
        arr.push_back(Point(line[0], line[1]));
        arr.push_back(Point(line[2], line[3]));
    }

    for (int i = 0; i < arr.size(); i += 2)
        src = drawRedLine(src, arr[i], arr[i + 1]);

    imshow("Result", src);
    waitKey(0);

    return 0;
}

bool compareLength(Vec4i line1, Vec4i line2)
{
    double length1 = sqrt(pow(line1[0] - line1[2], 2) + pow(line1[1] - line1[3], 2));
    double length2 = sqrt(pow(line2[0] - line2[2], 2) + pow(line2[1] - line2[3], 2));
    return length1 > length2;
}

vector<Point> getPointsInRoi(vector<Vec4i> lines, Rect roi)
{
    vector<Point> arr;
    for (auto line : lines)
    {
        Point p1(line[0], line[1]);
        Point p2(line[2], line[3]);
        if (roi.contains(p1))
        {
            arr.push_back(p1);
        }
        if (roi.contains(p2))
        {
            arr.push_back(p2);
        }
    }
    return arr;
}

Mat drawRedLine(Mat img, Point p1, Point p2)
{
    Mat res = img.clone();

    if (p1.y != p2.y) // if points are on same y cord
    {
        cout << "Error: Points are not on the same row" << endl;
        return res;
    }

    if (p1.x > p2.x) // if p1 is on the left of p2
        swap(p1, p2);

    for (int x = p1.x; x <= p2.x; x++)
        res.at<Vec3b>(p1.y, x) = Vec3b(0, 0, 255);

    return res;
}
