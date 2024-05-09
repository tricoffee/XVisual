#ifndef ItemManager_H
#define ItemManager_H

#include "GlobalStorage/ColleagueManager.h"

class ItemManager: public ColleagueManager
{
	public:
		static ItemManager& instance();
	protected:
		ItemManager();
};
#endif // ItemManager_H
