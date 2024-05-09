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
const std::string& Colleague::getClassName() const
{
	return className1;
}
const std::string& Colleague::getUniqueName() const
{
	return uniqueName;
}
void Colleague::setUniqueName(const std::string& mUniqueName)
{
	uniqueName = mUniqueName;
}