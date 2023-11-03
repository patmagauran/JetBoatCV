#include "Pose.h"
Pose::Pose() : Pose(0, 0, 0) {

}
Pose::Pose(float x, float y, float rotation) : position(x, y), rotation(rotation)
{
}

Pose::Pose(cv::Point2f position, float rotation) : position(position), rotation(rotation)
{
}

