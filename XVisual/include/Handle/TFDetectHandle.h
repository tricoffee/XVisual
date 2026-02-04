#ifndef TFDetectHandle_H
#define TFDetectHandle_H

#include <iostream>
#include "HandleBase/XBaseHandle.h"
#include "Core/Device/DeviceType.h"

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
		
		/**
		 * TFDetect 节点偏好 GPU 设备
		 * 
		 * PR-5.1: 实际仍在 CPU 执行，但会路由到 GPU 串行队列
		 * PR-5.2+: 真正的 GPU 推理
		 */
		DeviceType preferredDevice() const override { return DeviceType::GPU; }
	protected:
		
	private:
		TensorFlowModel* model = nullptr;
};

} // namespace XVisual

#endif // TFDetectHandle_H
