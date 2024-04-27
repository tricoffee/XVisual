#include "HandleBase/Colleague.h"

Colleague::Colleague()
{
	std::cout << " === class Colleague === " << std::endl;
}
Colleague::~Colleague() {}
ColleagueType Colleague::getColleagueType()
{
	return colleagueType;
}
const std::string& Colleague::getUuid() const
{
	return uuid;
}
void Colleague::setUuid(const std::string& mUuid)
{
	uuid = mUuid;
}
