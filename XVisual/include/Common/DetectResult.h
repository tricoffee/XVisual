#ifndef DetectResult_H
#define DetectResult_H

#include <iostream>
#include <string>

struct DetectResult
{
	int x1 = -1; // leftTop (x1,y1)
	int y1 = -1;
	int x2 = -1; // bottomRight (x2,y2)
	int y2 = -1;
	float score = -1.0; // score
	int classId = -1; // ClassId represents the order of a certain classname in the classnames list
	std::string className = "";
};

#endif // DetectResult_H
