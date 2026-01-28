#include "GlobalStorage/ItemManager.h"

namespace XVisual {
	ItemManager::ItemManager() {}

	ItemManager& ItemManager::instance()
	{
		static ItemManager manager;
		return manager;
	}
} //namespace XVisual