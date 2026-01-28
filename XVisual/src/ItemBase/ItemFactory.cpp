#include "ItemBase/ItemFactory.h"
#include "ItemBase/XBaseItem.h"

namespace XVisual {

	ItemRegistry::ItemRegistry() {}
	ItemRegistry::ItemClassRegistry& ItemRegistry::RegistryInstance()
	{
		static ItemClassRegistry* classRegistry_ = new ItemClassRegistry();
		return *classRegistry_;
	}
	void ItemRegistry::putConstructor(const std::string& type, Constructor constructor)
	{
		ItemClassRegistry& registry = RegistryInstance();
		if (registry.count(type) == 0)
		{
			registry[type] = constructor;
		}
	}
	// 新增参数 GraphicsWidget* gWidget
	XBaseItem* ItemRegistry::createObject(const std::string& type,
		GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	{
		ItemClassRegistry& registry = RegistryInstance();
		return registry[type](gWidget, contextMenu, parent);
	}


	ItemClassRegisterer::ItemClassRegisterer(const std::string& type,
		XBaseItem* (*creator)(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent))
	{
		ItemRegistry::putConstructor(type, creator);
	}
} // namespace XVisual 