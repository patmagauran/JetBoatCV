#include "ArucoTracker.h"
#include "./MultiTracker.h"
using namespace cv;
void ArucoTracker::run()
{
	cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
	cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_MIP_36h12);
	cv::aruco::ArucoDetector detector(dictionary, detectorParams);
	//Need to convert the rotatedRect to a or a bounding box
	while (1) {
		cv::Mat frame = multiTracker->getFrame();
		if (frame.empty())
			continue;
		std::vector<int> ids;
		std::vector<std::vector<cv::Point2f>> corners, rejected;
		detector.detectMarkers(frame, corners, ids, rejected);
		cv::RotatedRect bowRect, sternRect;
		for (int i = 0; i < ids.size(); i++)
		{
			if (ids[i] == bow_code)
			{
				bowRect = RotatedRect(corners[i][0], corners[i][1], corners[i][2]);
			}
			else if (ids[i] == stern_code)
			{
				sternRect = RotatedRect(corners[i][0], corners[i][1], corners[i][2]);
			}
		}

		//Average position of bow and stern
		Point2f bowCenter = bowRect.center;
		Point2f sternCenter = sternRect.center;
		Point2f center = (bowCenter + sternCenter) / 2;

		//Average rotation of bow and stern
		float bowAngle = bowRect.angle;
		float sternAngle = sternRect.angle;
		float angle = (bowAngle + sternAngle) / 2;
		float quality = 1;
		if (multiTracker->getStage() == RUNNING) {
			//Get difference between (bow to stern distance) and code spacing
			float distance = norm(bowCenter - sternCenter);
			float difference = distance - multiTracker->getCodeSpacing();
			//Get difference between (bow to stern angle) and code angle
			float angleDifference = (bowAngle - sternAngle) - multiTracker->getCodeAngle();

			float qualityPos = 1 - (abs(difference) / multiTracker->getCodeSpacing());
			float qualityAngle = 1 - (abs(angleDifference) / multiTracker->getCodeAngle());

			float quality = (qualityPos * QUALITY_COEF_POSITION) + (qualityAngle * QUALITY_COEF_ROTATION);
		}
		multiTracker->setArucoData(Pose(center, angle), quality, bowRect, sternRect);


	}

	//This will initialize the tracker and then continously detect and update the multi tracker
}

ArucoTracker::ArucoTracker(std::shared_ptr <MultiTracker> multiTracker, int bow_code, int stern_code) : multiTracker(multiTracker), bow_code(bow_code), stern_code(stern_code)
{
	ArucoTrackerThread = std::thread(&ArucoTracker::run, this);
}

ArucoTracker::~ArucoTracker()
{
	ArucoTrackerThread.join();
}
