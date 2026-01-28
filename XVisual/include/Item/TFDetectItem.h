#ifndef TFDetectItem_H
#define TFDetectItem_H

#include "ItemBase/XBaseFItem.h"

class QThread;
class QTimer;

namespace XVisual {

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

} // namespace XVisual

#endif //TFDetectItem_H


