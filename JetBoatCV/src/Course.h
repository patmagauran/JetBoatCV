#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <opencv2/core.hpp>
#define NULL_PT cv::Point2d(-1, -1) //we shouldn't have negative coords

class Course
{
	std::vector <cv::Point2d> waypoints;
	int waypointIndex = 0;
	double effective_scale = 1;
	double currentRotation = 0;
public:
	Course(std::string fileName);
	std::vector <cv::Point2d> getWaypoints();
	cv::Point2d getLookaheadPoint(cv::Point2d currentPosition, double lookahead);
	double distanceFromTrajectory(cv::Point2d currentPosition);
	cv::Point2d getClosestPointOnSegment(cv::Point2d currentPosition, cv::Point2d segmentStart, cv::Point2d segmentEnd);

	void transformCourse(cv::Point2d origin, double angle = 0, double scale = 1);
};

