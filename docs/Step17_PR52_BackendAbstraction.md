# PR-5.2: Backend 抽象 + 设备发现 + TensorFlow 后端接入

## 概述

PR-5.2 的核心目标是建立一个**可扩展的推理后端插件框架**，并将现有 TensorFlow C API 推理"挂"在该框架下。这是整个异构设备支持的"骨架"，后续添加 ONNX Runtime、PyTorch (libtorch) 等后端将只需要"新增实现 + 少量配置"，而不需要修改 GraphExecutor 主干。

## 设计原则

1. **GraphExecutor 不依赖任何推理框架** - TF/ONNX/Torch 都不 include
2. **DeviceSpec 框架无关** - 只包含 type/index/props，不输出 TF 专用字符串
3. **推理后端是插件** - `IInferenceBackend` + `BackendRegistry`
4. **模型资源缓存统一** - `ModelCache(key = backend + modelPath + deviceSpec + optionsHash)`
5. **最小侵入现有代码** - 保持 PR-5.1 的 DeviceQueue 机制不变

---

## 新增模块结构

```
XVisual/include/
├── Core/
│   ├── Device/
│   │   ├── DeviceType.h          (已存在，PR-5.1)
│   │   ├── DeviceSpec.h          (新增)
│   │   └── DeviceManager.h       (新增)
│   ├── Inference/
│   │   ├── IInferenceBackend.h   (新增)
│   │   ├── BackendRegistry.h     (新增)
│   │   ├── ModelDesc.h           (新增)
│   │   ├── ModelHandle.h         (新增)
│   │   ├── ModelCache.h          (新增)
│   │   └── RunResult.h           (新增)
│   └── Runtime/
│       └── Value.h               (新增，包装 std::any)
├── Backend/
│   └── TensorFlow/
│       └── TensorFlowBackend.h   (新增)

XVisual/src/
├── Core/
│   ├── Device/
│   │   ├── DeviceSpec.cpp
│   │   └── DeviceManager.cpp
│   ├── Inference/
│   │   ├── BackendRegistry.cpp
│   │   └── ModelCache.cpp
│   └── Runtime/
│       └── Value.cpp
└── Backend/
    └── TensorFlow/
        └── TensorFlowBackend.cpp
```

---

## 详细设计

### 1. DeviceSpec (Core/Device/DeviceSpec.h)

框架无关的设备描述符。

```cpp
#ifndef DEVICESPEC_H
#define DEVICESPEC_H

#include "Core/Device/DeviceType.h"
#include <string>
#include <unordered_map>

namespace XVisual {

/**
 * DeviceSpec: 框架无关的设备描述符
 * 
 * 用于描述推理设备的基本信息，不绑定任何特定推理框架。
 * 各后端（TensorFlow、ONNX Runtime、PyTorch）根据 DeviceSpec 自行解析并配置设备。
 */
struct DeviceSpec
{
    DeviceType type = DeviceType::CPU;      // 设备类型
    int index = 0;                           // 设备索引 (GPU 0, GPU 1, ...)
    std::string name;                        // 设备名称 (可选, 如 "NVIDIA GeForce RTX 3080")
    std::unordered_map<std::string, std::string> props;  // 扩展属性 (显存、算力等)
    
    // 便捷构造
    static DeviceSpec CPU();
    static DeviceSpec GPU(int index = 0);
    static DeviceSpec NPU(int index = 0);
    
    // 比较运算符 (用于 map key)
    bool operator==(const DeviceSpec& other) const;
    bool operator!=(const DeviceSpec& other) const;
    
    // 生成唯一标识字符串 (用于缓存 key)
    std::string toKeyString() const;
};

} // namespace XVisual

// std::hash 特化 (用于 unordered_map)
namespace std {
    template<>
    struct hash<XVisual::DeviceSpec> {
        size_t operator()(const XVisual::DeviceSpec& spec) const;
    };
}

#endif // DEVICESPEC_H
```

### 2. DeviceManager (Core/Device/DeviceManager.h)

设备发现与查询管理器。

