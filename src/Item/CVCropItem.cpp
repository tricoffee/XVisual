#include "Common/LoggerInstance.h"
#include "Common/OpenCVHeaders.h"
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
#include <QFileDialog>
#include <QDebug>
#include <optional>
#include <QInputDialog>
#include "Common/XThreadMacro.h"
#include "Item/CVCropItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "CustomDialog/CropDialog.h"
#include "MainWindow/GraphicsWidget.h"

CVCropItem::CVCropItem(QMenu* contextMenu, QGraphicsItem* parent)
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
	XLOG_INFO("CVCropItem::CVCropItem, uuid = " + uuid,CURRENT_THREAD_ID);
}

CVCropItem::CVCropItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(gWidget,contextMenu, parent)
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
	XLOG_INFO("CVCropItem::CVCropItem, uuid = " + uuid, CURRENT_THREAD_ID);
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
void CVCropItem::initParameters()
{
	isSourceFromOutside = false;
	cv::Mat image;
	REGISTER_MEMBER(sources, image, image);
	REGISTER_TYPE(sources, image, image);
	cv::Mat croppedImage;
	REGISTER_MEMBER(dests, croppedImage, croppedImage);
	REGISTER_TYPE(dests, croppedImage, croppedImage);
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
void CVCropItem::ItemXOP()
{
	cv::Mat image = GET_MEMBER_WITH_TYPE_STR(sources, cv::Mat,"image");
	int imageWidth = -1;
	int imageHeight = -1;
	if (!image.empty())
	{
		imageWidth = image.cols;
		imageHeight = image.rows;
		XLOG_INFO("CVCropItem::ItemXOP 图像宽度: " + std::to_string(imageWidth), CURRENT_THREAD_ID);
		XLOG_INFO("CVCropItem::ItemXOP 图像高度: " + std::to_string(imageHeight), CURRENT_THREAD_ID);
	}
	else
	{
		XLOG_INFO("CVCropItem::ItemXOP, image is empty ", CURRENT_THREAD_ID);
	}
	cv::Mat croppedImage;
	std::string imageFilename = uniqueName.toStdString() + "_croppedImage.png";
	if (imageWidth != -1 && imageHeight != -1 && regionOfInterest.width != -1 &&
		regionOfInterest.height != -1 &&
		regionOfInterest.width <= imageWidth &&
		regionOfInterest.height <= imageHeight)
	{
		XLOG_INFO("regionOfInterest.width = " + std::to_string(regionOfInterest.width), CURRENT_THREAD_ID);
		XLOG_INFO("regionOfInterest.height = " + std::to_string(regionOfInterest.height), CURRENT_THREAD_ID);
		croppedImage = image(regionOfInterest).clone();
		XLOG_INFO("CVCropItem::ItemXOP, croppedImage is generated successfully. ", CURRENT_THREAD_ID);
	}
	else
	{
		croppedImage = image.clone();
		XLOG_INFO("CVCropItem::ItemXOP, Region of Intersest is invalid, the cropped image is equivalent to the original image ", CURRENT_THREAD_ID);
	}
	XLOG_SAVE_IMAGE(croppedImage, imageFilename, CURRENT_THREAD_ID);
	REGISTER_MEMBER(dests, croppedImage, croppedImage);
	// TO-DO,Add code for sending show image signals to display images on SideWidgets
	std::string filename = uniqueName.toStdString();
	emit showImageSignal(filename, croppedImage, this);
}
void CVCropItem::createUniqueName()
{
	uniqueName = ItemManager::instance().getUniqueItemName("CVCrop");
	setObjectName(uniqueName);
}
REGISTER_CLASS(CVCrop);