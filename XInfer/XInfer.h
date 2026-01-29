#ifndef XInfer_H
#define XInfer_H

#include "Common/XParser.h"
#include <vector>
#include "XGraph/GraphNode.h"
#include <unordered_map>
#include "HandleBase/XBaseHandle.h"
#include "Common/ErrorCode.h"
#include "HandleBase/ColleagueType.h"
#include <any>
#include "Common/ColleagueData.h"
#include "Common/ColleagueNullValueMacro.h"

class XInfer 
{
	public:
		static XInfer& getInstance();
		~XInfer();
		XVisual::ErrorCode init(const std::string& filePath);
		XParser& getParser();
		XVisual::ErrorCode build();
		XVisual::ErrorCode constructHandle(int i);// change visited Flag from "public" to "private"
		XVisual::ErrorCode exec();
		XVisual::ErrorCode getResult(std::any& result);
	private:

		XInfer();
		XParser parser; 
		//std::unordered_map<std::string, std::string> idLink;
		//idLink维系每个handle节点的孩子节点
		std::unordered_map<std::string, std::set<std::string>> idLink;
		ColleagueData data;
		XBaseHandle* xHandle;
		cJSON* classInfoArr;
};

#endif // XInfer_H
