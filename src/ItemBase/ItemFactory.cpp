#include "ItemBase/ItemFactory.h"
#include "ItemBase/XBaseItem.h"
ItemRegistry::ItemRegistry() {}
ItemRegistry::ClassRegistry& ItemRegistry::RegistryInstance()
{
	static ClassRegistry* classRegistry_ = new ClassRegistry();
	return *classRegistry_;
}
void ItemRegistry::putConstructor(const std::string& type, Constructor constructor)
{
	ClassRegistry& registry = RegistryInstance();
	if (registry.count(type) == 0)
	{
		registry[type] = constructor;
	}
}
//XBaseItem* ItemRegistry::createObject(const std::string& type,
//	QMenu* contextMenu, QGraphicsItem* parent)
//{
//	ClassRegistry& registry = RegistryInstance();
//	return registry[type](contextMenu, parent);
//}
//ClasssRegisterer::ClasssRegisterer(const std::string& type,
//    XBaseItem* (*creator)(QMenu* contextMenu, QGraphicsItem* parent))
//{
//	ItemRegistry::putConstructor(type, creator);
//}
XBaseItem* ItemRegistry::createObject(const std::string& type,
	GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
{
	ClassRegistry& registry = RegistryInstance();
	return registry[type](gWidget, contextMenu, parent);
}
ClasssRegisterer::ClasssRegisterer(const std::string& type,
	XBaseItem* (*creator)(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent))
{
	ItemRegistry::putConstructor(type, creator);
}