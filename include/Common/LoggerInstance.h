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
		// 使用静态局部变量确保单例
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

		// 设置日志目录
		const std::string logDirectory = "logs/";
		google::SetLogDestination(google::INFO, logDirectory.c_str());

		// 创建日志目录(如果它不存在)
		if (!std::filesystem::is_directory(logDirectory))
		{
			std::filesystem::create_directory(logDirectory);
		}

		// 设置日志文件扩展名
		google::SetLogFilenameExtension(".logtxt");

		// 同时在控制台输出 INFO 和 ERROR 信息，将FLAGS_alsologtostderr设置为1，可以在google:;SetLogDestination之后
		FLAGS_alsologtostderr = 1;

		// 立即刷新日志
		FLAGS_logbufsecs = 0;

		// 初始化glog
		google::InitGoogleLogging("XVisual");
	}
};

/*
extern关键字用于声明全局变量，但它不分配存储空间。
它的目的是让编译器知道该变量是在另一个源文件中定义的。
在你的代码中，logger的定义很可能在另一个源文件中，
而这里你只是在这个头文件中声明了它们。
因此，这是正确的用法，因为它告诉编译器这些变量的定义在其他地方，
你只需要声明它们。去掉extern可能会导致重复定义的错误，因为变量可能在多个源文件中被定义。
所以，不建议去掉extern。
*/
extern LoggerInstance& logger;

#endif //LoggerInstance_H