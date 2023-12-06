#include "Common/OpenCVHeaders.h"
//   将CV::Mat转换为QImage
QImage cvMatToQImage(const cv::Mat& mat)
{
	// 将CV::Mat转换为RGB格式的QImage
	QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_RGB888);

	// 将QImage转换为格式适用于显示的格式（可能需要根据实际情况进行调整）
	return image.rgbSwapped();
}