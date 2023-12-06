#ifndef LoggerInstance_H
#define LoggerInstance_H

#include <glog/logging.h>
#include <filesystem>
#include "Common/LogMacro.h" //�������log�ĺ궨�巽�����
#include <mutex>  // ��ӻ�����
#include <opencv2/opencv.hpp>  // opencvͷ�ļ�
#include <thread>

class LoggerInstance
{
public:
	static LoggerInstance& GetInstance()
	{
		// ʹ�þ�̬�ֲ�����ȷ������
		static LoggerInstance instance;
		return instance;
	}

	void LogInfo(const std::string& message, const std::thread::id& threadId, const char* file, int line)
	{
		std::lock_guard<std::mutex> lock(mutex_);  // ����
		google::LogMessage(file, line, google::GLOG_INFO).stream() << " Thread ID: " << threadId  << " " <<  message;
	}//���������ʱ��lock_guard���Զ�����������mutex_

	void LogError(const std::string& message, const std::thread::id& threadId, const char* file, int line)
	{
		std::lock_guard<std::mutex> lock(mutex_);  // ����
		google::LogMessage(file, line, google::GLOG_ERROR).stream() << " Thread ID: " << threadId << " " << message;
	}//���������ʱ��lock_guard���Զ�����������mutex_

	// ���ڱ���ͼƬ��logsĿ¼�µ�debugImages��Ŀ¼
	void SaveImage(const cv::Mat& image, const std::string& filename, const std::thread::id& threadId, const char* file, int line)
	{
		std::lock_guard<std::mutex> lock(mutex_);  // ����
		const std::string imagePath = GetImageSavePath(filename);
		cv::imwrite(imagePath, image);
		google::LogMessage(file, line, google::GLOG_INFO).stream() << " Thread ID: " << threadId << " " << "Saved image: " << imagePath;
	}//���������ʱ��lock_guard���Զ�����������mutex_

private:
	// ��־Ŀ¼
	const std::string logDirectory = "logs/";
	// ���ڹ��������ı���ͼƬ·��
	std::string GetImageSavePath(const std::string& filename)
	{
		const std::string imageDirectory = logDirectory + "debugImages/";

		// ����ͼƬ����Ŀ¼(�����������)
		if (!std::filesystem::is_directory(imageDirectory))
		{
			std::filesystem::create_directory(imageDirectory);
		}

		return imageDirectory + filename;
	}
	LoggerInstance()
	{
		// ������־Ŀ¼
		google::SetLogDestination(google::INFO, logDirectory.c_str());

		// ������־Ŀ¼(�����������)
		if (!std::filesystem::is_directory(logDirectory))
		{
			std::filesystem::create_directory(logDirectory);
		}

		// ������־�ļ���չ��
		google::SetLogFilenameExtension(".logtxt");

		// ͬʱ�ڿ���̨��� INFO �� ERROR ��Ϣ����FLAGS_alsologtostderr����Ϊ1��������google::SetLogDestination֮��
		FLAGS_alsologtostderr = 1;

		// ����ˢ����־
		FLAGS_logbufsecs = 0;

		// ��ʼ��glog
		google::InitGoogleLogging("XVisual");
	}
	// ������
	std::mutex mutex_;
};

/*
extern�ؼ�����������ȫ�ֱ���������������洢�ռ䡣
����Ŀ�����ñ�����֪���ñ���������һ��Դ�ļ��ж���ġ�
��������У�logger�Ķ���ܿ�������һ��Դ�ļ��У�
��������ֻ�������ͷ�ļ������������ǡ�
��ˣ�������ȷ���÷�����Ϊ�����߱�������Щ�����Ķ����������ط���
��ֻ��Ҫ�������ǡ�ȥ��extern���ܻᵼ���ظ�����Ĵ�����Ϊ���������ڶ��Դ�ļ��б����塣
���ԣ�������ȥ��extern��
*/
extern LoggerInstance& logger;

#endif //LoggerInstance_H