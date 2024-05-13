#include "ParamWidget/ParamWidgetFactory.h"
#include "ParamWidget/PreParamWidget.h"

ParamWidgetRegistry::ParamWidgetRegistry() {}
ParamWidgetRegistry::ParamWidgetClassRegistry& ParamWidgetRegistry::RegistryInstance()
{
	static ParamWidgetClassRegistry* classRegistry_ = new ParamWidgetClassRegistry();
	return *classRegistry_;
}
void ParamWidgetRegistry::putConstructor(const std::string& type, Constructor constructor)
{
	ParamWidgetClassRegistry& registry = RegistryInstance();
	if (registry.count(type) == 0)
	{
		registry[type] = constructor;
	}
}
PreParamWidget* ParamWidgetRegistry::createObject(const std::string& type)
{
	ParamWidgetClassRegistry& registry = RegistryInstance();
	return registry[type]();
}
ParamWidgetClassRegisterer::ParamWidgetClassRegisterer(const std::string& type,
	PreParamWidget* (*creator)())
{
	ParamWidgetRegistry::putConstructor(type, creator);
}
