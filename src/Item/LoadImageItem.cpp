#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Common/OpenCVHeaders.h"
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
#include "Item/LoadImageItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "MainWindow/SideWidget.h"
#include <QFileDialog>
#include <QDebug>
#include <optional>
#include "MainWindow/GraphicsWidget.h"

LoadImageItem::LoadImageItem(QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(contextMenu, parent)
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
	XLOG_INFO("LoadImageItem::LoadImageItem, uuid = "+ uuid, CURRENT_THREAD_ID);
}

LoadImageItem::LoadImageItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(gWidget, contextMenu, parent)
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
	XLOG_INFO("LoadImageItem::LoadImageItem, uuid = " + uuid, CURRENT_THREAD_ID);
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
void LoadImageItem::initParameters()
{
	isSourceFromOutside = true;
	std::string imagePath;
	REGISTER_MEMBER(sources, imagePath, imagePath);
	REGISTER_TYPE(sources, imagePath, imagePath);
	cv::Mat image;
	REGISTER_MEMBER(dests, image, image);
	REGISTER_TYPE(dests, image, image);
}
void LoadImageItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	// Handle the double-click event here
	QString imagePath = QFileDialog::getOpenFileName(nullptr, "Select Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
	if (!imagePath.isEmpty())
	{
		XLOG_INFO("LoadImageItem::mouseDoubleClickEvent, imagePath is not empty ...", CURRENT_THREAD_ID);
		REGISTER_MEMBER(sources, imagePath, imagePath.toStdString());
	}
	else
	{
		XLOG_INFO("LoadImageItem::mouseDoubleClickEvent, imagePath is empty ...", CURRENT_THREAD_ID);
	}
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}
void LoadImageItem::ItemXOP()
{
	
	XLOG_INFO("LoadImageItem::ItemXOP ...", CURRENT_THREAD_ID);
	std::string imagePath = GET_MEMBER_WITH_TYPE_STR(sources, std::string, "imagePath");
	XLOG_INFO("LoadImageItem::ItemXOP, imagePath = "+ imagePath, CURRENT_THREAD_ID);
	cv::Mat image;
	if (!imagePath.empty())
	{
		image = cv::imread(imagePath);
		REGISTER_MEMBER(dests, image, image);
		XLOG_INFO("LoadImageItem::ItemXOP "+imagePath, CURRENT_THREAD_ID);
		if (image.empty())
		{
			XLOG_INFO("LoadImageItem::ItemXOP  image is empty", CURRENT_THREAD_ID);
		}
		else
		{
			XLOG_ERROR("LoadImageItem::ItemXOP image is not empty", CURRENT_THREAD_ID);
		}
	}
	else
	{
		XLOG_INFO("LoadImageItem::ItemXOP  imagePath is empty", CURRENT_THREAD_ID);
	}
	// TO-DO,Add code for sending show image signals to display images on SideWidget
	std::string filename = uniqueName.toStdString();
	emit showImageSignal(filename, image, this);
}
void LoadImageItem::createUniqueName()
{
	uniqueName = ItemManager::instance().getUniqueItemName("LoadImage");
	setObjectName(uniqueName);
}
REGISTER_CLASS(LoadImage);