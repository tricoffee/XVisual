#include "Common/QFileUtils.h"
#include <QFileInfo>
#include <QFile>
#include <QDir>

namespace XVisual {

	bool qGetBaseFileName(const QString& absoluteFilePath, QString& baseFileName)
	{
		QFileInfo fileInfo(absoluteFilePath);
		// 获取文件名
		baseFileName = fileInfo.fileName();
		return true;
	}

	bool qRemoveDirContents(const QString& dirPath)
	{
		QDir dir(dirPath);

		// 删除所有文件
		dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
		foreach(QString fileName, dir.entryList())
		{
			if (!dir.remove(fileName))
			{
				//qWarning() << "无法删除文件：" << fileName;
				return false;
			}
		}

		// 删除所有子文件夹及其内容
		dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
		foreach(QString subDirName, dir.entryList())
		{
			QDir subDir(dir.absoluteFilePath(subDirName));
			if (!subDir.removeRecursively())
			{
				//qWarning() << "无法递归删除目录：" << subDirName;
				return false;
			}
		}

		return true;
	}

	// 拷贝文件
	bool qCopyFile(const QString& srcFilePath, const QString& destFilePath)
	{
		if (QFile::exists(destFilePath))
		{
			QFile::remove(destFilePath);
		}
		return QFile::copy(srcFilePath, destFilePath);
	}

	// 拷贝文件夹
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
} //namespace XVisual