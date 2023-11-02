#include "ObjectTrackingTracker.h"

void ObjectTrackingTracker::run()
{
	//This will setup the Object Tracker(It will need to get the points from the multi Tracker somehow)
	//It Will then loop and update the multi tracker with the new points
	//There needs to be a way to easily reset the tracker(ideally without destroying the thread and this object) to use new points
}


ObjectTrackingTracker::ObjectTrackingTracker()
{
	ObjectTrackingTrackerThread = std::thread(&ObjectTrackingTracker::run, this);
}



ObjectTrackingTracker::~ObjectTrackingTracker()
{
	ObjectTrackingTrackerThread.join();

}