```cpp
#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "Core/Device/DeviceSpec.h"
#include <vector>
#include <mutex>

namespace XVisual {

/**
 * DeviceInfo: 设备详细信息
 */
struct DeviceInfo
{
    DeviceSpec spec;
    size_t totalMemory = 0;      // 总内存 (bytes)
    size_t freeMemory = 0;       // 可用内存 (bytes)
    int computeCapability = 0;   // 算力等级 (可选)
    bool isAvailable = true;     // 是否可用
};

/**
 * DeviceManager: 设备发现与查询
 * 
 * 单例模式，负责：
 * 1. 发现系统中可用的 CPU/GPU/NPU 设备
 * 2. 提供设备信息查询接口
 * 3. 不依赖任何推理框架
 */
class DeviceManager
{
public:
    static DeviceManager& instance();
    
    // 设备发现 (启动时调用一次)
    void discoverDevices();
    
    // 设备查询
    std::vector<DeviceInfo> getAllDevices() const;
    std::vector<DeviceInfo> getDevices(DeviceType type) const;
    bool hasDevice(DeviceType type) const;
    int getDeviceCount(DeviceType type) const;
    
    // 获取默认设备
    DeviceSpec getDefaultDevice(DeviceType type) const;
    
private:
    DeviceManager() = default;
    ~DeviceManager() = default;
    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;
    
    void discoverCPU();
    void discoverGPU();
    void discoverNPU();
    
    std::vector<DeviceInfo> devices_;
    mutable std::mutex mutex_;
    bool discovered_ = false;
};

} // namespace XVisual

#endif // DEVICEMANAGER_H
```

### 3. ModelDesc (Core/Inference/ModelDesc.h)

模型描述符。

```cpp
#ifndef MODELDESC_H
#define MODELDESC_H

#include <string>
#include <unordered_map>

namespace XVisual {

/**
 * ModelDesc: 模型描述符
 * 
 * 描述一个推理模型的元信息，与具体推理框架无关。
 */
struct ModelDesc
{
    std::string path;           // 模型路径 (文件或目录)
    std::string format;         // 模型格式:
                                //   - "tf_savedmodel" (TensorFlow SavedModel)
                                //   - "onnx"          (ONNX)
                                //   - "torchscript"   (TorchScript)
                                //   - "tflite"        (TensorFlow Lite)
    std::unordered_map<std::string, std::string> options;  // 额外选项
    
    // 便捷构造
    static ModelDesc TFSavedModel(const std::string& path);
    static ModelDesc ONNX(const std::string& path);
    static ModelDesc TorchScript(const std::string& path);
    
    // 生成唯一标识字符串 (用于缓存 key)
    std::string toKeyString() const;
};

} // namespace XVisual

#endif // MODELDESC_H
```

### 4. IInferenceBackend (Core/Inference/IInferenceBackend.h)

推理后端接口 - **核心扩展点**。

```cpp
#ifndef IINFERENCEBACKEND_H
#define IINFERENCEBACKEND_H

#include "Core/Inference/ModelDesc.h"
#include "Core/Inference/ModelHandle.h"
#include "Core/Inference/RunResult.h"
#include "Core/Device/DeviceSpec.h"
#include <string>
#include <unordered_map>
#include <any>

namespace XVisual {

// 输入/输出映射类型
using InputMap = std::unordered_map<std::string, std::any>;
using OutputMap = std::unordered_map<std::string, std::any>;

/**
 * BackendOptions: 后端配置选项
 */
struct BackendOptions
{
    bool enableProfiling = false;
    bool allowGrowthMemory = true;   // GPU 内存按需增长
    float memoryFraction = 0.0f;     // GPU 内存占用比例 (0 = 不限制)
    int intraOpParallelism = 0;      // 单 op 并行度 (0 = 默认)
    int interOpParallelism = 0;      // op 间并行度 (0 = 默认)
    std::unordered_map<std::string, std::string> extra;
};

/**
 * IInferenceBackend: 推理后端接口
 * 
 * 所有推理后端（TensorFlow、ONNX Runtime、PyTorch）都实现此接口。
 * GraphExecutor 不直接依赖任何具体后端，而是通过此接口进行交互。
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
     * 检查是否支持指定设备
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
     * 卸载模型 (可选, ModelCache 会自动管理生命周期)
     */
    virtual void unload(ModelHandle handle) = 0;
    
    /**
     * 获取模型输入/输出签名 (可选)
     */
    virtual std::vector<std::string> getInputNames(ModelHandle handle) const { return {}; }
    virtual std::vector<std::string> getOutputNames(ModelHandle handle) const { return {}; }
};

} // namespace XVisual

#endif // IINFERENCEBACKEND_H
```

