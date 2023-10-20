#ifndef StepItem_H
#define StepItem_H

#include "ItemBase/XBaseItem.h"

class StepItem : public XBaseItem
{
	Q_OBJECT
	public:
		StepItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void ItemXOP(Source& sources, Dest& dests) override;
};

#endif //StepItem_H