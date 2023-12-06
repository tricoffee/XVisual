#include "Common/OpenCVHeaders.h"
//   ��CV::Matת��ΪQImage
QImage cvMatToQImage(const cv::Mat& mat)
{
	// ��CV::Matת��ΪRGB��ʽ��QImage
	QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_RGB888);

	// ��QImageת��Ϊ��ʽ��������ʾ�ĸ�ʽ��������Ҫ����ʵ��������е�����
	return image.rgbSwapped();
}