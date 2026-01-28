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

namespace XVisual {

CVCropItem::CVCropItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(gWidget,contextMenu, parent)
{
	configItem("CVCrop");
	XLOG_INFO("CVCropItem::CVCropItem, uuid = " + uuid, CURRENT_THREAD_ID);
}
void CVCropItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	//Done 使用CropDialog获取用户输入的裁剪区域坐标和尺寸
	CVCropHandle* cvHandle = dynamic_cast<CVCropHandle*>(xHandle);
	cv::Rect regionOfInterest = cvHandle->getRoI();
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
		cvHandle->setRoI(regionOfInterest);
	}
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}
void CVCropItem::xOperate()
{
	xHandle->xOperate();
	//Source& s = xHandle->getSources();
	Dest& d = xHandle->getDests();
	// cv::Mat image = GET_MEMBER_WITH_TYPE_STR(s, cv::Mat, "image");
	cv::Mat croppedImage = GET_MEMBER_WITH_TYPE_STR(d, cv::Mat, "croppedImage");
	// std::string imageFilename = uniqueName.toStdString() + "_croppedImage.png";
	// Done,Add code for sending show image signals to display images on SideWidgets
	std::string filename = uniqueName;
	if (croppedImage.empty())
	{
		XLOG_INFO("CVCropItem::xOperate, croppedImage is EMPTY ... ", CURRENT_THREAD_ID);
	}
	else
	{
		XLOG_INFO("CVCropItem::xOperate, croppedImage is NOT empty ... ", CURRENT_THREAD_ID);
	}
	emit showImageSignal(filename, croppedImage, this);
}

REGISTER_ITEM(CVCrop);

} // namespace XVisual
