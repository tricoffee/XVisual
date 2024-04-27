#include "Handle/LoadImageHandle.h"
#include "Common/OpenCVHeaders.h"

LoadImageHandle::LoadImageHandle() : XBaseHandle()
{
	std::cout << "class LoadImageHandle: public XBaseHandle " << std::endl;
	initParams();
}
LoadImageHandle::~LoadImageHandle()
{

}
void LoadImageHandle::initParams() 
{
	std::string imagePath;
	REGISTER_MEMBER_ATTR(sources, imagePath, imagePath, true);
	REGISTER_TYPE(sources, imagePath, imagePath);
	cv::Mat image;
	REGISTER_MEMBER_ATTR(dests, image, image, false);
	REGISTER_TYPE(dests, image, image);
}
void LoadImageHandle::xOperate()
{

	XLOG_INFO("LoadImageHandle::xOperate ...", CURRENT_THREAD_ID);
	std::string imagePath = GET_MEMBER_WITH_TYPE_STR(sources, std::string, "imagePath");
	XLOG_INFO("LoadImageHandle::xOperate, imagePath = " + imagePath, CURRENT_THREAD_ID);
	cv::Mat image;
	if (!imagePath.empty())
	{
		image = cv::imread(imagePath);
		REGISTER_MEMBER_STR(dests, "image", image);
		XLOG_INFO("LoadImageHandle::xOperate " + imagePath, CURRENT_THREAD_ID);
		if (image.empty())
		{
			XLOG_INFO("LoadImageHandle::xOperate  image is empty", CURRENT_THREAD_ID);
		}
		else
		{
			XLOG_ERROR("LoadImageHandle::xOperate image is NOT empty", CURRENT_THREAD_ID);
		}
	}
	else
	{
		XLOG_INFO("LoadImageHandle::xOperate  imagePath is empty", CURRENT_THREAD_ID);
	}
}

REGISTER_HANDLE(LoadImage);