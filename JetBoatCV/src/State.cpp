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



void State::addPose(Pose pose, bool addPoint)
{
	//need to research risk of inserting while reading. We don't care if we don't recieve the latest point, but need to be sure.
	constexpr auto tolerance = 0.1;
	constexpr auto jumpTolerancePx = 90;
	constexpr auto jumpToleranceAng = 1500;

	//Don't insert if too far away, likely indicates a bad estimation
	Pose latestPoseCopy = this->latestPose.load();
	//std::cout << "Diff x " << abs(latestPoseCopy.position.x - pose.position.x) << std::endl;
	//std::cout << "Diff y " << abs(latestPoseCopy.position.y - pose.position.y) << std::endl;
	//std::cout << "Diff ang " << abs(latestPoseCopy.rotation - pose.rotation) << std::endl;

	//std::cout << "latestPose(" << latestPoseCopy.position.x << ", " << latestPoseCopy.position.y << ", " << latestPoseCopy.rotation << ")" << std::endl;
	//std::cout << "pose(" << pose.position.x << ", " << pose.position.y << ", " << pose.rotation << ")" << std::endl;
	if (latestPoseCopy.position.x != 0 && latestPoseCopy.position.y != 0) {
		if (abs(latestPoseCopy.position.x - pose.position.x) > jumpTolerancePx ||
			abs(latestPoseCopy.position.y - pose.position.y) > jumpTolerancePx ||
			abs(latestPoseCopy.rotation - pose.rotation) > jumpToleranceAng) {
			//std::cout << "Jump detected, not inserting" << std::endl;
			return;
		}
	}

	//compare to latest pose and only insert if different enough
	if (abs(latestPoseCopy.position.x - pose.position.x) > tolerance || 
		abs(latestPoseCopy.position.y - pose.position.y) > tolerance || 
		abs(latestPoseCopy.rotation - pose.rotation) > tolerance) {

		this->latestPose.store(pose);
		if (addPoint) {
			points.push_back(pose.position);
		}
	}
}

Pose State::getPose()
{
	return latestPose.load();
}

void State::setCourse(std::shared_ptr<Course> course)
{
	this->course = course;
}

std::shared_ptr<Course> State::getCourse()
{
	return course;
}

void State::setScore(long score)
{
	this->score.store(score);
}

long State::getScore()
{
	return score.load();
}

void State::setContrast(double incontrast)
{
	this->contrast.store(incontrast);
}

double State::getContrast()
{
	return contrast.load();
}

void State::setBrightness(double ibrightness)
{
	this->brightness.store(ibrightness);
}

double State::getBrightness()
{
	return brightness.load();
}

void State::setCameraBrightness(double brightness)
{
	this->cameraBrightness.store(brightness);
}

double State::getCameraBrightness()
{
	return cameraBrightness.load();
}

void State::setTrackingFrame(cv::Mat frame)
{
	trackingFrameMutex.lock();
	this->trackingFrame = frame;
	trackingFrameMutex.unlock();
}

cv::Mat State::getTrackingFrame()
{
	trackingFrameMutex.lock();
	cv::Mat trackingFrameCopy = this->trackingFrame.clone();
	trackingFrameMutex.unlock();
	return trackingFrameCopy;
}

void State::setTrackerFPS(double fps)
{
	this->trackerFPS.store(fps);
}

double State::getTrackerFPS()
{
	return trackerFPS.load();
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
