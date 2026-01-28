#include "ItemFactory_shared_ptr.h"

namespace XVisual {

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
std::shared_ptr<XBaseItem> ItemRegistry::createObject(const std::string& type, QMenu* contextMenu, QGraphicsItem* parent)
{
	ClassRegistry& registry = RegistryInstance();
	return registry[type](contextMenu, parent);
}
ClasssRegisterer::ClasssRegisterer(const std::string& type, std::shared_ptr<XBaseItem>(*creator)(QMenu* contextMenu, QGraphicsItem* parent))
{
	ItemRegistry::putConstructor(type, creator);
}

} // namespace XVisual