### 5. BackendRegistry (Core/Inference/BackendRegistry.h)

后端注册表。

```cpp
#ifndef BACKENDREGISTRY_H
#define BACKENDREGISTRY_H

#include "Core/Inference/IInferenceBackend.h"
#include <memory>
#include <unordered_map>
#include <mutex>

namespace XVisual {

/**
 * BackendRegistry: 后端注册表
 * 
 * 单例模式，管理所有已注册的推理后端。
 * 各后端在静态初始化阶段自动注册。
 */
class BackendRegistry
{
public:
    static BackendRegistry& instance();
    
    /**
     * 注册后端
     */
    void registerBackend(std::unique_ptr<IInferenceBackend> backend);
    
    /**
     * 获取后端 (by name)
     * @return 后端指针, 如果不存在返回 nullptr
     */
    IInferenceBackend* get(const std::string& name);
    
    /**
     * 获取支持指定模型的后端
     * @return 后端指针, 如果不存在返回 nullptr
     */
    IInferenceBackend* getForModel(const ModelDesc& modelDesc);
    
    /**
     * 获取所有已注册后端名称
     */
    std::vector<std::string> getRegisteredNames() const;
    
    /**
     * 检查后端是否已注册
     */
    bool hasBackend(const std::string& name) const;

private:
    BackendRegistry() = default;
    ~BackendRegistry() = default;
    BackendRegistry(const BackendRegistry&) = delete;
    BackendRegistry& operator=(const BackendRegistry&) = delete;
    
    std::unordered_map<std::string, std::unique_ptr<IInferenceBackend>> backends_;
    mutable std::mutex mutex_;
};

/**
 * 后端注册辅助宏
 * 
 * 用法:
 *   REGISTER_BACKEND(TensorFlowBackend);
 */
#define REGISTER_BACKEND(BackendClass) \
    static struct BackendClass##Registerer { \
        BackendClass##Registerer() { \
            XVisual::BackendRegistry::instance().registerBackend( \
                std::make_unique<BackendClass>()); \
        } \
    } g_##BackendClass##Registerer

} // namespace XVisual

#endif // BACKENDREGISTRY_H
```

### 6. ModelCache (Core/Inference/ModelCache.h)

模型缓存管理。

```cpp
#ifndef MODELCACHE_H
#define MODELCACHE_H

#include "Core/Inference/IInferenceBackend.h"
#include "Core/Inference/ModelDesc.h"
#include "Core/Device/DeviceSpec.h"
#include <mutex>
#include <unordered_map>
#include <memory>
#include <functional>

namespace XVisual {

/**
 * CacheKey: 缓存键
 */
struct CacheKey
{
    std::string backendName;
    std::string modelPath;
    DeviceSpec deviceSpec;
    std::string optionsHash;
    
    bool operator==(const CacheKey& other) const;
    std::string toString() const;
};

} // namespace XVisual

// std::hash 特化
namespace std {
    template<>
    struct hash<XVisual::CacheKey> {
        size_t operator()(const XVisual::CacheKey& key) const;
    };
}

namespace XVisual {

/**
 * CachedModel: 缓存的模型条目
 */
struct CachedModel
{
    ModelHandle handle;
    IInferenceBackend* backend = nullptr;
    int refCount = 0;
    std::chrono::steady_clock::time_point lastUsed;
};

/**
 * ModelCache: 模型缓存管理器
 * 
 * 单例模式，负责：
 * 1. 缓存已加载的模型，避免重复加载
 * 2. 管理模型生命周期（引用计数）
 * 3. 提供线程安全的访问
 */
class ModelCache
{
public:
    static ModelCache& instance();
    
    /**
     * 获取或创建模型
     * 
     * 如果缓存中存在，返回缓存的句柄；否则调用 factory 创建。
     */
    ModelHandle getOrCreate(
        const CacheKey& key,
        std::function<ModelHandle()> factory);
    
    /**
     * 释放模型引用
     */
    void release(const CacheKey& key);
    
    /**
     * 清理未使用的缓存
     */
    void cleanup(std::chrono::seconds maxIdleTime = std::chrono::seconds(300));
    
    /**
     * 清空所有缓存
     */
    void clear();
    
    /**
     * 获取缓存统计
     */
    size_t size() const;
    size_t hitCount() const { return hitCount_; }
    size_t missCount() const { return missCount_; }

private:
    ModelCache() = default;
    ~ModelCache();
    ModelCache(const ModelCache&) = delete;
    ModelCache& operator=(const ModelCache&) = delete;
    
    std::unordered_map<CacheKey, CachedModel> cache_;
    mutable std::mutex mutex_;
    size_t hitCount_ = 0;
    size_t missCount_ = 0;
};

} // namespace XVisual

#endif // MODELCACHE_H
```

