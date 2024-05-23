#ifndef TFDetectItem_H
#define TFDetectItem_H

#include "ItemBase/XBaseFItem.h"

class QThread;
class QTimer;

class TFDetectItem : public XBaseFItem
{
	Q_OBJECT
	public:
		TFDetectItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		~TFDetectItem();
		void xOperate() override;
	protected:
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
		void fileCopyReadyUpdateFunc(FileCopyData data) override;
	private:
		
};

#endif //TFDetectItem_H


