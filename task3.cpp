#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>

using namespace cv;
using namespace std;

/* GLOBAL */
struct Points
{
    vector<Point> points1; // points on the 1st line
    vector<Point> points2; // points on the 2nd line
};
struct ROIregion
{
    Point topLeft;     // topleft coordinates of the ROI
    Point bottomRight; // bottom right coordinates of the ROI
};

/* DECLARATIONS */
Mat colorLines(Mat src, ROIregion reg, Points pts);
bool compareLength(Vec4i line1, Vec4i line2);
// Mat drawRedLine(Mat src, Point p1, Point p2);
// getPoints(vector<Vec4i> lines, Rect roi, Points *p);

int main(int argc, char **argv)
{
    string path = "../street_scene.png";
    Mat src = imread(path, IMREAD_COLOR);
    Mat grayImg, whiteMask, cannyImg, blurImg;

    cout << src.size();

    if (!src.data)
    {
        cout << " No such " << path << " found!" << endl;
        return -1;
    }

    cvtColor(src, grayImg, COLOR_BGR2GRAY);
    int min_threshold = 235; // near 255 to detect only white
    threshold(grayImg, whiteMask, min_threshold, 255, THRESH_BINARY);
    GaussianBlur(whiteMask, blurImg, Size(5, 5), 0);
    Canny(blurImg, cannyImg, 50, 150, 3); // edges

    // TAKE ROI
    Point topLeft(460, 100);
    Point bottomRight(800, 350);

    ROIregion reg;
    reg.bottomRight = bottomRight;
    reg.topLeft = topLeft;

    Rect roi(reg.topLeft.x, reg.topLeft.y, reg.bottomRight.x - reg.topLeft.x, reg.bottomRight.y - reg.topLeft.y);
    Mat result = cannyImg(roi);
    imshow("res", result);

    // HOUGH
    vector<Vec4i> lines;
    HoughLinesP(result, lines, 1, CV_PI / 180, 50, 50, 10);

    // SORT LINES
    sort(lines.begin(), lines.end(), compareLength);
    cout << "number of lines: " << lines.size() << endl;

    /* TAKES ALL POINTS FOR EVERY LINE */
    vector<Point> newPoints1;
    vector<Point> newPoints2;
    for (int i = 2; i < 4; i++)
    {
        auto line = lines[i];
        Point p1(line[0], line[1]);
        Point p2(line[2], line[3]);
        LineIterator iter(src, p1, p2);
        for (int j = 0; j < iter.count; j++, ++iter)
        {
            if (i % 2 == 0)
                newPoints1.push_back(iter.pos());
            else
                newPoints2.push_back(iter.pos());
        }
    }

    /*
        for (Point p : newPoints1)
            cout << "1 \t" << p.x << " " << p.y << endl;

        for (Point p : newPoints2)
            cout << "2 \t" << p.x << " " << p.y << endl;
    */

    Points pts;
    pts.points1 = newPoints1;
    pts.points2 = newPoints2;

    src = colorLines(src, reg, pts);

    imshow("Result", src);
    waitKey(0);

    return 0;
}

Mat colorLines(Mat src, ROIregion reg, Points pts)
{
    for (Point &p1 : pts.points1)
    {
        Point closestPoint;

        float dx = reg.bottomRight.x - reg.topLeft.x;
        float dy = reg.bottomRight.y - reg.topLeft.y;
        int maxDistanceROI = ceil(sqrt(dx * dx + dy * dy)); // Compute max distance in ROI

        int minDistance = maxDistanceROI; // set minDistance to Max

        for (Point &p2 : pts.points2)
        {
            if (p1.y == p2.y)
            {
                int distance = abs(p1.x - p2.x);
                if (distance < minDistance)
                {
                    minDistance = distance; // if distance is better, change!
                    closestPoint = p2;      // + new closest point!
                }
            }
        }
        // if for loops ended with different min distance...
        if (minDistance != maxDistanceROI)
        {
            Point fixedP1(p1.x + reg.topLeft.x, p1.y + reg.topLeft.y);                               // adjusted P1
            Point fixedClosestPoint(closestPoint.x + reg.topLeft.x, closestPoint.y + reg.topLeft.y); // adjusted Closest Point
            cv::line(src, fixedP1, fixedClosestPoint, Scalar(0, 0, 255), 3);
        }
    }
    return src;
}

bool compareLength(Vec4i line1, Vec4i line2)
{
    double length1 = sqrt(pow(line1[0] - line1[2], 2) + pow(line1[1] - line1[3], 2));
    double length2 = sqrt(pow(line2[0] - line2[2], 2) + pow(line2[1] - line2[3], 2));
    return length1 > length2;
}

/*
Mat drawRedLine(Mat img, Point p1, Point p2)
{
    Mat res = img.clone();

    if (p1.y != p2.y) // if points are on same y cord
        cout << "Error: Points are not on the same row" << endl;

    if (p1.x > p2.x) // if p1 is on the left of p2
        swap(p1, p2);

    for (int x = p1.x; x <= p2.x; x++)
        res.at<Vec3b>(p1.y, x) = Vec3b(0, 0, 255);

    return res;
}

void getPoints(vector<Vec4i> lines, Rect roi, Points *points)
{
    for (int i = 0; i < lines.size(); i++)
    {
        auto line = lines[i];
        Point p1(line[0], line[1]);
        Point p2(line[2], line[3]);
        if (roi.contains(p1))
        {
            if (i % 2 == 0)
                points->points1.push_back(p1);
            else
                points->points1.push_back(p1);
        }
        if (roi.contains(p2))
        {
            if (i % 2 == 0)
                points->points1.push_back(p2);
            else
                points->points1.push_back(p2);
        }
    }
}


*/
