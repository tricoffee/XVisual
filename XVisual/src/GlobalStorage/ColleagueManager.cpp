#include "GlobalStorage/ColleagueManager.h"
#include "ItemBase/ItemFactory.h"  // For ItemRegistry::isTemporaryMode()
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"

namespace XVisual {
	ColleagueManager::ColleagueManager() {}

	ColleagueManager& ColleagueManager::instance()
	{
		static ColleagueManager manager;
		return manager;
	}

	std::string ColleagueManager::getUniqueItemName(const std::string& baseName)
	{
		// 如果是临时模式（如 ToolBox 获取图标），返回临时名称，不增加计数
		bool isTemp = ItemRegistry::isTemporaryMode();
		XLOG_INFO("ColleagueManager::getUniqueItemName: baseName=" + baseName + 
		          ", isTemporaryMode=" + (isTemp ? "true" : "false"), CURRENT_THREAD_ID);
		if (isTemp)
		{
			std::string tempName = "Temp_" + baseName;
			XLOG_INFO("ColleagueManager::getUniqueItemName: returning tempName=" + tempName, CURRENT_THREAD_ID);
			return tempName;
		}
		
		int& itemCount = itemCounts[baseName];
		
		/*
		现在临时模式下不会增加计数，所以不再需要 itemCount - 1 的逻辑。
		
		原来的设计是：临时创建和正式创建共用计数器，所以用 itemCount - 1 来补偿。
		新设计是：临时模式下直接返回 Temp_xxx，不增加计数，正式创建时从 0 开始。
		
		命名规则：
		- itemCount = 0 -> "LoadImage" (第一个，无后缀)
		- itemCount = 1 -> "LoadImage1" (第二个)
		- itemCount = 2 -> "LoadImage2" (第三个)
		- ...
		*/
		std::string uniqueItemName;
		if (itemCount == 0)
		{
			// 第一个节点：无后缀
			uniqueItemName = baseName;
		}
		else
		{
			// 后续节点：添加数字后缀
			uniqueItemName = baseName + std::to_string(itemCount);
		}
		++itemCount;
		XLOG_INFO("ColleagueManager::getUniqueItemName: returning uniqueItemName=" + uniqueItemName + 
		          ", itemCount after=" + std::to_string(itemCount), CURRENT_THREAD_ID);
		return uniqueItemName;
	}
} // namespace XVisual
