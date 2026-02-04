#ifndef BACKENDREGISTRY_H
#define BACKENDREGISTRY_H

#include "Core/Inference/IInferenceBackend.h"
#include <memory>
#include <unordered_map>
#include <mutex>
#include <vector>

namespace XVisual {

/**
 * BackendRegistry: 后端注册表
 * 
 * 单例模式，管理所有已注册的推理后端。
 * 各后端在静态初始化阶段自动注册。
 * 
 * 使用方式:
 *   // 获取后端
 *   auto* backend = BackendRegistry::instance().get("tensorflow");
 *   if (backend) {
 *       auto handle = backend->load(modelDesc, deviceSpec);
 *       backend->run(handle, inputs, &outputs);
 *   }
 */
class BackendRegistry
{
public:
    /**
     * 获取单例实例
     */
    static BackendRegistry& instance();
    
    /**
     * 注册后端
     * 
     * @param backend 后端实例（所有权转移给 Registry）
     */
    void registerBackend(std::unique_ptr<IInferenceBackend> backend);
    
    /**
     * 获取后端 (by name)
     * 
     * @param name 后端名称
     * @return 后端指针, 如果不存在返回 nullptr
     */
    IInferenceBackend* get(const std::string& name);
    
    /**
     * 获取支持指定模型的后端
     * 
     * @param modelDesc 模型描述符
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
    
    /**
     * 获取已注册后端数量
     */
    size_t size() const;

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
 * 用法 (在 .cpp 文件中):
 *   REGISTER_BACKEND(TensorFlowBackend);
 * 
 * 这会在静态初始化阶段自动将后端注册到 BackendRegistry。
 */
#define REGISTER_BACKEND(BackendClass) \
    namespace { \
        static struct BackendClass##Registerer { \
            BackendClass##Registerer() { \
                XVisual::BackendRegistry::instance().registerBackend( \
                    std::make_unique<XVisual::BackendClass>()); \
            } \
        } g_##BackendClass##Registerer; \
    }

} // namespace XVisual

#endif // BACKENDREGISTRY_H
