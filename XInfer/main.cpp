//
// Created by Liheng on 2024-04-28.
//

#include <stdio.h>
#include <stdlib.h>
#include "XInfer.h"
#include "Common/OpenCVHeaders.h"
#include "GlobalStorage/GlobalVariable.h"

int main()
{
	// const char* filename1 = "C:\\NDev\\CPPDev\\CodeBlocksWorkspace\\CPPTutorial\\cJsonDemo\\sample3.json";
	const char* filename = "C:\\NDev\\CPPDev\\XVisual\\XVisual\\WorkSpace\\solutions.json";
	XInfer& infer = XInfer::getInstance();
	infer.init(filename);
	// XLOG_INFO("@@@ globalWorkSpaceDir = " + globalWorkSpaceDir, CURRENT_THREAD_ID);
	infer.build();
	infer.exec();
	std::any result;
	infer.getResult(result);
	cv::Mat dstImage = std::any_cast<cv::Mat>(result);
	const std::string dstFileName = "C:\\NDev\\CPPDev\\XVisual\\XInfer\\InferLog\\dst.png";
	cv::imwrite(dstFileName, dstImage);

	//std::cout << "OK ... " << std::endl;

	XLOG_INFO("== === ===  OK ...  === === ===", CURRENT_THREAD_ID);

	system("pause");
	return 0;
}