#include "Common/LoggerInstance.h"
#include "Common/OpenCVHeaders.h"
#include <QFileDialog>
#include <QDebug>
#include <optional>
#include <QInputDialog>
#include "Common/XThreadMacro.h"
#include "Item/ImagePreItem.h"
#include "ItemWidget/ItemWidget.h"
#include "GlobalStorage/ItemManager.h"
#include "CustomDialog/CropDialog.h"
#include "MainWindow/GraphicsWidget.h"
#include "Handle/ImagePreHandle.h"
#include "Common/StrUtils.h"
#include "ParamWidgetDialog/ComboDialog.h"

ImagePreItem::ImagePreItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent)
	: XBaseItem(gWidget, contextMenu, parent)
{
	configItem("ImagePre");
	XLOG_INFO("ImagePreItem::ImagePreItem, uuid = " + uuid, CURRENT_THREAD_ID);
}
void ImagePreItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	//Done 使用ComboDialog获取用户输入的图像预处理参数
	ImagePreHandle* imagePreHandle = dynamic_cast<ImagePreHandle*>(xHandle);
	PreParam mParam = imagePreHandle->getImagePreParams();
	ComboDialog dialog;
	dialog.setParams(mParam);
	if (dialog.exec() == QDialog::Accepted)
	{
		dialog.getParams(mParam);
		imagePreHandle->setImagePreParams(mParam);
#ifdef DEBUG_PRINT 
		printPreParam(mParam);
#endif 
	}
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}
void ImagePreItem::xOperate()
{
	xHandle->xOperate();
	Dest& d = xHandle->getDests();
	cv::Mat outputImage = GET_MEMBER_WITH_TYPE_STR(d, cv::Mat, "outputImage");
	// Done,Add code for sending show image signals to display images on SideWidgets
	std::string filename = uniqueName;
	emit showImageSignal(filename, outputImage, this);
}
#ifdef DEBUG_PRINT
void ImagePreItem::printPreParam(const PreParam& mParam)
{
	if ("Normalize01" == mParam.preTag)
	{
		XLOG_INFO(" === ImagePreItem::mouseDoubleClickEvent === PreParam.preTag = " + mParam.preTag, CURRENT_THREAD_ID);
		std::unordered_map<std::string, std::any> preParams = std::any_cast<std::unordered_map<std::string, std::any>>(mParam.params);
		float scale = std::any_cast<float>(preParams["scale"]);
		XLOG_INFO(" === ImagePreItem::mouseDoubleClickEvent === scale = " + std::to_string(scale), CURRENT_THREAD_ID);
	}
	else if ("Resize" == mParam.preTag)
	{
		XLOG_INFO(" === ImagePreItem::mouseDoubleClickEvent === PreParam.preTag = " + mParam.preTag, CURRENT_THREAD_ID);
		std::unordered_map<std::string, std::any> preParams = std::any_cast<std::unordered_map<std::string, std::any>>(mParam.params);
		int targetWidth = std::any_cast<int>(preParams["targetWidth"]);
		int targetHeight = std::any_cast<int>(preParams["targetHeight"]);
		XLOG_INFO(" === ImagePreItem::mouseDoubleClickEvent === targetWidth = " + std::to_string(targetWidth), CURRENT_THREAD_ID);
		XLOG_INFO(" === ImagePreItem::mouseDoubleClickEvent === targetHeight = " + std::to_string(targetHeight), CURRENT_THREAD_ID);
	}
	else if ("ResizePaste" == mParam.preTag)
	{
		XLOG_INFO(" === ImagePreItem::mouseDoubleClickEvent === PreParam.preTag = " + mParam.preTag, CURRENT_THREAD_ID);
		std::unordered_map<std::string, std::any> preParams = std::any_cast<std::unordered_map<std::string, std::any>>(mParam.params);
		int dstWidth = std::any_cast<int>(preParams["dstWidth"]);
		int dstHeight = std::any_cast<int>(preParams["dstHeight"]);
		XLOG_INFO(" === ImagePreItem::mouseDoubleClickEvent === dstWidth = " + std::to_string(dstWidth), CURRENT_THREAD_ID);
		XLOG_INFO(" === ImagePreItem::mouseDoubleClickEvent === dstHeight = " + std::to_string(dstHeight), CURRENT_THREAD_ID);
	}
}
#endif 

REGISTER_ITEM(ImagePre);