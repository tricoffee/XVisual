#ifndef TENSORFLOWBACKEND_H
#define TENSORFLOWBACKEND_H

#include "Core/Inference/IInferenceBackend.h"
#include <unordered_map>
#include <memory>
#include <mutex>
#include <vector>

// Forward declarations (避免在头文件中 include TF)
struct TF_Graph;
struct TF_Session;
struct TF_Status;
struct TF_SessionOptions;
struct TF_Buffer;
struct TF_Output;
struct TF_Tensor;

namespace XVisual {

/**
 * TensorFlowSession: TensorFlow 会话封装
 * 
 * 封装 TensorFlow C API 的会话相关资源。
 */
struct TensorFlowSession
{
    TF_Graph* graph = nullptr;
    TF_Session* session = nullptr;
    TF_Status* status = nullptr;
    TF_Buffer* metagraph = nullptr;
    TF_SessionOptions* sessionOpts = nullptr;
    
    DeviceSpec deviceSpec;
    std::string modelPath;
    
    // 输入/输出节点信息
    std::string inputName;
    std::string outputName;
    std::vector<int64_t> inputShape;
    std::vector<int64_t> outputShape;
    
    // 输入/输出 tensor 信息
    TF_Output* inputs = nullptr;
    TF_Output* outputs = nullptr;
    int numInputs = 1;
    int numOutputs = 1;
    
    // 析构时清理资源
    ~TensorFlowSession();
    
    // 禁止拷贝
    TensorFlowSession() = default;
    TensorFlowSession(const TensorFlowSession&) = delete;
    TensorFlowSession& operator=(const TensorFlowSession&) = delete;
    TensorFlowSession(TensorFlowSession&& other) noexcept;
    TensorFlowSession& operator=(TensorFlowSession&& other) noexcept;
};

/**
 * TensorFlowBackend: TensorFlow 推理后端
 * 
 * 实现 IInferenceBackend 接口，封装 TensorFlow C API。
 * 支持 TensorFlow SavedModel 格式。
 */
class TensorFlowBackend : public IInferenceBackend
{
public:
    TensorFlowBackend();
    ~TensorFlowBackend() override;
    
    // IInferenceBackend 接口实现
    std::string name() const override { return "tensorflow"; }
    bool supports(const ModelDesc& modelDesc) const override;
    bool supportsDevice(DeviceType type) const override;
    
    ModelHandle load(
        const ModelDesc& modelDesc,
        const DeviceSpec& deviceSpec,
        const BackendOptions& options = {}) override;
    
    RunResult run(
        ModelHandle handle,
        const InputMap& inputs,
        OutputMap* outputs) override;
    
    void unload(ModelHandle handle) override;
    
    std::vector<std::string> getInputNames(ModelHandle handle) const override;
    std::vector<std::string> getOutputNames(ModelHandle handle) const override;

private:
    /**
     * 创建 TensorFlow SessionOptions
     * 
     * 根据 DeviceSpec 和 BackendOptions 配置：
     * - CPU: 禁用 GPU
     * - GPU: 指定 GPU index, 配置内存增长策略
     */
    TF_SessionOptions* createSessionOptions(
        const DeviceSpec& deviceSpec,
        const BackendOptions& options);
    
    /**
     * 获取会话（内部使用）
     */
    TensorFlowSession* getSession(ModelHandle handle);
    const TensorFlowSession* getSession(ModelHandle handle) const;
    
    // 会话管理
    std::unordered_map<ModelHandle, std::unique_ptr<TensorFlowSession>> sessions_;
    ModelHandle nextHandle_ = 1;
    mutable std::mutex mutex_;
};

} // namespace XVisual

#endif // TENSORFLOWBACKEND_H
