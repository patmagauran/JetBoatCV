#pragma once
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/aruco.hpp>
#include "tracking/Pose.h"
#include "Util/concurrent/AppendOnlyVector.h"
enum AppStage
{
	STARTING, ALIGNINGFRAME, ALIGNCONFIRMEDBYUSER, RUNNING, STOPPING
};
//Only MultiTracker can move from Alignconfirmed to running
class State
{

	std::atomic<AppStage> stage;


	std::atomic<Pose> latestPose;
	AppendOnlyVector<cv::Point2f> points; // need to replace with concurrent structure
	//cv::RotatedRect boatRect;
	cv::Mat latestFrame; // needs a mutex lock
	std::mutex frameMutex;
//	cv::Mat displayFrame;
	std::atomic_uint64_t frameCount = 0;

	//Other possible things to track -> number of aruco markers, number of points, 

public:
	State();
	~State();
	void setStage(AppStage stage);
	AppStage getStage();
	void setLatestFrame(cv::Mat frame);
	cv::Mat getLatestFrame();
	AppendOnlyVector<cv::Point2f> getPoints();

	void addPose(Pose pose);
	Pose getPose();
	//void setPoints(std::vector<cv::Point2f> points);
	//void setBoatRect(cv::RotatedRect boatRect);
	//cv::RotatedRect getBoatRect();

//	cv::Mat getDisplayFrame();

	int getFrameCount();
};

