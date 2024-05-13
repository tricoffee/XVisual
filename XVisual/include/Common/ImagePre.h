#ifndef ImagePre_H
#define ImagePre_H

#include <iostream>
#include <unordered_map>
#include <string>
#include "OpenCVHeaders.h"
#include "Common/PreParam.h"
#include "cJSON.h"
#include "Common/ErrorCode.h"

class ImagePre
{
	public:
		ImagePre(const PreParam& params, bool printDebugInfo=false);
		void preprocess(const cv::Mat& srcImage, cv::Mat& dstImage);
		void decode(const cv::Mat& srcImage, cv::Mat& dstImage);
		std::unordered_map<std::string, std::any> getDecodeParams();
		PreParam getPreParam();
		XVisual::ErrorCode readPreParam(cJSON* cjson_preParamPtr);
		XVisual::ErrorCode writePreParam(cJSON* cjson_preparam);
	private:
		bool printDebugInfo_ = false;

		PreParam params_;

		// Member function: Scaling and pasting images
		void resize_paste(const cv::Mat& srcImage, cv::Mat& dstImage);
		// Member function: Scaling images
		void resize(const cv::Mat& inImage, cv::Mat& outImage);

		// Member function: Restores the processed image to its original size
		void decode_resize_paste(const cv::Mat& srcImage, cv::Mat& dstImage);
		void decode_resize(const cv::Mat& inImage, cv::Mat& outImage);

		void normalize_01(const cv::Mat& srcImage, cv::Mat& dstImage);
		void decode_normalize_01(const cv::Mat& srcImage, cv::Mat& dstImage);
};

#endif //ImagePre_H