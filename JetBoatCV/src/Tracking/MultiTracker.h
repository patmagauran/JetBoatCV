#pragma once
#include "../State.h"
class MultiTracker
{

private:
	std::shared_ptr <State> state;
	inline static std::thread MultiTrackerThread = std::thread();

	void run();

public:
	MultiTracker();
	~MultiTracker();
};