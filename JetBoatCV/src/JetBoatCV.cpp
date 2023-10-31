// JetBoatCV.cpp : This file contains the 'main' function. Program execution begins and ends there.
//https://cppsecrets.com/users/152911510411798104971095548554848555648494864103109971051084699111109/C00-OpenCV-Taking-input-from-camera.php   

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
using namespace cv;
int main(int argc, char** argv)
{
	try {
		cv::namedWindow("test", WINDOW_AUTOSIZE);

		cv::VideoCapture cap;
		cv::QRCodeDetector qrDecoder = cv::QRCodeDetector::QRCodeDetector();
		if (argc == 1) {

			cap.open(0);   // To open the first camera

		}

		else
		{

			cap.open(argv[1]);

		}

		if (!cap.isOpened()) {

			std::cerr << "couldn't open capture." << std::endl;

			return -1;

		}

		cv::Mat frame;

		while (1) {

			cap >> frame;

			if (frame.empty())

				break;

			cv::imshow("test", frame);
			std::vector<cv::String> data;
			std::vector<cv::Point> points;
			qrDecoder.detectAndDecodeMulti(frame, data, points);
			for (auto& it : data)
			{
				std::cout << it << std::endl;
			}
			for (auto& it : points)
			{
				std::cout << it << std::endl;
			}
			if (cv::waitKey(30) >= 0) {
				break;
			}

		}

		return 0;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
