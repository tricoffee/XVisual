#ifndef XType_H
#define XType_H

#include <iostream>
#include <string>
#include <vector>

enum class XType
{
	Unknown = 0,
	Str = 1,
	Arr = 2,
	CVRect = 3
};

std::pair<std::string, XType> Number2XType(int number);

#endif //XType_H