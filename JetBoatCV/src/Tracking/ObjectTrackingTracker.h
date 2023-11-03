#pragma once
#include "../State.h"
class MultiTracker;
class ObjectTrackingTracker
{

private:
	std::shared_ptr <MultiTracker> multiTracker;
	inline static std::thread ObjectTrackingTrackerThread = std::thread();
	std::atomic_bool reinitializeTracker = true;
	void run();

public:
	ObjectTrackingTracker(std::shared_ptr <MultiTracker> multiTracker);
	~ObjectTrackingTracker();
	void TriggerReinitialization();
};