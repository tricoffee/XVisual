#ifndef RevertBoxHandle_H
#define RevertBoxHandle_H

#include <iostream>
#include "HandleBase/XBaseHandle.h"

class Colleague;
class RevertBoxHandle : public XBaseHandle
{
	public:
		RevertBoxHandle();
		~RevertBoxHandle();
		XVisual::ErrorCode setInnerParam(cJSON* innerParamPtr) override;
		XVisual::ErrorCode setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap) override;
		XVisual::ErrorCode writeOuterParam(cJSON* cjson_variableSource, const std::string& xName) override;
		XVisual::ErrorCode writeInnerParam(cJSON* cjson_innerParam) override;
		void initParams() override;
		void xOperate() override;
	protected:

	private:

};

#endif // RevertBoxHandle_H

