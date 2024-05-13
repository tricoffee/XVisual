#include "TFModel/TFType2CPPType.h"

TF_DataType parseDataTypeStr(std::string type_str)
{
	if (type_str == "DT_FLOAT")
	{
		return TF_FLOAT;
	}
	else if (type_str == "DT_DOUBLE")
	{
		return TF_DOUBLE;
	}
	else if (type_str == "DT_INT32")
	{
		return TF_INT32;
	}
	else if (type_str == "DT_INT64")
	{
		return TF_INT64;
	}
	else if (type_str == "DT_STRING")
	{
		return TF_STRING;
	}
	else if (type_str == "DT_BOOL")
	{
		return TF_BOOL;
	}
	else if (type_str == "DT_COMPLEX64")
	{
		return TF_COMPLEX64;
	}
	else if (type_str == "DT_COMPLEX128")
	{
		return TF_COMPLEX128;
	}
	else if (type_str == "DT_QINT8")
	{
		return TF_QINT8;
	}
	else if (type_str == "DT_QINT16")
	{
		return TF_QINT16;
	}
	else if (type_str == "DT_QUINT8")
	{
		return TF_QUINT8;
	}
	else if (type_str == "DT_RESOURCE")
	{
		return TF_RESOURCE;
	}
	else if (type_str == "DT_VARIANT")
	{
		return TF_VARIANT;
	}
	else
	{
		std::string msg = "Unknown or Unsupported type: " + type_str;
		throw std::invalid_argument(msg); // 使用无效参数异常抛出
	}
}