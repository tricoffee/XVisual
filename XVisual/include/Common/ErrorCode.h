#ifndef ErrorCode_H
#define ErrorCode_H
#include <iostream>

namespace XVisual
{
	enum class ErrorCode
	{
		Success = 0,
		FileNotFound = 1,
		UmapKeyNoFound = 2,
		UnImplementedError = 3,
		JsonParseFailed = 301,
		ParseClassInfoArrFromJsonFailed = 302,
		ParseClassNameFromJsonFailed = 303,
		ParseColleagueTypeFromJsonFailed = 304,
		ParseColleagueIdFromJsonFailed = 305,
		ParseSourceFromJsonFailed = 306,
		ParseInnerParamFromJsonFailed = 307,
		ParseInnerParamElementFailed = 308,
		ParseOuterParamFromJsonFailed = 309,
		ParseJsonUnknownObjectItemError = 310,
		ParseJsonUnknownError = 311,
		UnknownXVariableNameError = 501,
		UnknownPreParamPreTagError = 601,
	};

	/*
	函数将 ErrorCode 转换为对应的字符串。
	将函数定义为 inline，可以让编译器在每个使用函数的地方插入函数的代码，从而避免重复定义的问题。
	*/
	inline std::string errorCodeToStr(ErrorCode code)
	{
		switch (code) 
		{
			case ErrorCode::Success:
				return "Success";
			case ErrorCode::FileNotFound:
				return "FileNotFoundError";
			case ErrorCode::UmapKeyNoFound:
				return "UmapKeyNoFoundError";
			default:
				return "UnknownError";
		}
	}

}// namespace XVisual

#endif // ErrorCode_H