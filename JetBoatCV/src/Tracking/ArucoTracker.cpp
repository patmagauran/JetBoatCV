#include "ArucoTracker.h"

void ArucoTracker::run()
{
	//This will initialize the tracker and then continously detect and update the multi tracker
}

ArucoTracker::ArucoTracker()
{
	ArucoTrackerThread = std::thread(&ArucoTracker::run, this);
}

ArucoTracker::~ArucoTracker()
{
	ArucoTrackerThread.join();
}
