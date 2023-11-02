// JetBoatCV.cpp : This file contains the 'main' function. Program execution begins and ends there.
//https://cppsecrets.com/users/152911510411798104971095548554848555648494864103109971051084699111109/C00-OpenCV-Taking-input-from-camera.php
//https://learnopencv.com/object-tracking-using-opencv-cpp-python/

//When compiling opencv, include intelOneAPI(TBB, OPenMP, MKL), and gstreamer
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/aruco.hpp>

using namespace std;

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()
#include <iostream>
using namespace cv;
int main(int argc, char** argv)
{
	try {
		cv::namedWindow("test", WINDOW_AUTOSIZE);

		cv::VideoCapture cap;
		cv::QRCodeDetector qrDecoder = cv::QRCodeDetector::QRCodeDetector();
		Ptr<Tracker> tracker = TrackerKCF::create();
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
		/*
		1. Work by detecting aruco markers. User has to start tracking
		2. Once tracking starts, run in parallel, the detecting and tracking
		3. Rely on detecting. if detecting fails, use tracking
		4. Reinitialize tracking at a certain interval
		5. If tracking fails, reinitialize tracking
		
		*/

		cv::Mat frame;
		std::vector<cv::String> data;
		std::vector<cv::Point> points;

		cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
		cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_MIP_36h12);
		cv::aruco::ArucoDetector detector(dictionary, detectorParams);

		while (1) {
			cap >> frame;

			if (frame.empty())

				break;

			cv::Mat imageCopy;
			frame.copyTo(imageCopy);
			std::vector<int> ids;
			std::vector<std::vector<cv::Point2f>> corners, rejected;
			detector.detectMarkers(frame, corners, ids, rejected);
			// if at least one marker detected
			if (ids.size() > 0)
				cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);
			cv::imshow("test", imageCopy);
		
/*
			cv::imshow("test", frame);
		//	std::vector<cv::String> data;
			std::vector<cv::Point> points2;
			//qrDecoder.detectAndDecodeMulti(frame, data, points);
			qrDecoder.detectMulti(frame, points2);

			for (auto& it : data)
			{
				std::cout << it << std::endl;
			}
			for (auto& it : points2)
			{
				std::cout << it << std::endl;
			}
			*/
			if (cv::waitKey(30) >= 0) {
				break;
			}


		}
		Rect bbox(points[0], points[2]);
		rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);

		imshow("test", frame);
		tracker->init(frame, bbox);
		while (1) {

			cap >> frame;

			if (frame.empty())

				break;

			// Start timer
			double timer = (double)getTickCount();

			// Update the tracking result
			bool ok = tracker->update(frame, bbox);

			// Calculate Frames per second (FPS)
			float fps = getTickFrequency() / ((double)getTickCount() - timer);

			if (ok)
			{
				// Tracking success : Draw the tracked object
				rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
			}
			else
			{
				// Tracking failure detected.
				putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
			}

			// Display tracker type on frame
			putText(frame,  " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

			// Display FPS on frame
			putText(frame, "FPS : " + to_string(fps), Point(100, 50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

			// Display frame.
			imshow("test", frame);

			// Exit if ESC pressed.
			int k = waitKey(1);
			if (k == 27)
			{
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
