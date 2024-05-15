#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include "Common/ImagePre.h"

#define CHECK_NULLPTR(ptr) ((ptr) == nullptr)
#define CHECK_ObjectItem(object) ((object != nullptr) && ((object)->type & cJSON_Object))

//Deprecated Function
//void updateOutParams(const std::unordered_map<std::string, float>& src, std::unordered_map<std::string, float>& dst)
//{
//	// 遍历 src 中的每个键值对
//	for (const auto& pair : src)
//	{
//		const std::string& key = pair.first;
//		float value = pair.second;
//
//		// 在 dst 中查找当前键是否已存在
//		auto it = dst.find(key);
//		if (it != dst.end())
//		{
//			// 如果键已存在，则更新对应的值
//			it->second = value;
//		}
//		else
//		{
//			// 如果键不存在，则添加新的键值对
//			dst[key] = value;
//		}
//	}
//}

ImagePre::ImagePre(const PreParam& params, bool printDebugInfo)
{
	params_ = params;
	printDebugInfo_ = printDebugInfo;
}

void ImagePre::preprocess(const cv::Mat& srcImage, cv::Mat& dstImage)
{
	if ("ResizePaste" == params_.preTag)
	{
		resize_paste(srcImage, dstImage);
	}
	else if ("Resize" == params_.preTag)
	{
		resize(srcImage, dstImage);
	}
	else if ("Normalize01" == params_.preTag)
	{
		normalize_01(srcImage, dstImage);
	}
}

void ImagePre::decode(const cv::Mat& srcImage, cv::Mat& dstImage)
{
	if ("ResizePaste" == params_.preTag)
	{
		decode_resize_paste(srcImage, dstImage);
	}
	else if ("Resize" == params_.preTag)
	{
		decode_resize(srcImage, dstImage);
	}
	else if ("Normalize01" == params_.preTag)
	{
		decode_normalize_01(srcImage, dstImage);
	}
}

//Revert coordinate points leftTopPoint(x1, y1) and bottomRightPoint(x2, y2) to the spatial scale of the original image
void ImagePre::revertBox(int& x1, int& y1, int& x2, int& y2)
{
	if ("ResizePaste" == params_.preTag)
	{
		revertBox_resize_paste(x1, y1, x2, y2);
	}
	else if ("Resize" == params_.preTag)
	{
		revertBox_resize(x1, y1, x2, y2);
	}
	else if ("Normalize01" == params_.preTag)
	{
		//do nothing
	}
}

std::unordered_map<std::string, std::any> ImagePre::getDecodeParams()
{
	return params_.decodeParams;
}


PreParam ImagePre::getPreParam()
{
	return params_;
}

