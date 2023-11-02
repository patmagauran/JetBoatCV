#pragma once
#include "../State.h"
#include <thread>
class ArucoTracker
{

private:
	std::shared_ptr <State> state;
	 
	inline static std::thread ArucoTrackerThread = std::thread();

	void run();

public:
	ArucoTracker();
	~ArucoTracker();
};