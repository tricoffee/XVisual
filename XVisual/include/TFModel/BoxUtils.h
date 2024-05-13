#ifndef BoxUtils_H
#define BoxUtils_H

#include <iostream>
#include <vector>
#include "Common/OpenCVHeaders.h"

void acquireValidBoxes(float* output_data, const int output_elements_num, float sx, float sy, float dx, float dy,
	int imageWidth, int imageHeight, std::vector<std::vector<float> >& results);

void drawBox(cv::Mat image, std::vector<float> boxInfo, std::vector<std::string> class_names);

void drawBoxes(cv::Mat image, std::vector<std::vector<float> > boxes, std::vector<std::string> class_names);

#endif //BoxUtils_H