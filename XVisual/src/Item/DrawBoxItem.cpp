#include "Common/LoggerInstance.h"
#include "Common/OpenCVHeaders.h"
#include <QFileDialog>
#include <QDebug>
#include <optional>
#include <QInputDialog>
#include "Common/XThreadMacro.h"
#include "Item/DrawBoxItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "CustomDialog/CropDialog.h"
#include "MainWindow/GraphicsWidget.h"
#include "Handle/DrawBoxHandle.h"
#include "Common/StrUtils.h"

DrawBoxItem::DrawBoxItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(gWidget, contextMenu, parent)
{
	std::string classNameStr = "DrawBox";
	setClassName(classNameStr);
	createUniqueName(classNameStr);
	setObjectName(QString::fromStdString(uniqueName));
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
	// 设置显示在Item上的文本
	setEditText(QString::fromStdString(uniqueName));
	XLOG_INFO("CVCropItem::CVCropItem, uuid = " + uuid, CURRENT_THREAD_ID);
	// 创建handle
	xHandle = HandleRegistry::createObject(classNameStr);
	xHandle->setUuidConsistentWithItem(uuid);
	initParams();
}
QPixmap DrawBoxItem::image()
{
	XBaseItem::myPolygon = myPolygon;
	return XBaseItem::image();
}
void DrawBoxItem::debug()
{
	qDebug() << "DrawBoxItem::debug() ... ";
	qDebug() << "DrawBoxItem::boundingRect() " << boundingRect();
	qDebug() << "DrawBoxItem::boundingRect().width() " << boundingRect().width();
	qDebug() << "DrawBoxItem::boundingRect().height() " << boundingRect().height();
	qDebug() << "DrawBoxItem::uuid " << QString::fromStdString(uuid);
}
void DrawBoxItem::initParams()
{

}
void DrawBoxItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}
void DrawBoxItem::xOperate()
{
	xHandle->xOperate();
	Dest& d = xHandle->getDests();
	cv::Mat resultImage = GET_MEMBER_WITH_TYPE_STR(d, cv::Mat, "resultImage");
	// Done,Add code for sending show image signals to display images on SideWidgets
	std::string filename = uniqueName;
	emit showImageSignal(filename, resultImage, this);
}

REGISTER_ITEM(DrawBox);