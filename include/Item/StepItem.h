#ifndef StepItem_H
#define StepItem_H

#include "ItemBase/XBaseItem.h"

class StepItem : public XBaseItem
{
	Q_OBJECT
	public:
		StepItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		// 新增 GraphicsWidget* 参数
		StepItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void ItemXOP() override;
	protected:
		void createUniqueName() override;
};

#endif //StepItem_H