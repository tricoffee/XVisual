#ifndef XParser_H
#define XParser_H

#include<iostream>
#include<string>

#include "CJSON/cJSON.h"

namespace XVisual {

class XParser
{
	public:
		static XParser& getInstance();
		~XParser();
		bool initParser(const std::string& filePath);
		bool getClassInfoArr(cJSON*& classInfoArr);
		bool getClassInfoItemAt(int i = 0);
		bool getClassInfoItemAt(cJSON*& classInfoElement, int i = 0);

		// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_ClassName for Item 
		bool getItemClassNameAt(cJSON*& itemClassName, int i);
		// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_ColleagueType for Item 
		bool getItemColleagueTypeAt(cJSON*& itemColleagueType, int i = 0);
		// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_ColleagueId for Item 
		bool getItemColleagueIdAt(cJSON*& itemColleagueId, int i = 0);
		// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_UniqueName for Item 
		bool getItemUniqueNameAt(cJSON*& itemUniqueName, int i = 0);

		// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_Position for Item 
		bool getItemPosition(cJSON*& itemPos, int i = 0);

		// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 ClassName for Handle  
		bool getClassNameAt(cJSON*& className, int i = 0); 
		// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 ColleagueType for Handle 
		bool getColleagueTypeAt(cJSON*& colleagueType, int i = 0);
		// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 ColleagueId for Handle 
		bool getColleagueIdAt(cJSON*& colleagueId, int i = 0);
		// 获取 ClassInfo 数组在索引 i 处的 classinfo_element 的 UniqueName for Handle 
		bool getUniqueNameAt(cJSON*& uniqueName, int i = 0);

		bool getSourceFromAt(cJSON*& sourceFrom, int i = 0);
		bool getInnerParamAt(cJSON*& innerParam, int i = 0);
	private:
		XParser();
		// json 文件文本内容的 const char*表示
		const char* json_data;
		// 解析 JSON 获得的根节点
		cJSON* root;
		// 获取 ClassInfo 数组
		cJSON* classinfo_array;
		// ClassInfo 数组在索引 i 处的 classinfo_element
		cJSON* classinfo_element;

		// ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_ClassName for Item
		cJSON* item_class_name;
		// ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_ColleagueType for Item 
		cJSON* item_colleague_type;
		// ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_ColleagueId for Item
		cJSON* item_colleague_id;
		// ClassInfo 数组在索引 i 处的 classinfo_element 的 Item_UniqueName for Item
		cJSON* item_unique_name;

		// ClassInfo 数组在索引 i 处的 classinfo_element 的 ClassName for Handle
		cJSON* class_name;
		// ClassInfo 数组在索引 i 处的 classinfo_element 的 ColleagueType for Handle
		cJSON* colleague_type;
		// ClassInfo 数组在索引 i 处的 classinfo_element 的 ColleagueId for Handle
		cJSON* colleague_id;
		// ClassInfo 数组在索引 i 处的 classinfo_element 的 UniqueName for Handle
		cJSON* unique_name;

		// ClassInfo 数组在索引 i 处的 classinfo_element 的 SourceFrom
		cJSON* source_from;
		// ClassInfo 数组在索引 i 处的 classinfo_element 的 InnerParam
		cJSON* inner_param;
};

} // namespace XVisual

#endif // XParser_H
