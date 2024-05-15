#ifndef TFDetectItem_H
#define TFDetectItem_H

#include "ItemBase/XBaseItem.h"

class TFDetectItem : public XBaseItem
{
	Q_OBJECT
	public:
		TFDetectItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void initParams() override;
		void xOperate() override;
	protected:
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	private:

};

#endif //TFDetectItem_H


