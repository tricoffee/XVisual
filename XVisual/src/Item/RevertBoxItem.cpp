#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Common/OpenCVHeaders.h"
#include "Item/RevertBoxItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "MainWindow/SideWidget.h"
#include <QFileDialog>
#include <QDebug>
#include <optional>
#include "MainWindow/GraphicsWidget.h"
#include "Handle/RevertBoxHandle.h"
#include "Common/StrUtils.h"

RevertBoxItem::RevertBoxItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(gWidget, contextMenu, parent)
{

	std::string classNameStr = "RevertBox";
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
	XLOG_INFO("RevertBoxItem::RevertBoxItem, uuid = " + uuid, CURRENT_THREAD_ID);
	// 创建xHandle
	xHandle = HandleRegistry::createObject(classNameStr);
	xHandle->setUuidConsistentWithItem(uuid);
	initParams();
}
QPixmap RevertBoxItem::image()
{
	XBaseItem::myPolygon = myPolygon;
	return XBaseItem::image();
}
void RevertBoxItem::debug()
{
	qDebug() << "RevertBoxItem::debug() ... ";
	qDebug() << "RevertBoxItem::boundingRect() " << boundingRect();
	qDebug() << "RevertBoxItem::boundingRect().width() " << boundingRect().width();
	qDebug() << "RevertBoxItem::boundingRect().height() " << boundingRect().height();
	qDebug() << "RevertBoxItem::uuid " << QString::fromStdString(uuid);
}
void RevertBoxItem::initParams()
{
	// do nothing
}
void RevertBoxItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}
void RevertBoxItem::xOperate()
{
	xHandle->xOperate();
}

REGISTER_ITEM(RevertBox);