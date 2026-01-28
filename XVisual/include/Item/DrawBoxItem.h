#ifndef DrawBoxItem_H
#define DrawBoxItem_H

#include "ItemBase/XBaseItem.h"

namespace XVisual {

class DrawBoxItem : public XBaseItem
{
	Q_OBJECT
	public:
		DrawBoxItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		void xOperate() override;
	protected:
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	private:

};

} // namespace XVisual

#endif //DrawBoxItem_H
