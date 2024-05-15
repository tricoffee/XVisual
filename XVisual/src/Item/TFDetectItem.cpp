#include "Common/LoggerInstance.h"
#include "Common/OpenCVHeaders.h"
#include <QFileDialog>
#include <QDebug>
#include <optional>
#include <QInputDialog>
#include "Common/XThreadMacro.h"
#include "Item/TFDetectItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "CustomDialog/CropDialog.h"
#include "MainWindow/GraphicsWidget.h"
#include "Handle/TFDetectHandle.h"
#include "Common/StrUtils.h"
#include "Common/DetectResult.h"

TFDetectItem::TFDetectItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(gWidget, contextMenu, parent)
{
	std::string classNameStr = "TFDetect";
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
	XLOG_INFO("TFDetectItem::TFDetectItem, uuid = " + uuid, CURRENT_THREAD_ID);
	// 创建handle
	xHandle = HandleRegistry::createObject(classNameStr);
	xHandle->setUuidConsistentWithItem(uuid);
	initParams();
}
QPixmap TFDetectItem::image()
{
	XBaseItem::myPolygon = myPolygon;
	return XBaseItem::image();
}
void TFDetectItem::debug()
{
	qDebug() << "TFDetectItem::debug() ... ";
	qDebug() << "TFDetectItem::boundingRect() " << boundingRect();
	qDebug() << "TFDetectItem::boundingRect().width() " << boundingRect().width();
	qDebug() << "TFDetectItem::boundingRect().height() " << boundingRect().height();
	qDebug() << "TFDetectItem::uuid " << QString::fromStdString(uuid);
}
void TFDetectItem::initParams()
{

}
void TFDetectItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	// Using Dialogs to Retrieve SavedModel
	Source& s = xHandle->getSources();
	QString lastPath = QString::fromStdString(GET_MEMBER_WITH_TYPE_STR(s, std::string, "savedModelPath"));
	QString defaultPath = lastPath.isEmpty() ? QDir::homePath() : lastPath;

	// Open direcatory to select savedModel
	QString savedModelDir = QFileDialog::getExistingDirectory(nullptr, "Select savedModel", defaultPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (!savedModelDir.isEmpty())
	{
		// Save the currently selected file path
		XLOG_INFO("TFDetectItem::mouseDoubleClickEvent, savedModelDir is not empty ...", CURRENT_THREAD_ID);
		XLOG_INFO("TFDetectItem::mouseDoubleClickEvent, savedModelDir = " + savedModelDir.toStdString(), CURRENT_THREAD_ID);
		REGISTER_MEMBER_STR(s, "savedModelPath", savedModelDir.toStdString());
	}
	else
	{
		XLOG_INFO("TFDetectItem::mouseDoubleClickEvent, savedModelDir is EMPTY ...", CURRENT_THREAD_ID);
	}
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);

}
void TFDetectItem::xOperate()
{
	xHandle->xOperate();
	//Dest& d = xHandle->getDests();
	//std::vector<DetectResult> detectResults = GET_MEMBER_WITH_TYPE_STR(d, std::vector<DetectResult>, "detectResults");
}

REGISTER_ITEM(TFDetect);