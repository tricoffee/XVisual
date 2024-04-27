#ifndef Colleague_H
#define Colleague_H

#include <iostream>
#include "ColleagueType.h"
#include "QUuid.h"

class Colleague
{
	public:
		Colleague();
		virtual ~Colleague();
		ColleagueType getColleagueType();
		const std::string& getUuid() const;
		virtual void setUuid(const std::string& mUuid);
	protected:
		virtual void createUuid() = 0;
		virtual void setColleagueType() = 0;
		ColleagueType colleagueType;
		std::string uuid;
};

#endif // Colleague_H