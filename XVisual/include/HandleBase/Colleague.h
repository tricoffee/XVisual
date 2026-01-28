#ifndef Colleague_H
#define Colleague_H

#include <iostream>
#include "ColleagueType.h"
//#include "QUuid.h"

namespace XVisual {

class Colleague
{
	public:
		Colleague();
		virtual ~Colleague();
		ColleagueType getColleagueType();
		const std::string& getUuid() const;
		virtual void setUuid(const std::string& mUuid);
		const std::string& getClassName() const;
		const std::string& getUniqueName() const;
		virtual void setUniqueName(const std::string& mUniqueName);
		virtual void initParams() = 0;
		virtual void initOperands() = 0;
		virtual void xOperate() = 0;
	protected:
		virtual void createUniqueName(const std::string& classNameStr) = 0;
		virtual void createUuid() = 0;
		virtual void setColleagueType() = 0;
		virtual void setClassName(const std::string& classNameStr) = 0;
		ColleagueType colleagueType;
		std::string uuid;
		std::string className1;
		std::string uniqueName;
};

} // namespace XVisual

#endif // Colleague_H
