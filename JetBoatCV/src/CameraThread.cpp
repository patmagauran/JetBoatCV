#include "CameraThread.h"

void CameraThread::run()
{
	//This will setup the camera feed, and then continously update the latest frame in the state
}

CameraThread::CameraThread()
{
	CameraThreadThread = std::thread(&CameraThread::run, this);
}



CameraThread::~CameraThread()
{
	CameraThreadThread.join();

}