#include "ArucoTracker.h"
#include "./MultiTracker.h"
using namespace cv;
inline static bool readCameraParameters(std::string filename, cv::Mat& camMatrix, cv::Mat& distCoeffs) {
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["cameraMatrix"] >> camMatrix;
	fs["dist_coeffs"] >> distCoeffs;
	return true;
}
//https://stackoverflow.com/questions/54970421/python-opencv-solvepnp-convert-to-euler-angles
cv::Vec3d getEulerFromRvec(cv::Vec3d rvec) {
	/*
	    R = cv2.Rodrigues(rvecs)[0]
    roll = 180*atan2(-R[2][1], R[2][2])/pi
    pitch = 180*asin(R[2][0])/pi
    yaw = 180*atan2(-R[1][0], R[0][0])/pi*/
    cv::Mat R;

	cv::Rodrigues(rvec, R);
	double roll, pitch, yaw;
	pitch = asin(R.at<double>(2, 0));
	roll = atan2(-R.at<double>(2, 1), R.at<double>(2, 2));
	yaw = atan2(-R.at<double>(1, 0), R.at<double>(0, 0));
	return cv::Vec3d(roll, pitch, yaw);
}

cv::Point2f getCenterFromCorners(std::vector<cv::Point2f> corners) {
	if (corners.size() != 4) {
		return cv::Point2d(0, 0);
	}
	cv::Point2f center = cv::Point2f(0, 0);
	for (int i = 0; i < corners.size(); i++) {
		center = center + corners[i];
	}
	center.x /= corners.size();
	center.y /= corners.size();
	return center;
}

cv::RotatedRect getRotatedRectFromCornersAndEuler(std::vector<cv::Point2f> corners, cv::Vec3d euler, Size2f size) {
	Point2f cenr = getCenterFromCorners(corners);
	return RotatedRect(cenr, size, euler[2] * 180 / CV_PI);
}

void ArucoTracker::run()
{
	cv::Mat cameraMatrix, distCoeffs;
	if (!readCameraParameters("C:\\Users\\patma\\source\\repos\\JetBoatCV\\JetBoatCV\\cameraParameters.xml", cameraMatrix, distCoeffs)) {
		std::cout << "Invalid camera file" << std::endl;
	}
	cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
	cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_MIP_36h12);
	cv::aruco::ArucoDetector detector(dictionary, detectorParams);
	// Set coordinate system
	cv::Mat objPoints(4, 1, CV_32FC3);
	float markerLength = 100;
	objPoints.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-markerLength / 2.f, markerLength / 2.f, 0);
	objPoints.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(markerLength / 2.f, markerLength / 2.f, 0);
	objPoints.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(markerLength / 2.f, -markerLength / 2.f, 0);
	objPoints.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-markerLength / 2.f, -markerLength / 2.f, 0);
	Size2f size = Size2f(markerLength, markerLength);
	//Need to convert the rotatedRect to a or a bounding box
	while (1) {
		cv::Mat frame = multiTracker->getFrame();
		if (frame.empty())
			continue;
		std::vector<int> ids;
		std::vector<std::vector<cv::Point2f>> corners;
		detector.detectMarkers(frame, corners, ids);
		std::vector<cv::Vec3d> rvecs(2), tvecs(2);
		cv::Vec3d rotationBow, rotationStern;

		cv::RotatedRect bowRect, sternRect;
		for (int i = 0; i < ids.size(); i++)
		{
			int id = ids[i];
			if (id == bow_code)
			{
				solvePnP(objPoints, corners.at(i), cameraMatrix, distCoeffs, rvecs.at(id), tvecs.at(id));
				rotationBow = getEulerFromRvec(rvecs.at(id));
				
				bowRect = getRotatedRectFromCornersAndEuler(corners.at(i), rotationBow, size);
			}
			else if (id == stern_code)
			{
				solvePnP(objPoints, corners.at(i), cameraMatrix, distCoeffs, rvecs.at(id), tvecs.at(id));
				rotationStern = getEulerFromRvec(rvecs.at(id));

				sternRect = getRotatedRectFromCornersAndEuler(corners.at(i), rotationStern, size);
				
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

			quality = (qualityPos * QUALITY_COEF_POSITION) + (qualityAngle * QUALITY_COEF_ROTATION);
		}
		multiTracker->setArucoData(Pose(center, angle), quality, bowRect, sternRect, ids, corners);


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
