#ifndef ParamWidgetFactory_H
#define ParamWidgetFactory_H

#include<iostream>
#include<string>
#include<map>

namespace XVisual {

class PreParamWidget;

class ParamWidgetRegistry
{
public:
	typedef PreParamWidget* (*Constructor)();
	typedef std::map<std::string, Constructor> ParamWidgetClassRegistry;
	static ParamWidgetClassRegistry& RegistryInstance();
	static void putConstructor(const std::string& type, Constructor constructor);
	static PreParamWidget* createObject(const std::string& type);

private:
	ParamWidgetRegistry();
};


class ParamWidgetClassRegisterer
{
public:
	ParamWidgetClassRegisterer(const std::string& type,
		PreParamWidget* (*creator)());
};
#define REGISTER_ParamWidget(T) \
    PreParamWidget* xpre_##T##_constructor() \
    { \
        return (PreParamWidget*)(new T##ParamWidget()); \
    } \
    static ParamWidgetClassRegisterer  xpre_##T##_register(#T, xpre_##T##_constructor)

} // namespace XVisual

#endif // ParamWidgetFactory_H

