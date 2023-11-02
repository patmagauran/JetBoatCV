#pragma once
#include "../State.h"
class ObjectTrackingTracker
{

private:
	std::shared_ptr <State> state;
	inline static std::thread ObjectTrackingTrackerThread = std::thread();

	void run();

public:
	ObjectTrackingTracker();
	~ObjectTrackingTracker();
};