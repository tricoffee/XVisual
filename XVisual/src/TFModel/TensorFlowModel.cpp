#include "TFModel/TensorFlowModel.h"
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"

namespace XVisual {

TensorFlowModel::TensorFlowModel() : status_(nullptr), session_(nullptr), graph_(nullptr), session_opts_(nullptr), run_options_(nullptr), metagraph_(nullptr) {}

TensorFlowModel::~TensorFlowModel()
{
	// 释放资源
	for (int i = 0; i < NumInputs_; ++i)
	{
		if (InputValues_[i] != nullptr)
		{
			TF_DeleteTensor(InputValues_[i]);
		}

	}
	for (int i = 0; i < NumOutputs_; ++i)
	{
		if (OutputValues_[i] != nullptr)
		{
			TF_DeleteTensor(OutputValues_[i]);
		}
	}
	if (session_ != nullptr) TF_DeleteSession(session_, status_);
	if (metagraph_ != nullptr) TF_DeleteBuffer(metagraph_);
	if (graph_ != nullptr) TF_DeleteGraph(graph_);
	if (run_options_ != nullptr) TF_DeleteBuffer(run_options_);
	if (session_opts_ != nullptr) TF_DeleteSessionOptions(session_opts_);
	if (status_ != nullptr) TF_DeleteStatus(status_);
}

void TensorFlowModel::printOperations(const std::string& tfOperNameRecordDir)
{
	const std::string tfOperNameRecordPath = tfOperNameRecordDir + "tfOperationNameRecord.txt";

	// 打开文件流以写入操作名称记录
	std::ofstream outFile(tfOperNameRecordPath);
	if (!outFile.is_open())
	{
		std::cerr << "Failed to open file: " << tfOperNameRecordPath << std::endl;
		/*	return;*/
	}

	size_t pos = 0;
	TF_Operation* oper = TF_GraphNextOperation(graph_, &pos);

	while (oper != nullptr)
	{
		std::string tfOperName = TF_OperationName(oper);
		//std::cout << "Operation name: " << TF_OperationName(oper) << std::endl;
		std::cout << "Operation name: " << tfOperName << std::endl;
		if (outFile.is_open())
		{
			// 将操作名称写入文件
			outFile << tfOperName << std::endl;
		}
		oper = TF_GraphNextOperation(graph_, &pos);
	}
}

bool TensorFlowModel::loadSavedModel(const std::string& saved_model_dir)
{

	// 定义数据目录和文件名
	// e.g., saved_model_dir = "C:\\NDev\\code\\logs\\mAP_Shiyan3_11\\exported_model"
	const std::string file_name = "saved_model_info.txt";

	// 创建 std::filesystem::path 对象表示数据目录和文件名
	std::filesystem::path file_path = saved_model_dir;
	file_path /= file_name; // 使用 /= 运算符进行路径拼接

	std::string filePath2 = file_path.string();

	XLOG_INFO("TensorFlowModel::loadSavedModel, file_path = " + filePath2, CURRENT_THREAD_ID);

	acquireModelInfo(file_path.string(), input_key_, inputType1Str_, inputShape1Str_, inputName1Str_,
		output_key_, outputType1Str_, outputShape1Str_, outputName1Str_);

	//std::cout << "input_key_ =" << input_key_ << std::endl;
	//std::cout << "ouput_key_ =" << output_key_ << std::endl;


	input_key_ = "image_input";
	output_key_ = "detections";

	status_ = TF_NewStatus();
	graph_ = TF_NewGraph();
	session_opts_ = TF_NewSessionOptions();
	run_options_ = nullptr;
	metagraph_ = TF_NewBuffer();

	//这一部分转移到private
	//            const char* tag1 = "serve";
	//            const char* const tags[] = { tag1 };
	//            int num_tags = 1;


	session_ = TF_LoadSessionFromSavedModel(session_opts_, run_options_,
		saved_model_dir.c_str(), tags_,
		num_tags_, graph_, metagraph_, status_);
	if (TF_GetCode(status_) != TF_OK)
	{
		std::cerr << "Error loading SavedModel: " << TF_Message(status_) << std::endl;
		TF_DeleteBuffer(metagraph_);
		TF_DeleteBuffer(run_options_);
		TF_DeleteSessionOptions(session_opts_);
		TF_DeleteStatus(status_);
		return false;
	}

	tensorflow::MetaGraphDef metagraph_def;
	metagraph_def.ParseFromArray(metagraph_->data, metagraph_->length);
	const auto signature_def_map = metagraph_def.signature_def();
	const auto signature_def = signature_def_map.at("serving_default");
	//这里改为调用acquireModelInfo函数自动获取input_key_和output_key_
	// input_key_ = "image_input";
	// output_key_ = "detections";
	const std::string input_name_str = signature_def.inputs().at(input_key_).name();
	const std::string output_name_str = signature_def.outputs().at(output_key_).name();
	// input_name_ 等同于 inputName1Str
	input_name_ = extractSubstrBeforeDelimiter(input_name_str, ":");
	// output_name_ 等同于 outputName1Str
	output_name_ = extractSubstrBeforeDelimiter(output_name_str, ":");
	const char* input_name_cstr = input_name_.c_str();
	const char* output_name_cstr = output_name_.c_str();

	if (input_name_.compare(inputName1Str_) == 0)
	{
		std::cout << "input_name_ 等同于 inputName1Str_ " << std::endl;
	}

	if (output_name_.compare(outputName1Str_) == 0)
	{
		std::cout << "output_name_ 等同于 outputName1Str_ " << std::endl;
	}


	//get input tensor
    //int NumInputs = 1; 此处仅考虑单输入的情况
	inputs_ = reinterpret_cast<TF_Output*>(std::malloc(sizeof(TF_Output) * NumInputs_));
	TF_Output t1 = { TF_GraphOperationByName(graph_, input_name_cstr), 0 };
	if (t1.oper == NULL)
		printf("ERROR: Failed TF_GraphOperationByName \"%s\"\n", input_name_cstr);
	else
		printf("TF_GraphOperationByName \"%s\" is OK\n", input_name_cstr);
	inputs_[0] = t1;

	//get output tensor
    //int NumOutputs = 1; 此处仅考虑单输出的情况
	outputs_ = reinterpret_cast<TF_Output*>(std::malloc(sizeof(TF_Output) * NumOutputs_));
	TF_Output t2 = { TF_GraphOperationByName(graph_, output_name_cstr), 0 };
	if (t2.oper == NULL)
		printf("ERROR: Failed TF_GraphOperationByName \"%s\"\n", output_name_cstr);
	else
		printf("TF_GraphOperationByName \"%s\" is OK\n", output_name_cstr);
	outputs_[0] = t2;

	// Allocate data for inputs & outputs
	InputValues_ = reinterpret_cast<TF_Tensor**>(malloc(sizeof(TF_Tensor*) * NumInputs_));
	OutputValues_ = reinterpret_cast<TF_Tensor**>(malloc(sizeof(TF_Tensor*) * NumOutputs_));

	// Allocate data for inputs
	for (int i = 0; i < NumInputs_; ++i)
	{
		// 根据输入张量的形状和数据类型创建 TF_Tensor
		// 读取saved_model_info.txt里面的inputs的某个input的shape并解析, 不用手动设置 const std::vector<int64_t> input_dims = { 1, 416, 416, 3 };
		std::vector<int64_t> input_dims = parseShape<int64_t>(inputShape1Str_);
		input_dims[0] = 1;
		const size_t input_dims_num = input_dims.size();
		const int input_elements_num = std::accumulate(input_dims.begin(), input_dims.end(), 1, std::multiplies<int64_t>());
		// 输入张量的数据类型, const TF_DataType input_type = TF_FLOAT; 现改为从saved_model_info.txt自动获取
		std::cout << "inputType1Str_ =" << inputType1Str_ << std::endl;
		const ::TF_DataType input_type = parseDataTypeStr(inputType1Str_);

		//Error 模板参数包含非静态存储持续时间的变量不能用作非类型参数
        //const TF_DataType input_type2 = input_type;
        //使用 using 语句定义变量的类型
        //using MyFloatType2 = decltype(getCppType<input_type2>());
        //声明变量并调用 getCppType<TF_FLOAT>() 赋值
        //MyFloatType2 value2 = getCppType<input_type2>();

		InputValues_[i] = TF_AllocateTensor(input_type, input_dims.data(), input_dims_num, input_elements_num * sizeof(float));
	}

	// Allocate data for outputs
	for (int i = 0; i < NumOutputs_; ++i)
	{
		// 根据输出张量的形状和数据类型创建 TF_Tensor
		//读取saved_model_info.txt里面outputs的某个output的shape并解析, 不用手动设置 const std::vector<int64_t> input_dims = { 1, 100, 6 };
		std::vector<int64_t> output_dims = parseShape<int64_t>(outputShape1Str_);
		output_dims[0] = 1;
		const size_t output_dims_num = output_dims.size();
		const int output_elements_num = std::accumulate(output_dims.begin(), output_dims.end(), 1, std::multiplies<int64_t>());
		// 输出张量的数据类型, 比如, const TF_DataType output_type = TF_FLOAT; 现改为从saved_model_info.txt自动获取
		std::cout << "outputType1Str_ =" << outputType1Str_ << std::endl;
		const ::TF_DataType output_type = parseDataTypeStr(outputType1Str_);
		OutputValues_[i] = TF_AllocateTensor(output_type, output_dims.data(), output_dims_num, output_elements_num * sizeof(float));
	}

	return true;
}

void TensorFlowModel::loadInputData(cv::Mat image)
{
	input_data_ = static_cast<float*>(TF_TensorData(InputValues_[0])); // 获取输入张量的数据指针
	const int height = image.rows;
	const int width = image.cols;

	image_height_ = height;
	image_width_ = width;

	const int channels = image.channels();

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			for (int c = 0; c < channels; ++c)
			{
				input_data_[y * width * channels + x * channels + c] = static_cast<float>(image.at<cv::Vec3b>(y, x).val[c]) / 255.0f;
			}
		}
	}
}

void TensorFlowModel::doRun()
{
	// 执行 TensorFlow 会话
	TF_SessionRun(session_, nullptr, inputs_, InputValues_, NumInputs_, outputs_, OutputValues_, NumOutputs_, nullptr, 0, nullptr, status_);
}

// The coordinate size of the Results obtained here is relative to the model input scale, not to the original image size
void TensorFlowModel::getResults(std::vector<DetectResult>& results)
{
	// 获取输出张量的值
	float* output_data = static_cast<float*>(TF_TensorData(OutputValues_[0])); // 获取输出张量的数据指针
	// 输出张量的形状, 比如 const std::vector<int64_t> output_dims = { 1, 100, 6 }; 现改为从saved_model_info.txt文本自动获取
	std::vector<int64_t> output_dims = parseShape<int64_t>(outputShape1Str_);
	output_dims[0] = 1;
	const int output_elements_num = std::accumulate(output_dims.begin(), output_dims.end(), 1, std::multiplies<int64_t>());
	acquireValidBoxes(output_data, output_elements_num, image_width_, image_height_, results);
}

} // namespace XVisual

