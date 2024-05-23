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
	// ����Ҫ��XBaseFItem����configItem("XBase"); ��ΪXBaseFItem�ǻ��಻����ʵ��ҵ��
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
	// ��������͵�����
	QString baseFileName;
	qGetBaseFileName(srcFilePath, baseFileName);
	XLOG_INFO("XBaseFItem::constructDataAndDeliver, baseFileName = " + baseFileName.toStdString(), CURRENT_THREAD_ID);
	QString dstFilePath = QString::fromStdString(globalWorkSpaceDir + "/" + baseFileName.toStdString());
	XLOG_INFO("XBaseFItem::constructDataAndDeliver, dstFilePath = " + dstFilePath.toStdString(), CURRENT_THREAD_ID);
	fileCopyData.srcFilePath = srcFilePath;
	fileCopyData.dstFilePath = dstFilePath;
	fileCopyData.fileReady = false;
	// ������������
	asynProcessor = new AFProcessor();
	// ���������߳�
	asynFileCopyThread = new QThread();
	//
	asynEmitter = new AFEmitter();
	// �����������ƶ��������߳�
	asynProcessor->moveToThread(asynFileCopyThread);
	// �����źźͲۣ�ʹ�� Qt::QueuedConnection
	QObject::connect(asynEmitter, &AFEmitter::copyFileSignal, asynProcessor, &AFProcessor::copyFileSlot, Qt::QueuedConnection);
	// ����fileCopyReadyUpdateSignal�����߳��еĲۺ���, ʹ�� Qt::QueuedConnection
	QObject::connect(asynProcessor, &AFProcessor::fileCopyReadyUpdateSignal, this, &XBaseFItem::fileCopyReadyUpdateSlot, Qt::QueuedConnection);
	// ���������߳�
	asynFileCopyThread->start();
	// �����ź�
	emit asynEmitter->copyFileSignal(fileCopyData);
}
// ͨ�����ѻ���Ĳۺ������Ϊ�麯��, ���ⶨ��һ���麯��XBaseFItem::fileCopyReadyUpdateFunc, Ȼ���������麯��XBaseFItem::fileCopyReadyUpdateFunc
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