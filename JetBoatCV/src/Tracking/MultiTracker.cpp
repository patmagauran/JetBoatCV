#include "MultiTracker.h"

void MultiTracker::run()
{
	//If we are in the Aligning state it will only run the aruco. Once we move to running it will also run the object tracker.

	//Needs to communicate aruco corners to the object tracker


	//This will take in the latest frame and copy it to be processed by the trackers
	//It will monitor for extensive deviations from one another and implement a voting system to determine the most likely position
	//It will then update the state with the most likely position
	//If tracking is lost it will reset the tracker

}

MultiTracker::MultiTracker()
{
	MultiTrackerThread = std::thread(&MultiTracker::run, this);
}



MultiTracker::~MultiTracker()
{
	MultiTrackerThread.join();

}
