#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Handle/RevertBoxHandle.h"
#include "Common/OpenCVHeaders.h"
#include "Common/DetectResult.h"
#include "Common/ImagePre.h"
#include "Common/BoxUtils.h"

namespace XVisual {

RevertBoxHandle::RevertBoxHandle() : XBaseHandle()
{
	XLOG_INFO("class RevertBoxHandle: public XBaseHandle ...", CURRENT_THREAD_ID);
	std::string classNameStr = "RevertBox";
	setClassName(classNameStr);
	createUniqueName(classNameStr);
	initParams();
}
RevertBoxHandle::~RevertBoxHandle()
{

}
ErrorCode RevertBoxHandle::setInnerParam(cJSON* innerParamPtr)
{
	return ErrorCode::Success;
}
ErrorCode RevertBoxHandle::setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap)
{
	return ErrorCode::Success;
}
ErrorCode RevertBoxHandle::writeOuterParam(cJSON* cjson_variableSource, const std::string& xName)
{
	return ErrorCode::Success;
}
ErrorCode RevertBoxHandle::writeInnerParam(cJSON* cjson_innerParam)
{
	return ErrorCode::Success;
}
void RevertBoxHandle::initParams()
{
	std::vector<DetectResult>  detectResults;
	REGISTER_MEMBER_ATTR_STR(sources, "detectResults", detectResults, false);
	REGISTER_TYPE_STR(sources, "detectResults", detectResults);
	PreParam preParam;
	REGISTER_MEMBER_ATTR_STR(sources, "preParam", preParam, false);
	REGISTER_TYPE_STR(sources, "preParam", preParam);
	std::vector<DetectResult>  detections;
	REGISTER_MEMBER_ATTR_STR(dests, "detections", detections, false);
	REGISTER_TYPE_STR(dests, "detections", detections);
}
void RevertBoxHandle::xOperate()
{
	std::vector<DetectResult> detectResults = GET_MEMBER_WITH_TYPE_STR(sources, std::vector<DetectResult>, "detectResults");
	PreParam preParam = GET_MEMBER_WITH_TYPE_STR(sources, PreParam, "preParam");
	std::vector<DetectResult> detections;
	revertBoxes(detectResults, detections, preParam);
	REGISTER_MEMBER_STR(dests, "detections", detections);
}


REGISTER_HANDLE(RevertBox);

} // namespace XVisual