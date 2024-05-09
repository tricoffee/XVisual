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
	std::to_string ����Ĳ�����itemCount - 1������itemCount������Ϊ����Ŀ���ú궨��ע��ÿ��XXXItem
	�ڱ���ʱ��ִ��һ��ÿ��XXXItem�Ĺ��캯������ʱitemCount��0��
	�����Ļ��ڵ�scene�������XXXItem��ʱ��itemCount����1
	*/
	std::string uniqueItemName = baseName + std::to_string(itemCount - 1);
	if (itemCount - 1 == 0)
	{
		/*
		���itemCount - == 0�������ʾʵ�ʵ�uniqueItemName��XXXItem0;
		ϰ������ʾXXXItem��XXXItem0Ҫ�õö�;
		���Խ�uniqueItemName����ΪbaseName + std::string("")
		*/
		uniqueItemName = baseName + std::string("");
	}
	++itemCount;
	return uniqueItemName;
}
