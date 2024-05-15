#ifndef RevertBoxItem_H
#define RevertBoxItem_H

#include "ItemBase/XBaseItem.h"

class RevertBoxItem : public XBaseItem
{
	Q_OBJECT
	public:
		RevertBoxItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void initParams() override;
		void xOperate() override;
	protected:
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	private:

};

#endif //RevertBoxItem_H
