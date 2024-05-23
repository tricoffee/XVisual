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
	configItem("DrawBox");
	XLOG_INFO("DrawBoxItem::DrawBoxItem, uuid = " + uuid, CURRENT_THREAD_ID);
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