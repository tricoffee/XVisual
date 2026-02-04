#include "Backend/TensorFlow/TensorFlowBackend.h"
#include "Core/Inference/BackendRegistry.h"
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "Common/OpenCVHeaders.h"
#include "Common/DetectResult.h"
#include "TFModel/ModelUtils.h"
#include "Common/StrUtils.h"
#include "Common/BoxUtils.h"

// TensorFlow C API headers
#include "tensorflow/c/c_api.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/protobuf/meta_graph.pb.h"
#include "tensorflow/core/protobuf/config.pb.h"

#include <fstream>
#include <numeric>
#include <chrono>

namespace XVisual {

// TensorFlowSession 析构函数
TensorFlowSession::~TensorFlowSession()
{
    if (inputs != nullptr)
    {
        std::free(inputs);
        inputs = nullptr;
    }
    if (outputs != nullptr)
    {
        std::free(outputs);
        outputs = nullptr;
    }
    if (session != nullptr && status != nullptr)
    {
        TF_DeleteSession(session, status);
        session = nullptr;
    }
    if (metagraph != nullptr)
    {
        TF_DeleteBuffer(metagraph);
        metagraph = nullptr;
    }
    if (graph != nullptr)
    {
        TF_DeleteGraph(graph);
        graph = nullptr;
    }
    if (sessionOpts != nullptr)
    {
        TF_DeleteSessionOptions(sessionOpts);
        sessionOpts = nullptr;
    }
    if (status != nullptr)
    {
        TF_DeleteStatus(status);
        status = nullptr;
    }
}

// 移动构造函数
TensorFlowSession::TensorFlowSession(TensorFlowSession&& other) noexcept
    : graph(other.graph)
    , session(other.session)
    , status(other.status)
    , metagraph(other.metagraph)
    , sessionOpts(other.sessionOpts)
    , deviceSpec(std::move(other.deviceSpec))
    , modelPath(std::move(other.modelPath))
    , inputName(std::move(other.inputName))
    , outputName(std::move(other.outputName))
    , inputShape(std::move(other.inputShape))
    , outputShape(std::move(other.outputShape))
    , inputs(other.inputs)
    , outputs(other.outputs)
    , numInputs(other.numInputs)
    , numOutputs(other.numOutputs)
{
    other.graph = nullptr;
    other.session = nullptr;
    other.status = nullptr;
    other.metagraph = nullptr;
    other.sessionOpts = nullptr;
    other.inputs = nullptr;
    other.outputs = nullptr;
}

// 移动赋值
TensorFlowSession& TensorFlowSession::operator=(TensorFlowSession&& other) noexcept
{
    if (this != &other)
    {
        // 清理当前资源
        this->~TensorFlowSession();
        
        // 移动资源
        graph = other.graph;
        session = other.session;
        status = other.status;
        metagraph = other.metagraph;
        sessionOpts = other.sessionOpts;
        deviceSpec = std::move(other.deviceSpec);
        modelPath = std::move(other.modelPath);
        inputName = std::move(other.inputName);
        outputName = std::move(other.outputName);
        inputShape = std::move(other.inputShape);
        outputShape = std::move(other.outputShape);
        inputs = other.inputs;
        outputs = other.outputs;
        numInputs = other.numInputs;
        numOutputs = other.numOutputs;
        
        other.graph = nullptr;
        other.session = nullptr;
        other.status = nullptr;
        other.metagraph = nullptr;
        other.sessionOpts = nullptr;
        other.inputs = nullptr;
        other.outputs = nullptr;
    }
    return *this;
}

// TensorFlowBackend 构造函数
TensorFlowBackend::TensorFlowBackend()
{
    XLOG_INFO("TensorFlowBackend: Created", CURRENT_THREAD_ID);
}

// TensorFlowBackend 析构函数
TensorFlowBackend::~TensorFlowBackend()
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.clear();
    XLOG_INFO("TensorFlowBackend: Destroyed", CURRENT_THREAD_ID);
}

