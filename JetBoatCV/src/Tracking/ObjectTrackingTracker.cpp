#include "ObjectTrackingTracker.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include "./MultiTracker.h"
using namespace cv;
void ObjectTrackingTracker::run()
{

	//Two trackers, one for bow, one for stern


	Ptr<Tracker> trackerBow = TrackerKCF::create();
	Ptr<Tracker> trackerStern = TrackerKCF::create();
	Rect bboxBow, bboxStern;
	while (1) {
		if (multiTracker->getStage() != AppStage::RUNNING) {
			break;
		}
		cv::Mat frame = multiTracker->getFrame();
		if (reinitializeTracker.load()) {
			multiTracker->getBowSternRect(bboxBow, bboxStern);
			if (!(bboxBow.size().empty() || bboxStern.size().empty())) {
				trackerBow->init(frame, bboxBow);
				trackerStern->init(frame, bboxStern);
				reinitializeTracker.store(false);
			}
		}
		bool okBow = trackerBow->update(frame, bboxBow);
		bool okStern = trackerStern->update(frame, bboxStern);
		Pose pose;
		float quality;
		if (okBow && okStern) {

			Point2f bowCenter = (bboxBow.br() + bboxBow.tl()) * 0.5;
			Point2f sternCenter = (bboxStern.br() + bboxStern.tl()) * 0.5;
			Point2f center = (bowCenter + sternCenter) / 2;


			//Angle is angle of line between bow and stern
			float angle = atan2(bowCenter.y - sternCenter.y, bowCenter.x - sternCenter.x) * 180 / CV_PI;
			float quality = 1;
			//Get difference between (bow to stern distance) and code spacing
			float distance = norm(bowCenter - sternCenter);
			float difference = distance - multiTracker->getCodeSpacing();
			//Get difference between (bow to stern angle) and code angle
			float angleDifference = angle - multiTracker->getCodeAngle();

			float qualityPos = 1 - (abs(difference) / multiTracker->getCodeSpacing());
			float qualityAngle = 1 - (abs(angleDifference) / multiTracker->getCodeAngle());

			quality = (qualityPos * QUALITY_COEF_POSITION) + (qualityAngle * QUALITY_COEF_ROTATION);
			pose = Pose(center, angle);
		}
		else {
			quality = 0;
		}
		
		multiTracker->setTrackingPose(pose, quality);
		
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
}