### 7. TensorFlowBackend (Backend/TensorFlow/TensorFlowBackend.h)

TensorFlow 后端实现。

```cpp
#ifndef TENSORFLOWBACKEND_H
#define TENSORFLOWBACKEND_H

#include "Core/Inference/IInferenceBackend.h"
#include <unordered_map>
#include <memory>

// Forward declarations (避免在头文件中 include TF)
struct TF_Graph;
struct TF_Session;
struct TF_Status;
struct TF_SessionOptions;

namespace XVisual {

/**
 * TensorFlowSession: TensorFlow 会话封装
 */
struct TensorFlowSession
{
    TF_Graph* graph = nullptr;
    TF_Session* session = nullptr;
    TF_Status* status = nullptr;
    DeviceSpec deviceSpec;
    std::string modelPath;
    
    // 输入/输出节点信息
    std::string inputName;
    std::string outputName;
    std::vector<int64_t> inputShape;
    std::vector<int64_t> outputShape;
};

/**
 * TensorFlowBackend: TensorFlow 推理后端
 * 
 * 实现 IInferenceBackend 接口，封装 TensorFlow C API。
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
     * - CPU: 禁用 GPU (CUDA_VISIBLE_DEVICES="" 或 visible_device_list="")
     * - GPU: 指定 GPU index, 配置内存增长策略
     */
    TF_SessionOptions* createSessionOptions(
        const DeviceSpec& deviceSpec,
        const BackendOptions& options);
    
    /**
     * 生成 TensorFlow ConfigProto 二进制数据
     */
    std::vector<uint8_t> createConfigProto(
        const DeviceSpec& deviceSpec,
        const BackendOptions& options);
    
    // 会话管理
    std::unordered_map<ModelHandle, std::unique_ptr<TensorFlowSession>> sessions_;
    ModelHandle nextHandle_ = 1;
    std::mutex mutex_;
};

} // namespace XVisual

#endif // TENSORFLOWBACKEND_H
```

---

## TensorFlowBackend::load() 设备配置详解

这是 PR-5.2 的核心实现点。根据 `DeviceSpec` 生成正确的 TensorFlow 配置。

### TensorFlow 设备控制机制

TensorFlow C API 通过 `ConfigProto` (protobuf) 控制设备放置：

```protobuf
// tensorflow/core/protobuf/config.proto
message ConfigProto {
  DeviceCountEntry device_count = 1;     // 设备数量限制
  bool allow_soft_placement = 7;          // 允许自动设备放置
  GPUOptions gpu_options = 6;             // GPU 配置
}

message GPUOptions {
  double per_process_gpu_memory_fraction = 1;  // GPU 内存占用比例
  bool allow_growth = 4;                        // 内存按需增长
  string visible_device_list = 5;               // 可见 GPU 列表 ("0,1" 或 "0")
}
```

### 实现策略

```cpp
std::vector<uint8_t> TensorFlowBackend::createConfigProto(
    const DeviceSpec& deviceSpec,
    const BackendOptions& options)
{
    tensorflow::ConfigProto config;
    
    // 通用设置
    config.set_allow_soft_placement(true);
    
    // 设备特定配置
    switch (deviceSpec.type)
    {
    case DeviceType::CPU:
        // 强制 CPU: 禁用所有 GPU
        (*config.mutable_device_count())["GPU"] = 0;
        // 或者: config.mutable_gpu_options()->set_visible_device_list("");
        break;
        
    case DeviceType::GPU:
        // GPU 配置
        {
            auto* gpuOpts = config.mutable_gpu_options();
            
            // 指定可见 GPU (按 index)
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
        }
        break;
        
    case DeviceType::NPU:
        // NPU: TensorFlow 原生不支持，可能需要第三方插件
        // 先 fallback 到 CPU
        (*config.mutable_device_count())["GPU"] = 0;
        break;
        
    default:
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
    
    // 序列化为二进制
    std::string serialized;
    config.SerializeToString(&serialized);
    return std::vector<uint8_t>(serialized.begin(), serialized.end());
}

TF_SessionOptions* TensorFlowBackend::createSessionOptions(
    const DeviceSpec& deviceSpec,
    const BackendOptions& options)
{
    TF_SessionOptions* opts = TF_NewSessionOptions();
    
    // 生成 ConfigProto
    std::vector<uint8_t> configData = createConfigProto(deviceSpec, options);
    
    // 设置配置
    TF_Status* status = TF_NewStatus();
    TF_SetConfig(opts, configData.data(), configData.size(), status);
    
    if (TF_GetCode(status) != TF_OK)
    {
        XLOG_INFO("TensorFlowBackend: Failed to set config: " + 
                  std::string(TF_Message(status)), CURRENT_THREAD_ID);
    }
    
    TF_DeleteStatus(status);
    return opts;
}
```

