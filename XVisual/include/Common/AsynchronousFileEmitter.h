#ifndef AsynchronousFileEmitter_H
#define AsynchronousFileEmitter_H

#include <QObject>
#include <QString>
#include "Common/FileCopyData.h"

// ����һ�������źŵĶ���
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
