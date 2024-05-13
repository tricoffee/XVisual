#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Handle/LoadImageHandle.h"
#include "Common/OpenCVHeaders.h"

LoadImageHandle::LoadImageHandle() : XBaseHandle()
{
	//std::cout << "class LoadImageHandle: public XBaseHandle " << std::endl;
	XLOG_INFO("class LoadImageHandle: public XBaseHandle ...", CURRENT_THREAD_ID);
	std::string classNameStr = "LoadImage";
	setClassName(classNameStr);
	createUniqueName(classNameStr);
	initParams();
}
LoadImageHandle::~LoadImageHandle()
{

}
XVisual::ErrorCode LoadImageHandle::setInnerParam(cJSON* innerParamPtr)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode LoadImageHandle::setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap)
{
	// Done, ����outerParamUMap, ��sources��ĳ��key���Ӧ��ֵ����Ϊ����������value

	if (!outerParamUMap.empty())
	{
		auto it = outerParamUMap.find("imagePath");
		if (it != outerParamUMap.end() && cJSON_IsString(outerParamUMap["imagePath"]))
		{
			std::string image_path = outerParamUMap["imagePath"]->valuestring;
			REGISTER_MEMBER_STR(sources, "imagePath", image_path);
		}
		else
		{
			return  XVisual::ErrorCode::ParseOuterParamFromJsonFailed;
		}

	}
	else
	{
		return  XVisual::ErrorCode::ParseOuterParamFromJsonFailed;
	}
	
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode LoadImageHandle::writeOuterParam(cJSON* cjson_variableSource, const std::string& xName)
{
	if (xName == "imagePath")
	{
		std::string imagePath = GET_MEMBER_WITH_TYPE_STR(sources, std::string, "imagePath");
		cJSON_AddStringToObject(cjson_variableSource, "outerParam", imagePath.c_str());
	}
	else
	{
		return XVisual::ErrorCode::UnknownXVariableNameError;
	}
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode LoadImageHandle::writeInnerParam(cJSON* cjson_innerParam)
{
	return XVisual::ErrorCode::Success;
}
void LoadImageHandle::initParams() 
{
	std::string imagePath;
	REGISTER_MEMBER_ATTR_STR(sources, "imagePath", imagePath, true);
	REGISTER_TYPE_STR(sources, "imagePath", imagePath);
	cv::Mat image;
	REGISTER_MEMBER_ATTR_STR(dests, "image", image, false);
	REGISTER_TYPE_STR(dests, "image", image);
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