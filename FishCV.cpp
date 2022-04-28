#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <chrono>



double PI = 3.14159265359;

static int d = 10;

cv::Mat img;
cv::Mat Processed;
cv::Mat Origin;

cv::VideoCapture cap(0);
char charCheckForEscKey = 0;

std::vector<std::vector<int>> newPoints;  // to store all points

std::vector<std::vector<int>> theColors{ {11,225,197,22,255,255},// orange
	{44,46,122,118,225,225} //green
};

std::vector<cv::Scalar> ColorTrailValues
{
	{0,255,0},// green
	{255,165,0}//orange
};

cv::Point getContours(cv::Mat image) {


	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	findContours(image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	drawContours(img, contours, -1, cv::Scalar(255, 0, 255), 2);
	std::vector<std::vector<cv::Point>> conPoly(contours.size());
	std::vector<cv::Rect> boundRect(contours.size());

	cv::Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		std::cout << area << std::endl;

		std::string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			std::cout << conPoly[i].size() << std::endl;
			boundRect[i] = boundingRect(conPoly[i]);
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y;

			drawContours(img, conPoly, i, cv::Scalar(255, 0, 255), 2);
		 rectangle(img, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 255, 0), 5);
		}
	}
	return myPoint;
}

cv::Mat matOriginal;
cv::Mat matProcessed;

int morph_pixel = 5;

int Y1 = 7;
int Cr1 = 0;
int Cb1 = 0;
int Y2 = 170;
int Cr2 = 144;
int Cb2 = 131;

std::vector<cv::Vec3f> v3fCircles;

int acc_res = 2;
int min_dis = 500;
int high_thres = 50;
int low_thres = 35;
int min_rad = 10;
int max_rad = 110;

std::string posision;
float Corner;
std::string S_Corner;
float theDistance;
std::string S_distance;
float Diameter;
int x_center;
int y_center;
float R;
float d_near;
float d_far;
float x_corner = 160;
float y_corner = 490; //if the camera's viewing angle is 49.2 degrees y_angle value : 490 || if the angle of view of the camera is 47.2 degrees, the value of y_angle: 450


//y = ar + b
float a_near1 = 0.0528045;
float b_near1 = 90.084;
float a_far1 = 0.046262;
float b_far1 = 47.6737;
float j_ref_near1 = 50;
float j_ref_far1 = 100;

int limit = 77; //if the camera's viewing angle is 49.2 degrees limit value1 : 77 || if the camera's viewing angle is 47.2 degrees, the limit value is 1: 58

float a_near2 = 0.046262;
float b_near2 = 47.6737;
float a_far2 = 0.0500693;
float b_far2 = 32.951;
float j_ref_near2 = 100;
float j_ref_far2 = 150;

int batas2 = 45; //if the camera's viewing angle is 49.2 degrees limit value2 : 45 || if the camera angle of view is 47.2 degrees, the value of limit2: 27

float a_near3 = 0.0500693;
float b_near3 = 32.951;
float a_far3 = 0.0674082;
float b_far3 = 22.4401;
float j_ref_near3 = 150;
float j_ref_far3 = 200;

int batas3 = 29; //if the angle of view of the camera is 49.2 degrees the limit value3 : 29 || if the camera's viewing angle is 47.2 degrees, the limit value is 3 : 11

float a_near4 = 0.0674082;
float b_near4 = 22.4401;
float a_far4 = 0.0389122;
float b_far4 = 20.1244;
float j_ref_near4 = 200;
float j_ref_far4 = 250;

int batas4 = 17; //if the camera angle of view is 49.2 degrees limit value4 : 17 || if the camera angle of view is 47.2 degrees limit value4 : 0

float a_near5 = 0.0389122;
float b_near5 = 20.1244;
float a_far5 = 0.0232639;
float b_far5 = 18.7136;
float j_ref_near5 = 250;
float j_ref_far5 = 300;


class calculation
{

public:
	float difference;

	float theRadian(float x, float y) {
		float R = sqrt((pow((x - 160), 2)) + (pow((y - 120), 2)));
		return R;
	}

	float near(float near_one, float near_two, float r) {
		float near_three = ((near_one * r) + near_two);
		return near_three;
	}

	float far(float fat_one, float far_two, float r) {
		float far_three = ((fat_one * r) + far_two);
		return far_three;
	}

	float distance(float num, float d_near, float d_far, float jr_near, float jr_far) {
		float dist = (((jr_far - jr_near) * (num - d_near)) / (d_far - d_near)) + jr_near;
		difference = dist;
		return dist;
	};

	float corner(float x_center, float y_center) {
		float theCorner = float(atan((x_corner - x_center) / (y_corner - y_center)) * 180 / PI);
		return theCorner;
	}
};


