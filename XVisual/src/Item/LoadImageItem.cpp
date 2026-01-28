#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Common/OpenCVHeaders.h"
#include "Item/LoadImageItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "MainWindow/SideWidget.h"
#include <QFileDialog>
#include "MainWindow/GraphicsWidget.h"
#include "Handle/LoadImageHandle.h"
#include "Common/StrUtils.h"
#include "Common/FileUtils.h"
#include "GlobalStorage/GlobalVariable.h"
#include "Common/QFileUtils.h"
#include "Common/DeletePtrMacro.h"
#include <QTimer>

namespace XVisual {

	LoadImageItem::LoadImageItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
		: XBaseFItem(gWidget, contextMenu, parent)
	{
		configItem("LoadImage");
		XLOG_INFO("LoadImageItem::LoadImageItem, uuid = " + uuid, CURRENT_THREAD_ID);
	}
	LoadImageItem::~LoadImageItem()
	{

	}
	void LoadImageItem::fileCopyReadyUpdateFunc(FileCopyData data)
	{
		if (data.fileReady)
		{
			fileCopyData.fileReady = true;
			XLOG_INFO("LoadImageItem::fileCopyReadyUpdateSlot, fileCopyData.fileReady is true ", CURRENT_THREAD_ID);
			XLOG_INFO("LoadImageItem::checkFileReady, dstFilePath = " + fileCopyData.dstFilePath.toStdString(), CURRENT_THREAD_ID);
			QString baseFilePath;
			qGetBaseFileName(fileCopyData.dstFilePath, baseFilePath);
			Source& s = xHandle->getSources();
			REGISTER_MEMBER_STR(s, "imagePath", baseFilePath.toStdString());
		}
	}
	void LoadImageItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
	{
		Source& s = xHandle->getSources();
		QString lastPath = QString::fromStdString(GET_MEMBER_WITH_TYPE_STR(s, std::string, "imagePath"));
		QString defaultPath = lastPath.isEmpty() ? QDir::homePath() : lastPath;
		// Handle the double-click event here
		QString imagePath = QFileDialog::getOpenFileName(nullptr, "Select Image", defaultPath, "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
		if (!imagePath.isEmpty())
		{
			std::string currentParentDir;
			getParentPathStr(imagePath.toStdString(), currentParentDir);
			if (currentParentDir == globalWorkSpaceDir)
			{
				// 保存当前选中的文件路径
				XLOG_INFO("LoadImageItem::mouseDoubleClickEvent, imagePath is NOT empty ...", CURRENT_THREAD_ID);
				REGISTER_MEMBER_STR(s, "imagePath", imagePath.toStdString());
			}
			else
			{
				XLOG_INFO("LoadImageItem::mouseDoubleClickEvent, currentParentDir != globalWorkSpaceDir", CURRENT_THREAD_ID);
				constructDataAndDeliver(imagePath);
			}
		}
		else
		{
			XLOG_INFO("LoadImageItem::mouseDoubleClickEvent, imagePath is EMPTY ...", CURRENT_THREAD_ID);
		}
		// Call the base class event handler
		QGraphicsPolygonItem::mouseDoubleClickEvent(event);
	}
	void LoadImageItem::xOperate()
	{
		xHandle->xOperate();
		Source& s = xHandle->getSources();
		Dest& d = xHandle->getDests();

		//std::string imagePath = GET_MEMBER_WITH_TYPE_STR(s, std::string, "imagePath");
		cv::Mat image = GET_MEMBER_WITH_TYPE_STR(d, cv::Mat, "image");
		//Done,Add code for sending show image signals to display images on SideWidget
		std::string filename = uniqueName;
		emit showImageSignal(filename, image, this);
	}

	REGISTER_ITEM(LoadImage);

} // namespace XVisual 