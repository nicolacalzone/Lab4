#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// settings for eddge detection
Mat img = imread("street_scene.png");
Mat output = img.clone();
Mat mask(img.rows, img.cols, CV_8UC1);

Mat canny, hsv;
int KSize = 3;
int lowT = 0; // optimal values
int maxT = lowT + 100;

static void CannyT(int, void *)
{
    // computing edges on the previously computed smask
    cvtColor(img, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(0, 0, 255 - lowT), Scalar(180, 50, 255), mask);
    bitwise_not(mask, mask);
    Canny(mask, canny, 50, 150, 3);

    imshow("Edge Map", canny);
}

int main(int argc, char **argv)
{

    int offset = ceil((KSize - 1) / 2);
    int T = 51, HouhghT = 100;
    uchar white = 255;

    try
    {
        if (img.empty())
        {
            throw invalid_argument("Image empty.");
        }
    }
    catch (invalid_argument &e)
    {
        cout << "Error: " << e.what() << endl;
    }

    namedWindow("Input", WINDOW_AUTOSIZE);
    imshow("Input", img);
    waitKey(0);

    // PRE-PROCESSING BY COLOR
    for (int i = offset; i < img.rows - offset; i++)
    {
        for (int j = offset; j < img.cols - offset; j++)
        {

            if (!((i - offset < 0)(i + offset > img.rows)(j - offset < 0) || (j + offset > img.cols)))
            {
                {

                    Rect ker(j - offset, i - offset, KSize, KSize); // <--
                    Mat kernel = img(ker);
                    Scalar mean = cv::mean(kernel);

                    // mask computing
                    if ((mean[0] < white + T) && (mean[0] > white - T) &&
                        (mean[1] < white + T) && (mean[1] > white - T) &&
                        (mean[2] < white + T) && (mean[2] > white - T))
                    {

                        mask.at<uchar>(i, j) = 255;
                    }
                    else
                    {
                        mask.at<uchar>(i, j) = 127;
                    }
                }
                else
                {
                    cout << "kernel cant be computed on the borders" << endl;
                    mask.at<uchar>(i, j) = 0;
                }
            }
        }
        imshow("Mask computed by preprocessing", mask);
        waitKey(0);

        namedWindow("Edge Map", WINDOW_AUTOSIZE);
        createTrackbar("Min Treshold: ", "Edge Map", &lowT, maxT, CannyT); // a good T could be '90'
        CannyT(0, 0);
        waitKey(0);
        destroyAllWindows();

        namedWindow("Edge detected", WINDOW_AUTOSIZE);
        imshow("Edge detected", canny);
        waitKey(0);

        vector<Vec4i> lines;
        HoughLinesP(canny, lines, 1, CV_PI / 180, HouhghT, 100, 30);

        // Find the two strongest lines
        Vec4i line0(0, 0, 0, 0);
        Vec4i line1(0, 0, 0, 0);
        Vec4i line2(0, 0, 0, 0);
        double max0 = 0, max1 = 0, max2 = 0;

        for (size_t i = 0; i < lines.size(); i++)
        {

            Vec4i l = lines[i];
            double len = sqrt(pow(l[2] - l[0], 2) + pow(l[3] - l[1], 2));
            if (len > max0)
            {
                line2 = line1;
                max2 = max1;
                line1 = line0;
                max1 = max0;
                line0 = l;
                max0 = len;
            }
            else if (len > max1)
            {
                line2 = line1;
                max2 = max1;
                line1 = l;
                max1 = len;
            }
            else if (len > max2)
            {
                line2 = l;
                max2 = len;
            }
        }

        Point pt1 = Point(line1[0], line1[1]);
        Point pt2 = Point(line1[2], line1[3]);
        Point pt3 = Point(line2[0], line2[1]);
        Point pt4 = Point(line2[2], line2[3]);

        // Calculate the intersection point
        double slope1 = (double)(pt2.y - pt1.y) / (double)(pt2.x - pt1.x);
        double y1 = pt1.y - slope1 * pt1.x;
        double slope2 = (double)(pt4.y - pt3.y) / (double)(pt4.x - pt3.x);
        double y2 = pt3.y - slope2 * pt3.x;

        double x_intersect = (y2 - y1) / (slope1 - slope2);
        double y_intersect = slope1 * x_intersect + y1;
        Point point(x_intersect, y_intersect);

        // calculate the intersection with the 'origin'
        double x1 = (output.rows - y1) / slope1;
        double x2 = (output.rows - y2) / slope2;

        // Fill the area between the lines in red
        std::vector<Point> roi = {Point(x1, output.rows), point, point, Point(x2, output.rows)};
        fillPoly(output, roi, Scalar(0, 0, 255));

        namedWindow("Filled lines", WINDOW_AUTOSIZE);
        imshow("Filled lines", output);
        waitKey(0);

        destroyAllWindows();
        return 0;
    }