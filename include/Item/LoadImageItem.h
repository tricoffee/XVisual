#ifndef LoadImageItem_H
#define LoadImageItem_H

#include "ItemBase/XBaseItem.h"

class LoadImageItem : public XBaseItem
{
    Q_OBJECT
	public:
		//LoadImageItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		// 新增 GraphicsWidget* gWidget 参数
		LoadImageItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void initParams() override;
		void xOperate() override;
    protected:
	   void createUniqueName(const QString& classNameQStr) override;
	   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
	private:
		QString lastImagePath; // 声明存储上次选中的文件路径的私有成员变量

};
#endif //LoadImageItem_H