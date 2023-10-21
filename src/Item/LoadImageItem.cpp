#include "Item/LoadImageItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QFileDialog>
#include <QDebug>
#include <optional>

LoadImageItem::LoadImageItem(QMenu* contextMenu, QGraphicsItem* parent)
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
Source& LoadImageItem::getSources()
{
	qDebug() << "Source& LoadImageItem::getSources() ";
	return sources;
}
Dest& LoadImageItem::getDests()
{
	qDebug() << "Dest& LoadImageItem::getDests() ";
	return dests;
}
void LoadImageItem::initParameters()
{
	std::string imagePath;
	REGISTER_MEMBER(sources, imagePath, imagePath);
	cv::Mat image;
	REGISTER_MEMBER(dests, image, image);
}
void LoadImageItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	// Handle the double-click event here
	QString imagePath = QFileDialog::getOpenFileName(nullptr, "Select Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
	if (!imagePath.isEmpty())
	{
		REGISTER_MEMBER(sources, imagePath, imagePath.toStdString());
		cv::Mat image = cv::imread(imagePath.toStdString());
		REGISTER_MEMBER(dests, image, image);
		qDebug() << imagePath;
		if (image.empty())
		{
			qDebug() << "image is empty";
		}
		else
		{
			qDebug() << "image is not empty";
		}
	}
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}
void LoadImageItem::ItemXOP(Source& sources, Dest& dests)
{
	cv::Mat InputImage = GET_MEMBER_WITH_TYPE(dests, cv::Mat, image);

}
void LoadImageItem::createUniqueName()
{
	uniqueName = ItemManager::instance().getUniqueItemName("LoadImage");
	setObjectName(uniqueName);
}
REGISTER_CLASS(LoadImage);