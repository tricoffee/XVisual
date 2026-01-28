#ifndef ItemFactory_H
#define ItemFactory_H

#include<iostream>
#include<string>
#include<map>
#include<QGraphicsItem>
#include<QMenu>

namespace XVisual {

class XBaseItem;
class GraphicsWidget;

class ItemRegistry
{
public:
	// 新增 GraphicsWidget* gWidget 参数
	typedef XBaseItem* (*Constructor)(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent);
	typedef std::map<std::string, Constructor> ItemClassRegistry;
	static ItemClassRegistry& RegistryInstance();
	static void putConstructor(const std::string& type, Constructor constructor);
	// 新增参数GraphicsWidget* gWidget
	static XBaseItem* createObject(const std::string& type, 
		GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);

private:
	ItemRegistry();
};


// 新增 GraphicsWidget* gWidget
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

} // namespace XVisual

#endif //ItemFactory_H