bool TensorFlowBackend::supports(const ModelDesc& modelDesc) const
{
    return modelDesc.format == "tf_savedmodel";
}

bool TensorFlowBackend::supportsDevice(DeviceType type) const
{
    switch (type)
    {
    case DeviceType::CPU:
        return true;
    case DeviceType::GPU:
        return true;  // TensorFlow 支持 GPU（如果有 CUDA）
    case DeviceType::NPU:
        return false; // TensorFlow 原生不支持 NPU
    case DeviceType::Any:
        return true;
    default:
        return false;
    }
}

TF_SessionOptions* TensorFlowBackend::createSessionOptions(
    const DeviceSpec& deviceSpec,
    const BackendOptions& options)
{
    TF_SessionOptions* opts = TF_NewSessionOptions();
    
    // 创建 ConfigProto
    tensorflow::ConfigProto config;
    config.set_allow_soft_placement(true);
    
    switch (deviceSpec.type)
    {
    case DeviceType::CPU:
        // 强制 CPU：禁用所有 GPU
        (*config.mutable_device_count())["GPU"] = 0;
        XLOG_INFO("TensorFlowBackend: Configured for CPU only", CURRENT_THREAD_ID);
        break;
        
    case DeviceType::GPU:
        {
            auto* gpuOpts = config.mutable_gpu_options();
            
            // 指定可见 GPU
            gpuOpts->set_visible_device_list(std::to_string(deviceSpec.index));
            
            // 内存配置
            if (options.allowGrowthMemory)
            {
                gpuOpts->set_allow_growth(true);
            }
            else if (options.memoryFraction > 0.0f)
            {
                gpuOpts->set_per_process_gpu_memory_fraction(options.memoryFraction);
            }
            
            XLOG_INFO("TensorFlowBackend: Configured for GPU[" + std::to_string(deviceSpec.index) + 
                      "], allow_growth=" + std::to_string(options.allowGrowthMemory), CURRENT_THREAD_ID);
        }
        break;
        
    case DeviceType::Any:
    default:
        // 默认：允许 TensorFlow 自动选择
        XLOG_INFO("TensorFlowBackend: Configured for auto device selection", CURRENT_THREAD_ID);
        break;
    }
    
    // 并行度配置
    if (options.intraOpParallelism > 0)
    {
        config.set_intra_op_parallelism_threads(options.intraOpParallelism);
    }
    if (options.interOpParallelism > 0)
    {
        config.set_inter_op_parallelism_threads(options.interOpParallelism);
    }
    
    // 序列化并设置配置
    std::string configData;
    config.SerializeToString(&configData);
    
    TF_Status* status = TF_NewStatus();
    TF_SetConfig(opts, configData.data(), configData.size(), status);
    
    if (TF_GetCode(status) != TF_OK)
    {
        XLOG_INFO("TensorFlowBackend: Warning - Failed to set config: " + 
                  std::string(TF_Message(status)), CURRENT_THREAD_ID);
    }
    
    TF_DeleteStatus(status);
    return opts;
}

