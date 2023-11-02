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
	QPixmap image() override;
	void debug();
	void initParameters() override;
	Source& getSources();
	Dest& getDests();
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	void ItemXOP(Source& sources, Dest& dests) override;
protected:
	void createUniqueName() override;
private:
	Source sources;
	Dest dests;
	cv::Rect regionOfInterest;
};

#endif //CVCropItem_H

