#pragma once
#include<iostream>
#include<string>
#include<map>
class QGraphicsItem;
class QMenu;
class XBaseItem;
class ItemRegistry
{
public:
	typedef std::shared_ptr<XBaseItem>(*Constructor)(QMenu* contextMenu, QGraphicsItem* parent);
	typedef std::map<std::string, Constructor> ClassRegistry;
	static ClassRegistry& RegistryInstance();
	static void putConstructor(const std::string& type, Constructor constructor);
	static std::shared_ptr<XBaseItem> createObject(const std::string& type, QMenu* contextMenu, QGraphicsItem* parent=nullptr);
private:
	ItemRegistry();
};
class ClasssRegisterer 
{
public:
	ClasssRegisterer(const std::string& type, std::shared_ptr<XBaseItem>(*creator)(QMenu* contextMenu, QGraphicsItem* parent));
};
#define REGISTER_CLASS(T) \
	std::shared_ptr<XBaseItem> xvisual_##T##_construtor(QMenu* contextMenu, QGraphicsItem* parent) \
	{ \
		return std::shared_ptr<XBaseItem>(new T##Item(contextMenu,parent)); \
	} \
    static ClasssRegisterer  xvisual_##T##_register(#T, xvisual_##T##_construtor)