XVisual::ErrorCode ImagePre::readPreParam(cJSON* cjson_preParamPtr)
{
	cJSON* preTag_ptr = cJSON_GetObjectItemCaseSensitive(cjson_preParamPtr, "preTag");
	if (cJSON_IsString(preTag_ptr) && (preTag_ptr->valuestring != NULL))
	{
		std::string preTagStr = preTag_ptr->valuestring;
		params_.preTag = preTagStr;
#ifdef DEBUG_PRINT
		XLOG_INFO(" @@@ ImagePre::readPreParam --- --- --- update params_.preTag @@@ ", CURRENT_THREAD_ID);
#endif
		if ("ResizePaste" == preTagStr)
		{
			cJSON* params_ptr = cJSON_GetObjectItemCaseSensitive(cjson_preParamPtr, "params");
			if (CHECK_ObjectItem(params_ptr))
			{
				cJSON* dstWidth_ptr = cJSON_GetObjectItemCaseSensitive(params_ptr, "dstWidth");
				cJSON* dstHeight_ptr = cJSON_GetObjectItemCaseSensitive(params_ptr, "dstHeight");
				if (cJSON_IsNumber(dstWidth_ptr) && (dstWidth_ptr->valueint != -1) && cJSON_IsNumber(dstHeight_ptr) && (dstHeight_ptr->valueint != -1))
				{
					int dstWidth = dstWidth_ptr->valueint;
					int dstHeight = dstHeight_ptr->valueint;
					params_.params["dstWidth"] = dstWidth;
					params_.params["dstHeight"] = dstHeight;
				}
				else
				{
					return XVisual::ErrorCode::ParseInnerParamElementFailed;
				}
			}
			else
			{
				return XVisual::ErrorCode::ParseInnerParamElementFailed;
			}

			cJSON* decodeParams_ptr = cJSON_GetObjectItemCaseSensitive(cjson_preParamPtr, "decodeParams");
			if (CHECK_ObjectItem(decodeParams_ptr))
			{
				cJSON* sxPtr = cJSON_GetObjectItemCaseSensitive(decodeParams_ptr, "sx");
				cJSON* syPtr = cJSON_GetObjectItemCaseSensitive(decodeParams_ptr, "sy");
				cJSON* dxPtr = cJSON_GetObjectItemCaseSensitive(decodeParams_ptr, "dx");
				cJSON* dyPtr = cJSON_GetObjectItemCaseSensitive(decodeParams_ptr, "dy");
				if (cJSON_IsNumber(sxPtr) && (sxPtr->valuedouble != -1) && cJSON_IsNumber(syPtr) && (syPtr->valuedouble != -1) &&
					cJSON_IsNumber(dxPtr) && (sxPtr->valueint != -1) && cJSON_IsNumber(dyPtr) && (syPtr->valueint != -1)
					)
				{
					float sx = sxPtr->valuedouble;
					float sy = syPtr->valuedouble;
					int dx = dxPtr->valuedouble;
					int dy = dyPtr->valuedouble;
					params_.decodeParams["sx"] = sx;
					params_.decodeParams["sy"] = sy;
					params_.decodeParams["dx"] = dx;
					params_.decodeParams["dy"] = dy;
				}
				else
				{
					return XVisual::ErrorCode::ParseInnerParamElementFailed;
				}
			}
			else
			{
				return XVisual::ErrorCode::ParseInnerParamElementFailed;
			}

			return XVisual::ErrorCode::Success;

		}// end for --- if ("ResizePaste" == preTagStr)

		else if ("Resize" == preTagStr)
		{
			cJSON* params_ptr = cJSON_GetObjectItemCaseSensitive(cjson_preParamPtr, "params");
			if (CHECK_ObjectItem(params_ptr))
			{
				cJSON* dstWidth_ptr = cJSON_GetObjectItemCaseSensitive(params_ptr, "targetWidth");
				cJSON* dstHeight_ptr = cJSON_GetObjectItemCaseSensitive(params_ptr, "targetHeight");
				if (cJSON_IsNumber(dstWidth_ptr) && (dstWidth_ptr->valueint != -1) && cJSON_IsNumber(dstHeight_ptr) && (dstHeight_ptr->valueint != -1))
				{
					int dstWidth = dstWidth_ptr->valueint;
					int dstHeight = dstHeight_ptr->valueint;
					params_.params["targetWidth"] = dstWidth;
					params_.params["targetHeight"] = dstHeight;
				}
				else
				{
					return XVisual::ErrorCode::ParseInnerParamElementFailed;
				}
			}
			else
			{
				return XVisual::ErrorCode::ParseInnerParamElementFailed;
			}

			cJSON* decodeParams_ptr = cJSON_GetObjectItemCaseSensitive(cjson_preParamPtr, "decodeParams");
			if (CHECK_ObjectItem(decodeParams_ptr))
			{
				cJSON* sxPtr = cJSON_GetObjectItemCaseSensitive(decodeParams_ptr, "sx");
				cJSON* syPtr = cJSON_GetObjectItemCaseSensitive(decodeParams_ptr, "sy");
				if (cJSON_IsNumber(sxPtr) && (sxPtr->valuedouble != -1) && cJSON_IsNumber(syPtr) && (syPtr->valuedouble != -1))
				{
					float sx = sxPtr->valuedouble;
					float sy = syPtr->valuedouble;
					params_.decodeParams["sx"] = sx;
					params_.decodeParams["sy"] = sy;
				}
				else
				{
					return XVisual::ErrorCode::ParseInnerParamElementFailed;
				}
			}
			else
			{
				return XVisual::ErrorCode::ParseInnerParamElementFailed;
			}

			return XVisual::ErrorCode::Success;

		} // end for --- else if ("Resize" == preTagStr)
		else if ("Normalize01" == preTagStr)
		{
			//Do nothing
			return XVisual::ErrorCode::Success;
		} // end for --- else if ("Normalize01" == preTagStr)
		else // UnknownPreParamPreTagError
		{
			return XVisual::ErrorCode::UnknownPreParamPreTagError;
		}
	} // end for --- if (cJSON_IsString(preTag_ptr) && (preTag_ptr->valuestring != NULL))

	else
	{
		return XVisual::ErrorCode::ParseInnerParamElementFailed;
	}
}

