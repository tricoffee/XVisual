#ifndef FileCopyData_H
#define FileCopyData_H

#include <iostream>
#include <QString>
#include <QMetaType>

namespace XVisual {

struct FileCopyData
{
	QString srcFilePath;
	QString dstFilePath;
	bool fileReady = false;
};

} // namespace XVisual

Q_DECLARE_METATYPE(XVisual::FileCopyData)

#endif // FileCopyData_H
