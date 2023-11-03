#pragma once
#include "State.h"
class CameraThread
{

private:
	std::shared_ptr <State> state;
	inline static std::thread CameraThreadThread = std::thread();

	void run();

public:
	CameraThread(std::shared_ptr <State> state);
	~CameraThread();
};