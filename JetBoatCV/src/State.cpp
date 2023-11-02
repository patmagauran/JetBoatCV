#include "State.h"

State::State()
{
}

State::~State()
{
}

void State::setStage(AppStage stage)
{
}

AppStage State::getStage()
{
	return AppStage();
}

void State::setPoints(std::vector<cv::Point2f> points)
{
}

std::vector<cv::Point2f> State::getPoints()
{
	return std::vector<cv::Point2f>();
}

void State::setLatestFrame(cv::Mat frame)
{
}

cv::Mat State::getLatestFrame()
{
	return cv::Mat();
}
