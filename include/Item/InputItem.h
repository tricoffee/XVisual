#ifndef InputItem_H
#define InputItem_H

#include "ItemBase/XBaseItem.h"

class InputItem : public XBaseItem
{
	Q_OBJECT
	public:
		InputItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void ItemXOP(Source& sources, Dest& dests) override;
};

#endif //InputItem_H