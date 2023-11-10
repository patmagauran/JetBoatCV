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


AppendOnlyVector<cv::Point2f> State::getPoints()
{

	return points;
}

void State::setArucoData(std::vector<int> ids, std::vector<std::vector<cv::Point2f>> corners, float quality)
{
	const std::lock_guard<std::mutex> lock(arucoDataMutex);
	this->ids.clear();
	this->corners.clear();
	this->arucoQuality = quality;
	for (int i = 0; i < ids.size(); i++) {
		int id = ids[i];
		if (id == 0 || id == 1) {
			this->ids.push_back(id);
			this->corners.push_back(corners[i]);
		}
	}
}

void State::getArucoData(std::vector<int>& ids, std::vector<std::vector<cv::Point2f>>& corners, float& quality)
{
	const std::lock_guard<std::mutex> lock(arucoDataMutex);
	// Need to make copies of ids and corners
	ids.clear();
	corners.clear();
	for (int i = 0; i < this->ids.size(); i++) {
		int id = this->ids[i];
		if (id == 0 || id == 1) {
			ids.push_back(id);
			corners.push_back(this->corners[i]);
		}
	}
	quality = this->arucoQuality;
}

void State::setTrackingData(cv::Rect bboxBow, cv::Rect bboxStern, float quality)
{
	const std::lock_guard<std::mutex> lock(trackingDataMutex);

	this->bboxBow = bboxBow;
	this->bboxStern = bboxStern;
	this->trackingQuality = quality;
}

void State::getTrackingData(cv::Rect& bboxBow, cv::Rect& bboxStern, float& quality)
{
	const std::lock_guard<std::mutex> lock(trackingDataMutex);

	bboxBow = this->bboxBow;
	bboxStern = this->bboxStern;
	quality = this->trackingQuality;
}



void State::addPose(Pose pose)
{
	//need to research risk of inserting while reading. We don't care if we don't recieve the latest point, but need to be sure.
	constexpr auto tolerance = 0.1;
	//compare to latest pose and only insert if different enough
	if (abs(latestPose.load().position.x - pose.position.x) > tolerance || 
		abs(latestPose.load().position.y - pose.position.y) > tolerance || 
		abs(latestPose.load().rotation - pose.rotation) > tolerance) {

		this->latestPose.store(pose);
		points.push_back(pose.position);
	}
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
