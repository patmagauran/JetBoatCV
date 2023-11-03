#include "CameraThread.h"

void CameraThread::run()
{
	//This will setup the camera feed, and then continously update the latest frame in the state

	cv::VideoCapture cap;
	
	//if (argc == 1) {

		cap.open(0);   // To open the first camera

	//}

	//else
	//{

	//	cap.open(argv[1]);

	//}

	if (!cap.isOpened()) {

		std::cerr << "couldn't open capture." << std::endl;

		throw std::exception("Couldn't Open Capture");

	}
	cv::Mat frame;
	state->setStage(AppStage::ALIGNINGFRAME);
	while (1) {
		cap >> frame;

		if (frame.empty())
			break;
		if (this->state->getStage() == AppStage::STOPPING)
			break;
		
		this->state->setLatestFrame(frame);
	}

}

CameraThread::CameraThread(std::shared_ptr <State> state) : state(state)
{
	CameraThreadThread = std::thread(&CameraThread::run, this);
}



CameraThread::~CameraThread()
{
	CameraThreadThread.join();

}