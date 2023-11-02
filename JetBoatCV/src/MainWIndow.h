#pragma once
#include "State.h"
class MainWindow
{
	
private:
	std::shared_ptr <State> state;
	inline static std::thread MainWindowThread = std::thread();

	void run();

	public:
		MainWindow();
	~MainWindow();
};

