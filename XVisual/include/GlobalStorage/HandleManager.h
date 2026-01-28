#ifndef HandleManager_H
#define HandleManager_H

#include "GlobalStorage/ColleagueManager.h"

namespace XVisual {

class HandleManager: public ColleagueManager
{
	public:
		static HandleManager& instance();
	protected:
		HandleManager();
};

} // namespace XVisual

#endif // HandleManager_H
