#pragma once
#include "../State.h"
#include <thread>
class MultiTracker;
class ArucoTracker
{

private:
	std::shared_ptr <MultiTracker> multiTracker;
	int bow_code, stern_code;
	inline static std::thread ArucoTrackerThread = std::thread();

	void run();

public:
	ArucoTracker(std::shared_ptr <MultiTracker> multiTracker, int bow_code, int stern_code);
	~ArucoTracker();

};