#ifndef XBaseFItem_H
#define XBaseFItem_H

#include "ItemBase/XBaseItem.h"
#include "Common/AsynchronousFileProcessor.h"
#include "Common/AsynchronousFileEmitter.h"

class QThread;

class XBaseFItem : public XBaseItem
{
	Q_OBJECT
	public:
		XBaseFItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		~XBaseFItem();
	signals:
		void copyFileSignal(FileCopyData& data);
	protected:
		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
		void constructDataAndDeliver(const QString& srcFilePath);
		virtual void fileCopyReadyUpdateFunc(FileCopyData data);
	protected slots:
		void fileCopyReadyUpdateSlot(FileCopyData data);
	protected:
		QThread* asynFileCopyThread;
		AFProcessor* asynProcessor;
		AFEmitter* asynEmitter;
		FileCopyData fileCopyData;
};

#endif //XBaseFItem_H
