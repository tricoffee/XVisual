#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Common/OpenCVHeaders.h"
#include "Item/RevertBoxItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "MainWindow/SideWidget.h"
#include <QFileDialog>
#include <QDebug>
#include <optional>
#include "MainWindow/GraphicsWidget.h"
#include "Handle/RevertBoxHandle.h"
#include "Common/StrUtils.h"

namespace XVisual {

RevertBoxItem::RevertBoxItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(gWidget, contextMenu, parent)
{
	configItem("RevertBox");
	XLOG_INFO("RevertBoxItem::RevertBoxItem, uuid = " + uuid, CURRENT_THREAD_ID);
}
void RevertBoxItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}
void RevertBoxItem::xOperate()
{
	xHandle->xOperate();
}

REGISTER_ITEM(RevertBox);

} // namespace XVisual