#include "Common/LoggerInstance.h"
#include "Common/OpenCVHeaders.h"
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
#include "Handle/CVCropHandle.h"
#include "Common/StrUtils.h"

CVCropItem::CVCropItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(gWidget,contextMenu, parent)
{
	std::string classNameStr = "CVCrop";
	QString classNameQStr = QString::fromStdString(classNameStr);
	createUniqueName(classNameQStr);
	setObjectName(uniqueName);
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
	setEditText(uniqueName);
	XLOG_INFO("CVCropItem::CVCropItem, uuid = " + uuid, CURRENT_THREAD_ID);
	// 创建handle
	xHandle = HandleRegistry::createObject(classNameStr);
	xHandle->setUuidConsistentWithItem(uuid);
	initParams();
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
void CVCropItem::initParams()
{
	CVCropHandle* cvHandle = dynamic_cast<CVCropHandle*>(xHandle);
	regionOfInterest = cvHandle->getRoI();
}
void CVCropItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	//Done 使用CropDialog获取用户输入的裁剪区域坐标和尺寸
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
		CVCropHandle* cvHandle = dynamic_cast<CVCropHandle*>(xHandle);
		cvHandle->setRoI(regionOfInterest);
	}
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}
void CVCropItem::xOperate()
{
	xHandle->xOperate();
	Source& s = xHandle->getSources();
	Dest& d = xHandle->getDests();
	// cv::Mat image = GET_MEMBER_WITH_TYPE_STR(s, cv::Mat, "image");
	cv::Mat croppedImage = GET_MEMBER_WITH_TYPE_STR(d, cv::Mat, "croppedImage");
	// std::string imageFilename = uniqueName.toStdString() + "_croppedImage.png";
	// Done,Add code for sending show image signals to display images on SideWidgets
	std::string filename = uniqueName.toStdString();
	emit showImageSignal(filename, croppedImage, this);
}
void CVCropItem::createUniqueName(const QString& classNameQStr)
{
	uniqueName = ItemManager::instance().getUniqueItemName(classNameQStr);
}
REGISTER_ITEM(CVCrop);