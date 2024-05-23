#include "Common/AsynchronousFileEmitter.h"
#include <QMetaType>

AsynchronousFileEmitter::AsynchronousFileEmitter()
{
	// ע������
	qRegisterMetaType<FileCopyData>("FileCopyData");
}

void AFEmitter::emitFileCopySignal(FileCopyData data)
{
	emit copyFileSignal(data);
}
