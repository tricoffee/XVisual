#include "HandleBase/XBaseHandle.h"
#include "GlobalStorage/GlobalStorage.h"
#include "Exception/UmapKeyNoFoundException.h"
#include "Common/StrUtils.h"

XBaseHandle::XBaseHandle(): Colleague(), sources(Source::getInstance()), dests(Dest::getInstance())
{
	std::cout << "class XBaseHandle: public Colleague " << std::endl;
	setColleagueType();
	initParams();
}
XBaseHandle::~XBaseHandle()
{

}
void XBaseHandle::setColleagueType()
{
	colleagueType = ColleagueType::HandleType;
}
void XBaseHandle::createUuid()
{
	QUuid id = QUuid::createUuid();
	uuid = "Handle_" + id.toString().toStdString();
}
void XBaseHandle::setUuidConsistentWithItem(std::string xitemUuid)
{
	std::string delimiter = "_";
	std::string uuidStr2 = extractSubstrAfterDelimiter(xitemUuid, delimiter);
	const std::string handleUuid = "Handle_" + uuidStr2;
	//Here, uuid is the protected member variable of the XHandle class
	this->uuid = handleUuid;
}
Source& XBaseHandle::getSources()
{
	//qDebug() << "Source& XBaseHandle::getSources() ";
	return this->sources;
}
Dest& XBaseHandle::getDests()
{
	//qDebug() << "Dest& XBaseHandle::getDests() ";
	return this->dests;
}
void XBaseHandle::setSources(Source mSourcs)
{
	this->sources = mSourcs;
}
void XBaseHandle::setDests(Dest mDests)
{
	this->dests = mDests;
}
void XBaseHandle::initParams()
{

}
void XBaseHandle::xOperate()
{

}
/*
��sourceFrom����sources�����ÿ����������Դ���ҳ�ʼ���ò���
*/
void XBaseHandle::initOperands()
{
	XLOG_INFO("XBaseHandle::initItemOperands ...... ", CURRENT_THREAD_ID);
	std::vector<std::string> xVaribleNames = ACQUIRE_NAMES(sources);
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
			}// ���������ʱ��lock_guard ���Զ�����������handleMapMutex

			if (yHandle == nullptr)
			{
				XLOG_INFO("XBaseHandle::initItemOperands:  yHandle is NULLPtr", CURRENT_THREAD_ID);
			}
			else
			{
				XLOG_INFO("XBaseHandle::initItemOperands:  yHandle is Not NULLPtr", CURRENT_THREAD_ID);
			}

			//Debugʱ���õ�,ע�͵�
			//Dest& mDest = yHandle->getDests();
			//std::vector<std::string> yVaribleNames = ACQUIRE_NAMES(mDest);
			//// ʹ�÷�Χѭ������ std::vector<std::string>
			//for (const std::string& yNameStr : yVaribleNames)
			//{
			//	//std::cout << yNameStr << std::endl;
			//	XLOG_INFO("@ yNameStr = " + yNameStr, CURRENT_THREAD_ID);
			//}


			// XBaseHandle�����һ����Ա����, isSourceFromOutside, ����if�����޸�ΪyHandle != nullptr && isSourceFromOutside == false
			// ���� isSourceFromOutside ���ԣ�ֱ��ͨ�� IS_MEMBER_FROM_OUTSIDE_STR �궨���ж�

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
					REGISTER_MEMBER_STR(sources, xName, yValue);
				}
				XLOG_INFO("====== XBaseHandle::xIsFromInside  ======", CURRENT_THREAD_ID);
			}// end if (xIsFromOutside)--else

		}// end if (yHandleId.empty() || yName.empty())--else
	}// end for (const auto& xName : xVaribleNames) 
}
/*
����ĳ��������sourceFrom��������֪������������
*/
void XBaseHandle::setSourceFrom(const std::string& xVariableName, const SourceFrom& sourceFrom)
{
	// XBaseHandle�����һ����Ա����, isSourceFromOutside, ����isSourceFromOutside == trueΪ��ʱ, ֱ�ӷ���true
	// ���� isSourceFromOutside ���ԣ�ֱ��ͨ�� IS_MEMBER_FROM_OUTSIDE_STR �궨���ж�
	bool xIsFromOutside = IS_MEMBER_FROM_OUTSIDE_STR(sources, xVariableName);
	if (xIsFromOutside == true)
	{
		//doNoting
	}
	else
	{
		SET_SOURCEFROM_STR(sources, xVariableName, sourceFrom);
	}
}
/*
�����ֲ���ĳ��������sourceFrom����ִ�м���ͼǰ��ʼ������ʱ������������
*/
void XBaseHandle::loadSourceFrom(const std::string& xVariableName, SourceFrom& sourceFrom)
{
	// XBaseHandle�����һ����Ա����, isSourceFromOutside, ����isSourceFromOutside == trueΪ��ʱ, ֱ�ӷ���true
	// ���� isSourceFromOutside ���ԣ�ֱ��ͨ�� IS_MEMBER_FROM_OUTSIDE_STR �궨���ж�
	bool xIsFromOutside = IS_MEMBER_FROM_OUTSIDE_STR(sources, xVariableName);
	if (xIsFromOutside == true)
	{
		//doNothing
	}
	else
	{
		try
		{
			sourceFrom = GET_SOURCEFROM_STR(sources, xVariableName);
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