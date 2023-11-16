#include "CameraThread.h"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
void CameraThread::run()
{
	//This will setup the camera feed, and then continously update the latest frame in the state


	cv::VideoCapture cap;
	
	//if (argc == 1) {

#ifdef USE_VIDEO
		cap.open("C:\\Users\\patma\\source\\repos\\JetBoatCV\\JetBoatCV\\WIN_20231116_10_24_41_Pro.mp4");
#else		
	cap.open(0);   // To open the first camera

#endif // USE_VIDEO
		std::cout << cap.get(VideoCaptureProperties::CAP_PROP_BRIGHTNESS) << std::endl;
		std::cout << cap.get(CAP_PROP_CONTRAST) << std::endl;
		std::cout << cap.get(cv::CAP_PROP_GAIN) << std::endl;
		std::cout << cap.get(cv::CAP_PROP_EXPOSURE) << std::endl;
		/*
		128
32
0
-6
		*/

		//cap.set(CAP_PROP_CONTRAST, 32);
		//cap.set(cv::CAP_PROP_GAIN, 0);
	//	cap.set(cv::CAP_PROP_EXPOSURE, -8);


		std::cout << cap.get(VideoCaptureProperties::CAP_PROP_BRIGHTNESS) << std::endl;
		std::cout << cap.get(CAP_PROP_CONTRAST) << std::endl;
		std::cout << cap.get(cv::CAP_PROP_GAIN) << std::endl;
		std::cout << cap.get(cv::CAP_PROP_EXPOSURE) << std::endl;
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
		cap.set(CAP_PROP_BRIGHTNESS, this->state->getCameraBrightness());

		cap >> frame;

		if (frame.empty())
			break;
		if (this->state->getStage() == AppStage::STOPPING)
			break;
		
		this->state->setLatestFrame(frame);
#ifdef USE_VIDEO
		//sleep 1/30 of a second
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
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