#include "Common/BoxUtils.h"
#include "Common/ImagePre.h"

// ImageWidth, ImageHeight is expected to match the input size of TFModel
void acquireValidBoxes(float* output_data, const int output_elements_num, 
	int imageWidth, int imageHeight, std::vector<DetectResult>& results)
{

	for (int i = 0; i < output_elements_num; i += 6)
	{
		if (output_data[i] == -1)
		{
			continue;
		}
		int x1 = static_cast<int>(output_data[i]);
		int y1 = static_cast<int>(output_data[i + 1]);
		int x2 = static_cast<int>(output_data[i + 2]);
		int y2 = static_cast<int>(output_data[i + 3]);
		float score = output_data[i + 4];
		int classId = static_cast<int>(output_data[i + 5]);
		x1 = std::max(x1, 0);
		y1 = std::max(y1, 0);
		x2 = std::min(x2, imageWidth - 1);
		y2 = std::min(y2, imageHeight - 1);
		DetectResult res = { x1, y1, x2, y2, score, classId };
		results.push_back(res);
	}
}

void revertBoxes(const std::vector<DetectResult>& results, std::vector<DetectResult>& revertedResults, const PreParam& preParam,
	int rawWidth, int rawHeight)
{
	ImagePre pre1(preParam);
	for (const auto& detectRes : results)
	{
		int x1 = detectRes.x1;
		int y1 = detectRes.y1;
		int x2 = detectRes.x2;
		int y2 = detectRes.y2;
		float score = detectRes.score;
		int classId = detectRes.classId;
		pre1.revertBox(x1, y1, x2, y2);
		/*
		If the provided rawWidth is not - 1 and rawHeight is not - 1,
		it indicates that the widthand height of the original image provided are legal.
		Therefore, illegal boxes that exceed the size of the original image can be filtered out
		Usually, this code is not executed, and the legality check of boxes is done in ImagePre::restoreBox
		So let rawWidth remain at the default value, which is equal to -1;
		So let rawHeight remain at the default value, which is equal to -1
		*/
		if (rawWidth != -1 && rawHeight != -1) 
		{
			x1 = std::max(x1, 0);
			y1 = std::max(y1, 0);
			x2 = std::min(x2, rawWidth - 1);
			y2 = std::min(y2, rawHeight - 1);
		}
		DetectResult res = { x1, y1, x2, y2, score, classId };
		revertedResults.push_back(res);
	}
}

void drawBox(cv::Mat image, const DetectResult& boxInfo, const std::vector<std::string>& class_names)
{
	int x1 = boxInfo.x1;
	int y1 = boxInfo.y1;
	int x2 = boxInfo.x2;
	int y2 = boxInfo.y2;
	float score = boxInfo.score;
	std::string className = class_names[boxInfo.classId];
	// Draw bounding box
	cv::rectangle(image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 0), 2);
	// Add "Score" and "ClassName" tags
	std::string label = className + "," + std::to_string(score);
	cv::putText(image, label, cv::Point(x1, y1 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 1);
}

void drawBoxes(cv::Mat image, const std::vector<DetectResult>& boxes, const std::vector<std::string>& class_names)
{
	for (int i = 0; i < boxes.size(); ++i)
	{
		drawBox(image, boxes[i], class_names);
	}
}