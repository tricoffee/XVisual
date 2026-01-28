#ifndef ItemManager_H
#define ItemManager_H

#include "GlobalStorage/ColleagueManager.h"

namespace XVisual {

class ItemManager: public ColleagueManager
{
	public:
		static ItemManager& instance();
	protected:
		ItemManager();
};

} // namespace XVisual

#endif // ItemManager_H