### 环境变量备选方案

如果 `ConfigProto` 方式不可用，可以使用环境变量：

```cpp
void TensorFlowBackend::setDeviceEnvironment(const DeviceSpec& deviceSpec)
{
    switch (deviceSpec.type)
    {
    case DeviceType::CPU:
        // 禁用所有 GPU
        #ifdef _WIN32
        _putenv_s("CUDA_VISIBLE_DEVICES", "");
        #else
        setenv("CUDA_VISIBLE_DEVICES", "", 1);
        #endif
        break;
        
    case DeviceType::GPU:
        // 只暴露指定 GPU
        std::string gpuList = std::to_string(deviceSpec.index);
        #ifdef _WIN32
        _putenv_s("CUDA_VISIBLE_DEVICES", gpuList.c_str());
        #else
        setenv("CUDA_VISIBLE_DEVICES", gpuList.c_str(), 1);
        #endif
        break;
    }
}
```

> **注意**: 环境变量方式是进程级别的，在多 GPU 并发场景下不推荐。优先使用 `ConfigProto`。

---

## Handle 与 Backend 的连接方式

### 方案：配置驱动（推荐）

TFDetectHandle 通过 VarBag/Param 读取 `backend` 字段，而不是修改 INode 接口：

```cpp
// TFDetectHandle.cpp
void TFDetectHandle::initParams()
{
    // ... 现有代码 ...
    
    // 新增: backend 配置 (默认 "tensorflow")
    std::string backend = "tensorflow";
    REGISTER_MEMBER_ATTR_STR((*sources), "backend", backend, true);
}

void TFDetectHandle::xOperate()
{
    // 获取 backend 配置
    std::string backendName = GET_MEMBER_WITH_TYPE_STR((*sources), std::string, "backend");
    
    // 获取后端实例
    IInferenceBackend* backend = BackendRegistry::instance().get(backendName);
    if (!backend)
    {
        XLOG_INFO("TFDetectHandle: Backend not found: " + backendName, CURRENT_THREAD_ID);
        return;
    }
    
    // 获取设备配置
    DeviceSpec deviceSpec = DeviceSpec::GPU();  // 或从配置读取
    
    // 构建 ModelDesc
    std::string savedModelPath = GET_MEMBER_WITH_TYPE_STR((*sources), std::string, "savedModelPath");
    ModelDesc modelDesc = ModelDesc::TFSavedModel(savedModelPath);
    
    // 缓存 key
    CacheKey cacheKey{backendName, savedModelPath, deviceSpec, ""};
    
    // 获取或加载模型
    ModelHandle handle = ModelCache::instance().getOrCreate(cacheKey, [&]() {
        return backend->load(modelDesc, deviceSpec, {});
    });
    
    // 准备输入
    cv::Mat image = GET_MEMBER_WITH_TYPE_STR((*sources), cv::Mat, "input_image");
    InputMap inputs;
    inputs["image_input"] = image;  // 需要转换为 tensor
    
    // 执行推理
    OutputMap outputs;
    RunResult result = backend->run(handle, inputs, outputs);
    
    if (result.success)
    {
        // 处理输出
        auto& detections = std::any_cast<std::vector<DetectResult>&>(outputs["detections"]);
        REGISTER_MEMBER_STR((*dests), "detectResults", detections);
    }
}
```

### 过渡策略

为了最小侵入，可以分两阶段：

**阶段 1 (PR-5.2)**:
- TFDetectHandle 内部直接使用 `TensorFlowBackend`
- 不修改现有调用流程
- 只验证 Backend 框架和 ModelCache 工作正常