XVisual::ErrorCode ImagePre::writePreParam(cJSON* cjson_preparam)
{
	std::string preTagStr = params_.preTag;
	std::unordered_map<std::string, std::any> inParams = std::any_cast<std::unordered_map<std::string, std::any>>(params_.params);
	std::unordered_map<std::string, std::any> decodeParams = std::any_cast<std::unordered_map<std::string, std::any>>(params_.decodeParams);
	if ("ResizePaste" == params_.preTag)
	{
#ifdef DEBUG_PRINT
		XLOG_INFO(" @@@ ImagePre::writePreParam @@@ ", CURRENT_THREAD_ID);
#endif 
		// write params.preTag to Json 
#ifdef DEBUG_PRINT
		XLOG_INFO(" @@@ write params.preTag to Json   @@@ ", CURRENT_THREAD_ID);
#endif
		cJSON_AddStringToObject(cjson_preparam, "preTag", preTagStr.c_str());
		// write params_.params to Json 
#ifdef DEBUG_PRINT
		XLOG_INFO(" @@@ ImagePre::writePreParam, write params_.params to Json  @@@ ", CURRENT_THREAD_ID);
#endif
		cJSON* cjson_preparam_in = cJSON_CreateObject();
		int dstWidth = std::any_cast<int>(inParams["dstWidth"]);
		int dstHeight = std::any_cast<int>(inParams["dstHeight"]);
		cJSON_AddNumberToObject(cjson_preparam_in, "dstWidth", dstWidth);
		cJSON_AddNumberToObject(cjson_preparam_in, "dstHeight", dstHeight);
		cJSON_AddItemToObject(cjson_preparam, "params", cjson_preparam_in);
		// write params_.decodeParams to Json 
#ifdef DEBUG_PRINT
		XLOG_INFO(" @@@ ImagePre::writePreParam, write params_.decodeParams to Json   @@@ ", CURRENT_THREAD_ID);
		int numElements = decodeParams.size();
		XLOG_INFO(" @@@ numElements = " + std::to_string(numElements) + " @@@ ", CURRENT_THREAD_ID);
#endif 
		cJSON* cjson_preparam_out = cJSON_CreateObject();
		float sx = std::any_cast<float>(decodeParams["sx"]); // float 
		float sy = std::any_cast<float>(decodeParams["sy"]);  // float 
		int dx = std::any_cast<int>(decodeParams["dx"]); // int 
		int dy = std::any_cast<int>(decodeParams["dy"]);  // int 
		cJSON_AddNumberToObject(cjson_preparam_out, "sx", sx);
		cJSON_AddNumberToObject(cjson_preparam_out, "sy", sy);
		cJSON_AddNumberToObject(cjson_preparam_out, "dx", dx);
		cJSON_AddNumberToObject(cjson_preparam_out, "dy", dy);
		cJSON_AddItemToObject(cjson_preparam, "decodeParams", cjson_preparam_out);
#ifdef DEBUG_PRINT
		XLOG_INFO(" @@@ ImagePre::writePreParam, XVisual::ErrorCode::Success;   @@@ ", CURRENT_THREAD_ID);
#endif
		return XVisual::ErrorCode::Success;
	}
	if ("Resize" == params_.preTag)
	{
#ifdef DEBUG_PRINT
		XLOG_INFO(" @@@ ImagePre::writePreParam @@@ ", CURRENT_THREAD_ID);
#endif 
		// write params.preTag to Json 
#ifdef DEBUG_PRINT
		XLOG_INFO(" @@@ write params.preTag to Json   @@@ ", CURRENT_THREAD_ID);
#endif
		cJSON_AddStringToObject(cjson_preparam, "preTag", preTagStr.c_str());
		// write params_.params to Json 
#ifdef DEBUG_PRINT
		XLOG_INFO(" @@@ ImagePre::writePreParam, write params_.params to Json  @@@ ", CURRENT_THREAD_ID);
#endif
		cJSON* cjson_preparam_in = cJSON_CreateObject();
		int dstWidth = std::any_cast<int>(inParams["targetWidth"]);
		int dstHeight = std::any_cast<int>(inParams["targetHeight"]);
		cJSON_AddNumberToObject(cjson_preparam_in, "targetWidth", dstWidth);
		cJSON_AddNumberToObject(cjson_preparam_in, "targetHeight", dstHeight);
		cJSON_AddItemToObject(cjson_preparam, "params", cjson_preparam_in);
		// write params_.decodeParams to Json 
#ifdef DEBUG_PRINT
		XLOG_INFO(" @@@ ImagePre::writePreParam, write params_.decodeParams to Json   @@@ ", CURRENT_THREAD_ID);
		int numElements = decodeParams.size();
		XLOG_INFO(" @@@ numElements = " + std::to_string(numElements) + " @@@ ", CURRENT_THREAD_ID);
#endif 
		cJSON* cjson_preparam_out = cJSON_CreateObject();
		float sx = std::any_cast<float>(decodeParams["sx"]); // float 
		float sy = std::any_cast<float>(decodeParams["sy"]);  // float 
		cJSON_AddNumberToObject(cjson_preparam_out, "sx", sx);
		cJSON_AddNumberToObject(cjson_preparam_out, "sy", sy);
		cJSON_AddItemToObject(cjson_preparam, "decodeParams", cjson_preparam_out);
#ifdef DEBUG_PRINT
		XLOG_INFO(" @@@ ImagePre::writePreParam, XVisual::ErrorCode::Success;   @@@ ", CURRENT_THREAD_ID);
#endif
		return XVisual::ErrorCode::Success;
	}
	else if ("Normalize01" == params_.preTag)
	{
		cJSON_AddStringToObject(cjson_preparam, "preTag", preTagStr.c_str());
		return XVisual::ErrorCode::Success;
	}
	else
	{
		return XVisual::ErrorCode::UnknownPreParamPreTagError;
	}
}

