#ifndef XParser_H
#define XParser_H

#include<iostream>
#include<string>

#include "cJSON.h"
class XParser
{
	public:
		static XParser& getInstance();
		~XParser();
		bool initParser(const std::string& filePath);
		bool getClassInfoArr(cJSON*& classInfoArr);
		bool getClassInfoItemAt(int i = 0);
		bool getClassInfoItemAt(cJSON*& classInfoElement, int i = 0);

		// ��ȡ ClassInfo ���������� i ���� classinfo_element �� Item_ClassName for Item 
		bool getItemClassNameAt(cJSON*& itemClassName, int i);
		// ��ȡ ClassInfo ���������� i ���� classinfo_element �� Item_ColleagueType for Item 
		bool getItemColleagueTypeAt(cJSON*& itemColleagueType, int i = 0);
		// ��ȡ ClassInfo ���������� i ���� classinfo_element �� Item_ColleagueId for Item 
		bool getItemColleagueIdAt(cJSON*& itemColleagueId, int i = 0);
		// ��ȡ ClassInfo ���������� i ���� classinfo_element �� Item_UniqueName for Item 
		bool getItemUniqueNameAt(cJSON*& itemUniqueName, int i = 0);

		// ��ȡ ClassInfo ���������� i ���� classinfo_element �� Item_Position for Item 
		bool getItemPosition(cJSON*& itemPos, int i = 0);

		// ��ȡ ClassInfo ���������� i ���� classinfo_element �� ClassName for Handle  
		bool getClassNameAt(cJSON*& className, int i = 0); 
		// ��ȡ ClassInfo ���������� i ���� classinfo_element �� ColleagueType for Handle 
		bool getColleagueTypeAt(cJSON*& colleagueType, int i = 0);
		// ��ȡ ClassInfo ���������� i ���� classinfo_element �� ColleagueId for Handle 
		bool getColleagueIdAt(cJSON*& colleagueId, int i = 0);
		// ��ȡ ClassInfo ���������� i ���� classinfo_element �� UniqueName for Handle 
		bool getUniqueNameAt(cJSON*& uniqueName, int i = 0);

		bool getSourceFromAt(cJSON*& sourceFrom, int i = 0);
		bool getInnerParamAt(cJSON*& innerParam, int i = 0);
	private:
		XParser();
		// json �ļ��ı����ݵ� const char*��ʾ
		const char* json_data;
		// ���� JSON ��õĸ��ڵ�
		cJSON* root;
		// ��ȡ ClassInfo ����
		cJSON* classinfo_array;
		// ClassInfo ���������� i ���� classinfo_element
		cJSON* classinfo_element;

		// ClassInfo ���������� i ���� classinfo_element �� Item_ClassName for Item
		cJSON* item_class_name;
		// ClassInfo ���������� i ���� classinfo_element �� Item_ColleagueType for Item 
		cJSON* item_colleague_type;
		// ClassInfo ���������� i ���� classinfo_element �� Item_ColleagueId for Item
		cJSON* item_colleague_id;
		// ClassInfo ���������� i ���� classinfo_element �� Item_UniqueName for Item
		cJSON* item_unique_name;

		// ClassInfo ���������� i ���� classinfo_element �� ClassName for Handle
		cJSON* class_name;
		// ClassInfo ���������� i ���� classinfo_element �� ColleagueType for Handle
		cJSON* colleague_type;
		// ClassInfo ���������� i ���� classinfo_element �� ColleagueId for Handle
		cJSON* colleague_id;
		// ClassInfo ���������� i ���� classinfo_element �� UniqueName for Handle
		cJSON* unique_name;

		// ClassInfo ���������� i ���� classinfo_element �� SourceFrom
		cJSON* source_from;
		// ClassInfo ���������� i ���� classinfo_element �� InnerParam
		cJSON* inner_param;
};

#endif // XParser_H
