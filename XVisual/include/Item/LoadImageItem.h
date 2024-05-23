#ifndef LoadImageItem_H
#define LoadImageItem_H

#include "ItemBase/XBaseFItem.h"

class QThread;
class QTimer;

class LoadImageItem : public XBaseFItem
{
    Q_OBJECT
	public:
		LoadImageItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		~LoadImageItem();
		void xOperate() override;
    protected:
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
		void fileCopyReadyUpdateFunc(FileCopyData data) override;
	private:

};
#endif //LoadImageItem_H