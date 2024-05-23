#include "Common/AsynchronousFileEmitter.h"
#include <QMetaType>

AsynchronousFileEmitter::AsynchronousFileEmitter()
{
	// ◊¢≤·¿‡–Õ
	qRegisterMetaType<FileCopyData>("FileCopyData");
}

void AFEmitter::emitFileCopySignal(FileCopyData data)
{
	emit copyFileSignal(data);
}
