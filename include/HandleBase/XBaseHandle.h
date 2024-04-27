#ifndef XBaseHandle_H
#define XBaseHandle_H

#include <iostream>
#include "HandleFactory.h"
#include "ColleagueType.h"
#include "Colleague.h"
#include "XVariable/Source.h"
#include "XVariable/Dest.h"
#include "XVariable/XMacro.h"
#include "XVariable/SourceFrom.h"

class XBaseItem;
class CVCropItem;
class LoadImageItem;

class XBaseHandle : public Colleague
{
	public:
		XBaseHandle();
		virtual ~XBaseHandle();
		Source& getSources();
		Dest& getDests();
		void setSources(Source mSources);
		void setDests(Dest mDests);
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
		void createUuid();
};

#endif // XBaseHandle_H