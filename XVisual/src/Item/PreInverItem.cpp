#include "Common/LoggerInstance.h"
#include "Common/OpenCVHeaders.h"
#include <QFileDialog>
#include <QDebug>
#include <optional>
#include <QInputDialog>
#include "Common/XThreadMacro.h"
#include "Item/PreInverItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "CustomDialog/CropDialog.h"
#include "MainWindow/GraphicsWidget.h"
#include "Handle/PreInverHandle.h"
#include "Common/StrUtils.h"
#include "ParamWidgetDialog/ComboDialog.h"

PreInverItem::PreInverItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(gWidget, contextMenu, parent)
{
	std::string classNameStr = "PreInver";
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
	XLOG_INFO("ImagePreItem::ImagePreItem, uuid = " + uuid, CURRENT_THREAD_ID);
	// 创建handle
	xHandle = HandleRegistry::createObject(classNameStr);
	xHandle->setUuidConsistentWithItem(uuid);
	initParams();
}
QPixmap PreInverItem::image()
{
	XBaseItem::myPolygon = myPolygon;
	return XBaseItem::image();
}
void PreInverItem::debug()
{
	qDebug() << "PreInverItem::debug() ... ";
	qDebug() << "PreInverItem::boundingRect() " << boundingRect();
	qDebug() << "PreInverItem::boundingRect().width() " << boundingRect().width();
	qDebug() << "PreInverItem::boundingRect().height() " << boundingRect().height();
	qDebug() << "PreInverItem::uuid " << QString::fromStdString(uuid);
}
void PreInverItem::initParams()
{
	//do nothing
}
void PreInverItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}
void PreInverItem::xOperate()
{
	xHandle->xOperate();
	Dest& d = xHandle->getDests();
	cv::Mat outputImage = GET_MEMBER_WITH_TYPE_STR(d, cv::Mat, "restoredImage");
	// Done,Add code for sending show image signals to display images on SideWidgets
	std::string filename = uniqueName;
	emit showImageSignal(filename, outputImage, this);
}


REGISTER_ITEM(PreInver);