#include "Common/SettingsReader.h"
#include <QFile>

namespace XVisual {

	SettingsReader& SettingsReader::getInstance()
	{
		static SettingsReader reader;
		return reader;
	}
	SettingsReader::SettingsReader()
	{

	}
	SettingsReader::~SettingsReader()
	{

	}
	bool SettingsReader::initReader(const QString& settingsFilePath)
	{
		settingsFile.setFileName(settingsFilePath);
		if (!settingsFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			return false;
		}
		settingsData = settingsFile.readAll();
		settingsFile.close();
		if (settingsData.isEmpty())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	bool SettingsReader::getSettingsDoc(QJsonDocument& mSettingsDoc)
	{
		settingsDoc = QJsonDocument::fromJson(settingsData);
		if (settingsDoc.isNull() || !settingsDoc.isObject())
		{
			return false;
		}
		else
		{
			mSettingsDoc = settingsDoc;
			return true;
		}
	}
	bool SettingsReader::getSettingsObj(QJsonObject& mSettingsObj)
	{
		settingsObj = settingsDoc.object();
		if (settingsObj.isEmpty())
		{
			return false;
		}
		else
		{
			mSettingsObj = settingsObj;
			return true;
		}
	}
	bool SettingsReader::getWorkSpaceObj(QJsonObject& mWorkSpaceObj)
	{
		if (settingsObj.contains("WorkSpace") && settingsObj["WorkSpace"].isObject())
		{
			workspaceObj = settingsObj["WorkSpace"].toObject();
			if (workspaceObj.isEmpty())
			{
				return false;
			}
			else
			{
				mWorkSpaceObj = workspaceObj;
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	bool SettingsReader::updateSettingsObj(const QJsonObject& mSettingsObj)
	{
		settingsObj = mSettingsObj;
		settingsDoc.setObject(settingsObj);
		return true;
	}
	// 将更新后的JSON数据写回settings.json文件
	bool SettingsReader::reWriteJson()
	{
		if (!settingsFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
		{
			return false;
		}
		settingsFile.write(settingsDoc.toJson());
		settingsFile.close();
		return true;
	}
	XVisual::ErrorCode SettingsReader::getData(const QString& settingsFilePath, WorkSpaceData& data)
	{
		bool settingsReaderInitFlag = initReader(settingsFilePath);
		if (!settingsReaderInitFlag)
		{
			XVisual::ErrorCode m_lastError = XVisual::ErrorCode::SettingsReaderInitError;
			return m_lastError;
		}
		// Parse json data
		QJsonDocument settingsDoc;
		bool settingsDocReaderFlag = getSettingsDoc(settingsDoc);
		if (!settingsDocReaderFlag)
		{
			XVisual::ErrorCode m_lastError = XVisual::ErrorCode::SettingsDocReadFailedError;
			return m_lastError;
		}
		QJsonObject settingsObj;
		bool settingsObjReaderFlag = getSettingsObj(settingsObj);
		if (!settingsObjReaderFlag)
		{
			XVisual::ErrorCode m_lastError = XVisual::ErrorCode::SettingsObjReadFailedError;
			return m_lastError;
		}
		QJsonObject workspaceObj;
		bool workspaceObjReaderFlag = getWorkSpaceObj(workspaceObj);
		if (!workspaceObjReaderFlag)
		{
			XVisual::ErrorCode m_lastError = XVisual::ErrorCode::WorkSpaceObjReadFailedError;
			return m_lastError;
		}

		QString customWorkSpace = workspaceObj.value("CustomWorkSpace").toString();
		QString defaultWorkSpace = workspaceObj.value("DefaultWorkSpace").toString();
		int enableCustom = workspaceObj.value("EnableCustom").toInt();

		data.customWorkSpace = customWorkSpace;
		data.defaultWorkSpace = defaultWorkSpace;
		data.enableCustom = enableCustom;

	}

	XVisual::ErrorCode SettingsReader::updateData(const QString& settingsFilePath, const WorkSpaceData& data)
	{
		bool settingsReaderInitFlag = initReader(settingsFilePath);
		if (!settingsReaderInitFlag)
		{
			XVisual::ErrorCode m_lastError = XVisual::ErrorCode::SettingsReaderInitError;
			return m_lastError;
		}
		// Parse json data
		QJsonDocument settingsDoc;
		bool settingsDocReaderFlag = getSettingsDoc(settingsDoc);
		if (!settingsDocReaderFlag)
		{
			XVisual::ErrorCode m_lastError = XVisual::ErrorCode::SettingsDocReadFailedError;
			return m_lastError;
		}
		QJsonObject settingsObj;
		bool settingsObjReaderFlag = getSettingsObj(settingsObj);
		if (!settingsObjReaderFlag)
		{
			XVisual::ErrorCode m_lastError = XVisual::ErrorCode::SettingsObjReadFailedError;
			return m_lastError;
		}
		QJsonObject workspaceObj;
		bool workspaceObjReaderFlag = getWorkSpaceObj(workspaceObj);
		if (!workspaceObjReaderFlag)
		{
			XVisual::ErrorCode m_lastError = XVisual::ErrorCode::WorkSpaceObjReadFailedError;
			return m_lastError;
		}

		workspaceObj["CustomWorkSpace"] = data.customWorkSpace;
		workspaceObj["DefaultWorkSpace"] = data.defaultWorkSpace;
		workspaceObj["EnableCustom"] = data.enableCustom;

		// 将修改后的对象重新插入到根对象中
		settingsObj["WorkSpace"] = workspaceObj;

		updateSettingsObj(settingsObj);

		// 将更新后的JSON数据写回settings.json文件
		bool reWriteFlag = reWriteJson();
		if (!reWriteFlag)
		{
			XVisual::ErrorCode m_lastError = XVisual::ErrorCode::UpdateSettingsJsonFailedError;
			return m_lastError;
		}

		return XVisual::ErrorCode::Success;

	}
} //namespace XVisual 