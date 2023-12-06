#ifndef ConditionItem_H
#define ConditionItem_H

#include "ItemBase/XBaseItem.h"

class ConditionItem : public XBaseItem
{
	Q_OBJECT
	public:
		ConditionItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		ConditionItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void ItemXOP() override;
    protected:
		void createUniqueName() override;
};

#endif //ConditionItem_H