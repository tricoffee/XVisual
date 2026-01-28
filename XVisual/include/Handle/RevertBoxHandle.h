#ifndef RevertBoxHandle_H
#define RevertBoxHandle_H

#include <iostream>
#include "HandleBase/XBaseHandle.h"

namespace XVisual {

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

} // namespace XVisual

#endif // RevertBoxHandle_H

