#include "Handle/CVCropHandle.h"
#include "Common/OpenCVHeaders.h"

CVCropHandle::CVCropHandle() : XBaseHandle()
{
	std::cout << "class CVCropHandle: public XBaseHandle " << std::endl;
	initParams();
}
CVCropHandle::~CVCropHandle()
{

}
void CVCropHandle::initParams()
{
	cv::Mat image;
	REGISTER_MEMBER_ATTR(sources, image, image, false);
	REGISTER_TYPE(sources, image, image);
	cv::Mat croppedImage;
	REGISTER_MEMBER_ATTR(dests, croppedImage, croppedImage, false);
	REGISTER_TYPE(dests, croppedImage, croppedImage);
	regionOfInterest.x = 0;
	regionOfInterest.y = 0;
	regionOfInterest.width = -1;
	regionOfInterest.height = -1;
}
void CVCropHandle::xOperate()
{
	cv::Mat image = GET_MEMBER_WITH_TYPE_STR(sources, cv::Mat, "image");
	int imageWidth = -1;
	int imageHeight = -1;
	if (!image.empty())
	{
		imageWidth = image.cols;
		imageHeight = image.rows;
		XLOG_INFO("CVCropItem::ItemXOP Í¼Ïñ¿í¶È: " + std::to_string(imageWidth), CURRENT_THREAD_ID);
		XLOG_INFO("CVCropItem::ItemXOP Í¼Ïñ¸ß¶È: " + std::to_string(imageHeight), CURRENT_THREAD_ID);
		cv::Mat croppedImage;
		if (imageWidth != -1 && imageHeight != -1 && regionOfInterest.width != -1 &&
			regionOfInterest.height != -1 &&
			regionOfInterest.width <= imageWidth &&
			regionOfInterest.height <= imageHeight)
		{
			XLOG_INFO("regionOfInterest.width = " + std::to_string(regionOfInterest.width), CURRENT_THREAD_ID);
			XLOG_INFO("regionOfInterest.height = " + std::to_string(regionOfInterest.height), CURRENT_THREAD_ID);
			croppedImage = image(regionOfInterest).clone();
			XLOG_INFO("CVCropItem::ItemXOP, croppedImage is generated successfully. ", CURRENT_THREAD_ID);
		}
		else
		{
			croppedImage = image.clone();
			XLOG_INFO("CVCropItem::ItemXOP, Region of Intersest is invalid, the cropped image is equivalent to the original image ", CURRENT_THREAD_ID);
		}
		REGISTER_MEMBER(dests, croppedImage, croppedImage);
	}
	else
	{
		XLOG_INFO("CVCropItem::ItemXOP, image is empty ", CURRENT_THREAD_ID);
	}
}
void CVCropHandle::setRoI(const cv::Rect& roi)
{
	regionOfInterest = roi;
}
cv::Rect CVCropHandle::getRoI()
{
	return regionOfInterest;
}

REGISTER_HANDLE(CVCrop);