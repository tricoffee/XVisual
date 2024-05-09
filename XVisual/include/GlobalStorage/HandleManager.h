#ifndef HandleManager_H
#define HandleManager_H

#include "GlobalStorage/ColleagueManager.h"

class HandleManager: public ColleagueManager
{
	public:
		static HandleManager& instance();
	protected:
		HandleManager();
};
#endif // HandleManager_H
