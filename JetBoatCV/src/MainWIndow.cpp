#include "MainWindow.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/aruco.hpp>
#include "Tracking/Pose.h"
using namespace cv;

constexpr auto BOAT_WIDTH = 10;
constexpr auto BOAT_LENGTH = 20;
void MainWindow::run()
{
	//This will need to ingest the frame and points and display them on the window
	//Also should be the primary handler of User inputs
	//Needs to be able to stop other threads somehow

	while (1)
	{
		cv::Mat frame = state->getLatestFrame();
		if (frame.empty()) {
			continue;
		}
		cv::Mat displayFrame = frame.clone();
		AppendOnlyVector<Point2f> points = state->getPoints();

		Pose pose = state->getPose();

		RotatedRect boatRect = RotatedRect(pose.position, Size2f(BOAT_WIDTH, BOAT_LENGTH), pose.rotation);
		//state->setBoatRect(boatRect);




		//draw rotated rectange in opencv
		cv::Point2f vertices[4];
		boatRect.points(vertices);
		for (int i = 0; i < 4; i++){
			line(displayFrame, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0));
		}

		//draw points
		for (int i = 0; i < points.size(); i++)
		{
			circle(displayFrame, points[i], 5, Scalar(0, 0, 255), -1);
		}


		cv::imshow("test", displayFrame);

		int k = cv::waitKey(1);
		if (k == 27)
		{
			break;
		}
		if (state->getStage() == AppStage::ALIGNINGFRAME && k == 13)
		{
			state->setStage(AppStage::ALIGNCONFIRMEDBYUSER);
		}
		//cv::waitKey(1);
	}
}

MainWindow::MainWindow(std::shared_ptr <State> state) : state(state)
{
	MainWindowThread = std::thread(&MainWindow::run, this);
}



MainWindow::~MainWindow()
{
	MainWindowThread.join();

}