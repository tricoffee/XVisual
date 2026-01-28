#ifndef SettingsReader_H
#define SettingsReader_H

#include <QString>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include "Common/ErrorCode.h"

namespace XVisual {

struct WorkSpaceData
{
	QString defaultWorkSpace;
	QString customWorkSpace;
	bool enableCustom = false;
};

class SettingsReader
{
	public:
		static SettingsReader& getInstance();
		~SettingsReader();
		bool initReader(const QString& settingsFilePath);
		bool getSettingsDoc(QJsonDocument& mSettingsDoc);
		bool getSettingsObj(QJsonObject& mSettingsObj);
		bool getWorkSpaceObj(QJsonObject& mWorkSpaceObj);
		bool updateSettingsObj(const QJsonObject& mSettingsObj);
		bool reWriteJson();
		XVisual::ErrorCode getData(const QString& settingsFilePath, WorkSpaceData& data);
		XVisual::ErrorCode updateData(const QString& settingsFilePath, const WorkSpaceData& data);
	private:
		SettingsReader();
		QFile settingsFile;
		QByteArray settingsData;
		QJsonDocument settingsDoc;
		QJsonObject settingsObj;
		QJsonObject workspaceObj;
};

} // namespace XVisual

#endif // SettingsReader_H
