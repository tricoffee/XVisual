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
	CVCropItem(QMenu* contextMenu, QGraphicsItem* parent = nullptr);
	CVCropItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent = nullptr);
	QPixmap image() override;
	void debug();
	void initParameters() override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	void ItemXOP() override;
protected:
	void createUniqueName() override;
private:
	cv::Rect regionOfInterest;
};

#endif //CVCropItem_H

