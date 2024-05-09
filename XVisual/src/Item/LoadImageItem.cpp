#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Common/OpenCVHeaders.h"
#include "Item/LoadImageItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "MainWindow/SideWidget.h"
#include <QFileDialog>
#include <QDebug>
#include <optional>
#include "MainWindow/GraphicsWidget.h"
#include "Handle/LoadImageHandle.h"
#include "Common/StrUtils.h"

LoadImageItem::LoadImageItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(gWidget, contextMenu, parent)
{

	std::string classNameStr = "LoadImage";
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
	XLOG_INFO("LoadImageItem::LoadImageItem, uuid = " + uuid, CURRENT_THREAD_ID);
	// 创建xHandle
	xHandle = HandleRegistry::createObject(classNameStr);
	xHandle->setUuidConsistentWithItem(uuid);
	initParams();
}
QPixmap LoadImageItem::image()
{
	XBaseItem::myPolygon = myPolygon;
	return XBaseItem::image();
}
void LoadImageItem::debug()
{
	qDebug() << "LoadImageItem::debug() ... ";
	qDebug() << "LoadImageItem::boundingRect() " << boundingRect();
	qDebug() << "LoadImageItem::boundingRect().width() " << boundingRect().width();
	qDebug() << "LoadImageItem::boundingRect().height() " << boundingRect().height();
	qDebug() << "LoadImageItem::uuid " << QString::fromStdString(uuid);
}
void LoadImageItem::initParams()
{

}
void LoadImageItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	Source& s = xHandle->getSources();
	QString lastPath = QString::fromStdString(GET_MEMBER_WITH_TYPE_STR(s, std::string, "imagePath"));
	QString defaultPath = lastPath.isEmpty() ? QDir::homePath() : lastPath;
	// Handle the double-click event here
	QString imagePath = QFileDialog::getOpenFileName(nullptr, "Select Image", defaultPath, "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
	if (!imagePath.isEmpty())
	{
		// 保存当前选中的文件路径
		XLOG_INFO("LoadImageItem::mouseDoubleClickEvent, imagePath is not empty ...", CURRENT_THREAD_ID);
		REGISTER_MEMBER_STR(s, "imagePath", imagePath.toStdString());
	}
	else
	{
		XLOG_INFO("LoadImageItem::mouseDoubleClickEvent, imagePath is empty ...", CURRENT_THREAD_ID);
	}
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}
void LoadImageItem::xOperate()
{
	xHandle->xOperate();
	Source& s = xHandle->getSources();
	Dest& d = xHandle->getDests();

	//std::string imagePath = GET_MEMBER_WITH_TYPE_STR(s, std::string, "imagePath");
	cv::Mat image = GET_MEMBER_WITH_TYPE_STR(d, cv::Mat, "image");
	//Done,Add code for sending show image signals to display images on SideWidget
	std::string filename = uniqueName;
	emit showImageSignal(filename, image, this);
}

REGISTER_ITEM(LoadImage);