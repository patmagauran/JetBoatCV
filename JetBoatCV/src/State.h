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
#include "Course.h"
#define USE_VIDEO

enum AppStage
{
	STARTING, ALIGNINGFRAME, ALIGNCONFIRMEDBYUSER, RUNNING, STOPPING
};
//Only MultiTracker can move from Alignconfirmed to running
class State
{

	std::atomic<AppStage> stage;
	std::shared_ptr<Course> course;

	std::atomic<Pose> latestPose;
	AppendOnlyVector<cv::Point2f> points; // need to replace with concurrent structure
	//cv::RotatedRect boatRect;
	cv::Mat latestFrame; // needs a mutex lock
	cv::Mat trackingFrame; // needs a mutex lock
	std::mutex frameMutex, trackingFrameMutex,arucoDataMutex, trackingDataMutex;
//	cv::Mat displayFrame;
	std::atomic_uint64_t frameCount = 0;
	cv::Rect bboxBow, bboxStern;
	std::vector<int> ids;
	std::vector<std::vector<cv::Point2f>> corners;
	std::atomic<float> arucoQuality, trackingQuality;
	std::atomic_llong score = 0;

	std::atomic<double> contrast = 1.0;
	std::atomic<double> brightness = 1.0;
	std::atomic<double> cameraBrightness = 64;

	std::atomic<double> trackerFPS = 0;
	//Other possible things to track -> number of aruco markers, number of points, 

public:
	State();
	~State();
	void setStage(AppStage stage);
	AppStage getStage();
	void setLatestFrame(cv::Mat frame);
	cv::Mat getLatestFrame();
	AppendOnlyVector<cv::Point2f> getPoints();

	void setArucoData(std::vector<int> ids, std::vector<std::vector<cv::Point2f>> corners, float quality);
	void getArucoData(std::vector<int>& ids, std::vector<std::vector<cv::Point2f>>& corners, float& quality);
	void setTrackingData(cv::Rect bboxBow, cv::Rect bboxStern, float quality);
	void getTrackingData(cv::Rect& bboxBow, cv::Rect& bboxStern, float& quality);
	void addPose(Pose pose, bool addPoint = true);
	Pose getPose();

	void setCourse(std::shared_ptr<Course> course);

	std::shared_ptr<Course> getCourse();

	void setScore(long score);
	long getScore();

	void setContrast(double contrast);
	double getContrast();

	void setBrightness(double brightness);
	double getBrightness();

	void setCameraBrightness(double brightness);
	double getCameraBrightness();

	void setTrackingFrame(cv::Mat frame);
	cv::Mat getTrackingFrame();

	void setTrackerFPS(double fps);
	double getTrackerFPS();

	//void setPoints(std::vector<cv::Point2f> points);
	//void setBoatRect(cv::RotatedRect boatRect);
	//cv::RotatedRect getBoatRect();

//	cv::Mat getDisplayFrame();

	int getFrameCount();
};

