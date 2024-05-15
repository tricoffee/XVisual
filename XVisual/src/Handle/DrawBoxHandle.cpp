#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Handle/DrawBoxHandle.h"
#include "Common/OpenCVHeaders.h"
#include "Common/DetectResult.h"
#include "Common/BoxUtils.h"
#include "Common/FileUtils.h"

DrawBoxHandle::DrawBoxHandle() : XBaseHandle()
{
	XLOG_INFO("class DrawBoxHandle: public XBaseHandle ...", CURRENT_THREAD_ID);
	std::string classNameStr = "DrawBox";
	setClassName(classNameStr);
	createUniqueName(classNameStr);
	initParams();
}
DrawBoxHandle::~DrawBoxHandle()
{

}
XVisual::ErrorCode DrawBoxHandle::setInnerParam(cJSON* innerParamPtr)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode DrawBoxHandle::setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode DrawBoxHandle::writeOuterParam(cJSON* cjson_variableSource, const std::string& xName)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode DrawBoxHandle::writeInnerParam(cJSON* cjson_innerParam)
{
	return XVisual::ErrorCode::Success;
}
void DrawBoxHandle::initParams()
{
	cv::Mat image;
	REGISTER_MEMBER_ATTR_STR(sources, "image", image, false);
	REGISTER_TYPE_STR(sources, "image", image);
	std::vector<DetectResult> detections;
	REGISTER_MEMBER_ATTR_STR(sources, "detections", detections, false);
	REGISTER_TYPE_STR(sources, "detections", detections);

//#ifdef DEBUG_UNUSED
	std::vector<std::string> classNames;
	REGISTER_MEMBER_ATTR_STR(sources, "classNames", classNames, false);
	REGISTER_TYPE_STR(sources, "classNames", classNames);
//#endif

	cv::Mat resultImage;
	REGISTER_MEMBER_ATTR_STR(dests, "resultImage", resultImage, false);
	REGISTER_TYPE_STR(dests, "resultImage", resultImage);
}
void DrawBoxHandle::xOperate()
{
	cv::Mat image = GET_MEMBER_WITH_TYPE_STR(sources, cv::Mat, "image");
	if (!image.empty())
	{
		int imageWidth = image.cols;
		int imageHeight = image.rows;
		XLOG_INFO("DrawBoxHandle::xOperate Í¼Ïñ¿í¶È: " + std::to_string(imageWidth), CURRENT_THREAD_ID);
		XLOG_INFO("DrawBoxHandle::xOperate Í¼Ïñ¸ß¶È: " + std::to_string(imageHeight), CURRENT_THREAD_ID);
		std::vector<DetectResult> detections = GET_MEMBER_WITH_TYPE_STR(sources, std::vector<DetectResult>, "detections");

//#ifdef DEBUG_UNUSED
		std::vector<std::string> classNames = GET_MEMBER_WITH_TYPE_STR(sources, std::vector<std::string>, "classNames");
//#endif 

#ifdef DEBUG_UNUSED
		std::vector<std::string> classNames;
		std::string classNamesPath = "C:\\NDev\\code\\logs\\mAP_Shiyan3_11\\exported_model\\classnames.txt";
		readClassNames(classNamesPath, classNames);
#endif 

		drawBoxes(image, detections, classNames);
		REGISTER_MEMBER_STR(dests, "resultImage", image);
	}
	else
	{
		XLOG_INFO("DrawBoxHandle::xOperate, image is EMPTY ", CURRENT_THREAD_ID);
	}
}

REGISTER_HANDLE(DrawBox);