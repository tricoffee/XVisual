#ifndef DrawBoxHandle_H
#define DrawBoxHandle_H

#include <iostream>
#include "HandleBase/XBaseHandle.h"

class Colleague;
class DrawBoxHandle : public XBaseHandle
{
	public:
		DrawBoxHandle();
		~DrawBoxHandle();
		XVisual::ErrorCode setInnerParam(cJSON* innerParamPtr) override;
		XVisual::ErrorCode setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap) override;
		XVisual::ErrorCode writeOuterParam(cJSON* cjson_variableSource, const std::string& xName) override;
		XVisual::ErrorCode writeInnerParam(cJSON* cjson_innerParam) override;
		void initParams() override;
		void xOperate() override;
	protected:

	private:

};

#endif // DrawBoxHandle_H

