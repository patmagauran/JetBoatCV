#include "MainWindow.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/aruco.hpp>
#include "Tracking/Pose.h"
using namespace cv;

constexpr auto BOAT_WIDTH = 10;
constexpr auto BOAT_LENGTH = 20;
constexpr auto COURSE_FILENAME_1 = "C:\\Users\\patma\\source\\repos\\JetBoatCV\\JetBoatCV\\src\\points.csv";

static void on_trackbar_alpha(int alpha, void* state)
{
	((State*)state)->setContrast(alpha / 50.0);
	//state->setContrast(alpha / 100.0);
}
static void on_trackbar_beta(int beta, void* state)
{
	((State*)state)->setBrightness(beta - 50.0);
	//state->setContrast(alpha / 100.0);
}
static void onTrackbarCamBright(int beta, void* state)
{
	((State*)state)->setCameraBrightness(beta);
	//state->setContrast(alpha / 100.0);
}


/*
def image_rotate_without_crop(mat, angle):
	# https://stackoverflow.com/questions/22041699/rotate-an-image-without-cropping-in-opencv-in-c
	# angle in degrees

	height, width = mat.shape[:2]
	image_center = (width/2, height/2)

	rotation_mat = cv2.getRotationMatrix2D(image_center, angle, 1)

	abs_cos = abs(rotation_mat[0,0])
	abs_sin = abs(rotation_mat[0,1])

	bound_w = int(height * abs_sin + width * abs_cos)
	bound_h = int(height * abs_cos + width * abs_sin)

	rotation_mat[0, 2] += bound_w/2 - image_center[0]
	rotation_mat[1, 2] += bound_h/2 - image_center[1]

	rotated_mat = cv2.warpAffine(mat, rotation_mat, (bound_w, bound_h))

	return rotated_mat
	*/
static cv::Mat imageRotateWithoutCrop(cv::Mat input, Pose pose) {
	cv::Mat rotated;
	cv::Point2f center = pose.position;
	cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, pose.rotation, 1.0);
	int height = input.rows;
	int width = input.cols;
	double absCos = abs(rotationMatrix.at<double>(0, 0));
	double absSin = abs(rotationMatrix.at<double>(0, 1));

	int boundW = int(height * absSin + width * absCos);
	int boundH = int(height * absCos + width * absSin);

	rotationMatrix.at<double>(0, 2) += boundW / 2 - center.x;
	rotationMatrix.at<double>(1, 2) += boundH / 2 - center.y;

	cv::warpAffine(input, rotated, rotationMatrix, cv::Size(boundW, boundH));
	return rotated;
}

