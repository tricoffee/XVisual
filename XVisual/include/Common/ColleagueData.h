#ifndef ColleagueData_H
#define ColleagueData_H

#include <iostream>
#include <string>
#include <unordered_map>
#include "CJSON/cJSON.h"
#include "HandleBase/ColleagueType.h"
#include "XVariable/SourceFrom.h"
#include "Common/ColleagueNULLValueMacro.h"

namespace XVisual {

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
	// 因为每个Handle的某个source的某个variableName的outerParam是不同的, 比如有的是std::string, 有的是cv::Mat, 有的是std::vector, 所以用cJSON*维护比用std::unordered_map维护更合适, 细粒度的解析逻辑交给具体的Handle类来处理
	
	//cJSON* outerParam; // Decrapted
	std::unordered_map<std::string, cJSON*> outerParamUMap;

	// 因为每个Handle的innerParam是不同的, 比如有的是roi, 有的是threshold, 有的是alpha, 所以用cJSON*维护比用std::unordered_map维护更合适, 细粒度的解析逻辑交给具体的Handle类来处理
	cJSON* innerParam;
};

} // namespace XVisual

#endif // ColleagueData_H