std::vector<std::vector<int>> findColor(cv::Mat img)
{
	cv::Mat imgHSV;
	cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);

	for (int i = 0; i < theColors.size(); i++)
	{
		cv::Scalar lower(theColors[i][0], theColors[i][1], theColors[i][2]);
		cv::Scalar upper(theColors[i][3], theColors[i][4], theColors[i][5]);
		cv::Mat mask;
		inRange(imgHSV, lower, upper, mask);
		imshow(std::to_string(i), mask);
		cv::Point myPoint = getContours(mask);
		if (myPoint.x != 0) {
			newPoints.push_back({ myPoint.x,myPoint.y,i });
		}
	}
	return newPoints;
}

void drawOnCanvas(std::vector<std::vector<int>> newPoints, std::vector<cv::Scalar> myColorValues)
{

	for (int i = 0; i < newPoints.size(); i++)
	{
		circle(img, cv::Point(newPoints[i][0], newPoints[i][1]), 10, myColorValues[newPoints[i][2]], cv::FILLED);
	}
}


int main() {
	static int d = 10;
	calculation count;
	cv::VideoCapture cap(0);
	cap.set((640, 200), 320);
	cap.set((640, 200), 240);

	if (cap.isOpened() == false)
	{
		std::cout << "error: Webcam not accessed successfully\n\n";
		return(0);
	}

	while (true && cap.isOpened()) {

		cv::namedWindow("MORPH", (640, 200));
		cv::createTrackbar("morph_pixel", "MORPH", &morph_pixel, 20);

		cv::namedWindow("ColorField", (640, 200));
		cv::createTrackbar("Y1", "ColorField", &Y1, 255);
		cv::createTrackbar("Cr1", "ColorField", &Cr1, 255);
		cv::createTrackbar("Cb1", "ColorField", &Cb1, 255);
		cv::createTrackbar("Y2", "ColorField", &Y2, 255);
		cv::createTrackbar("Cr2", "ColorField", &Cr2, 255);
		cv::createTrackbar("Cb2", "ColorField", &Cb2, 255);

		cv::namedWindow("Circle", 640);
		cv::createTrackbar("acc_res", "Circle", &acc_res, 255);
		cv::createTrackbar("min_dis", "Circle", &min_dis, 255);
		cv::createTrackbar("high_thres", "Circle", &high_thres, 255);
		cv::createTrackbar("low_thres", "Circle", &low_thres, 255);
		cv::createTrackbar("min_rad", "Circle", &min_rad, 255);
		cv::createTrackbar("max_rad", "Circle", &max_rad, 1000);

		while (charCheckForEscKey != 27 && cap.isOpened()) {
			bool blnFrameReadSuccessfully = cap.read(matOriginal);

			if (!blnFrameReadSuccessfully || matOriginal.empty()) {
				std::cout << "error: frame not read from webcam\n";
				break;
			}
		cap.read(img);
		newPoints = findColor(img);
		drawOnCanvas(newPoints, ColorTrailValues);

		imshow("Image", img);
		cv::waitKey(1);
		cvtColor(matOriginal, matProcessed, 36);

		inRange(matProcessed, cv::Scalar(Y1, Cr1, Cb1), cv::Scalar(Y2, Cr2, Cb2), matProcessed);
		bitwise_not(matProcessed, matProcessed);

		erode(matProcessed, matProcessed, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(morph_pixel, morph_pixel)));

		dilate(matProcessed, matProcessed, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(morph_pixel, morph_pixel)));

		HoughCircles(matProcessed, v3fCircles, 3, acc_res, min_dis, high_thres, low_thres, min_rad, max_rad);
		
		for (int i = 0; i < v3fCircles.size(); i++) 
		{

			Diameter = (v3fCircles[i][2]) * 2;
			x_center = (int)v3fCircles[i][0]; //the x coordinate
			y_center = (int)v3fCircles[i][1]; //the y coordinate

			circle(matOriginal, cv::Point(x_center, y_center), 1, cv::Scalar(255, 0, 0), -1);
			circle(matOriginal, cv::Point(x_center, y_center), (int)v3fCircles[i][2], cv::Scalar(0, 0, 255), 2);
			// line(matOriginal, Point(160, 240), Point(x_center, y_center), Scalar(0, 255, 0), 2, CV_AA);

			if (y_center >= limit) {
				R = count.theRadian(x_center, y_center);
				d_near = count.near(a_near1, b_near1, R);
				d_far = count.far(a_far1, b_far1, R);
				theDistance = count.distance(Diameter, d_near, d_far, j_ref_near1, j_ref_far1);
			}
			else if (y_center < limit && y_center >= batas2) {
				R = count.theRadian(x_center, y_center);
				d_near = count.near(a_near2, b_near2, R);
				d_far = count.far(a_far2, b_far2, R);
				theDistance = count.distance(Diameter, d_near, d_far, j_ref_near2, j_ref_far2);
			}
			else if (y_center < batas2 && y_center >= batas3) {
				R = count.theRadian(x_center, y_center);
				d_near = count.near(a_near3, b_near3, R);
				d_far = count.far(a_far3, b_far3, R);
				theDistance = count.distance(Diameter, d_near, d_far, j_ref_near3, j_ref_far3);
			}
			else if (y_center < batas3 && y_center >= batas4) {
				R = count.theRadian(x_center, y_center);
				d_near = count.near(a_near4, b_near4, R);
				d_far = count.far(a_far4, b_far4, R);
				theDistance = count.distance(Diameter, d_near, d_far, j_ref_near4, j_ref_near4);
			}
			else {
				R = count.theRadian(x_center, y_center);
				d_near = count.near(a_near5, b_near5, R);
				d_far = count.far(a_far5, b_far5, R);
				theDistance = count.distance(Diameter, d_near, d_far, j_ref_near5, j_ref_far5);
			}

			if (x_center > 160) {
				posision = "right";
			}
			else {
				posision = "left";
			}

			Corner = count.corner(x_center, y_center);
			if (Corner < 0) {
				Corner = -Corner;
			}
			else {
				Corner = Corner;
			}

			std::stringstream j;
			j << std::fixed << std::setprecision(2) << theDistance;
			S_distance = j.str();
			putText(matOriginal, S_distance, cv::Point(x_center, (y_center + 10)), cv::FONT_HERSHEY_SIMPLEX, .7, cv::Scalar(255, 0, 0), 2, 8, false);
			putText(matOriginal, "cm", cv::Point((x_center + 80), (y_center + 10)), cv::FONT_HERSHEY_PLAIN, .7, cv::Scalar(255, 0, 0), 1, 8, false);

			std::stringstream x;
			x << x_center;
			std::string S_x_center = x.str();
			putText(matOriginal, "X ", cv::Point(185, 220), cv::FONT_HERSHEY_PLAIN, .7, cv::Scalar(0, 0, 0), 1, 4, false);
			putText(matOriginal, S_x_center, cv::Point(195, 220), cv::FONT_HERSHEY_PLAIN, .7, cv::Scalar(255, 255, 255), 1, 4, false);

			std::stringstream y;
			y << y_center;
			std::string S_y_center = y.str();
			putText(matOriginal, "Y ", cv::Point(185, 230), cv::FONT_HERSHEY_PLAIN, .7, cv::Scalar(0, 0, 0), 1, 4, false);
			putText(matOriginal, S_y_center, cv::Point(195, 230), cv::FONT_HERSHEY_PLAIN, .7, cv::Scalar(255, 255, 255), 1, 4, false);

			std::stringstream d;
			d << Diameter;
			std::string S_Diameter = d.str();
			putText(matOriginal, "Diameter ", cv::Point(230, 230), cv::FONT_HERSHEY_PLAIN, .7, cv::Scalar(0, 0, 0), 1, 4, false);
			putText(matOriginal, S_Diameter, cv::Point(270, 230), cv::FONT_HERSHEY_PLAIN, .7, cv::Scalar(255, 255, 255), 1, 4, false);

			putText(matOriginal, "Posision ", cv::Point(230, 220), cv::FONT_HERSHEY_PLAIN, .7, cv::Scalar(0, 0, 0), 1, 4, false);
			putText(matOriginal, posision, cv::Point(270, 220), cv::FONT_HERSHEY_PLAIN, .7, cv::Scalar(255, 255, 255), 1, 4, false);

			std::stringstream s;
			s << std::fixed << std::setprecision(2) << Corner;
			S_Corner = s.str();
			putText(matOriginal, "corner ", cv::Point(230, 210), cv::FONT_HERSHEY_PLAIN, .7, cv::Scalar(0, 0, 0), 1, 4, false);
			putText(matOriginal, S_Corner, cv::Point(270, 210), cv::FONT_HERSHEY_PLAIN, .7, cv::Scalar(255, 255, 255), 1, 4, false);


		}
		
		// line(matOriginal, Point(160, 240), Point(160, 0), Scalar(0, 0, 0), 1, CV_AA);
		imshow("tresh", matProcessed);
		imshow("OUTPUT", matOriginal);

		std::cout << "\t X= " << x_center
			<< "\t Y= " << y_center
			<< "\t DIAMETER= " << Diameter
			<< "\t DISTANCE= " << S_distance
			<< "\t  CORNER= " << S_Corner
			<< std::endl;

		
		charCheckForEscKey = cv::waitKey(1);
		
		int theDifference;
		

			// this is to keep track of the fishes over all distance travled on the Z axis
			int alter;
			for (;;)
			{
				//std::this_thread::sleep_for(std::chrono::milliseconds(100));
				if (theDistance > d)
				{
					theDifference = (theDistance - d);
						std::cout << theDifference;
						alter = theDifference;
					break;
				}
				else if (theDistance < d)
				{
					theDifference = (d - theDistance);
						std::cout << d;
						alter = d;
					break;
				}
				alter += alter;
		
				break;

			}
			std::cout << "distance traveled: " << alter;
		
		
	}
	//return(0);
	
	}
}

