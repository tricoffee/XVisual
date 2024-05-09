#include "GlobalStorage/ColleagueManager.h"

ColleagueManager::ColleagueManager() {}

ColleagueManager& ColleagueManager::instance()
{
	static ColleagueManager manager;
	return manager;
}

std::string ColleagueManager::getUniqueItemName(const std::string& baseName)
{
	int& itemCount = itemCounts[baseName];
	//std::cout << "itemCount = " << std::to_string(itemCount);
	/*
	std::to_string 里面的参数是itemCount - 1而不是itemCount，是因为该项目采用宏定义注册每个XXXItem
	在编译时会执行一遍每个XXXItem的构造函数，此时itemCount是0；
	这样的话在到scene里面添加XXXItem的时候itemCount则变成1
	*/
	std::string uniqueItemName = baseName + std::to_string(itemCount - 1);
	if (itemCount - 1 == 0)
	{
		/*
		如果itemCount - == 0成立则表示实际的uniqueItemName是XXXItem0;
		习惯上显示XXXItem比XXXItem0要好得多;
		所以将uniqueItemName重置为baseName + std::string("")
		*/
		uniqueItemName = baseName + std::string("");
	}
	++itemCount;
	return uniqueItemName;
}
