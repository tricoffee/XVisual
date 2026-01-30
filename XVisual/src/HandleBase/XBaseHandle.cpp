#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "HandleBase/XBaseHandle.h"
#include "GlobalStorage/HandleGlobalStorage.h"
#include "Exception/UmapKeyNoFoundException.h"
#include "Common/StrUtils.h"
#include "Common/UuidGenerator.h"
#include "GlobalStorage/HandleManager.h"
#include <stop_token>

namespace XVisual {

	XBaseHandle::XBaseHandle()
		: Colleague()
		, sources(std::make_unique<VarBag>())
		, dests(std::make_unique<VarBag>())
	{
		//std::cout << "class XBaseHandle: public Colleague " << std::endl;
		XLOG_INFO("class XBaseHandle: public Colleague ...", CURRENT_THREAD_ID);
		setColleagueType();
		std::string classNameStr = "XBase";
		setClassName(classNameStr);
		createUniqueName(classNameStr);
	}
	XBaseHandle::~XBaseHandle()
	{

	}
	void XBaseHandle::setClassName(const std::string& classNameStr)
	{
		className1 = classNameStr + "Handle";
	}
	void XBaseHandle::setColleagueType()
	{
		colleagueType = ColleagueType::HandleType;
	}
	void XBaseHandle::createUuid()
	{
		//QUuid id = QUuid::createUuid();
		//uuid = "Handle_" + id.toString().toStdString();

		uuid = "Handle_" + generateUUID();
	}
	void XBaseHandle::createUniqueName(const std::string& classNameStr)
	{
		uniqueName = HandleManager::instance().getUniqueItemName(classNameStr);
	}
	void XBaseHandle::setUuidConsistentWithItem(std::string xitemUuid)
	{
		std::string delimiter = "_";
		std::string uuidStr2 = extractSubstrAfterDelimiter(xitemUuid, delimiter);
		const std::string handleUuid = "Handle_" + uuidStr2;
		//Here, uuid is the protected member variable of the XHandle class
		this->uuid = handleUuid;
	}
	VarBag& XBaseHandle::getSources()
	{
		return *sources;
	}
	VarBag& XBaseHandle::getDests()
	{
		return *dests;
	}
	XVisual::ErrorCode XBaseHandle::setInnerParam(cJSON* innerParamPtr)
	{
		return XVisual::ErrorCode::UnImplementedError;
	}
	XVisual::ErrorCode XBaseHandle::setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap)
	{
		return XVisual::ErrorCode::UnImplementedError;
	}
	XVisual::ErrorCode XBaseHandle::writeOuterParam(cJSON* cjson_variableSource, const std::string& xName)
	{
		return XVisual::ErrorCode::UnImplementedError;
	}
	XVisual::ErrorCode XBaseHandle::writeInnerParam(cJSON* cjson_innerParam)
	{
		return XVisual::ErrorCode::UnImplementedError;
	}
	const std::unordered_map<std::string, std::any>& XBaseHandle::getInnerParam() const
	{
		return innerParam;
	}
	void XBaseHandle::initParams()
	{

	}
	void XBaseHandle::xOperate()
	{
	}

	void XBaseHandle::execute(std::stop_token st)
	{
		// 默认实现：先初始化输入参数，再执行计算
		initOperands();
		xOperate();
	}
	/*
	从sourceFrom查找sources里面的每个参数的来源并且初始化该参数
	*/
	void XBaseHandle::initOperands()
	{
		XLOG_INFO("XBaseHandle::initItemOperands ...... ", CURRENT_THREAD_ID);
		std::vector<std::string> xVaribleNames = ACQUIRE_NAMES((*sources));
		int num = xVaribleNames.size();
		XLOG_INFO("XBaseHandle::initItemOperands,xVaribleNames.size() = " + std::to_string(num), CURRENT_THREAD_ID);
		for (const auto& xName : xVaribleNames)
		{
			XLOG_INFO("XBaseHandle::initItemOperands: " + xName, CURRENT_THREAD_ID);
			SourceFrom sourceFrom;
			try
			{
				loadSourceFrom(xName, sourceFrom);
			}
			catch (UmapKeyNoFoundException& e)
			{
				//        std::cout << "=== === ===" << e.what() << "=== === ===" << std::endl;
				XLOG_INFO("====== void XBaseHandle::initItemOperands(), UmapKeyNoFoundException  ======" + std::string(e.what()), CURRENT_THREAD_ID);
				throw UmapKeyNoFoundException(e.what());
			}
			std::string yHandleId = sourceFrom.itemId;
			std::string yName = sourceFrom.variableName;
			XLOG_INFO("XBaseHandle::initItemOperands: " + yHandleId, CURRENT_THREAD_ID);
			XLOG_INFO("XBaseHandle::initItemOperands: " + yName, CURRENT_THREAD_ID);
			if (yHandleId.empty() || yName.empty())
			{
				//doNothing
			}
			else
			{
				XBaseHandle* yHandle = nullptr;
				{
					std::lock_guard<std::mutex> lock(handleMapMutex);
					yHandle = globalHandleMap[yHandleId];
				}// 作用域结束时，lock_guard 会自动解锁互斥锁handleMapMutex

				if (yHandle == nullptr)
				{
					XLOG_INFO("XBaseHandle::initItemOperands:  yHandle is NULLPtr", CURRENT_THREAD_ID);
				}
				else
				{
					XLOG_INFO("XBaseHandle::initItemOperands:  yHandle is Not NULLPtr", CURRENT_THREAD_ID);
				}

				//Debug时候用的,注释掉
				//Dest& mDest = yHandle->getDests();
				//std::vector<std::string> yVaribleNames = ACQUIRE_NAMES(mDest);
				//// 使用范围循环遍历 std::vector<std::string>
				//for (const std::string& yNameStr : yVaribleNames)
				//{
				//	//std::cout << yNameStr << std::endl;
				//	XLOG_INFO("@ yNameStr = " + yNameStr, CURRENT_THREAD_ID);
				//}


				// XBaseHandle再添加一个成员变量, isSourceFromOutside, 并且if条件修改为yHandle != nullptr && isSourceFromOutside == false
				// 弃用 isSourceFromOutside 属性，直接通过 IS_MEMBER_FROM_OUTSIDE_STR 宏定义判断

				bool xIsFromOutside = IS_MEMBER_FROM_OUTSIDE_STR(yHandle->getDests(), yName);
				if (xIsFromOutside)
				{
					XLOG_INFO("====== XBaseHandle::xIsFromOutside ======", CURRENT_THREAD_ID);
				}
				else
				{
					if (yHandle != nullptr && xIsFromOutside == false)
					{
						auto yValue = GET_MEMBER_STR(yHandle->getDests(), yName);
						REGISTER_MEMBER_STR((*sources), xName, yValue);
					}
					XLOG_INFO("====== XBaseHandle::xIsFromInside  ======", CURRENT_THREAD_ID);
				}// end if (xIsFromOutside)--else

			}// end if (yHandleId.empty() || yName.empty())--else
		}// end for (const auto& xName : xVaribleNames) 
	}
	/*
	设置某个变量的sourceFrom，让我们知道它从哪里来
	*/
	void XBaseHandle::setSourceFrom(const std::string& xVariableName, const SourceFrom& sourceFrom)
	{
		// XBaseHandle再添加一个成员变量, isSourceFromOutside, 条件isSourceFromOutside == true为真时, 直接返回true
		// 弃用 isSourceFromOutside 属性，直接通过 IS_MEMBER_FROM_OUTSIDE_STR 宏定义判断
		bool xIsFromOutside = IS_MEMBER_FROM_OUTSIDE_STR((*sources), xVariableName);
		if (xIsFromOutside == true)
		{
			//doNoting
		}
		else
		{
			SET_SOURCEFROM_STR((*sources), xVariableName, sourceFrom);
		}
	}
	/*
	按名字查找某个变量的sourceFrom，在执行计算图前初始化参数时会调用这个函数
	*/
	void XBaseHandle::loadSourceFrom(const std::string& xVariableName, SourceFrom& sourceFrom)
	{
		// XBaseHandle再添加一个成员变量, isSourceFromOutside, 条件isSourceFromOutside == true为真时, 直接返回true
		// 弃用 isSourceFromOutside 属性，直接通过 IS_MEMBER_FROM_OUTSIDE_STR 宏定义判断
		bool xIsFromOutside = IS_MEMBER_FROM_OUTSIDE_STR((*sources), xVariableName);
		if (xIsFromOutside == true)
		{
			//doNothing
		}
		else
		{
			try
			{
				sourceFrom = GET_SOURCEFROM_STR((*sources), xVariableName);
			}
			catch (UmapKeyNoFoundException& e)
			{
				//        std::cout << "=== === ===" << e.what() << "=== === ===" << std::endl;
				XLOG_INFO("====== void XBaseHandle::loadSourceFrom, UmapKeyNoFoundException  ======" + std::string(e.what()), CURRENT_THREAD_ID);
				throw UmapKeyNoFoundException(e.what());
			}
		}
	}
	REGISTER_HANDLE(XBase);

} // namespace XVisual
