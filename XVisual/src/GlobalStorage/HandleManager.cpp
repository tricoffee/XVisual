#include "GlobalStorage/HandleManager.h"

HandleManager::HandleManager() {}

HandleManager& HandleManager::instance()
{
	static HandleManager manager;
	return manager;
}
