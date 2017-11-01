#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "highgui.h"
#include <iostream>

using namespace cv;
using namespace std;

int main()
{

	VideoCapture cap(0);

	if(!cap.isOpened())
	{
		cout << "Cannot load the image\n";
		return -1;
	}

	namedWindow("original + Tracking", CV_WINDOW_NORMAL);
	namedWindow("Binary_Image",CV_WINDOW_NORMAL);

	int lowH = 75;
	int highH = 160;

	int lowS = 50;
	int highS = 200;

	int lowV = 50;
	int highV = 220;

	createTrackbar("low Hue", "original", &lowH, 179);
	createTrackbar("high hue", "original", &highH, 179);

	createTrackbar("low saturation", "original", &lowS, 255);
	createTrackbar("high saturation", "original", &highS, 255);

	createTrackbar("low value", "original", &lowV, 255);
	createTrackbar("high value", "original", &highV, 255);

	int x = -1;
	int y = -1;

	Mat temp_img;
	cap.read(temp_img);

	Mat lineimg = Mat::zeros(temp_img.size(), CV_8UC3);

	while(1)
	{
		Mat frame;
		bool can = cap.read(frame);

		if(!can)
		{
			cout << "cannot load the frame\n";
			break;
		}
		Mat img_HSV;
		cvtColor(frame, img_HSV, CV_BGR2HSV);
		Mat threshhold_img;

		inRange(img_HSV, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), threshhold_img);

		erode(threshhold_img, threshhold_img, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(threshhold_img, threshhold_img, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		dilate(threshhold_img, threshhold_img, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(threshhold_img, threshhold_img, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		Moments om = moments(threshhold_img);
		double m01 = om.m01;
		double m10 = om.m10;
		double area = om.m00;

		if(area > 400)
		{
			int posx = m10/area;
			int posy = m01/area;

			system(("xdotool mousemove " + to_string(posx) + " " + to_string(posy)).c_str());

			if(x >= 0 and y >= 0 and posx >= 0 and posy >= 0)
			{
				line(lineimg, Point(posx, posy), Point(x, y), Scalar(0,12,200), 7, 3);
			}

			x = posx;
			y = posy;

		}
		frame = frame + lineimg;

		imshow("original + Tracking", frame);
		imshow("Binary_Image", threshhold_img);

		if(waitKey(30) == 27)
		{
			cout << "Pressed esc\n";
			break;
		}
	}
	return 0;
}