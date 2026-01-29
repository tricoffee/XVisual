#ifndef ErrorCode_H
#define ErrorCode_H
#include <iostream>

namespace XVisual
{
	enum class ErrorCode
	{
		Success = 0,
			Canceled = 10,
		FileNotFound = 1,
		UmapKeyNoFound = 2,
		UnImplementedError = 3,
		InvalidJsonFileError = 4,
		OpenJsonFileError = 5,
		JsonParseFailed = 301,
		ParseClassInfoArrFromJsonFailed = 302,
		ParseClassNameFromJsonFailed = 303,
		ParseColleagueTypeFromJsonFailed = 304,
		ParseColleagueIdFromJsonFailed = 305,
		ParseSourceFromJsonFailed = 306,
		ParseInnerParamFromJsonFailed = 307,
		ParseInnerParamElementFailed = 308,
		ParseOuterParamFromJsonFailed = 309,
		ParseJsonUnknownObjectItemError = 310,
		ParseJsonUnknownError = 311,

		SettingsReaderInitError = 401,
		SettingsDocReadFailedError = 402,
		SettingsObjReadFailedError = 403,
		WorkSpaceObjReadFailedError = 404,
		UpdateSettingsJsonFailedError = 405,

		UnknownXVariableNameError = 501,
		UnknownPreParamPreTagError = 601,

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
			case ErrorCode::Canceled:
				return "Canceled";
			case ErrorCode::FileNotFound:
				return "FileNotFoundError";
			case ErrorCode::UmapKeyNoFound:
				return "UmapKeyNoFoundError";
			case ErrorCode::UnImplementedError:
				return "UnImplementedError";
			case ErrorCode::InvalidJsonFileError:
				return "InvalidJsonFileError";
			case ErrorCode::OpenJsonFileError:
				return "OpenJsonFileError";
			case ErrorCode::JsonParseFailed:
				return "JsonParseFailed";
			case ErrorCode::ParseClassInfoArrFromJsonFailed:
				return "ParseClassInfoArrFromJsonFailed";
			case ErrorCode::ParseClassNameFromJsonFailed:
				return "ParseClassNameFromJsonFailed";
			case ErrorCode::ParseColleagueTypeFromJsonFailed:
				return "ParseColleagueTypeFromJsonFailed";
			case ErrorCode::ParseColleagueIdFromJsonFailed:
				return "ParseColleagueIdFromJsonFailed";
			case ErrorCode::ParseSourceFromJsonFailed:
				return "ParseSourceFromJsonFailed";
			case ErrorCode::ParseInnerParamFromJsonFailed:
				return "ParseInnerParamFromJsonFailed";
			case ErrorCode::ParseInnerParamElementFailed:
				return "ParseInnerParamElementFailed";
			case ErrorCode::ParseOuterParamFromJsonFailed:
				return "ParseOuterParamFromJsonFailed";
			case ErrorCode::ParseJsonUnknownObjectItemError:
				return "ParseJsonUnknownObjectItemError";
			case ErrorCode::ParseJsonUnknownError:
				return "ParseJsonUnknownError";

			case ErrorCode::SettingsReaderInitError:
				return "SettingsReaderInitError";
			case ErrorCode::SettingsDocReadFailedError:
				return "SettingsDocReadFailedError";
			case ErrorCode::SettingsObjReadFailedError:
				return "SettingsObjReadFailedError";
			case ErrorCode::WorkSpaceObjReadFailedError:
				return "WorkSpaceObjReadFailedError";
			case ErrorCode::UpdateSettingsJsonFailedError:
				return "UpdateSettingsJsonFailedError";

			case ErrorCode::UnknownXVariableNameError:
				return "UnknownXVariableNameError";
			case ErrorCode::UnknownPreParamPreTagError:
				return "UnknownPreParamPreTagError";
			default:
				return "UnknownError";
		}
	}

}// namespace XVisual

#endif // ErrorCode_H
