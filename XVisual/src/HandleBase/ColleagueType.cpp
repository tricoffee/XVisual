#include "HandleBase/ColleagueType.h"

ColleagueType Number2ColleagueType(int number)
{
	ColleagueType cType = ColleagueType::NullType;
	switch (number)
	{
		case static_cast<int>(ColleagueType::NullType):
		case static_cast<int>(ColleagueType::ItemType):
		case static_cast<int>(ColleagueType::HandleType):
			cType = static_cast<ColleagueType>(number);
			break;
		default:
			// 處理數字不在列舉中的情況
			// 這裡可以選擇報錯、設置默認值或者執行其他操作
			cType = ColleagueType::NullType;
			break;
	}
	return cType;
};