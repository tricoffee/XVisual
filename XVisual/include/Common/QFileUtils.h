#ifndef QFileUtils_H
#define QFileUtils_H

#include <QString>

bool qGetBaseFileName(const QString& absoluteFilePath, QString& baseFileName);

bool qRemoveDirContents(const QString& dirPath);

// �����ļ�
bool qCopyFile(const QString& srcFilePath, const QString& destFilePath);

// �����ļ���
bool qCopyDirectory(const QString& srcDirPath, const QString& destDirPath);

#endif // QFileUtils_H
