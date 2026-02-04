#include "ItemBase/ItemFactory.h"
#include "ItemBase/XBaseItem.h"

namespace XVisual {

	// 静态成员变量定义
	bool ItemRegistry::temporaryMode_ = false;

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


	void ItemRegistry::setTemporaryMode(bool temporary)
	{
		temporaryMode_ = temporary;
	}

	bool ItemRegistry::isTemporaryMode()
	{
		return temporaryMode_;
	}

	ItemClassRegisterer::ItemClassRegisterer(const std::string& type,
		XBaseItem* (*creator)(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent))
	{
		ItemRegistry::putConstructor(type, creator);
	}
} // namespace XVisual
