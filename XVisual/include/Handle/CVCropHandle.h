#ifndef CVCropHandle_H
#define CVCropHandle_H

#include <iostream>
#include "HandleBase/XBaseHandle.h"

namespace XVisual {

class Colleague;

class CVCropHandle : public XBaseHandle
{
	public:
		CVCropHandle();
		~CVCropHandle();
		XVisual::ErrorCode setInnerParam(cJSON* innerParamPtr) override;
		XVisual::ErrorCode setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap) override;
		XVisual::ErrorCode writeOuterParam(cJSON* cjson_variableSource, const std::string& xName) override;
		XVisual::ErrorCode writeInnerParam(cJSON* cjson_innerParam) override;
		void initParams() override;
		void xOperate() override;
		void setRoI(const cv::Rect& regionOfInterest);
		cv::Rect getRoI();
	protected:

	private:

};

} // namespace XVisual

#endif // CVCropHandle_H