**阶段 2 (PR-5.3+)**:
- TFDetectHandle 改用 `BackendRegistry` 动态获取后端
- 支持通过配置切换 backend

---

## 实施步骤

### Step 1: 创建基础类型文件

1. `Core/Device/DeviceSpec.h/.cpp`
2. `Core/Device/DeviceManager.h/.cpp`
3. `Core/Inference/ModelDesc.h/.cpp`
4. `Core/Inference/ModelHandle.h`
5. `Core/Inference/RunResult.h`
6. `Core/Runtime/Value.h/.cpp`

### Step 2: 创建后端框架

1. `Core/Inference/IInferenceBackend.h`
2. `Core/Inference/BackendRegistry.h/.cpp`
3. `Core/Inference/ModelCache.h/.cpp`

### Step 3: 实现 TensorFlowBackend

1. `Backend/TensorFlow/TensorFlowBackend.h/.cpp`
2. 迁移现有 `TensorFlowModel` 代码
3. 实现 `createConfigProto()` / `createSessionOptions()`
4. 注册到 `BackendRegistry`

### Step 4: 修改 TFDetectHandle（最小侵入）

1. 内部使用 `TensorFlowBackend` 而非直接使用 `TensorFlowModel`
2. 使用 `ModelCache` 缓存模型
3. 保持外部接口不变

### Step 5: 测试验证

1. TFDetect 能正常工作
2. 日志中打印 backend=tensorflow, device=GPU/CPU
3. ModelCache 缓存生效（重复 run 不重复 load）
4. 不破坏 PR-4.5 心跳/UI 状态

---

## 验收标准

### 功能验收

| 编号 | 验收项 | 验收方式 |
|-----|-------|---------|
| F1 | TFDetect 在 GPU 上运行 | 日志显示 `device=GPU[0]` |
| F2 | TFDetect 在 CPU 上运行 | 配置 `device=CPU`，日志显示 `device=CPU` |
| F3 | 模型缓存生效 | 第二次 run 不打印 load 日志 |
| F4 | BackendRegistry 工作 | `BackendRegistry::instance().get("tensorflow")` 返回非空 |
| F5 | PR-4.5 不受影响 | 心跳/节点状态 UI 正常 |

### 代码验收

| 编号 | 验收项 | 验收方式 |
|-----|-------|---------|
| C1 | GraphExecutor 不 include TF | 检查 `GraphExecutor.h/cpp` 无 TF 头文件 |
| C2 | DeviceSpec 框架无关 | `DeviceSpec.h` 不依赖任何推理框架 |
| C3 | IInferenceBackend 接口完整 | 包含 load/run/unload/supports |
| C4 | REGISTER_BACKEND 宏工作 | TensorFlowBackend 自动注册 |

### 日志验收示例

```
[INFO] BackendRegistry: Registered backend "tensorflow"
[INFO] DeviceManager: Discovered 1 CPU, 1 GPU
[INFO] TFDetectHandle: Using backend=tensorflow, device=GPU[0]
[INFO] TensorFlowBackend: Loading model: /path/to/saved_model
[INFO] TensorFlowBackend: ConfigProto: visible_device_list="0", allow_growth=true
[INFO] ModelCache: MISS - loading model (key=tensorflow:/path:GPU[0])
[INFO] TensorFlowBackend: Model loaded successfully
[INFO] TFDetectHandle: Run completed, results=5 detections
...
[INFO] ModelCache: HIT - using cached model (key=tensorflow:/path:GPU[0])
```

---

## 关键实现细节

### ModelHandle 定义

```cpp
// Core/Inference/ModelHandle.h
#ifndef MODELHANDLE_H
#define MODELHANDLE_H

#include <cstdint>

namespace XVisual {

/**
 * ModelHandle: 模型句柄类型
 * 
 * 不透明句柄，由各后端内部管理实际含义。
 * 使用整数类型而非指针，避免跨后端的类型混淆。
 */
using ModelHandle = uint64_t;

constexpr ModelHandle INVALID_MODEL_HANDLE = 0;

} // namespace XVisual

#endif // MODELHANDLE_H
```

### RunResult 定义

