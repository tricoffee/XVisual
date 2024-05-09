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
			// ̎�픵�ֲ������e�е���r
			// �@�e�����x����e���O��Ĭ�Jֵ���߈�����������
			cType = ColleagueType::NullType;
			break;
	}
	return cType;
};