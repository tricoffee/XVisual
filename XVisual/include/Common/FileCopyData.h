#ifndef FileCopyData_H
#define FileCopyData_H

#include <iostream>
#include <QString>
#include <QMetaType>

struct FileCopyData
{
	QString srcFilePath;
	QString dstFilePath;
	bool fileReady = false;
};

Q_DECLARE_METATYPE(FileCopyData)

#endif // FileCopyData_H
