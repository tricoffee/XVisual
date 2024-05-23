#include "Common/AsynchronousFileProcessor.h"
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QThread>
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Common/QFileUtils.h"
#include <QMetaType>

std::string AFProcessor::getQCurrentIdStr()
{
	Qt::HANDLE threadId = QThread::currentThreadId();
	std::ostringstream oss;
	oss << threadId;
	return oss.str();
}

AFProcessor::AsynchronousFileProcessor()
{
	// ×¢²áÀàÐÍ
	qRegisterMetaType<FileCopyData>("FileCopyData");
}

void AFProcessor::copyFileSlot(FileCopyData data)
{
	// TODO,Copy File
	// qDebug() << "File copied in thread:" << QThread::currentThread();

	std::string QIdStr = getQCurrentIdStr();

	XLOG_INFO("AsynchronousFileProcessor::copyFileSlot," + data.srcFilePath.toStdString()
		+ ", QIdStr = " + QIdStr, CURRENT_THREAD_ID);

	QFileInfo fileInfo(data.srcFilePath);
	if (fileInfo.exists()) 
	{
		if (fileInfo.isFile()) 
		{
			//qDebug() << data.srcFilePath << "is a File.";
			
			XLOG_INFO("AsynchronousFileProcessor::copyFileSlot," + data.srcFilePath.toStdString()
				+ " is a File, QIdStr = " + QIdStr, CURRENT_THREAD_ID);
			bool copyFlag = qCopyFile(data.srcFilePath, data.dstFilePath);
			if (copyFlag)
			{
				data.fileReady = true;
			}
			else
			{
				data.fileReady = false;
			}
			emit fileCopyReadyUpdateSignal(data);
		}
		else if (fileInfo.isDir()) 
		{
			//qDebug() << data.srcFilePath << "is a Directory.";
			XLOG_INFO("AsynchronousFileProcessor::copyFileSlot," + data.srcFilePath.toStdString()
				+ " is a Directory, QIdStr = " + QIdStr, CURRENT_THREAD_ID);
			bool copyFlag = qCopyDirectory(data.srcFilePath, data.dstFilePath);
			if (copyFlag)
			{
				data.fileReady = true;
			}
			else
			{
				data.fileReady = false;
			}
			emit fileCopyReadyUpdateSignal(data);
		}
		else 
		{
			//qDebug() << data.srcFilePath << "is neither File or Directory.";
			XLOG_INFO("AsynchronousFileProcessor::copyFileSlot," + data.srcFilePath.toStdString()
				+ " is neither File or Directory, QIdStr = " + QIdStr, CURRENT_THREAD_ID);
		}
	}
	else 
	{
		//qDebug() << data.srcFilePath << "does not exists !";
		XLOG_INFO("AsynchronousFileProcessor::copyFileSlot," + data.srcFilePath.toStdString()
			+ " does not exists, QIdStr = " + QIdStr, CURRENT_THREAD_ID);
	}
}
