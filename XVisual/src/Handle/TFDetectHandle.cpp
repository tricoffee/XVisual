#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Handle/TFDetectHandle.h"
#include "Common/OpenCVHeaders.h"
#include "TFModel/TensorFlowModel.h"

TFDetectHandle::TFDetectHandle() : XBaseHandle()
{
	//std::cout << "class TFDetectHandle: public TFDetectHandle " << std::endl;
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
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode TFDetectHandle::writeOuterParam(cJSON* cjson_variableSource, const std::string& xName)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode TFDetectHandle::writeInnerParam(cJSON* cjson_innerParam)
{
	return XVisual::ErrorCode::Success;
}
void TFDetectHandle::initParams()
{
	model = nullptr;
	std::string savedModelPath;
	REGISTER_MEMBER_ATTR_STR(sources, "savedModelPath", savedModelPath, true);
	REGISTER_TYPE_STR(sources, "savedModelPath", savedModelPath);
	cv::Mat input_image;
	REGISTER_MEMBER_ATTR_STR(sources, "input_image", input_image, false);
	REGISTER_TYPE_STR(sources, "input_image", input_image);
	std::vector<std::vector<float> >  results;
	REGISTER_MEMBER_ATTR_STR(dests, "results", results, false);
	REGISTER_TYPE_STR(dests, "results", results);
}
void TFDetectHandle::xOperate()
{
	if (model == nullptr)
	{
		std::string saved_model_dir = GET_MEMBER_WITH_TYPE_STR(sources, std::string, "savedModelPath");
		model->loadSavedModel(saved_model_dir);
	}
	else
	{
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
			//TODO model->getResults


		}
	}
}

REGISTER_HANDLE(TFDetect);