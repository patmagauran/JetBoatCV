#pragma once
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/aruco.hpp>
enum AppStage
{
	STARTING, ALIGNINGFRAME, RUNNING, STOPPING
};
class State
{

	AppStage stage;
	std::vector<cv::Point2f> points; // need to replace with concurrent structure
	cv::Mat latestFrame; // needs a mutex lock
	std::mutex frameMutex;

	//Other possible things to track -> number of aruco markers, number of points, 

public:
	State();
	~State();
	void setStage(AppStage stage);
	AppStage getStage();
	void setPoints(std::vector<cv::Point2f> points);
	std::vector<cv::Point2f> getPoints();
	void setLatestFrame(cv::Mat frame);
	cv::Mat getLatestFrame();

};

