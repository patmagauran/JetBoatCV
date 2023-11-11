#include "Course.h"
using namespace std;
Course::Course(string fileName)
{
	//Read in list of waypoints from csv file

	//Open file
	std::ifstream file(fileName);
	vector<string> row;

	if (file.is_open())
	{
		//Read in lines
		std::string line, word;
		while (getline(file, line))
		{
			row.clear();

			stringstream str(line);

			while (getline(str, word, ','))
				row.push_back(word);

			//Convert tokens to doubles
			double x = 0;
			double y = 0;
		//	double z = 0;
			try
			{
				x = std::stod(row[0]);
				y = std::stod(row[1]);
				//z = std::stod(row[2]);
			}
			catch (std::exception e)
			{
				continue;
			}
			//Add waypoint
			this->waypoints.push_back(cv::Point2d(x, y));
		}
		file.close();
	}
	else
	{
		std::cout << "Unable to open file" << std::endl;
		throw std::exception("Unable to open file");
	}
}


std::vector<cv::Point2d> Course::getWaypoints()
{
	return this->waypoints;
}
/*
Returns the next point along the segment from currentWaypoint to nextWaypoint that is a distance lookahead away from currentPos
*/
cv::Point2d getPathCross(cv::Point2d currentWaypoint, cv::Point2d nextWaypoint, cv::Point2d currentPosition, double lookahead)
{

	double x0 = currentWaypoint.x;
	double	y0 = currentWaypoint.y;
//	double	z0 = currentWaypoint[2];
	double	x1 = nextWaypoint.x;
	double	y1 = nextWaypoint.y;
//	double	z1 = nextWaypoint[2];
	double	x = currentPosition.x;
	double	y = currentPosition.y;
//	double	z = currentPosition[2];

	double	A = pow((x0 - x), 2) + pow((y0 - y), 2) - pow(lookahead, 2);
	double	C = pow((x0 - x1), 2) + pow((y0 - y1), 2);
	double	B = pow((x1 - x), 2) + pow((y1 - y), 2) - A - C - pow(lookahead, 2);

	double determinent = pow(B, 2) - 4 * A * C;
	if (determinent < 0)
	{
		//No intersection
		return NULL_PT;
	}
	double t1 = (-B + sqrt(determinent)) / (2 * C);
	double t2 = (-B - sqrt(determinent)) / (2 * C);
	if (t1 < 0 && t2 < 0)
	{
		//No intersection
		return NULL_PT;
	}
	else if (t1 > 1 && t2 > 1)
	{
		return NULL_PT;
	}
	else if (t1 > 1 && t2 < 0)
	{
		return NULL_PT;
	}
	else if (t1 < 0 && t2 > 1)
	{
		return NULL_PT;
	}

	//There should always be two solutions, but we will pick the one closest to 1
	double t = max(t1, t2);
	//plug t into the parametric equation of the line to get the point of intersection
	double xCross = x0 * (1 - t) + t * (x1);
	double yCross = y0 + t * (y1 - y0);
//	double zCross = z0 + t * (z1 - z0);
	return cv::Point2d(xCross, yCross);

}