// 成员函数：对图像进行缩放和粘贴操作
void ImagePre::resize_paste(const cv::Mat& inImage, cv::Mat& outImage)
{
	std::unordered_map<std::string, std::any> inParams = std::any_cast<std::unordered_map<std::string, std::any>>(params_.params);
	int dstWidth = std::any_cast<int>(inParams["dstWidth"]);
	int dstHeight = std::any_cast<int>(inParams["dstHeight"]);

	int imageWidth = inImage.cols;
	int imageHeight = inImage.rows;

#ifdef DEBUG_PRINT
	XLOG_INFO(" @@@ ImagePre::resize, imageWidth = " + std::to_string(imageWidth) + " @@@ ", CURRENT_THREAD_ID);
	XLOG_INFO(" @@@ ImagePre::resize, imageHeight = " + std::to_string(imageHeight) + " @@@ ", CURRENT_THREAD_ID);
#endif

	//int inImageChannels = inImage.channels();

	float sx = dstWidth / (imageWidth * 1.0);
	float sy = dstHeight / (imageHeight * 1.0);

	float scale_min = std::min(sx, sy);
	sx = scale_min;
	sy = scale_min;

	int paddingWidth = static_cast<int>(imageWidth * scale_min);
	int paddingHeight = static_cast<int>(imageHeight * scale_min);

	int dx = static_cast<int>((dstWidth - paddingWidth) / 2);
	int dy = static_cast<int>((dstHeight - paddingHeight) / 2);

	cv::Mat scaled_image;
	cv::Size padding_size(paddingWidth, paddingHeight);
	// cv::resize is deepcopy function
	cv::resize(inImage, scaled_image, padding_size, 0, 0, cv::INTER_NEAREST);

	outImage = cv::Mat(dstHeight, dstWidth, CV_8UC3, cv::Scalar(128, 128, 128));
	//  roi is non-deepcopy of outImage
	cv::Mat roi = outImage(cv::Rect(dx, dy, scaled_image.cols, scaled_image.rows));
	// copyTo is deepcopy function
	scaled_image.copyTo(roi);

	std::unordered_map<std::string, std::any> decodeParams;

	decodeParams["sx"] = sx; // float 
	decodeParams["sy"] = sy; // float 
	decodeParams["dx"] = dx; // int  
	decodeParams["dy"] = dy; // int 

	// Update parameters required for preprocessing inverse transformation
	params_.decodeParams = decodeParams;
#ifdef DEBUG_PRINT
	int numElements2 = params_.decodeParams.size();
	XLOG_INFO(" @@@ numElements2 = " + std::to_string(numElements2) + " @@@ ", CURRENT_THREAD_ID);
#endif 
}

