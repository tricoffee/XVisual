#include "GlobalStorage/HandleManager.h"

namespace XVisual {
	HandleManager::HandleManager() {}

	HandleManager& HandleManager::instance()
	{
		static HandleManager manager;
		return manager;
	}
} //namespace XVisual