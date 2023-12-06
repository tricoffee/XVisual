#ifndef LoadImageItem_H
#define LoadImageItem_H

#include "ItemBase/XBaseItem.h"

class LoadImageItem : public XBaseItem
{
    Q_OBJECT
	public:
		LoadImageItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		LoadImageItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void initParameters() override;
		void ItemXOP() override;
    protected:
	   void createUniqueName() override;
	   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
	private:

};
#endif //LoadImageItem_H