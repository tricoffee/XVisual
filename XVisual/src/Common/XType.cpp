#include "Common/XType.h"

std::pair<std::string, XType> Number2XType(int number)
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
		// ̎�픵�ֲ������e�е���r
		// �@�e�����x����e���O��Ĭ�Jֵ���߈�����������
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