void MainWindow::run()
{
	//This will need to ingest the frame and points and display them on the window
	//Also should be the primary handler of User inputs
	//Needs to be able to stop other threads somehow
	std::shared_ptr<Course> course = std::make_shared<Course>(COURSE_FILENAME_1);
	state->setCourse(course);
	bool displayAuxWindow = true;
	bool auxWindowCreated = false;
	int quadrant = 1;
	while (1)
	{
		cv::Mat frame = state->getLatestFrame();
		if (frame.empty()) {
			continue;
		}
		cv::Mat displayFrame = frame.clone();
		AppendOnlyVector<Point2f> points = state->getPoints();

		Pose pose = state->getPose();

		RotatedRect boatRect = RotatedRect(pose.position, Size2f(BOAT_WIDTH, BOAT_LENGTH), pose.rotation);

		if (this->state->getStage() == ALIGNINGFRAME) {
			course->transformCourse(pose.position, pose.rotation, 1);
			//state->setCourse(course);
		//	if ()
			//Need to adjust course to match the boat
		}

		//state->setBoatRect(boatRect);

		//Draw Course as Points with lines connecting them
		std::vector<Point2d> coursePoints = course->getWaypoints();
		for (int i = 1; i < coursePoints.size(); i++)
		{
			//circle(displayFrame, coursePoints[i], 5, Scalar(255, 0, 0), -1);
			
			line(displayFrame, coursePoints[i-1], coursePoints[i], Scalar(255, 0, 0));
		}

		

		//draw rotated rectange in opencv
		cv::Point2f vertices[4];
		boatRect.points(vertices);
		for (int i = 0; i < 4; i++){
			line(displayFrame, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0));
		}

		//draw points
		for (int i = 0; i < points.size(); i++)
		{
			circle(displayFrame, points[i], 5, Scalar(0, 0, 255), -1);
		}

		//Draw Aruco markers
		std::vector<int> ids;
		std::vector<std::vector<cv::Point2f>> corners;
		float arucoQuality, trackingQuality;
		state->getArucoData(ids, corners, arucoQuality);
		cv::aruco::drawDetectedMarkers(displayFrame, corners, ids);
		

		//Draw tracking boxes
		cv::Rect bboxBow, bboxStern;
		state->getTrackingData(bboxBow, bboxStern, trackingQuality);
		rectangle(displayFrame, bboxBow, Scalar(255, 0, 0), 2, 1);
		rectangle(displayFrame, bboxStern, Scalar(255, 0, 0), 2, 1);

		//Write quality text
		std::string arucoQualityText = "Aruco Quality: " + std::to_string(arucoQuality);
		std::string trackingQualityText = "Tracking Quality: " + std::to_string(trackingQuality);
		putText(displayFrame, arucoQualityText, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));
		putText(displayFrame, trackingQualityText, Point(10, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));

		//Display current stage
		std::string stageText = "Stage: " + std::to_string(state->getStage());
		putText(displayFrame, stageText, Point(10, 60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));

		//display score
		std::string scoreText = "Score: " + std::to_string(state->getScore());
		putText(displayFrame, scoreText, Point(10, 80), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));




		//Display size of ArUco Codes
		double arucoSize = 0;
		//distance between corners 0 and 1
		if (corners.size() > 0) {
			if (corners[0].size() == 4) {
				float w = norm(corners[0][1] - corners[0][0]);
				float h = norm(corners[0][2] - corners[0][0]);
				std::string arucoSizeText = "Aruco Size: " + std::to_string(w) + "x" + std::to_string(h);
				putText(displayFrame, arucoSizeText, Point(10, 100), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));
			}
		}
		//display FPS
		std::string fpsText = "ms/frame: " + std::to_string(state->getTrackerFPS());
		putText(displayFrame, fpsText, Point(10, 120), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));

		cv::imshow("test", displayFrame);


		if (displayAuxWindow) {
			double alpha, beta;

			if (!auxWindowCreated) {
				int alpha_slider = 50, beta = 50, camBright = 64;

				namedWindow("Tracker View", cv::WindowFlags::WINDOW_AUTOSIZE); // Create Window

				createTrackbar("Contrast", "Tracker View", &alpha_slider, 100, on_trackbar_alpha, this->state.get());
				createTrackbar("Brightness", "Tracker View", &beta, 100, on_trackbar_beta, this->state.get());
				createTrackbar("Camera Brightness", "Tracker View", &camBright, 128, onTrackbarCamBright, this->state.get());
				auxWindowCreated = true;
			}

			cv::Mat adjFrame = frame.clone();
			//convert adjFrame to grayscale
			//cvtColor(adjFrame, adjFrame, COLOR_BGR2GRAY);
			alpha = this->state->getContrast();
			beta = this->state->getBrightness();
			adjFrame.convertTo(adjFrame, -1, alpha, beta);
			float rotation = 1.0* state->getPose().rotation;
			//if (rotation < 0) {
			//	rotation = 180 + rotation;
			//}
			
			//std::cout << "Rotation(initial): " << state->getPose().rotation << " adjusted to " << rotation << std::endl;



			Point2f center = state->getPose().position;
			//std::cout << "Rotation: " << rotation << std::endl;
			//display rotation in adjFrame
			std::string rotationText = "Rotation: " + std::to_string(rotation);
			cv::Mat rotationMatrix = getRotationMatrix2D(center, rotation, 1);
			cv::Mat rotatedFrame;// = imageRotateWithoutCrop(adjFrame, state->getPose());
			cv::warpAffine(adjFrame, rotatedFrame, rotationMatrix, adjFrame.size());
			//Center frame on center 
		/*	cv::Mat M = cv::Mat::eye(2, 3, CV_32F);
			M.at<float>(0, 2) = -1 * center.x;
			M.at<float>(1, 2) = -1 * center.y;
			cv::warpAffine(rotatedFrame, rotatedFrame, M, adjFrame.size());*/
			putText(rotatedFrame, rotationText, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));
			rectangle(rotatedFrame, bboxBow, Scalar(255, 0, 0), 2, 1);
			rectangle(rotatedFrame, bboxStern, Scalar(255, 0, 0), 2, 1);
			imshow("Tracker View", rotatedFrame);
			this->state->setTrackingFrame(adjFrame);
		}

		int k = cv::waitKey(1);
		if (k >= 0) {
			if (k == 27)
			{
				break;
			}
			if (state->getStage() == AppStage::ALIGNINGFRAME )
			{
				switch (k) {
				case 13: //enter
					state->setStage(AppStage::ALIGNCONFIRMEDBYUSER);

					break;
				case 108: //scale up(L)
					
					//state->setStage(AppStage::ALIGNCONFIRMEDBYUSER);
					course->transformCourse(NULL_PT, 0, 1.1);
					//state->setCourse(course);
					break;
				case 107: //scale down(K)
					//state->setStage(AppStage::ALIGNCONFIRMEDBYUSER);
					course->transformCourse(NULL_PT, 0, 0.9);
					//state->setCourse(course);
					break;
				}
			}
		}
		//cv::waitKey(1);
	}
}

MainWindow::MainWindow(std::shared_ptr <State> state) : state(state)
{
	MainWindowThread = std::thread(&MainWindow::run, this);
}



MainWindow::~MainWindow()
{
	MainWindowThread.join();

}