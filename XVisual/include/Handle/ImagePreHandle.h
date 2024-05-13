#ifndef ImagePreHandle_H
#define ImagePreHandle_H

#include <iostream>
#include "HandleBase/XBaseHandle.h"
#include "Common/PreParam.h"

class Colleague;
class ImagePreHandle : public XBaseHandle
{
	public:
		ImagePreHandle();
		~ImagePreHandle();
		XVisual::ErrorCode setInnerParam(cJSON* innerParamPtr) override;
		XVisual::ErrorCode setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap) override;
		XVisual::ErrorCode writeOuterParam(cJSON* cjson_variableSource, const std::string& xName) override;
		XVisual::ErrorCode writeInnerParam(cJSON* cjson_innerParam) override;
		void initParams() override;
		void xOperate() override;
		void setImagePreParams(const PreParam& mImagePreParams);
		PreParam getImagePreParams();
	protected:

	private:

};

#endif // ImagePreHandle_H

