#include "XInfer.h"
#include "Common/StrUtils.h"
#include "XGraph/XGraph.h"
#include "GlobalStorage/HandleGlobalStorage.h"
#include "GlobalStorage/GlobalVariable.h"
#include "Common/FileUtils.h"

#define CHECK_NULLPTR(ptr) ((ptr) == nullptr)

XInfer& XInfer::getInstance()
{
	static XInfer infer;
	return infer;
}
XInfer::XInfer():parser(XParser::getInstance())
{
	xHandle = nullptr;
	classInfoArr = nullptr;
}
XInfer::~XInfer() 
{ 

}
XVisual::ErrorCode XInfer::init(const std::string& filePath)
{
	
	bool jsonInitFlag = parser.initParser(filePath);
	if (jsonInitFlag)
	{
		// update globalWorkSpaceDir
		getParentPathStr(filePath, globalWorkSpaceDir);
		return XVisual::ErrorCode::Success;
	}
	else
	{
		XLOG_INFO("XInfer::init, jsonInitFlag Failed ", CURRENT_THREAD_ID);
		return XVisual::ErrorCode::JsonParseFailed;
	}
}
XParser& XInfer::getParser()
{
	return parser;
}

XVisual::ErrorCode XInfer::build()
{
	// Done, parse classInfoArr from classinfo_array;
	parser.getClassInfoArr(classInfoArr);
	if (!classInfoArr || !cJSON_IsArray(classInfoArr))
	{
		//printf("Failed to get 'ClassInfo' array.\n");
		//flag = false;
		return XVisual::ErrorCode::ParseClassInfoArrFromJsonFailed;
	}
	else
	{
		int arrSize = cJSON_GetArraySize(classInfoArr);
		XLOG_INFO("== === ===  classInfoArr.size() " + std::to_string(arrSize) + " === === ===", CURRENT_THREAD_ID);
		// Done, iterate XInfer::constructHandle over classInfoArr 
		// iterate over classInfoArr 
		for (int i = 0; i < cJSON_GetArraySize(classInfoArr); i++)
		{
			//cJSON* classinfoItem = nullptr;
			//parser.getClassInfoItemAt(classinfoItem, i);

			// get className
			std::string classNameStr = NullClassNameStr;
			cJSON* className = nullptr;
			parser.getClassNameAt(className, i);
			if (cJSON_IsString(className) && (className->valuestring != NULL))
			{
				//printf("ClassName: %s\n", className->valuestring);
				classNameStr = className->valuestring;
			}
			else
			{
				return XVisual::ErrorCode::ParseClassNameFromJsonFailed;
			}
			data.className = classNameStr;
			XLOG_INFO("*** *** *** data.className = " + data.className + " *** *** *** \n", CURRENT_THREAD_ID);

			// get colleagueType
			ColleagueType cType = ColleagueType::NullType;
			cJSON* colleagueType = nullptr;
			parser.getColleagueTypeAt(colleagueType, i);
			if (cJSON_IsNumber(colleagueType))
			{
				//printf("ColleagueType: %d\n", colleagueType->valueint);
				int vInt = colleagueType->valueint;
				cType = Number2ColleagueType(vInt);
			}
			else
			{
				return XVisual::ErrorCode::ParseColleagueTypeFromJsonFailed;
			}
			data.colleagueType = cType;
			std::string cTypeStr = std::to_string(static_cast<int>(data.colleagueType));
			XLOG_INFO("*** *** *** data.colleagueType = " + cTypeStr + " *** *** *** \n", CURRENT_THREAD_ID);

			// get ColleagueId
			std::string colleagueIdStr = NullColleagueId;
			cJSON* colleagueId = nullptr;
			parser.getColleagueIdAt(colleagueId, i);
			if (cJSON_IsString(colleagueId) && (colleagueId->valuestring != NULL))
			{
				//printf("ColleagueId: %s\n", colleagueId->valuestring);
				colleagueIdStr = colleagueId->valuestring;
			}
			else
			{
				return XVisual::ErrorCode::ParseColleagueIdFromJsonFailed;
			}
			data.colleagueId = colleagueIdStr;
			XLOG_INFO("*** *** *** data.colleagueId = " + data.colleagueId + " *** *** *** \n", CURRENT_THREAD_ID);


			data.outerParamUMap.clear();
			data.sourceFromUMap.clear();

			// get sourceFromUMap
			cJSON* sourceFromPtr = nullptr;
			parser.getSourceFromAt(sourceFromPtr, i);
			if (cJSON_IsObject(sourceFromPtr))
			{
				// 遍历 SourceFrom 的每个节点
				cJSON* source_node = NULL;
				cJSON_ArrayForEach(source_node, sourceFromPtr)
				{
					const char* source_node_name = source_node->string;
					XLOG_INFO("*** *** *** source_node_name = " + std::string(source_node_name) + " *** *** *** \n", CURRENT_THREAD_ID);
					//printf("source_node_name = %s\n", source_node_name);
					cJSON* source_obj = cJSON_GetObjectItem(sourceFromPtr, source_node_name);
					if (source_obj)
					{
						cJSON* yId = cJSON_GetObjectItemCaseSensitive(source_obj, "yId");
						cJSON* yName = cJSON_GetObjectItemCaseSensitive(source_obj, "yName");
						cJSON* yOuterParam = cJSON_GetObjectItemCaseSensitive(source_obj, "outerParam");

						data.outerParamUMap[source_node_name] = yOuterParam;

						if (cJSON_IsString(yId) && cJSON_IsString(yName))
						{
							XLOG_INFO("*** *** *** yId = " + std::string(yId->valuestring) + " *** *** *** \n", CURRENT_THREAD_ID);
							XLOG_INFO("*** *** *** yName = " + std::string(yName->valuestring) + " *** *** *** \n", CURRENT_THREAD_ID);
							//printf("SourceFrom (Name): %s, yId=%s, yName=%s\n", source_node_name, yId->valuestring, yName->valuestring);
							SourceFrom sourceFrom;
							sourceFrom.itemId = yId->valuestring;
							sourceFrom.variableName = yName->valuestring;
							data.sourceFromUMap[source_node_name] = sourceFrom;
							if (!sourceFrom.itemId.empty())
							{
								idLink[sourceFrom.itemId].insert(colleagueIdStr);
							}
						}
						else
						{
							return  XVisual::ErrorCode::ParseSourceFromJsonFailed;
						}
				
					}
				}
			}

			XVisual::ErrorCode constructResult = XInfer::constructHandle(i); // i is the index of classInfoArr
			
			if (XVisual::ErrorCode::Success != constructResult)
			{
				XLOG_INFO("*** *** *** constructResult is FAILED *** *** *** \n", CURRENT_THREAD_ID);
				return constructResult;
			}

		}

		
		for (const auto& e : idLink)
		{
			std::string startStr = e.first;
			std::set<std::string> endStrList = e.second;
			// 在 xHandleGraph 里面检索节点 startStr 对应的索引 startIndex
			int startIndex = XGraph::findNodeIndex(xHandleGraph, startStr);
			for (const auto& endStr : endStrList)
			{
				// 在 xHandleGraph 里面检索节点 endStr 对应的索引endIndex
				int endIndex = XGraph::findNodeIndex(xHandleGraph, endStr);
				// 把xHandleGraph[endIndex]设置为xHandleGraph[startIndex]的邻节点, 添加边, 标识为yHandleId的节点---》指向---》标识为xHandleId的节点(即getUuid()的返回值)的节点, 这个逻辑应该紧跟连线动作释放之后
				xHandleGraph[startIndex]->neighbors.push_back(xHandleGraph[endIndex]);
			}

		}

		return XVisual::ErrorCode::Success;
	}
}

