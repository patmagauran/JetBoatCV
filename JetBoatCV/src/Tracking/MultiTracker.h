#pragma once
#include "../State.h"
#include "./Pose.h"
#include <opencv2/core/types.hpp>



class ObjectTrackingTracker;
class ArucoTracker;
//class Pose;

constexpr auto BOW_CODE = 0;
constexpr auto STERN_CODE = 1;
constexpr auto POSITION_DIFF_CUTOFF = 1;
constexpr auto ROTATION_DIFF_CUTOFF = 1;
constexpr auto QUALITY_COEF_POSITION = 0.5;
constexpr auto QUALITY_COEF_ROTATION = 0.5;
class MultiTracker
{

private:

	//Mutexes
	std::mutex arucoMutex;
	std::mutex trackingMutex;

	//Data
	std::shared_ptr <State> state;
	std::shared_ptr<ObjectTrackingTracker> objectTracker;
	std::shared_ptr<ArucoTracker> arucoTracker;
	inline static std::thread MultiTrackerThread = std::thread();
	Pose arucoPose, trackingPose, finalPose;
	double arucoQuality, trackingQuality;
	cv::RotatedRect bowCodeRect, sternCodeRect;
	long lastObjectInitilizationFrame = 0;

	//cv::Rect bboxBow, bboxStern;
	//std::vector<int> ids;
	//std::vector<std::vector<cv::Point2f>> corners;

	double codeSpacing, codeAngle;
	void run();
	void initializeObjectTracker(bool immediate = false);
public:
	MultiTracker(std::shared_ptr <State> state);
	~MultiTracker();
	cv::Mat getFrame();
	void setArucoData(Pose pose, double quality, cv::RotatedRect bowRect, cv::RotatedRect sternRect, std::vector<int> ids, std::vector<std::vector<cv::Point2f>> corners);
	void setTrackingPose(Pose pose, double quality, cv::Rect bboxBow, cv::Rect bboxStern);
	//void setBowCodeRect(cv::RotatedRect rect);
	//void setSternCodeRect(cv::RotatedRect rect);
	void getBowSternRect(cv::Rect& bowRect, cv::Rect& sternRect);
	double getCodeSpacing();
	double getCodeAngle();
	AppStage getStage();
};