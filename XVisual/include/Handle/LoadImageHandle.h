#ifndef LoadImageHandle_H
#define LoadImageHandle_H

#include <iostream>
#include "HandleBase/XBaseHandle.h"

//class XBaseItem;
//class LoadImageItem;
class Colleague;
class LoadImageHandle : public XBaseHandle
{
	public:
		LoadImageHandle();
		~LoadImageHandle();
		XVisual::ErrorCode setInnerParam(cJSON* innerParamPtr) override;
		XVisual::ErrorCode setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap) override;
		XVisual::ErrorCode writeOuterParam(cJSON* cjson_variableSource, const std::string& xName) override;
		XVisual::ErrorCode writeInnerParam(cJSON* cjson_innerParam) override;
		void initParams() override;
		void xOperate() override;
		//friend class XBaseItem;
		//friend class LoadImageItem;
	private:

};

#endif // LoadImageHandle_H