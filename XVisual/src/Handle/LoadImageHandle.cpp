#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Handle/LoadImageHandle.h"
#include "Common/OpenCVHeaders.h"
#include "GlobalStorage/GlobalVariable.h"
#include <filesystem>

namespace XVisual {

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
ErrorCode LoadImageHandle::setInnerParam(cJSON* innerParamPtr)
{
	return ErrorCode::Success;
}
ErrorCode LoadImageHandle::setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap)
{
	// Done, 解析outerParamUMap, 将sources的某个key相对应的值重置为解析出来的value
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
			return  ErrorCode::ParseOuterParamFromJsonFailed;
		}

	}
	else
	{
		return  ErrorCode::ParseOuterParamFromJsonFailed;
	}
	return ErrorCode::Success;
}
ErrorCode LoadImageHandle::writeOuterParam(cJSON* cjson_variableSource, const std::string& xName)
{
	if (xName == "imagePath")
	{
		std::string imagePath = GET_MEMBER_WITH_TYPE_STR(sources, std::string, "imagePath");
		cJSON_AddStringToObject(cjson_variableSource, "outerParam", imagePath.c_str());
	}
	else
	{
		return ErrorCode::UnknownXVariableNameError;
	}
	return ErrorCode::Success;
}
ErrorCode LoadImageHandle::writeInnerParam(cJSON* cjson_innerParam)
{
	return ErrorCode::Success;
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
	std::string imagePath0 = GET_MEMBER_WITH_TYPE_STR(sources, std::string, "imagePath");
	XLOG_INFO("LoadImageHandle::xOperate, imagePath = " + imagePath0, CURRENT_THREAD_ID);

	cv::Mat image;
	if (!imagePath0.empty())
	{
		namespace fs = std::filesystem;
		fs::path p(imagePath0);

		// 仅当是相对路径时才拼接工作区
		if (p.is_relative()) {
			p = fs::path(globalWorkSpaceDir) / p;
		}

		std::string imagePath = p.lexically_normal().string();

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

} // namespace XVisual