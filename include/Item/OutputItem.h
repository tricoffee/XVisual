#ifndef OutputItem_H
#define OutputItem_H

#include "ItemBase/XBaseItem.h"

class OutputItem : public XBaseItem
{
	Q_OBJECT
	public:
		OutputItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		// 新增 GraphicsWidget* gWidget 参数
		OutputItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void ItemXOP() override;
    protected:
		void createUniqueName() override;
};

#endif //OutputItem_H