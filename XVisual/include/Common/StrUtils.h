//
// Created by Liheng on 2024-04-05.
//

#ifndef STRUTILS_H
#define STRUTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <regex>

namespace XVisual {

std::string strip(const std::string& str);
std::string stripPrefix(const std::string& str, const std::string& prefix);
std::string stripSuffix(const std::string& str, const std::string& suffix);

template<typename T>
std::vector<T> parseShape(const std::string& shapeStr)
{
	std::vector<T> shape;
	std::regex re("-?\\d+"); // 匹配一个或多个数字（包括负数）
	std::sregex_iterator it(shapeStr.begin(), shapeStr.end(), re);
	std::sregex_iterator end;

	while (it != end)
	{
		shape.push_back(std::stoi(it->str()));
		++it;
	}

	return shape;
}
std::string extractSubstrBeforeDelimiter(std::string input, std::string delimiter);
std::string extractSubstrBetweenColons(const std::string& str);
void printShape(const std::string& str1);
bool startsWith(const std::string& str, const std::string& prefix);
std::string extractSubstrAfterDelimiter(std::string input, std::string delimiter);

} // namespace XVisual

#endif STRUTILS_H