XVisual::ErrorCode XInfer::constructHandle(int i)
{
	//std::cout << "data.className = " << data.className << std::endl;
	XLOG_INFO("*** *** *** data.className = " + data.className + "*** *** *** \n", CURRENT_THREAD_ID);
	std::string classNameStr = stripSuffix(data.className, "Handle");
	//std::cout << "classNameStr = " << classNameStr << std::endl;
	XLOG_INFO("*** *** *** classNameStr = " + classNameStr + "*** *** *** \n", CURRENT_THREAD_ID);

	//TODO, check xHandle->getColleagueType == data.colleagueType

	xHandle = HandleRegistry::createObject(classNameStr);
	xHandle->setUuid(data.colleagueId);


	//Done, Write outerFrom into hanlde
	XVisual::ErrorCode eCode1 = xHandle->setOuterParam(data.outerParamUMap);
	if (eCode1 != XVisual::ErrorCode::Success)
	{
		XLOG_INFO("*** *** *** xHandle->setOuterParam is FAILED *** *** *** \n", CURRENT_THREAD_ID);
		return eCode1;
	}

	//Done, Write innerFrom into handle
	cJSON* innerParam = nullptr;
	parser.getInnerParamAt(innerParam, i);

	// 错误代码不用添加这些
	//if (CHECK_NULLPTR(innerParam))
	//{
	//	return XVisual::ErrorCode::ParseInnerParamFromJsonFailed;
	//}

	data.innerParam = innerParam;
	XVisual::ErrorCode eCode2 = xHandle->setInnerParam(data.innerParam);
	if (eCode2 != XVisual::ErrorCode::Success)
	{
		return eCode2;
	}

	//Done, Write sourceFrom into handle
	for (const auto& pair : data.sourceFromUMap)
	{
		XLOG_INFO("== === ===  pair.first " + pair.first + " === === ===", CURRENT_THREAD_ID);

		xHandle->setSourceFrom(pair.first, pair.second);
	}

	globalHandleMap[data.colleagueId] = xHandle;
	xHandleGraph.push_back(std::make_shared<GraphNode>(data.colleagueId));

	//for (const auto& e : globalHandleMap)
	//{
	//	XLOG_INFO("== === === data.colleagueId = " + e.first + " === === ===", CURRENT_THREAD_ID);
	//}

	return XVisual::ErrorCode::Success;
}

XVisual::ErrorCode XInfer::exec()
{
	XGraph::executeXGraph<XBaseHandle>(globalHandleMap, xHandleGraph);
	return XVisual::ErrorCode::Success;
}

XVisual::ErrorCode XInfer::getResult(std::any& result)
{
	std::string nId = nodeIdTopoSequence.back();
	XLOG_INFO("*** *** *** handleId = " + nId + "*** *** *** \n", CURRENT_THREAD_ID);
	auto it = globalHandleMap.find(nId);
	if (it != globalHandleMap.end())
	{
		XBaseHandle* iElement = it->second;
		Dest& d = iElement->getDests();
		result = GET_MEMBER_WITH_TYPE_STR(d, cv::Mat, "resultImage");
	}
	return XVisual::ErrorCode::Success;
}