ModelHandle TensorFlowBackend::load(
    const ModelDesc& modelDesc,
    const DeviceSpec& deviceSpec,
    const BackendOptions& options)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    XLOG_INFO("TensorFlowBackend: Loading model from " + modelDesc.path + 
              " on device " + deviceSpec.toString(), CURRENT_THREAD_ID);
    
    auto tfSession = std::make_unique<TensorFlowSession>();
    tfSession->modelPath = modelDesc.path;
    tfSession->deviceSpec = deviceSpec;
    
    // 初始化 TF 资源
    tfSession->status = TF_NewStatus();
    tfSession->graph = TF_NewGraph();
    tfSession->sessionOpts = createSessionOptions(deviceSpec, options);
    tfSession->metagraph = TF_NewBuffer();
    
    // 加载 SavedModel
    const char* tags[] = {"serve"};
    tfSession->session = TF_LoadSessionFromSavedModel(
        tfSession->sessionOpts,
        nullptr,  // run_options
        modelDesc.path.c_str(),
        tags,
        1,  // num_tags
        tfSession->graph,
        tfSession->metagraph,
        tfSession->status);
    
    if (TF_GetCode(tfSession->status) != TF_OK)
    {
        std::string error = "Failed to load SavedModel: " + std::string(TF_Message(tfSession->status));
        XLOG_INFO("TensorFlowBackend: " + error, CURRENT_THREAD_ID);
        throw std::runtime_error(error);
    }
    
    // 解析 MetaGraph 获取输入/输出名称
    tensorflow::MetaGraphDef metagraph_def;
    metagraph_def.ParseFromArray(tfSession->metagraph->data, tfSession->metagraph->length);
    
    const auto& signature_def_map = metagraph_def.signature_def();
    const auto& signature_def = signature_def_map.at("serving_default");
    
    // 获取输入名称 (假设 input_key 为 "image_input")
    std::string input_key = "image_input";
    if (signature_def.inputs().count(input_key))
    {
        const std::string input_name_str = signature_def.inputs().at(input_key).name();
        tfSession->inputName = extractSubstrBeforeDelimiter(input_name_str, ":");
    }
    
    // 获取输出名称 (假设 output_key 为 "detections")
    std::string output_key = "detections";
    if (signature_def.outputs().count(output_key))
    {
        const std::string output_name_str = signature_def.outputs().at(output_key).name();
        tfSession->outputName = extractSubstrBeforeDelimiter(output_name_str, ":");
    }
    
    XLOG_INFO("TensorFlowBackend: Input=" + tfSession->inputName + 
              ", Output=" + tfSession->outputName, CURRENT_THREAD_ID);
    
    // 设置输入/输出 TF_Output
    tfSession->inputs = reinterpret_cast<TF_Output*>(std::malloc(sizeof(TF_Output) * tfSession->numInputs));
    TF_Output t1 = {TF_GraphOperationByName(tfSession->graph, tfSession->inputName.c_str()), 0};
    if (t1.oper == nullptr)
    {
        XLOG_INFO("TensorFlowBackend: Warning - Input operation not found: " + tfSession->inputName, CURRENT_THREAD_ID);
    }
    tfSession->inputs[0] = t1;
    
    tfSession->outputs = reinterpret_cast<TF_Output*>(std::malloc(sizeof(TF_Output) * tfSession->numOutputs));
    TF_Output t2 = {TF_GraphOperationByName(tfSession->graph, tfSession->outputName.c_str()), 0};
    if (t2.oper == nullptr)
    {
        XLOG_INFO("TensorFlowBackend: Warning - Output operation not found: " + tfSession->outputName, CURRENT_THREAD_ID);
    }
    tfSession->outputs[0] = t2;
    
    // 分配句柄并存储
    ModelHandle handle = nextHandle_++;
    sessions_[handle] = std::move(tfSession);
    
    XLOG_INFO("TensorFlowBackend: Model loaded successfully, handle=" + std::to_string(handle), CURRENT_THREAD_ID);
    return handle;
}

