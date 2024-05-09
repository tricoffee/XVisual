#include "GlobalStorage/ItemManager.h"

ItemManager::ItemManager() {}

ItemManager& ItemManager::instance()
{
	static ItemManager manager;
	return manager;
}
