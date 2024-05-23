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
	configItem("PreInver");
	XLOG_INFO("PreInverItem::PreInverItem, uuid = " + uuid, CURRENT_THREAD_ID);
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