#include "State.h"

State::State()
{
}

State::~State()
{
}

void State::setStage(AppStage stage)
{
	this->stage.store(stage);
}

AppStage State::getStage()
{
	return this->stage.load();
}


std::vector<cv::Point2f> State::getPoints()
{
	return points;
}

void State::addPose(Pose pose)
{
	//need to research risk of inserting while reading. We don't care if we don't recieve the latest point, but need to be sure. 
	this->latestPose.store(pose);
	points.push_back(pose.position);
}

Pose State::getPose()
{
	return latestPose.load();
}

//void State::setBoatRect(cv::RotatedRect boatRect)
//{
//
//}
//
//cv::RotatedRect State::getBoatRect()
//{
//	return cv::RotatedRect();
//}

//cv::Mat State::getDisplayFrame()
//{
//	return cv::Mat();
//}

int State::getFrameCount()
{
	return frameCount.load();
}

void State::setLatestFrame(cv::Mat frame)
{
	frameMutex.lock();
	this->latestFrame = frame;
	frameCount++;
	frameMutex.unlock();
}

cv::Mat State::getLatestFrame()
{
	frameMutex.lock();
	cv::Mat latestFrameCopy = this->latestFrame.clone();
	frameMutex.unlock();
	return latestFrameCopy;
}
