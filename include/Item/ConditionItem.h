#ifndef ConditionItem_H
#define ConditionItem_H

#include "ItemBase/XBaseItem.h"

class ConditionItem : public XBaseItem
{
	Q_OBJECT
	public:
		ConditionItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void ItemXOP(Source& sources, Dest& dests) override;
};

#endif //ConditionItem_H