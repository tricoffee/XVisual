#include "Handle/CVCropHandle.h"
#include "Common/OpenCVHeaders.h"

CVCropHandle::CVCropHandle() : XBaseHandle()
{
	//std::cout << "class CVCropHandle: public XBaseHandle " << std::endl;
	XLOG_INFO("class CVCropHandle: public XBaseHandle ...", CURRENT_THREAD_ID);
	std::string classNameStr = "CVCrop";
	setClassName(classNameStr);
	createUniqueName(classNameStr);
	initParams();
}
CVCropHandle::~CVCropHandle()
{

}
XVisual::ErrorCode CVCropHandle::setInnerParam(cJSON* innerParamPtr)
{
	//	 获取 roi 数组
	cJSON* roi_array = cJSON_GetObjectItemCaseSensitive(innerParamPtr, "roi");
	if (cJSON_IsArray(roi_array))
	{
		//printf("ROI: [%d, %d, %d, %d]\n",
		//	cJSON_GetArrayItem(roi_array, 0)->valueint,
		//	cJSON_GetArrayItem(roi_array, 1)->valueint,
		//	cJSON_GetArrayItem(roi_array, 2)->valueint,
		//	cJSON_GetArrayItem(roi_array, 3)->valueint);

		int roiX = cJSON_GetArrayItem(roi_array, 0)->valueint;
		int roiY = cJSON_GetArrayItem(roi_array, 1)->valueint;
		int roiWidth = cJSON_GetArrayItem(roi_array, 2)->valueint;
		int roiHeight = cJSON_GetArrayItem(roi_array, 3)->valueint;
		XLOG_INFO("roiX = " + std::to_string(roiX) + ", roiY = " + std::to_string(roiY) + ", roiWidth = " + std::to_string(roiWidth) + ", roiHeight = " + std::to_string(roiHeight), CURRENT_THREAD_ID);
		cv::Rect regionOfInterest(roiX, roiY, roiWidth, roiHeight);
		innerParam["roi"] = regionOfInterest;
		return XVisual::ErrorCode::Success;
	}
	else
	{
		return XVisual::ErrorCode::ParseInnerParamElementFailed;
	}
}
XVisual::ErrorCode CVCropHandle::setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode CVCropHandle::writeOuterParam(cJSON* cjson_variableSource, const std::string& xName)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode CVCropHandle::writeInnerParam(cJSON* cjson_innerParam)
{
	cJSON* cjson_roi = cJSON_CreateArray();
	cv::Rect mRoI = std::any_cast<cv::Rect >(innerParam["roi"]);
	cJSON_AddItemToArray(cjson_roi, cJSON_CreateNumber(mRoI.x));
	cJSON_AddItemToArray(cjson_roi, cJSON_CreateNumber(mRoI.y));
	cJSON_AddItemToArray(cjson_roi, cJSON_CreateNumber(mRoI.width));
	cJSON_AddItemToArray(cjson_roi, cJSON_CreateNumber(mRoI.height));
	cJSON_AddItemToObject(cjson_innerParam, "roi", cjson_roi);
	return XVisual::ErrorCode::Success;
}
void CVCropHandle::initParams()
{
	cv::Mat image;
	REGISTER_MEMBER_ATTR(sources, image, image, false);
	REGISTER_TYPE(sources, image, image);
	cv::Mat croppedImage;
	REGISTER_MEMBER_ATTR(dests, croppedImage, croppedImage, false);
	REGISTER_TYPE(dests, croppedImage, croppedImage);
	cv::Rect regionOfInterest;
	regionOfInterest.x = 0;
	regionOfInterest.y = 0;
	regionOfInterest.width = -1;
	regionOfInterest.height = -1;
	innerParam["roi"] = regionOfInterest;
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
		XLOG_INFO("CVCropHandle::xOperate 图像宽度: " + std::to_string(imageWidth), CURRENT_THREAD_ID);
		XLOG_INFO("CVCropHandle::xOperate 图像高度: " + std::to_string(imageHeight), CURRENT_THREAD_ID);
		cv::Mat croppedImage;
		cv::Rect regionOfInterest = std::any_cast<cv::Rect>(innerParam["roi"]);
		if (imageWidth != -1 && imageHeight != -1 && regionOfInterest.width != -1 &&
			regionOfInterest.height != -1 &&
			regionOfInterest.width <= imageWidth &&
			regionOfInterest.height <= imageHeight)
		{
			XLOG_INFO("regionOfInterest.width = " + std::to_string(regionOfInterest.width), CURRENT_THREAD_ID);
			XLOG_INFO("regionOfInterest.height = " + std::to_string(regionOfInterest.height), CURRENT_THREAD_ID);
			croppedImage = image(regionOfInterest).clone();
			XLOG_INFO("CVCropHandle::xOperate, croppedImage is generated successfully. ", CURRENT_THREAD_ID);
		}
		else
		{
			croppedImage = image.clone();
			XLOG_INFO("CVCropHandle::xOperate, Region of Intersest is invalid, the cropped image is equivalent to the original image ", CURRENT_THREAD_ID);
		}
		REGISTER_MEMBER(dests, croppedImage, croppedImage);
	}
	else
	{
		XLOG_INFO("CVCropHandle::xOperate, image is empty ", CURRENT_THREAD_ID);
	}
}
void CVCropHandle::setRoI(const cv::Rect& roi)
{
	innerParam["roi"] = roi;
}
cv::Rect CVCropHandle::getRoI()
{
	return std::any_cast<cv::Rect>(innerParam["roi"]);
}

REGISTER_HANDLE(CVCrop);