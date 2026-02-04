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
	
	/**
	 * 临时模式标志：用于区分临时创建的 Item（如 ToolBox 获取图标）
	 * 
	 * 当 isTemporaryMode() 为 true 时，Item/Handle 的 UUID 将使用 Temp_ 前缀：
	 *   - Temp_Item_xxx
	 *   - Temp_Handle_xxx
	 * 
	 * 使用方式：
	 *   ItemRegistry::setTemporaryMode(true);
	 *   auto* item = ItemRegistry::createObject(...);
	 *   // 使用 item
	 *   delete item;
	 *   ItemRegistry::setTemporaryMode(false);
	 */
	static void setTemporaryMode(bool temporary);
	static bool isTemporaryMode();

private:
	ItemRegistry();
	static bool temporaryMode_;
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
