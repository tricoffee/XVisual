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
#include "Common/FileUtils.h"
#include "GlobalStorage/GlobalVariable.h"
#include "Common/QFileUtils.h"
#include "Common/DeletePtrMacro.h"
#include <QTimer>

TFDetectItem::TFDetectItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseFItem(gWidget, contextMenu, parent)
{
	configItem("TFDetect");
	XLOG_INFO("TFDetectItem::TFDetectItem, uuid = " + uuid, CURRENT_THREAD_ID);
}
TFDetectItem::~TFDetectItem()
{

}
void TFDetectItem::fileCopyReadyUpdateFunc(FileCopyData data)
{
	if (data.fileReady)
	{
		fileCopyData.fileReady = true;
		XLOG_INFO("TFDetectItem::fileCopyReadyUpdateSlot, fileCopyData.fileReady is true ", CURRENT_THREAD_ID);
		XLOG_INFO("TFDetectItem::fileCopyReadyUpdateSlot, dstFilePath = " + fileCopyData.dstFilePath.toStdString(), CURRENT_THREAD_ID);
		QString baseFilePath;
		qGetBaseFileName(fileCopyData.dstFilePath, baseFilePath);
		Source& s = xHandle->getSources();
		REGISTER_MEMBER_STR(s, "savedModelPath", baseFilePath.toStdString());
	}
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
		std::string currentParentDir;
		getParentPathStr(savedModelDir.toStdString(), currentParentDir);
		if (currentParentDir == globalWorkSpaceDir)
		{
			// Save the currently selected file path
			XLOG_INFO("TFDetectItem::mouseDoubleClickEvent, savedModelDir is not empty ...", CURRENT_THREAD_ID);
			XLOG_INFO("TFDetectItem::mouseDoubleClickEvent, savedModelDir = " + savedModelDir.toStdString(), CURRENT_THREAD_ID);
			REGISTER_MEMBER_STR(s, "savedModelPath", savedModelDir.toStdString());
		}
		else
		{
			XLOG_INFO("TFDetectItem::mouseDoubleClickEvent, currentParentDir != globalWorkSpaceDir", CURRENT_THREAD_ID);
			constructDataAndDeliver(savedModelDir);
		}
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