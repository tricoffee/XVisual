#ifndef XBaseHandle_H
#define XBaseHandle_H

#include <iostream>
#include <memory>
#include <unordered_map>
#include "HandleFactory.h"
#include "ColleagueType.h"
#include "Colleague.h"
#include "Core/Runtime/VarBag.h"
#include "Core/Executor/INode.h"
#include "XVariable/XMacro.h"
#include "XVariable/SourceFrom.h"
#include "CJSON/cJSON.h"
#include "Common/ErrorCode.h"

class XBaseItem;
class LoadImageItem;
class CVCropItem;
class ImagePreItem;
class TFDetectItem;

namespace XVisual {

class XBaseHandle : public Colleague, public INode
{
	public:
		XBaseHandle();
		virtual ~XBaseHandle();
		VarBag& getSources();
		VarBag& getDests();
		virtual XVisual::ErrorCode setInnerParam(cJSON* innerParamPtr);
		virtual XVisual::ErrorCode setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap);
		virtual XVisual::ErrorCode writeOuterParam(cJSON* cjson_variableSource, const std::string& xName);
		virtual XVisual::ErrorCode writeInnerParam(cJSON* cjson_innerParam);
		const std::unordered_map<std::string, std::any>& getInnerParam() const;
		virtual void initParams();
		virtual void initOperands();
		virtual void xOperate();
		// INode 接口实现
		void execute(std::stop_token st) override;
		void setSourceFrom(const std::string& xVariableName, const SourceFrom& sourceFrom);
		void loadSourceFrom(const std::string& xVariableName, SourceFrom& sourceFrom);
		void setUuidConsistentWithItem(std::string xitemUuid);
		friend class XBaseItem;
		friend class LoadImageItem;
		friend class CVCropItem;
		friend class ImagePreItem;
		friend class TFDetectItem;
		friend class RevertBoxItem;
		friend class DrawBoxItem;
	protected:
		std::unique_ptr<VarBag> sources;
		std::unique_ptr<VarBag> dests;
		void setColleagueType() override;
		void createUuid() override;
		void createUniqueName(const std::string& classNameStr) override;
		void setClassName(const std::string& classNameStr) override;
		std::unordered_map<std::string, std::any> innerParam;
		
};

} // namespace XVisual

#endif // XBaseHandle_H
