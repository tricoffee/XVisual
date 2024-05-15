#ifndef LoadImageItem_H
#define LoadImageItem_H

#include "ItemBase/XBaseItem.h"

class LoadImageItem : public XBaseItem
{
    Q_OBJECT
	public:
		LoadImageItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void initParams() override;
		void xOperate() override;
    protected:
	   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
	private:

};
#endif //LoadImageItem_H