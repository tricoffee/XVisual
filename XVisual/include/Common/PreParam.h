#ifndef PreParam_H
#define PreParam_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <any>

#define DefaultPreTag "Normalize01"

struct PreParam
{
	//Preprocessor type
	std::string preTag = DefaultPreTag;
	//Parameters required for preprocessing, 
	//ʹ����һ������ִ�е�Lambda���ʽ����ʼ��params
	std::unordered_map<std::string, std::any> params = []() {
		std::unordered_map<std::string, std::any> mParams;
		mParams["scale"] = 1 / 255.0f;
		return mParams;
	}();
	//Parameters required for preprocessing inverse transformation
	std::unordered_map<std::string, std::any> decodeParams;
};

# endif // PreParam_H