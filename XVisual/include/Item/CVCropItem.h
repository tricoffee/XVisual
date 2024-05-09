#ifndef CVCropItem_H
#define CVCropItem_H

#include "ItemBase/XBaseItem.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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
		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
		void xOperate() override;
	protected:

	private:

};

#endif //CVCropItem_H

