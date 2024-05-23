#ifndef AsynchronousFileEmitter_H
#define AsynchronousFileEmitter_H

#include <QObject>
#include <QString>
#include "Common/FileCopyData.h"

// 定义一个发射信号的对象
class AsynchronousFileEmitter : public QObject 
{
	Q_OBJECT
	public:
		AsynchronousFileEmitter();
		void emitFileCopySignal(FileCopyData data);
    signals:
		void copyFileSignal(FileCopyData data);
};

using AFEmitter = AsynchronousFileEmitter;

#endif // AsynchronousFileEmitter_H
