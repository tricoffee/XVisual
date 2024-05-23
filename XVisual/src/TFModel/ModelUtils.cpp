#include "TFModel/ModelUtils.h"
#include <fstream>
#include <vector>
#include "Common/StrUtils.h"
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"

void acquireModelInfo(std::string saved_model_txt_fnm,
	std::string& inputKey1, std::string& inputType1Str, std::string& inputShape1Str, std::string& inputName1Str,
	std::string& outputKey1, std::string& outputType1Str, std::string& outputShape1Str, std::string& outputName1Str
)
{

	//    std::cout << "acquireModelInfo ========" << std::endl;
	std::ifstream file(saved_model_txt_fnm);
	std::string line;

	//行号索引
	//当前行号的索引
	int currentLineIndex = 0;
	//"inputs[\""所在行号的索引
	int inputKey1LineIndex = -1;
	//"outputs[\""所在行号的索引
	int outputKey1LineIndex = -1;

	int inputType1LineIndex = 0;
	int inputShape1LineIndex = 0;
	int inputName1LineIndex = 0;

	int outputType1LineIndex = 0;
	int outputShape1LineIndex = 0;
	int outputName1LineIndex = 0;

	std::string inputStr1 = "inputs[\"";
	std::string outputStr1 = "outputs[\"";
	std::string strEnd = "\"]";

	int inputStr1Size = inputStr1.size();
	int outputStr1Size = outputStr1.size();

	std::vector<std::string> lines;

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			lines.push_back(line);

			// 寻找包含输入和输出信息的行
			if (line.find(inputStr1) != std::string::npos)
			{
				// 提取输入的字符串
				inputKey1 = line.substr(line.find(inputStr1) + inputStr1Size, line.find(strEnd) - (line.find(inputStr1) + inputStr1Size));
				// 记录inputStr1对应的行号
				inputKey1LineIndex = currentLineIndex;
				//                std::cout << "====" << "inputKey1" << inputKey1 << std::endl;
			}
			if (line.find(outputStr1) != std::string::npos)
			{
				// 提取输出的字符串
				outputKey1 = line.substr(line.find(outputStr1) + outputStr1Size, line.find(strEnd) - (line.find(outputStr1) + outputStr1Size));
				// 记录outputStr1对应的行号
				outputKey1LineIndex = currentLineIndex;
				//                std::cout << "====" << "outputKey1" << outputKey1 << std::endl;
			}
			currentLineIndex++; //递增当前行号
		}
		file.close();
	}
	else
	{
	    //std::cout << "没有打开" << saved_model_txt_fnm << std::endl;
	}
	XLOG_INFO(" Open " + saved_model_txt_fnm + " Successfully! ", CURRENT_THREAD_ID);

	if (inputKey1LineIndex != -1)
	{
		inputType1LineIndex = inputKey1LineIndex + 1;
		inputShape1LineIndex = inputKey1LineIndex + 2;
		inputName1LineIndex = inputKey1LineIndex + 3;
		std::string inputType1Str_0 = strip(lines[inputType1LineIndex]);
		std::string inputShape1Str_0 = strip(lines[inputShape1LineIndex]);
		std::string inputName1Str_0 = strip(lines[inputName1LineIndex]);
		std::string inputType1Str_1 = stripPrefix(inputType1Str_0, "dtype:");
		std::string inputShape1Str_1 = stripPrefix(inputShape1Str_0, "shape:");
		std::string inputName1Str_1 = stripPrefix(inputName1Str_0, "name:");
		inputType1Str = strip(inputType1Str_1);
		inputShape1Str = strip(inputShape1Str_1);
		std::string inputName1Str_2 = strip(inputName1Str_1);
		inputName1Str = extractSubstrBeforeDelimiter(inputName1Str_2, ":");
	}

	XLOG_INFO(" inputKey1 = " + inputKey1, CURRENT_THREAD_ID);
	XLOG_INFO(" inputType1Str = " + inputType1Str, CURRENT_THREAD_ID);
	XLOG_INFO(" inputShape1Str = " + inputShape1Str, CURRENT_THREAD_ID);
	XLOG_INFO(" inputName1Str = " + inputName1Str, CURRENT_THREAD_ID);

	if (outputKey1LineIndex != -1)
	{
		outputType1LineIndex = outputKey1LineIndex + 1;
		outputShape1LineIndex = outputKey1LineIndex + 2;
		outputName1LineIndex = outputKey1LineIndex + 3;
		std::string outputType1Str_0 = strip(lines[outputType1LineIndex]);
		std::string outputShape1Str_0 = strip(lines[outputShape1LineIndex]);
		std::string outputName1Str_0 = strip(lines[outputName1LineIndex]);
		std::string outputType1Str_1 = stripPrefix(outputType1Str_0, "dtype:");
		std::string outputShape1Str_1 = stripPrefix(outputShape1Str_0, "shape:");
		std::string outputName1Str_1 = stripPrefix(outputName1Str_0, "name:");
		outputType1Str = strip(outputType1Str_1);
		outputShape1Str = strip(outputShape1Str_1);
		std::string outputName1Str_2 = strip(outputName1Str_1);
		outputName1Str = extractSubstrBeforeDelimiter(outputName1Str_2, ":");
	}

	XLOG_INFO(" outputKey1 = " + outputKey1, CURRENT_THREAD_ID);
	XLOG_INFO(" outputType1Str = " + outputType1Str, CURRENT_THREAD_ID);
	XLOG_INFO(" outputShape1Str = " + outputShape1Str, CURRENT_THREAD_ID);
	XLOG_INFO(" outputName1Str = " + outputName1Str, CURRENT_THREAD_ID);

}
