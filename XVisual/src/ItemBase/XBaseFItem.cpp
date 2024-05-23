#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "ItemBase/XBaseFItem.h"
#include "MainWindow/GraphicsWidget.h"
#include <QThread>
#include "GlobalStorage/GlobalVariable.h"
#include "Common/QFileUtils.h"
#include "Common/DeletePtrMacro.h"
#include <QTimer>

XBaseFItem::XBaseFItem(GraphicsWidget* gWidget, QMenu* contextMenu, QGraphicsItem* parent):XBaseItem(gWidget, contextMenu, parent)
{
	// 不需要在XBaseFItem调用configItem("XBase"); 因为XBaseFItem是基类不负责实际业务
	XLOG_INFO("XBaseFItem::XBaseFItem, uuid = " + uuid, CURRENT_THREAD_ID);
	// init asynchronousData
	asynFileCopyThread = nullptr;
	asynProcessor = nullptr;
	asynEmitter = nullptr;
}
XBaseFItem::~XBaseFItem()
{
	SAFE_DELETE(asynFileCopyThread);
	SAFE_DELETE(asynProcessor);
	SAFE_DELETE(asynEmitter);
}
void XBaseFItem::constructDataAndDeliver(const QString& srcFilePath)
{
	// 构造待发送的数据
	QString baseFileName;
	qGetBaseFileName(srcFilePath, baseFileName);
	XLOG_INFO("XBaseFItem::constructDataAndDeliver, baseFileName = " + baseFileName.toStdString(), CURRENT_THREAD_ID);
	QString dstFilePath = QString::fromStdString(globalWorkSpaceDir + "/" + baseFileName.toStdString());
	XLOG_INFO("XBaseFItem::constructDataAndDeliver, dstFilePath = " + dstFilePath.toStdString(), CURRENT_THREAD_ID);
	fileCopyData.srcFilePath = srcFilePath;
	fileCopyData.dstFilePath = dstFilePath;
	fileCopyData.fileReady = false;
	// 创建工作对象
	asynProcessor = new AFProcessor();
	// 创建工作线程
	asynFileCopyThread = new QThread();
	//
	asynEmitter = new AFEmitter();
	// 将工作对象移动到工作线程
	asynProcessor->moveToThread(asynFileCopyThread);
	// 连接信号和槽，使用 Qt::QueuedConnection
	QObject::connect(asynEmitter, &AFEmitter::copyFileSignal, asynProcessor, &AFProcessor::copyFileSlot, Qt::QueuedConnection);
	// 连接fileCopyReadyUpdateSignal到主线程中的槽函数, 使用 Qt::QueuedConnection
	QObject::connect(asynProcessor, &AFProcessor::fileCopyReadyUpdateSignal, this, &XBaseFItem::fileCopyReadyUpdateSlot, Qt::QueuedConnection);
	// 启动工作线程
	asynFileCopyThread->start();
	// 发射信号
	emit asynEmitter->copyFileSignal(fileCopyData);
}
// 通常不把基类的槽函数设计为虚函数, 另外定义一个虚函数XBaseFItem::fileCopyReadyUpdateFunc, 然后调用这个虚函数XBaseFItem::fileCopyReadyUpdateFunc
void XBaseFItem::fileCopyReadyUpdateSlot(FileCopyData data)
{
	fileCopyReadyUpdateFunc(data);
}
void XBaseFItem::fileCopyReadyUpdateFunc(FileCopyData data)
{

}
void XBaseFItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	// Call the base class event handler
	QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}

REGISTER_ITEM(XBaseF);