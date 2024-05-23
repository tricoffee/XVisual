#ifndef QFileUtils_H
#define QFileUtils_H

#include <QString>

bool qGetBaseFileName(const QString& absoluteFilePath, QString& baseFileName);

bool qRemoveDirContents(const QString& dirPath);

// 拷贝文件
bool qCopyFile(const QString& srcFilePath, const QString& destFilePath);

// 拷贝文件夹
bool qCopyDirectory(const QString& srcDirPath, const QString& destDirPath);

#endif // QFileUtils_H
