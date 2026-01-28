#include "Common/XParser.h"
#include "Common/JsonFileUtils.h"

#define CHECK_NULLPTR(ptr) ((ptr) == nullptr)
#define CHECK_ObjectItem(object) ((object != nullptr) && ((object)->type & cJSON_Object))

namespace XVisual {
	XParser& XParser::getInstance()
	{
		static XParser xParser;
		return xParser;
	}
	XParser::XParser()
	{
		json_data = nullptr;
		// 解析 JSON 获得的根节点
		root = nullptr;
		// 获取 ClassInfo 数组
		classinfo_array = nullptr;
		// ClassInfo 数组在索引 i 处的 classinfo_element

		classinfo_element = nullptr;
		// ClassInfo 数组在索引 i 处的 classinfo_element 的 ClassName
		class_name = nullptr;
		// ClassInfo 数组在索引 i 处的 classinfo_element 的 ColleagueType
		colleague_type = nullptr;
		// ClassInfo 数组在索引 i 处的 classinfo_element 的 ColleagueId

		colleague_id = nullptr;
		// ClassInfo 数组在索引 i 处的 classinfo_element 的 SourceFrom
		source_from = nullptr;
		// ClassInfo 数组在索引 i 处的 classinfo_element 的 InnerParam
		inner_param = nullptr;
	}
	XParser::~XParser()
	{
		cJSON_Delete(root);
	}
	bool XParser::initParser(const std::string& filePath)
	{
		json_data = readJsonFile(filePath.c_str());
		// 解析 JSON
		root = cJSON_Parse(json_data);
		if (CHECK_NULLPTR(root))
		{
			//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
			return false;
		}
		else
		{
			return true;
		}
	}
	// 获取 ClassInfo 数组
	bool XParser::getClassInfoArr(cJSON*& classInfoArr)
	{
		classinfo_array = cJSON_GetObjectItemCaseSensitive(root, "ClassInfo");
		classInfoArr = classinfo_array;
		if (CHECK_NULLPTR(classinfo_array) || !cJSON_IsArray(classinfo_array))
		{
			//printf("Failed to get 'ClassInfo' array.\n");
			cJSON_Delete(root);
			return false;
		}
		else
		{
			return true;
		}
	}
	// 获取 ClassInfo 数组在索引 i 处的 classinfo_element
	bool XParser::getClassInfoItemAt(int i)
	{
		classinfo_element = cJSON_GetArrayItem(classinfo_array, i);
		if (CHECK_ObjectItem(classinfo_element))
		{
			//printf("Is ObjectItem \n");
			return true;
		}
		else
		{
			//printf("NOT ObjectItem \n");
			return false;
		}
	}
	// 获取 ClassInfo 数组在索引 i 处的 classinfo_element
	bool XParser::getClassInfoItemAt(cJSON*& classInfoElement, int i)
	{
		classinfo_element = cJSON_GetArrayItem(classinfo_array, i);
		classInfoElement = classinfo_element;
		if (CHECK_ObjectItem(classinfo_element))
		{
			//printf("Is ObjectItem \n");
			return true;
		}
		else
		{
			//printf("NOT ObjectItem \n");
			return false;
		}
	}


	// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_ClassName for Item 
	bool XParser::getItemClassNameAt(cJSON*& itemClassName, int i)
	{
		getClassInfoItemAt(i);
		item_class_name = cJSON_GetObjectItemCaseSensitive(classinfo_element, "Item_ClassName");
		itemClassName = item_class_name;
		if (cJSON_IsString(item_class_name) && (item_class_name->valuestring != NULL))
		{
			//printf("ClassName: %s\n", item_class_name->valuestring);
			return true;
		}
		else
		{
			return false;
		}
	}
	// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_ColleagueType for Item
	bool XParser::getItemColleagueTypeAt(cJSON*& itemColleagueType, int i)
	{
		getClassInfoItemAt(i);
		item_colleague_type = cJSON_GetObjectItemCaseSensitive(classinfo_element, "Item_ColleagueType");
		itemColleagueType = item_colleague_type;
		if (cJSON_IsNumber(item_colleague_type))
		{
			//printf("ColleagueType: %d\n", item_colleague_type->valueint);
			return true;
		}
		else
		{
			return false;
		}
	}
	// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_ColleagueId for Item 
	bool XParser::getItemColleagueIdAt(cJSON*& itemColleagueId, int i)
	{
		getClassInfoItemAt(i);
		item_colleague_id = cJSON_GetObjectItemCaseSensitive(classinfo_element, "Item_ColleagueId");
		itemColleagueId = item_colleague_id;
		if (cJSON_IsString(item_colleague_id) && (item_colleague_id->valuestring != NULL))
		{
			//printf("ColleagueId: %s\n", item_colleague_id->valuestring);
			return true;
		}
		else
		{
			return false;
		}
	}
	// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_UniqueName for Item 
	bool XParser::getItemUniqueNameAt(cJSON*& itemUniqueName, int i)
	{
		getClassInfoItemAt(i);
		item_unique_name = cJSON_GetObjectItemCaseSensitive(classinfo_element, "Item_UniqueName");
		itemUniqueName = item_unique_name;
		if (cJSON_IsString(item_unique_name) && (item_unique_name->valuestring != NULL))
		{
			//printf("ColleagueId: %s\n", item_unique_name->valuestring);
			return true;
		}
		else
		{
			return false;
		}
	}


	bool XParser::getItemPosition(cJSON*& itemPos, int i)
	{
		itemPos = cJSON_GetObjectItemCaseSensitive(classinfo_element, "Item_Position");
		if (cJSON_IsArray(itemPos))
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 ClassName for Handle
	bool XParser::getClassNameAt(cJSON*& className, int i)
	{
		getClassInfoItemAt(i);
		class_name = cJSON_GetObjectItemCaseSensitive(classinfo_element, "ClassName");
		className = class_name;
		if (cJSON_IsString(class_name) && (class_name->valuestring != NULL))
		{
			//printf("ClassName: %s\n", class_name->valuestring);
			return true;
		}
		else
		{
			return false;
		}
	}
	// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 ColleagueType for Handle 
	bool XParser::getColleagueTypeAt(cJSON*& colleagueType, int i)
	{
		getClassInfoItemAt(i);
		colleague_type = cJSON_GetObjectItemCaseSensitive(classinfo_element, "ColleagueType");
		colleagueType = colleague_type;
		if (cJSON_IsNumber(colleague_type))
		{
			//printf("ColleagueType: %d\n", colleague_type->valueint);
			return true;
		}
		else
		{
			return false;
		}
	}
	// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 ColleagueId for Handle 
	bool XParser::getColleagueIdAt(cJSON*& colleagueId, int i)
	{
		getClassInfoItemAt(i);
		colleague_id = cJSON_GetObjectItemCaseSensitive(classinfo_element, "ColleagueId");
		colleagueId = colleague_id;
		if (cJSON_IsString(colleague_id) && (colleague_id->valuestring != NULL))
		{
			//printf("ColleagueId: %s\n", colleague_id->valuestring);
			return true;
		}
		else
		{
			return false;
		}
	}
	// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 UniqueName for Handle 
	bool XParser::getUniqueNameAt(cJSON*& uniqueName, int i)
	{
		getClassInfoItemAt(i);
		unique_name = cJSON_GetObjectItemCaseSensitive(classinfo_element, "UniqueName");
		uniqueName = unique_name;
		if (cJSON_IsString(unique_name) && (unique_name->valuestring != NULL))
		{
			//printf("ColleagueId: %s\n", unique_name->valuestring);
			return true;
		}
		else
		{
			return false;
		}
	}


	// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 SourceFrom for Handle
	bool XParser::getSourceFromAt(cJSON*& sourceFrom, int i)
	{
		getClassInfoItemAt(i);
		source_from = cJSON_GetObjectItemCaseSensitive(classinfo_element, "SourceFrom");
		sourceFrom = source_from;
		if (CHECK_ObjectItem(source_from))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 InnerParam for Handle 
	bool XParser::getInnerParamAt(cJSON*& innerParam, int i)
	{
		getClassInfoItemAt(i);
		inner_param = cJSON_GetObjectItemCaseSensitive(classinfo_element, "InnerParam");
		innerParam = inner_param;
		if (CHECK_ObjectItem(inner_param))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
} //namespace XVisual