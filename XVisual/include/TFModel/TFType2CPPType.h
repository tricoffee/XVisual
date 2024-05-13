#ifndef TFType2CPPType_H
#define TFType2CPPType_H

#include <iostream>
#include <string>
#include <complex>
#include <variant>
#include <type_traits>
#include "tensorflow/c/c_api.h"


//enum TF_DataType
//{
//	TF_FLOAT,
//	TF_DOUBLE,
//	TF_INT32,
//	TF_INT64,
//	TF_STRING,
//	TF_BOOL,
//	TF_COMPLEX64,
//	TF_COMPLEX128,
//	TF_QINT8,
//	TF_QINT16,
//	TF_QUINT8,
//	TF_RESOURCE,
//	TF_VARIANT
//};


template <TF_DataType DataType>
struct TFTypeToCppType
{
	using type = void;
};

template <>
struct TFTypeToCppType<TF_FLOAT>
{
	using type = float;
};

template <>
struct TFTypeToCppType<TF_DOUBLE>
{
	using type = double;
};

template <>
struct TFTypeToCppType<TF_INT32>
{
	using type = std::int32_t;
};

template <>
struct TFTypeToCppType<TF_INT64>
{
	using type = std::int64_t;
};

template <>
struct TFTypeToCppType<TF_STRING>
{
	using type = std::string;
};

template <>
struct TFTypeToCppType<TF_BOOL>
{
	using type = bool;
};

template <>
struct TFTypeToCppType<TF_COMPLEX64>
{
	using type = std::complex<float>;
};

template <>
struct TFTypeToCppType<TF_COMPLEX128>
{
	using type = std::complex<double>;
};

template <>
struct TFTypeToCppType<TF_QINT8>
{
	using type = std::int8_t;
};

template <>
struct TFTypeToCppType<TF_QINT16>
{
	using type = std::int16_t;
};

template <>
struct TFTypeToCppType<TF_QUINT8>
{
	using type = std::uint8_t;
};

template <>
struct TFTypeToCppType<TF_RESOURCE>
{
	using type = void*;
};

template <>
struct TFTypeToCppType<TF_VARIANT>
{
	using type = void*;
};

template <TF_DataType DataType>
typename TFTypeToCppType<DataType>::type getCppType()
{
	return typename TFTypeToCppType<DataType>::type();
}

TF_DataType parseDataTypeStr(std::string type_str);

#endif //TFType2CPPType_H
