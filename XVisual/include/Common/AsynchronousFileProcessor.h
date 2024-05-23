#ifndef AsynchronousFileProcessor_H
#define AsynchronousFileProcessor_H

#include <QObject>
#include "Common/FileCopyData.h"

// ����һ���������󣬰����ۺ���
class AsynchronousFileProcessor : public QObject 
{
	Q_OBJECT
	public:
		AsynchronousFileProcessor();
	signals:
		void fileCopyReadyUpdateSignal(FileCopyData data);
	public slots:
		void copyFileSlot(FileCopyData data);
	private:
		std::string getQCurrentIdStr();
};

using AFProcessor = AsynchronousFileProcessor;

#endif // AsynchronousFileProcessor_H
