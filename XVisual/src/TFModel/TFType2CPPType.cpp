#include "TFModel/TFType2CPPType.h"

namespace XVisual {

    ::TF_DataType parseDataTypeStr(const std::string& type_str)
    {
        if (type_str == "DT_FLOAT")      return TF_FLOAT;
        if (type_str == "DT_DOUBLE")     return TF_DOUBLE;
        if (type_str == "DT_INT32")      return TF_INT32;
        if (type_str == "DT_INT64")      return TF_INT64;
        if (type_str == "DT_STRING")     return TF_STRING;
        if (type_str == "DT_BOOL")       return TF_BOOL;
        if (type_str == "DT_COMPLEX64")  return TF_COMPLEX64;
        if (type_str == "DT_COMPLEX128") return TF_COMPLEX128;
        if (type_str == "DT_QINT8")      return TF_QINT8;
        if (type_str == "DT_QINT16")     return TF_QINT16;
        if (type_str == "DT_QUINT8")     return TF_QUINT8;
        if (type_str == "DT_RESOURCE")   return TF_RESOURCE;
        if (type_str == "DT_VARIANT")    return TF_VARIANT;

        throw std::invalid_argument("Unknown or Unsupported type: " + type_str);
    }

} //namespace XVisual