RunResult TensorFlowBackend::run(
    ModelHandle handle,
    const InputMap& inputs,
    OutputMap* outputs)
{
    auto startTime = std::chrono::steady_clock::now();
    
    TensorFlowSession* session = getSession(handle);
    if (!session)
    {
        return RunResult::Failure("Invalid model handle");
    }
    
    // 获取输入图像
    auto it = inputs.find("image_input");
    if (it == inputs.end())
    {
        // 尝试其他常见名称
        it = inputs.find("input_image");
    }
    if (it == inputs.end())
    {
        return RunResult::Failure("Input 'image_input' not found");
    }
    
    cv::Mat image;
    try
    {
        image = std::any_cast<cv::Mat>(it->second);
    }
    catch (const std::bad_any_cast&)
    {
        return RunResult::Failure("Input is not cv::Mat type");
    }
    
    if (image.empty())
    {
        return RunResult::Failure("Input image is empty");
    }
    
    const int height = image.rows;
    const int width = image.cols;
    const int channels = image.channels();
    
    // 创建输入 Tensor
    std::vector<int64_t> input_dims = {1, height, width, channels};
    const int input_elements = height * width * channels;
    
    TF_Tensor* inputTensor = TF_AllocateTensor(
        TF_FLOAT,
        input_dims.data(),
        input_dims.size(),
        input_elements * sizeof(float));
    
    // 填充输入数据
    float* input_data = static_cast<float*>(TF_TensorData(inputTensor));
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            for (int c = 0; c < channels; ++c)
            {
                input_data[y * width * channels + x * channels + c] = 
                    static_cast<float>(image.at<cv::Vec3b>(y, x).val[c]) / 255.0f;
            }
        }
    }
    
    TF_Tensor* inputValues[] = {inputTensor};
    TF_Tensor* outputValues[1] = {nullptr};
    
    // 执行推理
    TF_SessionRun(
        session->session,
        nullptr,  // run_options
        session->inputs, inputValues, session->numInputs,
        session->outputs, outputValues, session->numOutputs,
        nullptr, 0,  // target_opers
        nullptr,  // run_metadata
        session->status);
    
    TF_DeleteTensor(inputTensor);
    
    if (TF_GetCode(session->status) != TF_OK)
    {
        std::string error = "TF_SessionRun failed: " + std::string(TF_Message(session->status));
        return RunResult::Failure(error);
    }
    
    // 处理输出
    if (outputValues[0] != nullptr && outputs != nullptr)
    {
        float* output_data = static_cast<float*>(TF_TensorData(outputValues[0]));
        
        // 获取输出形状
        int numDims = TF_NumDims(outputValues[0]);
        std::vector<int64_t> output_dims(numDims);
        for (int i = 0; i < numDims; ++i)
        {
            output_dims[i] = TF_Dim(outputValues[0], i);
        }
        
        int output_elements = 1;
        for (auto d : output_dims)
        {
            output_elements *= static_cast<int>(d);
        }
        
        // 解析检测结果
        std::vector<DetectResult> detectResults;
        acquireValidBoxes(output_data, output_elements, width, height, detectResults);
        
        (*outputs)["detections"] = detectResults;
        (*outputs)["detectResults"] = detectResults;
        
        TF_DeleteTensor(outputValues[0]);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    XLOG_INFO("TensorFlowBackend: Run completed in " + std::to_string(duration.count() / 1000.0) + "ms", CURRENT_THREAD_ID);
    
    return RunResult::Success(duration);
}

void TensorFlowBackend::unload(ModelHandle handle)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(handle);
    if (it != sessions_.end())
    {
        XLOG_INFO("TensorFlowBackend: Unloading model, handle=" + std::to_string(handle), CURRENT_THREAD_ID);
        sessions_.erase(it);
    }
}

std::vector<std::string> TensorFlowBackend::getInputNames(ModelHandle handle) const
{
    const TensorFlowSession* session = getSession(handle);
    if (session)
    {
        return {session->inputName};
    }
    return {};
}

std::vector<std::string> TensorFlowBackend::getOutputNames(ModelHandle handle) const
{
    const TensorFlowSession* session = getSession(handle);
    if (session)
    {
        return {session->outputName};
    }
    return {};
}

TensorFlowSession* TensorFlowBackend::getSession(ModelHandle handle)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(handle);
    if (it != sessions_.end())
    {
        return it->second.get();
    }
    return nullptr;
}

const TensorFlowSession* TensorFlowBackend::getSession(ModelHandle handle) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(handle);
    if (it != sessions_.end())
    {
        return it->second.get();
    }
    return nullptr;
}

// 注册后端
REGISTER_BACKEND(TensorFlowBackend);

} // namespace XVisual
