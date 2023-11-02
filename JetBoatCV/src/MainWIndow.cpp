#include "MainWindow.h"

void MainWindow::run()
{
	//This will need to ingest the frame and points and display them on the window
	//Also should be the primary handler of User inputs
	//Needs to be able to stop other threads somehow
}

MainWindow::MainWindow()
{
	MainWindowThread = std::thread(&MainWindow::run, this);
}



MainWindow::~MainWindow()
{
	MainWindowThread.join();

}