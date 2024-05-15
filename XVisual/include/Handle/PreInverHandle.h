#ifndef PreInverHandle_H
#define PreInverHandle_H

#include <iostream>
#include "HandleBase/XBaseHandle.h"
#include "Common/PreParam.h"

class Colleague;
class PreInverHandle : public XBaseHandle
{
	public:
		PreInverHandle();
		~PreInverHandle();
		XVisual::ErrorCode setInnerParam(cJSON* innerParamPtr) override;
		XVisual::ErrorCode setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap) override;
		XVisual::ErrorCode writeOuterParam(cJSON* cjson_variableSource, const std::string& xName) override;
		XVisual::ErrorCode writeInnerParam(cJSON* cjson_innerParam) override;
		void initParams() override;
		void xOperate() override;
	protected:

	private:

};

#endif // PreInverHandle_H


