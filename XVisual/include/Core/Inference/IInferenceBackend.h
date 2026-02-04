#ifndef IINFERENCEBACKEND_H
#define IINFERENCEBACKEND_H

#include "Core/Inference/ModelDesc.h"
#include "Core/Inference/ModelHandle.h"
#include "Core/Inference/RunResult.h"
#include "Core/Device/DeviceSpec.h"
#include "Core/Device/DeviceType.h"
#include <string>
#include <unordered_map>
#include <any>
#include <vector>

namespace XVisual {

// 输入/输出映射类型
using InputMap = std::unordered_map<std::string, std::any>;
using OutputMap = std::unordered_map<std::string, std::any>;

/**
 * BackendOptions: 后端配置选项
 */
struct BackendOptions
{
    bool enableProfiling = false;        // 启用性能分析
    bool allowGrowthMemory = true;       // GPU 内存按需增长
    float memoryFraction = 0.0f;         // GPU 内存占用比例 (0 = 不限制)
    int intraOpParallelism = 0;          // 单 op 并行度 (0 = 默认)
    int interOpParallelism = 0;          // op 间并行度 (0 = 默认)
    std::unordered_map<std::string, std::string> extra;  // 扩展选项
    
    // 生成选项哈希（用于缓存 key）
    std::string toHash() const;
};

/**
 * IInferenceBackend: 推理后端接口
 * 
 * 所有推理后端（TensorFlow、ONNX Runtime、PyTorch）都实现此接口。
 * GraphExecutor 不直接依赖任何具体后端，而是通过此接口进行交互。
 * 
 * 扩展点：后续添加新后端只需实现此接口并注册到 BackendRegistry。
 */
class IInferenceBackend
{
public:
    virtual ~IInferenceBackend() = default;
    
    /**
     * 后端名称 (唯一标识)
     * 例如: "tensorflow", "onnxruntime", "torchscript"
     */
    virtual std::string name() const = 0;
    
    /**
     * 检查是否支持指定模型格式
     */
    virtual bool supports(const ModelDesc& modelDesc) const = 0;
    
    /**
     * 检查是否支持指定设备类型
     */
    virtual bool supportsDevice(DeviceType type) const = 0;
    
    /**
     * 加载模型
     * 
     * @param modelDesc 模型描述符
     * @param deviceSpec 目标设备
     * @param options 后端选项
     * @return 模型句柄 (用于后续 run/unload)
     * @throws std::runtime_error 加载失败
     */
    virtual ModelHandle load(
        const ModelDesc& modelDesc,
        const DeviceSpec& deviceSpec,
        const BackendOptions& options = {}) = 0;
    
    /**
     * 执行推理
     * 
     * @param handle 模型句柄 (来自 load)
     * @param inputs 输入映射 (name -> tensor/value)
     * @param outputs 输出映射 (out param)
     * @return 执行结果
     */
    virtual RunResult run(
        ModelHandle handle,
        const InputMap& inputs,
        OutputMap* outputs) = 0;
    
    /**
     * 卸载模型
     * 
     * @param handle 要卸载的模型句柄
     */
    virtual void unload(ModelHandle handle) = 0;
    
    /**
     * 获取模型输入名称列表 (可选)
     */
    virtual std::vector<std::string> getInputNames(ModelHandle handle) const
    {
        (void)handle;
        return {};
    }
    
    /**
     * 获取模型输出名称列表 (可选)
     */
    virtual std::vector<std::string> getOutputNames(ModelHandle handle) const
    {
        (void)handle;
        return {};
    }
    
    /**
     * 检查句柄是否有效 (可选)
     */
    virtual bool isValidHandle(ModelHandle handle) const
    {
        return handle != INVALID_MODEL_HANDLE;
    }
};

} // namespace XVisual

#endif // IINFERENCEBACKEND_H
