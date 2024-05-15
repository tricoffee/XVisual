#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Handle/PreInverHandle.h"
#include "Common/OpenCVHeaders.h"
#include "Common/ImagePre.h"

#define CHECK_NULLPTR(ptr) ((ptr) == nullptr)
#define CHECK_ObjectItem(object) ((object != nullptr) && ((object)->type & cJSON_Object))

PreInverHandle::PreInverHandle() : XBaseHandle()
{
	XLOG_INFO("class PreInverHandle: public XBaseHandle ...", CURRENT_THREAD_ID);
	std::string classNameStr = "PreInver";
	setClassName(classNameStr);
	createUniqueName(classNameStr);
	initParams();
}
PreInverHandle::~PreInverHandle()
{

}
XVisual::ErrorCode PreInverHandle::setInnerParam(cJSON* innerParamPtr)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode PreInverHandle::setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode PreInverHandle::writeOuterParam(cJSON* cjson_variableSource, const std::string& xName)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode PreInverHandle::writeInnerParam(cJSON* cjson_innerParam)
{
	return XVisual::ErrorCode::Success;
}
void PreInverHandle::initParams()
{
	cv::Mat image;
	REGISTER_MEMBER_ATTR_STR(sources, "image", image, false);
	REGISTER_TYPE_STR(sources, "image", image);
	PreParam preParam;
	REGISTER_MEMBER_ATTR_STR(sources, "preParam", preParam, false);
	REGISTER_TYPE_STR(sources, "preParam", preParam);
	cv::Mat restoredImage;
	REGISTER_MEMBER_ATTR_STR(dests, "restoredImage", restoredImage, false);
	REGISTER_TYPE_STR(dests, "restoredImage", restoredImage);
}
void PreInverHandle::xOperate()
{
	cv::Mat image = GET_MEMBER_WITH_TYPE_STR(sources, cv::Mat, "image");
	if (!image.empty())
	{
		XLOG_INFO("PreInverHandle::xOperate, image is NOT empty ", CURRENT_THREAD_ID);
		PreParam preParams = GET_MEMBER_WITH_TYPE_STR(sources, PreParam, "preParam");
		ImagePre imgPre1(preParams);
		cv::Mat outputImage;
		imgPre1.decode(image, outputImage);
		REGISTER_MEMBER_STR(dests, "restoredImage", outputImage);
	}
	else
	{
		XLOG_INFO("PreInverHandle::xOperate, image is EMPTY ", CURRENT_THREAD_ID);
	}
}

REGISTER_HANDLE(PreInver);