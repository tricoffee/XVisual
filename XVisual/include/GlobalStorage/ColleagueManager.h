#ifndef ColleagueManager_H
#define ColleagueManager_H
#include <iostream>
#include <string>
#include <unordered_map>
class ColleagueManager
{
	public:
		static ColleagueManager& instance();
		std::string getUniqueItemName(const std::string& baseName);
	protected:
		ColleagueManager();
		std::unordered_map<std::string, int> itemCounts;
};
#endif // ColleagueManager_H
