#ifndef CVCropItem_H
#define CVCropItem_H

#include "ItemBase/XBaseItem.h"

class CVCropItem : public XBaseItem
{
		Q_OBJECT
	public:
		CVCropItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		void xOperate() override;
	protected:
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	private:

};

#endif //CVCropItem_H

