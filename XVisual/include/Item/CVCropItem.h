#ifndef CVCropItem_H
#define CVCropItem_H

#include "ItemBase/XBaseItem.h"

class CVCropItem : public XBaseItem
{
		Q_OBJECT
	public:
		//CVCropItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		// 新增 GraphicsWidget 参数
		CVCropItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void initParams() override;
		void xOperate() override;
	protected:
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	private:

};

#endif //CVCropItem_H

