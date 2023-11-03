#pragma once
#include <opencv2/core/types.hpp>
class Pose
{
public:
	float rotation;
	cv::Point2f position;
	Pose();
	Pose(cv::Point2f position, float rotation);
	Pose(float x, float y, float rotation);
	~Pose() = default;
};