cv::Point2d Course::getLookaheadPoint(cv::Point2d currentPosition, double lookahead) {
//	intersectionPoint = None
//		while (intersectionPoint is None) :
//			if (self.waypointIndex + 2 >= len(self.waypoints)) :
//				#We have reached the end of the path
//				return self.waypoints[-1]
//				#Not the most efficient but looksahead to next segment first.If not there, looks at current.Then increments waypoint index to look two ahead
//				intersectionPoint = self._getPathCross(self.waypoints[self.waypointIndex + 1], self.waypoints[self.waypointIndex + 2], currentPoint)
//				if (intersectionPoint is None) :
//
//					intersectionPoint = self._getPathCross(self.waypoints[self.waypointIndex], self.waypoints[self.waypointIndex + 1], currentPoint)
//				else:
//	self.waypointIndex += 1
//# if (self.waypointIndex > len(self.waypoints)-3):
//#     #We have reached the end of the path
//		#     return self.waypoints[-1]
//
//		return intersectionPoint

	cv::Point2d intersectionPoint = NULL_PT;
	while (intersectionPoint == NULL_PT)
	{
		if (this->waypointIndex + 2 >= this->waypoints.size())
		{
			//We have reached the end of the path
			return this->waypoints.back();
		}
		//Not the most efficient but looksahead to next segment first.If not there, looks at current.Then increments waypoint index to look two ahead
		intersectionPoint = getPathCross(this->waypoints[this->waypointIndex + 1], this->waypoints[this->waypointIndex + 2], currentPosition, lookahead);
		if (intersectionPoint == NULL_PT)
		{
			intersectionPoint = getPathCross(this->waypoints[this->waypointIndex], this->waypoints[this->waypointIndex + 1], currentPosition, lookahead);
			if (intersectionPoint == NULL_PT) {
				//Its not in current or next, we must have deviated too far from the path
				return NULL_PT;
			}
		}
		else
		{
			this->waypointIndex += 1;
		}
	}
return intersectionPoint;
}

double Course::distanceFromTrajectory(cv::Point2d currentPosition)
{
	double closestDistance = 1000000;
	for (int i = 0; i < this->waypoints.size() - 2; i++) {
		cv::Point2d closestPoint = getClosestPointOnSegment(currentPosition, waypoints.at(i), waypoints.at(i+1));
		double distanceToClosestPoint = cv::norm(closestPoint - currentPosition);
		if (distanceToClosestPoint < closestDistance) {
			closestDistance = distanceToClosestPoint;
		}
	}
	return closestDistance;
}

cv::Point2d Course::getClosestPointOnSegment(cv::Point2d currentPosition, cv::Point2d segmentStart, cv::Point2d segmentEnd)
{
//https://stackoverflow.com/questions/64663170/how-to-find-nearest-point-in-segment-in-a-3d-space
	cv::Point2d v = segmentEnd - segmentStart;
	cv::Point2d w = currentPosition - segmentStart;
	double c1 = w.dot(v);
	if (c1 <= 0) {
		return segmentStart;
	}
	double c2 = v.dot(v);
	if (c2 <= c1) {
		return segmentEnd;
	}
	double b = c1 / c2;
	cv::Point2d Pb = segmentStart + b * v;
	return Pb;
}

void Course::transformCourse(cv::Point2d origin, double angle, double scale)
{
//Translate the course so that the origin is at the origin
	if (origin != NULL_PT) {
		cv::Point2d delta = origin - this->waypoints[0];
		for (int i = 0; i < this->waypoints.size(); i++) {
			this->waypoints[i] = this->waypoints[i] + delta;
		}
	}

	//Rotate the course so that the angle is 0
	if (angle != 0) {
		//convert angle to radians
		angle = angle * 3.141 / 180;


		double delta = angle - currentRotation;

		//Rotate all waypoints about the first waypoint by delta
		//https://math.stackexchange.com/questions/4354438/how-to-rotate-a-point-on-a-cartesian-plane-around-something-other-than-the-origi#:~:text=If%20you%20want%20to%20rotate,the%20resultant%20vector%20of%20rotation.

		cv::Matx22d rotationMatrix = cv::Matx22d(cos(delta), -sin(delta), sin(delta), cos(delta));
		//We assume the first point is the origin(0,0)


		for (int i = 0; i < this->waypoints.size(); i++) {
			cv::Matx21d point = cv::Matx21d(this->waypoints[i].x, this->waypoints[i].y);
			cv::Matx21d rotated = rotationMatrix * cv::Matx21d((this->waypoints[i]));
					this->waypoints[i].x = rotated(0,0);
					this->waypoints[i].y = rotated(1,0);
		}
		currentRotation = angle;
	}

	if (scale != 1) {
		//Scale the course so that the scale is 1
		for (int i = 0; i < this->waypoints.size(); i++) {
			this->waypoints[i].x = this->waypoints[i].x * scale;
			this->waypoints[i].y = this->waypoints[i].y * scale;
		}
		effective_scale = effective_scale * scale;
		std::cout << "Effective scale: " << effective_scale << std::endl;
	}
}


