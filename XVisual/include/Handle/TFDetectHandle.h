#ifndef TFDetectHandle_H
#define TFDetectHandle_H

#include <iostream>
#include "HandleBase/XBaseHandle.h"

namespace XVisual {

class TensorFlowModel;
class Colleague;

class TFDetectHandle : public XBaseHandle
{
	public:
		TFDetectHandle();
		~TFDetectHandle();
		XVisual::ErrorCode setInnerParam(cJSON* innerParamPtr) override;
		XVisual::ErrorCode setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap) override;
		XVisual::ErrorCode writeOuterParam(cJSON* cjson_variableSource, const std::string& xName) override;
		XVisual::ErrorCode writeInnerParam(cJSON* cjson_innerParam) override;
		void initParams() override;
		void xOperate() override;
	protected:
		
	private:
		TensorFlowModel* model = nullptr;
};

} // namespace XVisual

#endif // TFDetectHandle_H

