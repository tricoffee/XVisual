#include "Item/CVCropItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "CustomDialog/CropDialog.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QFileDialog>
#include <QDebug>
#include <optional>
#include <QInputDialog>


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
	cv::Mat croppedImage;
	REGISTER_MEMBER(dests, croppedImage, croppedImage);

	regionOfInterest.x = 0;
	regionOfInterest.y = 0;
	regionOfInterest.width = -1;
	regionOfInterest.height = -1;
}
void CVCropItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	//TO-DO 使用CropDialog获取用户输入的裁剪区域坐标和尺寸
	CropDialog dialog;
	// 使用 setX, setY, setWidth 和 setHeight 来设置初始值
	dialog.setX(regionOfInterest.x);
	dialog.setY(regionOfInterest.y);
	dialog.setWidth(regionOfInterest.width);
	dialog.setHeight(regionOfInterest.height);
	if (dialog.exec() == QDialog::Accepted) 
	{
		int x = dialog.getX();
		int y = dialog.getY();
		int width = dialog.getWidth();
		int height = dialog.getHeight();
		// 定义裁剪区域(x, y, width, height)
		regionOfInterest = cv::Rect(x, y, width, height);
	}
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}
void CVCropItem::ItemXOP(Source& sources, Dest& dests)
{
	cv::Mat img = std::any_cast<cv::Mat>(GET_MEMBER(sources, img));
	int imageWidth = -1;
	int imageHeight = -1;
	if (!img.empty())
	{
		imageWidth = img.cols;
		imageWidth = img.rows;
		qDebug() << "图像宽度: " << imageWidth << "，图像高度: " << imageWidth;
	}
	else
	{
		qDebug() << "img is empty";
	}
	cv::Mat croppedImage;
	if (imageWidth != -1 && imageHeight != -1 && regionOfInterest.width != -1 &&
		regionOfInterest.height != -1 &&
		regionOfInterest.width <= imageWidth &&
		regionOfInterest.height <= imageHeight)
	{
		croppedImage = img(regionOfInterest).clone();
	}
	else
	{
		croppedImage = img.clone();
		qDebug() << "Region of Intersest is invalid, please set again";
	}
	REGISTER_MEMBER(dests, croppedImage, croppedImage);
}
void CVCropItem::createUniqueName()
{
	uniqueName = ItemManager::instance().getUniqueItemName("CVCrop");
	setObjectName(uniqueName);
}
REGISTER_CLASS(CVCrop);