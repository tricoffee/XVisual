#ifndef XBaseHandle_H
#define XBaseHandle_H

#include <iostream>
#include <unordered_map>
#include "HandleFactory.h"
#include "ColleagueType.h"
#include "Colleague.h"
#include "XVariable/Source.h"
#include "XVariable/Dest.h"
#include "XVariable/XMacro.h"
#include "XVariable/SourceFrom.h"
#include "cJSON.h"
#include "Common/ErrorCode.h"

class XBaseItem;
class LoadImageItem;
class CVCropItem;

class XBaseHandle : public Colleague
{
	public:
		XBaseHandle();
		virtual ~XBaseHandle();
		Source& getSources();
		Dest& getDests();
		void setSources(Source mSources);
		void setDests(Dest mDests);
		virtual XVisual::ErrorCode setInnerParam(cJSON* innerParamPtr);
		virtual XVisual::ErrorCode setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap);
		virtual XVisual::ErrorCode writeOuterParam(cJSON* cjson_variableSource, const std::string& xName);
		virtual XVisual::ErrorCode writeInnerParam(cJSON* cjson_innerParam);
		const std::unordered_map<std::string, std::any>& getInnerParam() const;
		virtual void initParams();
		virtual void initOperands();
		virtual void xOperate();
		void setSourceFrom(const std::string& xVariableName, const SourceFrom& sourceFrom);
		void loadSourceFrom(const std::string& xVariableName, SourceFrom& sourceFrom);
		void setUuidConsistentWithItem(std::string xitemUuid);
		friend class XBaseItem;
		friend class LoadImageItem;
		friend class CVCropItem;
	protected:
		Source sources;
		Dest dests;
		void setColleagueType() override;
		void createUuid() override;
		void createUniqueName(const std::string& classNameStr) override;
		void setClassName(const std::string& classNameStr) override;
		std::unordered_map<std::string, std::any> innerParam;
		
};

#endif // XBaseHandle_H