#include "Common/AsynchronousFileEmitter.h"
#include <QMetaType>

namespace XVisual {

AsynchronousFileEmitter::AsynchronousFileEmitter()
{
	// 注册类型
	qRegisterMetaType<FileCopyData>("FileCopyData");
}

void AFEmitter::emitFileCopySignal(FileCopyData data)
{
	emit copyFileSignal(data);
}

} // namespace XVisual
