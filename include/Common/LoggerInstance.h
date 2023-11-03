#ifndef LoggerInstance_H
#define LoggerInstance_H

#include <glog/logging.h>
#include <filesystem>
#include "Common/LogMacro.h"

class LoggerInstance
{
public:
	static LoggerInstance& GetInstance()
	{
		// ʹ�þ�̬�ֲ�����ȷ������
		static LoggerInstance instance;
		return instance;
	}

	void LogInfo(const std::string& message, const char* file, int line)
	{
		google::LogMessage(file, line, google::GLOG_INFO).stream() << message;
	}

	void LogError(const std::string& message, const char* file, int line)
	{
		google::LogMessage(file, line, google::GLOG_ERROR).stream() << message;
	}

private:
	LoggerInstance()
	{

		// ������־Ŀ¼
		const std::string logDirectory = "logs/";
		google::SetLogDestination(google::INFO, logDirectory.c_str());

		// ������־Ŀ¼(�����������)
		if (!std::filesystem::is_directory(logDirectory))
		{
			std::filesystem::create_directory(logDirectory);
		}

		// ������־�ļ���չ��
		google::SetLogFilenameExtension(".logtxt");

		// ͬʱ�ڿ���̨��� INFO �� ERROR ��Ϣ����FLAGS_alsologtostderr����Ϊ1��������google:;SetLogDestination֮��
		FLAGS_alsologtostderr = 1;

		// ����ˢ����־
		FLAGS_logbufsecs = 0;

		// ��ʼ��glog
		google::InitGoogleLogging("XVisual");
	}
};

/*
extern�ؼ�����������ȫ�ֱ���������������洢�ռ䡣
����Ŀ�����ñ�����֪���ñ���������һ��Դ�ļ��ж���ġ�
����Ĵ����У�logger�Ķ���ܿ�������һ��Դ�ļ��У�
��������ֻ�������ͷ�ļ������������ǡ�
��ˣ�������ȷ���÷�����Ϊ�����߱�������Щ�����Ķ����������ط���
��ֻ��Ҫ�������ǡ�ȥ��extern���ܻᵼ���ظ�����Ĵ�����Ϊ���������ڶ��Դ�ļ��б����塣
���ԣ�������ȥ��extern��
*/
extern LoggerInstance& logger;

#endif //LoggerInstance_H