// 成员函数：对图像进行缩放操作
void ImagePre::resize(const cv::Mat& inImage, cv::Mat& outImage)
{
	std::unordered_map<std::string, std::any> inParams = std::any_cast<std::unordered_map<std::string, std::any>>(params_.params);
	int dstWidth = std::any_cast<int>(inParams["targetWidth"]);
	int dstHeight = std::any_cast<int>(inParams["targetHeight"]);

	int imageWidth = inImage.cols;
	int imageHeight = inImage.rows;

#ifdef DEBUG_PRINT
	XLOG_INFO(" @@@ ImagePre::resize, imageWidth = " + std::to_string(imageWidth) + " @@@ ", CURRENT_THREAD_ID);
	XLOG_INFO(" @@@ ImagePre::resize, imageHeight = " + std::to_string(imageHeight) + " @@@ ", CURRENT_THREAD_ID);
#endif

	//int inImageChannels = inImage.channels();

	float sx = dstWidth / (imageWidth * 1.0);
	float sy = dstHeight / (imageHeight * 1.0);
	cv::Size dst_size(dstWidth, dstHeight);
	// cv::resize is deepcopy function
	cv::resize(inImage, outImage, dst_size, 0, 0, cv::INTER_NEAREST);

	std::unordered_map<std::string, std::any> decodeParams;

	decodeParams["sx"] = sx; // float 
	decodeParams["sy"] = sy; // float 

	// Update parameters required for preprocessing inverse transformation
	params_.decodeParams = decodeParams;
#ifdef DEBUG_PRINT
	int numElements2 = params_.decodeParams.size();
	XLOG_INFO(" @@@ numElements2 = " + std::to_string(numElements2) + " @@@ ", CURRENT_THREAD_ID);
#endif
}

