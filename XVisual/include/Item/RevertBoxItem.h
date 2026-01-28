#ifndef RevertBoxItem_H
#define RevertBoxItem_H

#include "ItemBase/XBaseItem.h"

namespace XVisual {

class RevertBoxItem : public XBaseItem
{
	Q_OBJECT
	public:
		RevertBoxItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		void xOperate() override;
	protected:
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	private:

};

} // namespace XVisual

#endif //RevertBoxItem_H
