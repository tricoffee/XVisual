#ifndef LoadImageItem_H
#define LoadImageItem_H

#include "ItemBase/XBaseItem.h"

class LoadImageItem : public XBaseItem
{
    Q_OBJECT
	public:
		LoadImageItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		Source& getSources();
		Dest& getDests();
		void initParameters() override;
		void ItemXOP(Source& sources, Dest& dests) override;
    protected:
	   void createUniqueName() override;
	   void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
	private:
		Source sources;
		Dest dests;
};
#endif //LoadImageItem_H