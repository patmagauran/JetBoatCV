#include "ObjectTrackingTracker.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include "./MultiTracker.h"

#define TRACKER_CLASS TrackerKCF
using namespace cv;
bool isTrackerRectEmpty(Rect rect) {
	//std::cout << "Rect width: " << rect.width << std::endl;
	//std::cout << "Rect height: " << rect.height << std::endl;
	//The ArUco tracker returns a 1x1 rect if it has lost the marker, therefore it is not "empty"
	return rect.width <= 1 || rect.height <= 1;
}

bool bothArUcosTracked(Rect bow, Rect stern) {
	if (isTrackerRectEmpty(bow)) {
		return false;
	}
	if (isTrackerRectEmpty(stern)) {
		return false;
	}
	return true;
	//TrackerDaSiamRPN;
	//TrackerMIL;

	//return !isTrackerRectEmpty(bow) && !isTrackerRectEmpty(stern);
}

void ObjectTrackingTracker::run()
{

	//Two trackers, one for bow, one for stern
	TRACKER_CLASS::Params params;
	/*params.desc_pca = TRACKER_CLASS::GRAY;
	params.desc_npca = TRACKER_CLASS::GRAY;*/
	int frameNo = 0;
	Ptr<Tracker> trackerBow = TRACKER_CLASS::create(params);
	Ptr<Tracker> trackerStern = TRACKER_CLASS::create(params);
	float initializedAngle = 0;
	while (1) {
		auto start = std::chrono::high_resolution_clock::now();
		Rect bboxBow, bboxStern;

		if (multiTracker->getStage() != AppStage::RUNNING) {
			continue;
		}
		cv::Mat frameBow = multiTracker->getFrame().clone();
		cv::Mat frameStern = multiTracker->getFrame().clone();
		/*
		cv::Mat frame = multiTracker->getFrame();
		cv::Mat frameBow, frameStern;
		cv::cvtColor(frame, frameBow, cv::COLOR_BGR2GRAY);
		frameStern = frameBow.clone();
		*/
		if (reinitializeTracker.load()) {
		//	std::cout << "We need to reinit the tracker" << std::endl;
			multiTracker->getBowSternRect(bboxBow, bboxStern);
			if (bothArUcosTracked(bboxBow, bboxStern)) {
				trackerBow = TRACKER_CLASS::create();
				trackerStern = TRACKER_CLASS::create();
				trackerBow->init(frameBow, bboxBow);
				trackerStern->init(frameStern, bboxStern);
				reinitializeTracker.store(false);
				initializedAngle = multiTracker->getAngle();
				continue;
			}
		}
		//Rotate frame according to previous determined angle
		//cv::Mat rotationMatrix = getRotationMatrix2D(center, rotation, 1);
		//cv::Mat rotatedFrame;// = imageRotateWithoutCrop(adjFrame, state->getPose());
		//cv::warpAffine(adjFrame, rotatedFrame, rotationMatrix, adjFrame.size());
		float rotation = multiTracker->getAngle() - initializedAngle;
		cv::Point2f center = multiTracker->getCenter();
		cv::Mat rotationMatrix = getRotationMatrix2D(center, rotation, 1);
		/*cv::warpAffine(frameBow, frameBow, rotationMatrix, frameBow.size());
		frameStern = frameBow.clone();*/
		bboxBow = Rect();
		bboxStern = Rect();
		bool okBow = trackerBow->update(frameBow, bboxBow);
		bool okStern = trackerStern->update(frameStern, bboxStern);
		Pose pose;
		float quality = 1;
		if (okBow && okStern) {
			//std::cout << "We have tracked both bow and stern" << std::endl;
			Point2f bowCenter = (bboxBow.br() + bboxBow.tl()) * 0.5;
			Point2f sternCenter = (bboxStern.br() + bboxStern.tl()) * 0.5;
			Point2f center = (bowCenter + sternCenter) / 2;


			//Angle is angle of line between bow and stern
			float angle = atan2(bowCenter.y - sternCenter.y, bowCenter.x - sternCenter.x) * 180 / CV_PI;
			angle -= 90;
		//	angle += rotation + initializedAngle;
			//float quality = 1;
			//Get difference between (bow to stern distance) and code spacing
			float distance = norm(bowCenter - sternCenter);
			float difference = distance - multiTracker->getCodeSpacing();
			//Get difference between (bow to stern angle) and code angle
			float angleDifference = 0;

			float qualityPos = 1 - (abs(difference) / multiTracker->getCodeSpacing());
			float qualityAngle = 1 - (abs(angleDifference) / multiTracker->getCodeAngle());

			quality = (qualityPos * QUALITY_COEF_POSITION) + (qualityAngle * QUALITY_COEF_ROTATION);
			pose = Pose(center, angle);
		}
		else {
		//	std::cout << "We have not tracked both bow" << okBow << "and stern" << okStern << std::endl;

			quality = 0;
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = duration_cast<std::chrono::milliseconds>(end - start);

		multiTracker->setTrackingPose(pose, quality, bboxBow, bboxStern, (duration.count()));
	}



	//This will setup the Object Tracker(It will need to get the points from the multi Tracker somehow)
	//It Will then loop and update the multi tracker with the new points
	//There needs to be a way to easily reset the tracker(ideally without destroying the thread and this object) to use new points
}




ObjectTrackingTracker::ObjectTrackingTracker(std::shared_ptr<MultiTracker> multiTracker) : multiTracker(multiTracker)
{
	ObjectTrackingTrackerThread = std::thread(&ObjectTrackingTracker::run, this);

}

ObjectTrackingTracker::~ObjectTrackingTracker()
{
	ObjectTrackingTrackerThread.join();

}

void ObjectTrackingTracker::TriggerReinitialization()
{
	//On initialization we will not reset it until we have two rectangles from aruco
	reinitializeTracker.store(true);
	//std::cout << "We have triggered reinitialization" << std::endl;
}

