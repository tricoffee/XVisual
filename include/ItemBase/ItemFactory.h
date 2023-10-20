#ifndef ItemFactory_H
#define ItemFactory_H

#include<iostream>
#include<string>
#include<map>

class QGraphicsItem;
class QMenu;
class XBaseItem;
class ItemRegistry
{
public:
	typedef XBaseItem* (*Constructor)(QMenu* contextMenu, QGraphicsItem* parent);
	typedef std::map<std::string, Constructor> ClassRegistry;
	static ClassRegistry& RegistryInstance();
	static void putConstructor(const std::string& type, Constructor constructor);
	static XBaseItem* createObject(const std::string& type,
		QMenu* contextMenu, QGraphicsItem* parent = nullptr);
private:
	ItemRegistry();
};
class ClasssRegisterer 
{
public:
	ClasssRegisterer(const std::string& type,
		XBaseItem* (*creator)(QMenu* contextMenu, QGraphicsItem* parent));
};
#define REGISTER_CLASS(T) \
	XBaseItem* xvisual_##T##_constructor(QMenu* contextMenu, QGraphicsItem* parent) \
	{ \
		return (XBaseItem*)(new T##Item(contextMenu,parent)); \
	} \
    static ClasssRegisterer  xvisual_##T##_register(#T, xvisual_##T##_constructor)

#endif //ItemFactory_H