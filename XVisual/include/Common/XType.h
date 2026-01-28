#ifndef XType_H
#define XType_H

#include <iostream>
#include <string>
#include <vector>

namespace XVisual {

enum class XType
{
	Unknown = 0,
	Str = 1,
	Arr = 2,
	CVRect = 3
};

std::pair<std::string, XType> EnumNumber2XType(int number);

} // namespace XVisual

#endif //XType_H