```cpp
// Core/Inference/RunResult.h
#ifndef RUNRESULT_H
#define RUNRESULT_H

#include <string>
#include <chrono>

namespace XVisual {

/**
 * RunResult: 推理执行结果
 */
struct RunResult
{
    bool success = false;
    std::string errorMessage;
    std::chrono::microseconds duration{0};
    
    static RunResult Success(std::chrono::microseconds dur = {}) {
        return {true, "", dur};
    }
    
    static RunResult Failure(const std::string& msg) {
        return {false, msg, {}};
    }
};

} // namespace XVisual

#endif // RUNRESULT_H
```

### Value 包装（为 PR-5.5 铺路）

```cpp
// Core/Runtime/Value.h
#ifndef VALUE_H
#define VALUE_H

#include <any>
#include <string>

namespace XVisual {

/**
 * Value: 运行时值包装
 * 
 * PR-5.2: 简单包装 std::any
 * PR-5.5: 将扩展为 TensorValue (含设备位置、dtype、shape)
 */
struct Value
{
    std::any payload;
    
    // 便捷构造
    template<typename T>
    static Value From(T&& val) {
        return Value{std::forward<T>(val)};
    }
    
    // 类型安全获取
    template<typename T>
    T& as() {
        return std::any_cast<T&>(payload);
    }
    
    template<typename T>
    const T& as() const {
        return std::any_cast<const T&>(payload);
    }
    
    bool hasValue() const { return payload.has_value(); }
};

} // namespace XVisual

#endif // VALUE_H
```

---

## 文件清单（vcxproj 需添加）

### 新增头文件

```
XVisual/include/Core/Device/DeviceSpec.h
XVisual/include/Core/Device/DeviceManager.h
XVisual/include/Core/Inference/IInferenceBackend.h
XVisual/include/Core/Inference/BackendRegistry.h
XVisual/include/Core/Inference/ModelDesc.h
XVisual/include/Core/Inference/ModelHandle.h
XVisual/include/Core/Inference/ModelCache.h
XVisual/include/Core/Inference/RunResult.h
XVisual/include/Core/Runtime/Value.h
XVisual/include/Backend/TensorFlow/TensorFlowBackend.h
```

### 新增源文件

```
XVisual/src/Core/Device/DeviceSpec.cpp
XVisual/src/Core/Device/DeviceManager.cpp
XVisual/src/Core/Inference/BackendRegistry.cpp
XVisual/src/Core/Inference/ModelCache.cpp
XVisual/src/Core/Runtime/Value.cpp
XVisual/src/Backend/TensorFlow/TensorFlowBackend.cpp
```

---

## 依赖关系

```
GraphExecutor (PR-2/4)
    │
    ├── INode::preferredDevice() (PR-5.1)
    │
    └── DeviceQueue (PR-5.1, 占位)

TFDetectHandle
    │
    ├── BackendRegistry ──► IInferenceBackend
    │                            │
    │                            └── TensorFlowBackend
    │
    ├── ModelCache
    │
    └── DeviceSpec
```

**关键点**：`GraphExecutor` 仍然不直接依赖任何后端，设备调度通过 `DeviceQueue` 完成，后端选择在 Handle 层完成。

---

## 后续 PR 预览

| PR | 目标 | 关键变化 |
|---|------|---------|
| PR-5.3 | 多队列路由 | DeviceRuntime, DevicePolicy |
| PR-5.4 | ONNX Runtime 后端 | OnnxBackend : IInferenceBackend |
| PR-5.5 | TensorValue + Copy 节点 | Value 扩展, 数据搬运 |
| PR-5.6 | TorchScript 后端 | TorchBackend : IInferenceBackend |

---

## 风险与缓解

| 风险 | 缓解措施 |
|-----|---------|
| TF ConfigProto 序列化复杂 | 可先用环境变量方案，后续优化 |
| 模型加载性能 | ModelCache 缓存 + 懒加载 |
| 内存泄漏 | RAII 封装 TF 资源 |
| 多线程竞争 | mutex 保护 BackendRegistry/ModelCache |

---

## 实施时间估计

| 步骤 | 预计时间 |
|-----|---------|
| Step 1: 基础类型 | 1-2 小时 |
| Step 2: 后端框架 | 2-3 小时 |
| Step 3: TensorFlowBackend | 3-4 小时 |
| Step 4: TFDetectHandle 适配 | 1-2 小时 |
| Step 5: 测试验证 | 1-2 小时 |
| **总计** | **8-13 小时** |

---

**文档版本**: v1.0  
**创建日期**: 2026-02-04  
**状态**: 等待实施指令
