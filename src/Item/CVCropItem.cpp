#include "Item/CVCropItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QFileDialog>
#include <QDebug>
#include <optional>

CVCropItem::CVCropItem(QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(contextMenu, parent), sources(Source::getInstance()), dests(Dest::getInstance())
{
	createUniqueName();
	QRect rect(-100, -100, 200, 200);
	QPainterPath path;
	int radius = 25;
	int cornersize = 2 * radius;
	path.moveTo(rect.left() + radius, rect.top());
	path.arcTo(rect.left(), rect.top(), cornersize, cornersize, 90, 90);
	path.lineTo(rect.left(), rect.bottom() - radius);
	path.arcTo(rect.left(), rect.bottom() - cornersize,
		cornersize, cornersize, 180, 90);
	path.lineTo(rect.right() - radius, rect.bottom());
	path.arcTo(rect.right() - cornersize, rect.bottom() - cornersize,
		cornersize, cornersize, 270, 90);
	path.lineTo(rect.right(), rect.top() + radius);
	path.arcTo(rect.right() - cornersize, rect.top(),
		cornersize, cornersize, 0, 90);
	myPolygon = path.toFillPolygon();
	setPolygon(myPolygon);
	initParameters();
	// 设置显示在Item上的文本
	setEditText(uniqueName);
}
QPixmap CVCropItem::image()
{
	XBaseItem::myPolygon = myPolygon;
	return XBaseItem::image();
}
void CVCropItem::debug()
{
	qDebug() << "CVCropItem::debug() ... ";
	qDebug() << "CVCropItem::boundingRect() " << boundingRect();
	qDebug() << "CVCropItem::boundingRect().width() " << boundingRect().width();
	qDebug() << "CVCropItem::boundingRect().height() " << boundingRect().height();
	qDebug() << "CVCropItem::uuid " << QString::fromStdString(uuid);
}
Source& CVCropItem::getSources()
{
	qDebug() << "Source& CVCropItem::getSources() ";
	return sources;
}
Dest& CVCropItem::getDests()
{
	qDebug() << "Dest& CVCropItem::getDests() ";
	return dests;
}
void CVCropItem::initParameters()
{
	cv::Mat img;
	REGISTER_MEMBER(sources, img, img);
	cv::Mat cropImage;
	REGISTER_MEMBER(dests, cropImage, cropImage);
}
void CVCropItem::ItemXOP(Source& sources, Dest& dests)
{


}
void CVCropItem::createUniqueName()
{
	uniqueName = ItemManager::instance().getUniqueItemName("CVCrop");
	setObjectName(uniqueName);
}
REGISTER_CLASS(CVCrop);