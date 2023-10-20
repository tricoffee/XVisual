#ifndef OutputItem_H
#define OutputItem_H

#include "ItemBase/XBaseItem.h"

class OutputItem : public XBaseItem
{
	Q_OBJECT
	public:
		OutputItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void ItemXOP(Source& sources, Dest& dests) override;
};

#endif //OutputItem_H