// Member function: Restores the processed image to its original size
void ImagePre::decode_resize_paste(const cv::Mat& inImage, cv::Mat& outImage)
{
	std::unordered_map<std::string, std::any> inParams = std::any_cast<std::unordered_map<std::string, std::any>>(params_.params);
	std::unordered_map<std::string, std::any> decodeParams = std::any_cast<std::unordered_map<std::string, std::any>>(params_.decodeParams);

	int dx = std::any_cast<int>(decodeParams["dx"]);
	int dy = std::any_cast<int>(decodeParams["dy"]);
	int dstW = std::any_cast<int>(inParams["dstWidth"]);
	int dstH = std::any_cast<int>(inParams["dstHeight"]);
	int scaleW = dstW - 2 * dx;
	int scaleH = dstH - 2 * dy;

	//  roi is non-deepcopy of dstImage
	cv::Mat roi = inImage(cv::Rect(dx, dy, scaleW, scaleH));

	float sx = std::any_cast<float>(decodeParams["sx"]);
	float sy = std::any_cast<float>(decodeParams["sy"]);

	// 计算目标图像的尺寸
	int dstWidth = static_cast<int>(scaleW / sx);
	int dstHeight = static_cast<int>(scaleH / sy);

	// 创建目标尺寸大小的 cv::Size 对象
	cv::Size dstSize(dstWidth, dstHeight);

	// 使用 cv::resize 函数进行缩放, cv::resize is deepcopy function
	cv::resize(roi, outImage, dstSize, 0, 0, cv::INTER_NEAREST);

#ifdef DEBUG_PRINT
	XLOG_INFO(" @@@ ImagePre::decode_resize_paste, dstWidth = " + std::to_string(dstWidth) + " @@@ ", CURRENT_THREAD_ID);
	XLOG_INFO(" @@@ ImagePre::decode_resize_paste, dstHeight = " + std::to_string(dstHeight) + " @@@ ", CURRENT_THREAD_ID);
#endif
}

// Member function: Restores the processed image to its original size
void ImagePre::decode_resize(const cv::Mat& inImage, cv::Mat& outImage)
{
	std::unordered_map<std::string, std::any> inParams = std::any_cast<std::unordered_map<std::string, std::any>>(params_.params);
	std::unordered_map<std::string, std::any> decodeParams = std::any_cast<std::unordered_map<std::string, std::any>>(params_.decodeParams);

	int dstW = std::any_cast<int>(inParams["targetWidth"]);
	int dstH = std::any_cast<int>(inParams["targetHeight"]);

	float sx = std::any_cast<float>(decodeParams["sx"]);
	float sy = std::any_cast<float>(decodeParams["sy"]);

	// 计算目标图像的尺寸
	int dstWidth = static_cast<int>(dstW / sx);
	int dstHeight = static_cast<int>(dstH / sy);

	// 创建目标尺寸大小的 cv::Size 对象
	cv::Size dstSize(dstWidth, dstHeight);

	// 使用 cv::resize 函数进行缩放, cv::resize is deepcopy function
	cv::resize(inImage, outImage, dstSize, 0, 0, cv::INTER_NEAREST);

#ifdef DEBUG_PRINT
	XLOG_INFO(" @@@ ImagePre::decode_resize, dstWidth = " + std::to_string(dstWidth) + " @@@ ", CURRENT_THREAD_ID);
	XLOG_INFO(" @@@ ImagePre::decode_resize, dstHeight = " + std::to_string(dstHeight) + " @@@ ", CURRENT_THREAD_ID);
#endif
}

