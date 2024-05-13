#include "TFModel/BoxUtils.h"

void acquireValidBoxes(float* output_data, const int output_elements_num, float sx, float sy, float dx, float dy,
	int imageWidth, int imageHeight, std::vector<std::vector<float> >& results)
{

	for (int i = 0; i < output_elements_num; i += 6)
	{
		if (output_data[i] == -1)
		{
			continue;
		}
		float xP1 = output_data[i];
		float yP1 = output_data[i + 1];
		float xP2 = output_data[i + 2];
		float yP2 = output_data[i + 3];
		float score = output_data[i + 4];
		float classId = output_data[i + 5];
		float x1 = (xP1 - dx) / sx;
		float y1 = (yP1 - dy) / sy;
		float x2 = (xP2 - dx) / sx;
		float y2 = (yP2 - dy) / sy;
		x1 = std::max(x1, float(0.0));
		y1 = std::max(y1, float(0.0));
		x2 = std::min(x2, float(imageWidth - 1.0));
		y2 = std::min(y2, float(imageHeight - 1.0));
		std::vector<float> res = { x1, y1, x2, y2, score, classId };
		results.push_back(res);
	}
}

void drawBox(cv::Mat image, std::vector<float> boxInfo, std::vector<std::string> class_names)
{
	float xmin = boxInfo[0];
	float ymin = boxInfo[1];
	float xmax = boxInfo[2];
	float ymax = boxInfo[3];
	int x1 = int(xmin);
	int y1 = int(ymin);
	int x2 = int(xmax);
	int y2 = int(ymax);
	float score = boxInfo[4];
	float classId = boxInfo[5];
	std::string className = class_names[int(classId)];
	// 绘制边界框
	cv::rectangle(image, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 0), 2);
	// 添加Score和ClassName标签
	std::string label = className + "," + std::to_string(score);
	cv::putText(image, label, cv::Point(x1, y1 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 1);
}

void drawBoxes(cv::Mat image, std::vector<std::vector<float> > boxes, std::vector<std::string> class_names)
{
	for (int i = 0; i < boxes.size(); ++i)
	{
		drawBox(image, boxes[i], class_names);
	}
}