//
// Created by Liheng on 2024-04-28.
//

#include <stdio.h>
#include <stdlib.h>
#include "XInfer.h"
#include "Common/OpenCVHeaders.h"

int main()
{
	// const char* filename1 = "C:\\NDev\\CPPDev\\CodeBlocksWorkspace\\CPPTutorial\\cJsonDemo\\sample3.json";
	const char* filename = "C:\\NDev\\CPPDev\\XVisual\\XVisual\\logs\\output.json";
	XInfer& infer = XInfer::getInstance();
	infer.init(filename);
	infer.build();
	infer.exec();
	std::any result;
	infer.getResult(result);
	cv::Mat dstImage = std::any_cast<cv::Mat>(result);
	const std::string dstFileName = "C:\\NDev\\CPPDev\\XVisual\\XInfer\\InferLog\\dst.png";
	cv::imwrite(dstFileName, dstImage);

	//std::cout << "OK ... " << std::endl;

	XLOG_INFO("== === ===  OK ...  === === ===", CURRENT_THREAD_ID);

	return 0;
}