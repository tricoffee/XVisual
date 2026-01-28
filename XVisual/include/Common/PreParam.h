#ifndef PreParam_H
#define PreParam_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <any>

#define DefaultPreTag "Normalize01"

namespace XVisual {

struct PreParam
{
	//Preprocessor type
	std::string preTag = DefaultPreTag;
	//Parameters required for preprocessing, 
	//使用了一个立即执行的Lambda表达式来初始化params
	std::unordered_map<std::string, std::any> params = []() {
		std::unordered_map<std::string, std::any> mParams;
		mParams["scale"] = 1 / 255.0f;
		return mParams;
	}();
	//Parameters required for preprocessing inverse transformation
	std::unordered_map<std::string, std::any> decodeParams;
};

} // namespace XVisual

# endif // PreParam_H