// Member function: Revert the box(x1,y1,x2,y2) to the spatial scale of the original image
void ImagePre::revertBox_resize_paste(int& x1,int& y1,int& x2,int& y2)
{
	std::unordered_map<std::string, std::any> inParams = std::any_cast<std::unordered_map<std::string, std::any>>(params_.params);
	std::unordered_map<std::string, std::any> decodeParams = std::any_cast<std::unordered_map<std::string, std::any>>(params_.decodeParams);

	int dx = std::any_cast<int>(decodeParams["dx"]);
	int dy = std::any_cast<int>(decodeParams["dy"]);
	int dstW = std::any_cast<int>(inParams["dstWidth"]);
	int dstH = std::any_cast<int>(inParams["dstHeight"]);
	int scaleW = dstW - 2 * dx;
	int scaleH = dstH - 2 * dy;

	float sx = std::any_cast<float>(decodeParams["sx"]);
	float sy = std::any_cast<float>(decodeParams["sy"]);

	// Calculate the width and height of the original image
	int rawWidth = static_cast<int>(scaleW / sx);
	int rawHeight = static_cast<int>(scaleH / sy);

	// Infer topLeftPoint(x1,y1), bottomRightPoint(x2,y2)  in the scale space of the original image
	x1 = static_cast<int>((x1 - dx) / sx);
	y1 = static_cast<int>((y1 - dy) / sy);
	x2 = static_cast<int>((x2 - dx) / sx);
	y2 = static_cast<int>((y2 - dy) / sy);

	// Filter out illegal boxes that exceed the original image size
	x1 = std::max(x1, 0);
	y1 = std::max(y1, 0);
	x2 = std::min(x2, rawWidth - 1);
	y2 = std::min(y2, rawHeight - 1);

#ifdef DEBUG_PRINT
	XLOG_INFO(" @@@ ImagePre::revertBox_resize_paste, rawWidth = " + std::to_string(rawWidth) + " @@@ ", CURRENT_THREAD_ID);
	XLOG_INFO(" @@@ ImagePre::revertBox_resize_paste, rawHeight = " + std::to_string(rawHeight) + " @@@ ", CURRENT_THREAD_ID);
#endif
}

// Member function: Reverts the box(x1,y1,x2,y2) to the spatial scale of the original image
void ImagePre::revertBox_resize(int& x1, int& y1, int& x2, int& y2)
{
	std::unordered_map<std::string, std::any> inParams = std::any_cast<std::unordered_map<std::string, std::any>>(params_.params);
	std::unordered_map<std::string, std::any> decodeParams = std::any_cast<std::unordered_map<std::string, std::any>>(params_.decodeParams);

	int dstW = std::any_cast<int>(inParams["targetWidth"]);
	int dstH = std::any_cast<int>(inParams["targetHeight"]);
	int scaleW = dstW;
	int scaleH = dstH;

	float sx = std::any_cast<float>(decodeParams["sx"]);
	float sy = std::any_cast<float>(decodeParams["sy"]);

	// Calculate the width and height of the original image
	int rawWidth = static_cast<int>(scaleW / sx);
	int rawHeight = static_cast<int>(scaleH / sy);

	// Infer topLeftPoint(x1,y1), bottomRightPoint(x2,y2)  in the scale space of the original image
	x1 = static_cast<int>(x1 / sx);
	y1 = static_cast<int>(y1 / sy);
	x2 = static_cast<int>(x2 / sx);
	y2 = static_cast<int>(y2 / sy);

	// Filter out illegal boxes that exceed the original image size
	x1 = std::max(x1, 0);
	y1 = std::max(y1, 0);
	x2 = std::min(x2, rawWidth - 1);
	y2 = std::min(y2, rawHeight - 1);

#ifdef DEBUG_PRINT
	XLOG_INFO(" @@@ ImagePre::revertBox_resize, rawWidth = " + std::to_string(rawWidth) + " @@@ ", CURRENT_THREAD_ID);
	XLOG_INFO(" @@@ ImagePre::revertBox_resize, rawHeight = " + std::to_string(rawHeight) + " @@@ ", CURRENT_THREAD_ID);
#endif
}

void ImagePre::normalize_01(const cv::Mat& srcImage, cv::Mat& dstImage)
{
	srcImage.convertTo(dstImage, CV_32FC3, 1.0 / 255.0);

#ifdef DEBUG_PRINT
	XLOG_INFO(" @@@ ImagePre::normalize_01, dstImage.cols = " + std::to_string(dstImage.cols) + " @@@ ", CURRENT_THREAD_ID);
	XLOG_INFO(" @@@ ImagePre::normalize_01, dstImage.rows = " + std::to_string(dstImage.rows) + " @@@ ", CURRENT_THREAD_ID);
#endif
}

void ImagePre::decode_normalize_01(const cv::Mat& srcImage, cv::Mat& dstImage)
{
	srcImage.convertTo(dstImage, CV_8UC3, 255.0);
}
