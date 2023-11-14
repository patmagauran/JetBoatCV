#include "MultiTracker.h"
#include "../State.h"
#include "./ObjectTrackingTracker.h"
#include "./ArucoTracker.h"
#include <opencv2/core/types.hpp>
#include "./Pose.h"
using namespace cv;
void MultiTracker::initializeObjectTracker(bool immediate) {
	if (immediate || this->state->getFrameCount() - lastObjectInitilizationFrame > 30) {
		objectTracker->TriggerReinitialization();
		lastObjectInitilizationFrame = this->state->getFrameCount();
	}
}



void MultiTracker::run()
{
	std::shared_ptr<MultiTracker> multiTracker = std::shared_ptr<MultiTracker>(this);
	this->arucoTracker = std::make_shared<ArucoTracker>(multiTracker, BOW_CODE, STERN_CODE);
	this->objectTracker = std::make_shared<ObjectTrackingTracker>(multiTracker);
	bool aruco = true;
	initializeObjectTracker(true);
	while (1) {
		if (state->getStage() == STOPPING) {
			break;
		}
		if (state->getFrameCount() % 30 == 0) {
			//initializeObjectTracker();
		}

		if (state->getStage() == ALIGNINGFRAME) {
			arucoMutex.lock();
			Pose arucoPoseCopy = arucoPose;
			arucoMutex.unlock();
			state->addPose(arucoPoseCopy, false);

		}

		else if (state->getStage() == RUNNING) {
			arucoMutex.lock();
			Pose arucoPoseCopy = arucoPose;
			double arucoQualityCopy = arucoQuality;
			arucoMutex.unlock();

			trackingMutex.lock();
			Pose trackingPoseCopy = trackingPose;
			double trackingQualityCopy = trackingQuality;
			trackingMutex.unlock();

			//Should also keep track of latest tracking quality and aruco quality to write on screen
			//Also keep data necessary to display marker boxes on screen

			float posDiff = abs(cv::norm(arucoPoseCopy.position - trackingPoseCopy.position));

			float rotDiff = abs(arucoPoseCopy.rotation - trackingPoseCopy.rotation);

			if (posDiff > POSITION_DIFF_CUTOFF || rotDiff > ROTATION_DIFF_CUTOFF) {
				initializeObjectTracker();
			}
			if (arucoQualityCopy > trackingQualityCopy) {
				finalPose = arucoPoseCopy;
				if (!aruco) {
					std::cout << "Switching to ArUco" << std::endl;
					aruco = true;
				}
			}
			else {
				if (aruco) {
					std::cout << "Switching to Object Tracker" << std::endl;
					aruco = false;
				}
				finalPose = trackingPoseCopy;
			}
			state->addPose(finalPose);
		}
		else if (state->getStage() == ALIGNCONFIRMEDBYUSER) {
			//check that we have two codes
			arucoMutex.lock();
			if (bowCodeRect.size.empty() || sternCodeRect.size.empty()) {
				std::cout << "Both ArUco codes not detected, alignment not done!" << std::endl;
				state->setStage(AppStage::ALIGNINGFRAME);
			}
			else {
				Pose arucoPoseCopy = arucoPose;
				//Calculate the code distance and angle
				Point2f bowCenter = bowCodeRect.center;
				Point2f sternCenter = sternCodeRect.center;
				float bowAngle = bowCodeRect.angle;
				float sternAngle = sternCodeRect.angle;
				float distance = norm(bowCenter - sternCenter);
				codeSpacing = abs(distance);
				codeAngle = abs(bowAngle - sternAngle);

				state->setStage(AppStage::RUNNING);
			}
			arucoMutex.unlock();
		}

		//Do comparison of the two poses and do the voting

		//Every 30 frames we will reinitialize the object tracker
	}

	//We shouldn't set the frame. They should just get it


	//If we are in the Aligning state it will only run the aruco. Once we move to running it will also run the object tracker.

	//Needs to communicate aruco corners to the object tracker




	//This will take in the latest frame and copy it to be processed by the trackers
	//It will monitor for extensive deviations from one another and implement a voting system to determine the most likely position
	//It will then update the state with the most likely position
	//If tracking is lost it will reset the tracker

}

MultiTracker::MultiTracker(std::shared_ptr <State> state) : state(state)
{
	MultiTrackerThread = std::thread(&MultiTracker::run, this);
}



MultiTracker::~MultiTracker()
{
	MultiTrackerThread.join();

}

cv::Mat MultiTracker::getFrame()
{
	return state->getTrackingFrame();
}

void MultiTracker::setArucoData(Pose pose, double quality, cv::RotatedRect bowRect, cv::RotatedRect sternRect, std::vector<int> ids, std::vector<std::vector<cv::Point2f>> corners)
{
	arucoMutex.lock();
	arucoPose = pose;
	arucoQuality = quality;
	bowCodeRect = bowRect;
	sternCodeRect = sternRect;
	this->state->setArucoData(ids, corners, quality);
	arucoMutex.unlock();

}

void MultiTracker::setTrackingPose(Pose pose, double quality, cv::Rect bboxBow, cv::Rect bboxStern)
{
	trackingMutex.lock();
	trackingPose = pose;
	trackingQuality = quality;
	this->state->setTrackingData(bboxBow, bboxStern, quality);
	trackingMutex.unlock();
}



void MultiTracker::getBowSternRect(cv::Rect& bowRect, cv::Rect& sternRect)
{
	arucoMutex.lock();
	Rect bowRectCopy = bowCodeRect.boundingRect();
	Rect sternRectCopy = sternCodeRect.boundingRect();
	bowRect = bowRectCopy;
	sternRect = sternRectCopy;
	arucoMutex.unlock();
}

double MultiTracker::getCodeSpacing()
{
	return codeSpacing;
}

double MultiTracker::getCodeAngle()
{
	return codeAngle;
}

AppStage MultiTracker::getStage()
{
	return state->getStage();
}
