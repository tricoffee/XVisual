#ifndef ItemFactory_H
#define ItemFactory_H

#include<iostream>
#include<string>
#include<map>

class QGraphicsItem;
class QMenu;
class XBaseItem;
class GraphicsWidget;
class ItemRegistry
{
public:
	// ���� GraphicsWidget* gWidget ����
	typedef XBaseItem* (*Constructor)(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent);
	typedef std::map<std::string, Constructor> ItemClassRegistry;
	static ItemClassRegistry& RegistryInstance();
	static void putConstructor(const std::string& type, Constructor constructor);
	// ��������GraphicsWidget* gWidget
	static XBaseItem* createObject(const std::string& type, 
		GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);

private:
	ItemRegistry();
};


// ���� GraphicsWidget* gWidget
class ItemClassRegisterer
{
    public:
	     ItemClassRegisterer(const std::string& type, 
			 XBaseItem* (*creator)(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent));
};
#define REGISTER_ITEM(T) \
	XBaseItem* xvisual_##T##_constructor(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent) \
	{ \
		return (XBaseItem*)(new T##Item(gWidget,contextMenu,parent)); \
	} \
    static ItemClassRegisterer  xvisual_##T##_register(#T, xvisual_##T##_constructor)

#endif //ItemFactory_H