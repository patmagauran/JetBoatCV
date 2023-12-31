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
#include "Tracking/MultiTracker.h"
#include "Tracking/Pose.h"
#include "./State.h"
#include "./CameraThread.h"
#include "./MainWindow.h"
using namespace std;

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()
#include <iostream>


using namespace cv;

/*
Main thread:
- Start other threads
*/




int main(int argc, char** argv)
{
	try {
		//cv::aruco::Dictionary dictionary = cv::aruco::extendDictionary(2, 3);
		//cv::Mat markerImage;
		//cv::aruco::generateImageMarker(dictionary, 0, 200, markerImage, 1);
		//cv::imwrite("marker0.png", markerImage);
		//cv::aruco::generateImageMarker(dictionary, 1, 200, markerImage, 1);
		//cv::imwrite("marker1.png", markerImage);

		////Write dictionary to file

		//cv::FileStorage fs("dictionary.yml", cv::FileStorage::WRITE);
		//dictionary.writeDictionary(fs, "aruco_3x3_2");
		//fs.release();

		//Import Course to follow
		//Need a way to align the course with the boat in the "Aligning" Stage
		//Need a way to scale (and rotate?) the course so it fits in the pool (This should be user controlled with hardcoded defaults once the camera is mounted)

		//Then compute the distance to the course to get the current score

		std::shared_ptr<State> state = std::make_shared<State>();
	/*	Course course("");
		state->setCourse(course);*/
		std::shared_ptr<MultiTracker> tracker = std::make_shared<MultiTracker>(state);
		std::shared_ptr<CameraThread> cameraThread = std::make_shared<CameraThread>(state);
		std::shared_ptr<MainWindow> mainWindow = std::make_shared<MainWindow>(state);

		//Generate ArUco Code to files
		////https://docs.opencv.org/3.4/d5/dae/tutorial_aruco_detection.html


		long lastFrameScored = -1;
		while (state->getStage() != STOPPING) {
			if (state->getStage() == RUNNING) {
				//Compute distance between boat and course
				//Compute score
				//Update Score in State
				if (lastFrameScored >= state->getFrameCount()) {
					//Already scored this frame
					continue;
				}
				Pose pose = state->getPose();
				double distance = state->getCourse()->distanceFromTrajectory(pose.position);
				
				//Should make this more sophisticated and take into account timing
				long score = state->getScore();
				score = score + distance + 1; //Add one every frame to promote quick completion

				state->setScore(score);
				lastFrameScored = state->getFrameCount();

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


//		cv::namedWindow("test", WINDOW_AUTOSIZE);
//
//		cv::VideoCapture cap;
//		cv::QRCodeDetector qrDecoder = cv::QRCodeDetector::QRCodeDetector();
//		Ptr<Tracker> tracker = TrackerKCF::create();
//		if (argc == 1) {
//
//			cap.open(0);   // To open the first camera
//
//		}
//
//		else
//		{
//
//			cap.open(argv[1]);
//
//		}
//
//		if (!cap.isOpened()) {
//
//			std::cerr << "couldn't open capture." << std::endl;
//
//			return -1;
//
//		}
//		/*
//		1. Work by detecting aruco markers. User has to start tracking
//		2. Once tracking starts, run in parallel, the detecting and tracking
//		3. Rely on detecting. if detecting fails, use tracking
//		4. Reinitialize tracking at a certain interval
//		5. If tracking fails, reinitialize tracking
//		
//		*/
//
//		cv::Mat frame;
//		std::vector<cv::String> data;
//		std::vector<cv::Point2f> points;
//
//		cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
//		cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_MIP_36h12);
//		cv::aruco::ArucoDetector detector(dictionary, detectorParams);
//
//		while (1) {
//			cap >> frame;
//
//			if (frame.empty())
//
//				break;
//
//			cv::Mat imageCopy;
//			frame.copyTo(imageCopy);
//			std::vector<int> ids;
//			std::vector<std::vector<cv::Point2f>> corners, rejected;
//			detector.detectMarkers(frame, corners, ids, rejected);
//			// if at least one marker detected
//			if (ids.size() > 0) {
//				cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);
//				points = corners[0];
//			}
//			cv::imshow("test", imageCopy);
//		
///*
//			cv::imshow("test", frame);
//		//	std::vector<cv::String> data;
//			std::vector<cv::Point> points2;
//			//qrDecoder.detectAndDecodeMulti(frame, data, points);
//			qrDecoder.detectMulti(frame, points2);
//
//			for (auto& it : data)
//			{
//				std::cout << it << std::endl;
//			}
//			for (auto& it : points2)
//			{
//				std::cout << it << std::endl;
//			}
//			*/
//			if (cv::waitKey(30) >= 0) {
//				break;
//			}
//
//
//		}
//		Rect bbox(points[0], points[2]);
//		rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
//
//		imshow("test", frame);
//		tracker->init(frame, bbox);
//		while (1) {
//
//			cap >> frame;
//
//			if (frame.empty())
//
//				break;
//
//			// Start timer
//			double timer = (double)getTickCount();
//
//			// Update the tracking result
//			bool ok = tracker->update(frame, bbox);
//
//			// Calculate Frames per second (FPS)
//			float fps = getTickFrequency() / ((double)getTickCount() - timer);
//
//			if (ok)
//			{
//				// Tracking success : Draw the tracked object
//				rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
//			}
//			else
//			{
//				// Tracking failure detected.
//				putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
//			}
//
//			// Display tracker type on frame
//			putText(frame,  " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);
//
//			// Display FPS on frame
//			putText(frame, "FPS : " + to_string(fps), Point(100, 50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);
//
//			// Display frame.
//			imshow("test", frame);
//
//			// Exit if ESC pressed.
//			int k = waitKey(1);
//			if (k == 27)
//			{
//				break;
//			}
//
//		}