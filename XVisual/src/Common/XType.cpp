#include "Common/XType.h"

namespace XVisual {
	std::pair<std::string, XType> EnumNumber2XType(int number)
	{
		std::string typeStr = "XType::Unknown";
		XType cType = XType::Unknown;
		switch (number)
		{
		case static_cast<int>(XType::Str):
			typeStr = "XType::Str";
			std::cout << typeStr << std::endl;
			cType = static_cast<XType>(number);
			break;
		case static_cast<int>(XType::Arr):
			typeStr = "XType::Arr";
			std::cout << typeStr << std::endl;
			cType = static_cast<XType>(number);
			break;
		case static_cast<int>(XType::CVRect):
			typeStr = "XType::CVRect";
			std::cout << typeStr << std::endl;
			cType = static_cast<XType>(number);
			break;
		default:
			// 處理數字不在列舉中的情況
			// 這裡可以選擇報錯、設置默認值或者執行其他操作
			typeStr = "XType::Unknown";
			std::cout << typeStr << std::endl;
			cType = XType::Unknown;
			break;
		}
		std::pair<std::string, XType> xPair;
		xPair.first = typeStr;
		xPair.second = cType;
		return xPair;
	};
} //namespace XVisual