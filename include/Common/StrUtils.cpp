#include "Common/StrUtils.h"

/*
Strip off the space characters at the beginning and end,
How to use it, please refer to demo1 in StrUtilsDemo.cpp
*/
std::string strip(const std::string& str)
{
	size_t start = str.find_first_not_of(" \t\n\r");
	size_t end = str.find_last_not_of(" \t\n\r");
	if (start == std::string::npos || end == std::string::npos)
	{
		// 字符串全为空格
		return "";
	}
	else
	{
		return str.substr(start, end - start + 1);
	}
}

/*
Stripping the specified string at the beginning
How to use it, please refer to demo2 in StrUtilsDemo.cpp
*/
std::string stripPrefix(const std::string& str, const std::string& prefix)
{
	size_t pos = str.find(prefix);
	if (pos == std::string::npos)
	{
		return str; // 如果未找到前缀，则返回原始字符串
	}
	return str.substr(pos + prefix.length()); // 返回去除前缀后的子字符串
}

/*
Extract substring before delimiter at the beginning
How to use it, please refer to demo3 in StrUtilsDemo.cpp
*/
std::string extractSubstrBeforeDelimiter(std::string input, std::string delimiter)
{
	size_t pos = input.find(delimiter);
	if (pos != std::string::npos)
	{
		return input.substr(0, pos);
	}
	else
	{
		return input;  // 如果找不到分隔符，返回整个字符串
	}
}

/*
Extract substring between two colons
How to use it, please refer to demo4 in StrUtilsDemo.cpp
*/
std::string extractSubstrBetweenColons(const std::string& str)
{
	size_t start = str.find(":") + 1; // 查找第一个冒号的位置，加1是为了跳过冒号本身
	size_t end = str.rfind(":"); // 查找最后一个冒号的位置
	if (start == std::string::npos || end == std::string::npos || start >= end)
	{
		return ""; // 如果没有找到两个冒号或者它们的顺序不正确，则返回空字符串
	}
	return str.substr(start, end - start); // 提取两个冒号之间的子字符串
}

void printShape(const std::string& str1)
{
	std::vector<int> shape1 = parseShape<int>(str1);
	int size1 = shape1.size();
	std::cout << "Parsed shape for \"" << str1 << "\": [";
	int i = 0;
	for (int num : shape1)
	{
		++i;
		std::cout << num;
		if (i != size1)
		{
			std::cout << ", ";
		}
	}
	std::cout << "]" << std::endl;
}

bool startsWith(const std::string& str, const std::string& prefix)
{
	return str.find(prefix) == 0;
}

std::string extractSubstrAfterDelimiter(std::string input, std::string delimiter)
{
	int strLen = input.size();
	size_t pos = input.find(delimiter);
	if (pos != std::string::npos)
	{
		return input.substr(pos + 1, strLen - pos);
	}
	else
	{
		return input;  // 如果找不到分隔符，返回整个字符串
	}
}
