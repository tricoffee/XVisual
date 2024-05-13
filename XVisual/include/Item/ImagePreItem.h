#ifndef ImagePreItem_H
#define ImagePreItem_H

#include "ItemBase/XBaseItem.h"
//#include "Common/DebugConfigMacro.h"
#include "Common/PreParam.h"

class ImagePreItem : public XBaseItem
{
	Q_OBJECT
	public:
		//ImagePreItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		// 新增 GraphicsWidget 参数
		ImagePreItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
		QPixmap image() override;
		void debug();
		void initParams() override;
		void xOperate() override;
	protected:
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	private:
	#ifdef DEBUG
		void printPreParam(const PreParam& mParam);
	#endif
};

#endif //ImagePreItem_H


