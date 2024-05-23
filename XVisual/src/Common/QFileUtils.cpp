#include "Common/QFileUtils.h"
#include <QFileInfo>
#include <QFile>
#include <QDir>

bool qGetBaseFileName(const QString& absoluteFilePath, QString& baseFileName)
{
	QFileInfo fileInfo(absoluteFilePath);
	// ��ȡ�ļ���
	baseFileName = fileInfo.fileName();
	return true;
}

bool qRemoveDirContents(const QString& dirPath)
{
	QDir dir(dirPath);

	// ɾ�������ļ�
	dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
	foreach(QString fileName, dir.entryList())
	{
		if (!dir.remove(fileName))
		{
			//qWarning() << "�޷�ɾ���ļ���" << fileName;
			return false;
		}
	}

	// ɾ���������ļ��м�������
	dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
	foreach(QString subDirName, dir.entryList())
	{
		QDir subDir(dir.absoluteFilePath(subDirName));
		if (!subDir.removeRecursively())
		{
			//qWarning() << "�޷��ݹ�ɾ��Ŀ¼��" << subDirName;
			return false;
		}
	}

	return true;
}

// �����ļ�
bool qCopyFile(const QString& srcFilePath, const QString& destFilePath)
{
	if (QFile::exists(destFilePath))
	{
		QFile::remove(destFilePath);
	}
	return QFile::copy(srcFilePath, destFilePath);
}

// �����ļ���
bool qCopyDirectory(const QString& srcDirPath, const QString& destDirPath)
{
	QDir srcDir(srcDirPath);
	if (!srcDir.exists())
	{
		return false;
	}

	bool rmFlag = qRemoveDirContents(destDirPath);
	if (!rmFlag)
	{
		return false;
	}

	QDir destDir(destDirPath);
	if (!destDir.exists())
	{
		destDir.mkpath(destDirPath);
	}

	foreach(QString fileName, srcDir.entryList(QDir::Files))
	{
		QString srcFilePath = srcDirPath + QDir::separator() + fileName;
		QString destFilePath = destDirPath + QDir::separator() + fileName;
		if (!qCopyFile(srcFilePath, destFilePath))
		{
			return false;
		}
	}

	foreach(QString dirName, srcDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		QString srcSubDirPath = srcDirPath + QDir::separator() + dirName;
		QString destSubDirPath = destDirPath + QDir::separator() + dirName;
		if (!qCopyDirectory(srcSubDirPath, destSubDirPath))
		{
			return false;
		}
	}

	return true;
}