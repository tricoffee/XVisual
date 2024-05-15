#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Handle/TFDetectHandle.h"
#include "Common/OpenCVHeaders.h"
#include "TFModel/TensorFlowModel.h"
#include "Common/DetectResult.h"
#include "Common/FileUtils.h"
#include <filesystem>

TFDetectHandle::TFDetectHandle() : XBaseHandle()
{
	XLOG_INFO("class TFDetectHandle: public TFDetectHandle ...", CURRENT_THREAD_ID);
	std::string classNameStr = "TFDetect";
	setClassName(classNameStr);
	createUniqueName(classNameStr);
	initParams();
}
TFDetectHandle::~TFDetectHandle()
{

}
XVisual::ErrorCode TFDetectHandle::setInnerParam(cJSON* innerParamPtr)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode TFDetectHandle::setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap)
{
	// Done, 解析outerParamUMap, 将sources的某个key相对应的值重置为解析出来的value
	if (!outerParamUMap.empty())
	{
		auto it = outerParamUMap.find("savedModelPath");
		if (it != outerParamUMap.end() && cJSON_IsString(outerParamUMap["savedModelPath"]))
		{
			std::string savedModelPath = outerParamUMap["savedModelPath"]->valuestring;
			REGISTER_MEMBER_STR(sources, "savedModelPath", savedModelPath);
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
XVisual::ErrorCode TFDetectHandle::writeOuterParam(cJSON* cjson_variableSource, const std::string& xName)
{
	if (xName == "savedModelPath")
	{
		std::string savedModelPath = GET_MEMBER_WITH_TYPE_STR(sources, std::string, "savedModelPath");
		cJSON_AddStringToObject(cjson_variableSource, "outerParam", savedModelPath.c_str());
	}
	else
	{
		return XVisual::ErrorCode::UnknownXVariableNameError;
	}
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode TFDetectHandle::writeInnerParam(cJSON* cjson_innerParam)
{
	return XVisual::ErrorCode::Success;
}
void TFDetectHandle::initParams()
{
	model = new TensorFlowModel();
	std::string savedModelPath;
	REGISTER_MEMBER_ATTR_STR(sources, "savedModelPath", savedModelPath, true);
	REGISTER_TYPE_STR(sources, "savedModelPath", savedModelPath);
	cv::Mat input_image;
	REGISTER_MEMBER_ATTR_STR(sources, "input_image", input_image, false);
	REGISTER_TYPE_STR(sources, "input_image", input_image);
	std::vector<DetectResult>  detectResults;
	REGISTER_MEMBER_ATTR_STR(dests, "detectResults", detectResults, false);
	REGISTER_TYPE_STR(dests, "detectResults", detectResults);

//#ifdef DEBUG_UNUSED
	std::vector<std::string>  classNames;
	REGISTER_MEMBER_ATTR_STR(dests, "classNames", classNames, false);
	REGISTER_TYPE_STR(dests, "classNames", classNames);
//#endif 

}
void TFDetectHandle::xOperate()
{ 
	XLOG_INFO(" === @@@ TFDetectHandle::xOperate === @@@ ", CURRENT_THREAD_ID);
	
	std::string saved_model_dir = GET_MEMBER_WITH_TYPE_STR(sources, std::string, "savedModelPath");
	XLOG_INFO("TFDetectHandle::xOperate, saved_model_dir = " + saved_model_dir, CURRENT_THREAD_ID);

//#ifdef DEBUG_UNUSED
	const std::string classNameTxtStr = "classnames.txt";
	std::filesystem::path classNamesPath = saved_model_dir;
	classNamesPath /= classNameTxtStr; // 使用 /= 运算符进行路径拼接
	std::vector<std::string> classNames;
	readClassNames(classNamesPath.string(), classNames);
	REGISTER_MEMBER_STR(dests, "classNames", classNames);
//#endif 

	if (model == nullptr)
	{
		XLOG_INFO("TFDetectHandle::xOperate, model is NULLPTR ", CURRENT_THREAD_ID);
	}
	else
	{
		XLOG_INFO("TFDetectHandle::xOperate, model is not nullptr ", CURRENT_THREAD_ID);
	}
	model->loadSavedModel(saved_model_dir);


	cv::Mat image = GET_MEMBER_WITH_TYPE_STR(sources, cv::Mat, "input_image");

	if (image.empty())
	{
		XLOG_INFO("TFDetectHandle::xOperate, \"input_image\" is EMPTY ...", CURRENT_THREAD_ID);
		//TODO XLOG input_image is empty and raise exception

	}
	else
	{
		model->loadInputData(image);
		model->doRun(); 
		std::vector<DetectResult> detectResults;
		model->getResults(detectResults);
		REGISTER_MEMBER_STR(dests, "detectResults", detectResults);
		for (const auto& DetectResult : detectResults)
		{
			XLOG_INFO("DetectResult.classId = " + std::to_string(DetectResult.classId), CURRENT_THREAD_ID);
		}

	}
	
}

REGISTER_HANDLE(TFDetect);