#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Handle/ImagePreHandle.h"
#include "Common/OpenCVHeaders.h"
#include "Common/ImagePre.h"

#define CHECK_NULLPTR(ptr) ((ptr) == nullptr)
#define CHECK_ObjectItem(object) ((object != nullptr) && ((object)->type & cJSON_Object))

ImagePreHandle::ImagePreHandle() : XBaseHandle()
{
	//std::cout << "class ImagePreHandle: public XBaseHandle " << std::endl;
	XLOG_INFO("class ImagePreHandle: public XBaseHandle ...", CURRENT_THREAD_ID);
	std::string classNameStr = "ImagePre";
	setClassName(classNameStr);
	createUniqueName(classNameStr);
	initParams();
}
ImagePreHandle::~ImagePreHandle()
{

}
XVisual::ErrorCode ImagePreHandle::setInnerParam(cJSON* innerParamPtr)
{
	cJSON* cjson_preParamPtr = cJSON_GetObjectItemCaseSensitive(innerParamPtr, "PreParam");
	if (CHECK_ObjectItem(cjson_preParamPtr))
	{
		XLOG_INFO("@@@ ImagePreHandle::setInnerParam @@@ ", CURRENT_THREAD_ID);
		PreParam mParams;
		ImagePre imgPre3(mParams);
		// Done, implement readPreParam function, which can read innerParam from Json 
		imgPre3.readPreParam(cjson_preParamPtr);
		mParams = imgPre3.getPreParam();
		innerParam["imagePreParams"] = mParams;
		return XVisual::ErrorCode::Success;
	}
	else
	{
		return XVisual::ErrorCode::ParseJsonUnknownObjectItemError;
	}
	
}
XVisual::ErrorCode ImagePreHandle::setOuterParam(std::unordered_map<std::string, cJSON*> outerParamUMap)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode ImagePreHandle::writeOuterParam(cJSON* cjson_variableSource, const std::string& xName)
{
	return XVisual::ErrorCode::Success;
}
XVisual::ErrorCode ImagePreHandle::writeInnerParam(cJSON* cjson_innerParam)
{
	cJSON* cjson_preparam = cJSON_CreateObject();
	PreParam mParams = std::any_cast<PreParam>(innerParam["imagePreParams"]);
	//Constructing an object is to call function "writePreParam" and write the parameter maintained by innerParam["imagePreParams"] into JSON
	ImagePre imgPre2(mParams);
	imgPre2.writePreParam(cjson_preparam);
	cJSON_AddItemToObject(cjson_innerParam, "PreParam", cjson_preparam);
	return XVisual::ErrorCode::Success;
}
void ImagePreHandle::initParams()
{
	cv::Mat image;
	REGISTER_MEMBER_ATTR_STR(sources, "image", image, false);
	REGISTER_TYPE_STR(sources, "image", image);
	cv::Mat outputImage;
	REGISTER_MEMBER_ATTR_STR(dests, "outputImage", outputImage, false);
	REGISTER_TYPE_STR(dests, "outputImage", outputImage);
	PreParam outputParam;
	REGISTER_MEMBER_ATTR_STR(dests, "outputParam", outputParam, false);
	REGISTER_TYPE_STR(dests, "outputParam", outputParam);
	// set initial PrePram
	std::string preTagStr = "ResizePaste";
	int dstWidth = 416;
	int dstHeight = 416;
	std::unordered_map<std::string, std::any> mParams;
	mParams["dstWidth"] = dstWidth;
	mParams["dstHeight"] = dstHeight;
	// Create a PreParam object
	PreParam imagePreParams;
	imagePreParams.preTag = preTagStr;
	imagePreParams.params = mParams;
	// Set the value corresponding to the key named "imagePreParams" in the innerParam dictionary to imagePreParams
	innerParam["imagePreParams"] = imagePreParams;
}
void ImagePreHandle::xOperate()
{
	cv::Mat image = GET_MEMBER_WITH_TYPE_STR(sources, cv::Mat, "image");
	if (!image.empty())
	{
		XLOG_INFO("ImagePreHandle::xOperate, image is NOT empty ", CURRENT_THREAD_ID);
		PreParam imagePreParams = std::any_cast<PreParam>(innerParam["imagePreParams"]);
		ImagePre imgPre1(imagePreParams);
		cv::Mat outputImage;
		imgPre1.preprocess(image, outputImage);
		REGISTER_MEMBER_STR(dests, "outputImage", outputImage);
		// Update "imaegPreParams" because PreParams.decodeParams has been updated after executing the preoprocess operation
		innerParam["imagePreParams"] = imgPre1.getPreParam();
		REGISTER_MEMBER_STR(dests, "outputParam", innerParam["imagePreParams"]);
	}
	else
	{
		XLOG_INFO("ImagePreHandle::xOperate, image is EMPTY ", CURRENT_THREAD_ID);
	}
}
void ImagePreHandle::setImagePreParams(const PreParam& mImagePreParams)
{
	innerParam["imagePreParams"] = mImagePreParams;
}
PreParam ImagePreHandle::getImagePreParams()
{
	return std::any_cast<PreParam>(innerParam["imagePreParams"]);
}

REGISTER_HANDLE(ImagePre);