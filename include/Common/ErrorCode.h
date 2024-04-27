#ifndef ErrorCode_H
#define ErrorCode_H
#include <iostream>

namespace XVisual
{
	enum class ErrorCode
	{
		Success = 0,
		FileNotFound = 1,
		UmapKeyNoFound = 2
	};

	/*
	������ ErrorCode ת��Ϊ��Ӧ���ַ�����
	����������Ϊ inline�������ñ�������ÿ��ʹ�ú����ĵط����뺯���Ĵ��룬�Ӷ������ظ���������⡣
	*/
	inline std::string errorCodeToStr(ErrorCode code)
	{
		switch (code) 
		{
			case ErrorCode::Success:
				return "Success";
			case ErrorCode::FileNotFound:
				return "FileNotFoundError";
			case ErrorCode::UmapKeyNoFound:
				return "UmapKeyNoFoundError";
			default:
				return "UnknownError";
		}
	}

}// namespace XVisual

#endif // ErrorCode_H