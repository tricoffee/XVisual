#ifndef BoxUtils_H
#define BoxUtils_H

#include <iostream>
#include <vector>
#include "Common/OpenCVHeaders.h"
#include "Common/DetectResult.h"
#include "Common/PreParam.h"

void acquireValidBoxes(float* output_data, const int output_elements_num, 
	int imageWidth, int imageHeight, std::vector<DetectResult>& results);

/*
Revert Box to its original scale
If the provided rawWidth is not - 1 and rawHeight is not - 1,
it indicates that the widthand height of the original image provided are legal.
Therefore, illegal boxes that exceed the size of the original image can be filtered out
*/
void revertBoxes(const std::vector<DetectResult>& results, std::vector<DetectResult>& restored, const PreParam& preParam,
	int rawWidth = -1, int rawHeight = -1);

void drawBox(cv::Mat image, const DetectResult& boxInfo, const std::vector<std::string>& class_names);

void drawBoxes(cv::Mat image, const std::vector<DetectResult>& boxes, const std::vector<std::string>& class_names);

#endif //BoxUtils_H