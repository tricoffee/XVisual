#include "HandleBase/HandleFactory.h"
#include "HandleBase/XBaseHandle.h"

namespace XVisual {

HandleRegistry::HandleRegistry() {}
HandleRegistry::HandleClassRegistry& HandleRegistry::RegistryInstance()
{
	static HandleClassRegistry* classRegistry_ = new HandleClassRegistry();
	return *classRegistry_;
}
void HandleRegistry::putConstructor(const std::string& type, Constructor constructor)
{
	HandleClassRegistry& registry = RegistryInstance();
	if (registry.count(type) == 0)
	{
		registry[type] = constructor;
	}
}
XBaseHandle* HandleRegistry::createObject(const std::string& type)
{
	HandleClassRegistry& registry = RegistryInstance();
	return registry[type]();
}
HandleClassRegisterer::HandleClassRegisterer(const std::string& type,
	XBaseHandle* (*creator)())
{
	HandleRegistry::putConstructor(type, creator);
}

} // namespace XVisual