#ifndef ColleagueData_H
#define ColleagueData_H

#include <iostream>
#include <string>
#include <unordered_map>
#include "CJSON/cJSON.h"
#include "HandleBase/ColleagueType.h"
#include "XVariable/SourceFrom.h"
#include "Common/ColleagueNULLValueMacro.h"

struct ColleagueData
{
	std::string itemClassName = NullClassNameStr;
	ColleagueType itemColleagueType = ColleagueType::NullType;
	std::string itemColleagueId = NullColleagueId;
	std::string itemUniqueName = NullUniqueName;

	std::pair<int, int> itemPos = std::pair<int, int>(0, 0);

	std::string className = NullClassNameStr;
	ColleagueType colleagueType = ColleagueType::NullType;
	std::string colleagueId = NullColleagueId;
	std::string uniqueName = NullUniqueName;

	std::unordered_map<std::string, SourceFrom> sourceFromUMap;
	// ��Ϊÿ��Handle��ĳ��source��ĳ��variableName��outerParam�ǲ�ͬ��, �����е���std::string, �е���cv::Mat, �е���std::vector, ������cJSON*ά������std::unordered_mapά��������, ϸ���ȵĽ����߼����������Handle��������
	
	//cJSON* outerParam; // Decrapted
	std::unordered_map<std::string, cJSON*> outerParamUMap;

	// ��Ϊÿ��Handle��innerParam�ǲ�ͬ��, �����е���roi, �е���threshold, �е���alpha, ������cJSON*ά������std::unordered_mapά��������, ϸ���ȵĽ����߼����������Handle��������
	cJSON* innerParam;
};


#endif // ColleagueData_H
