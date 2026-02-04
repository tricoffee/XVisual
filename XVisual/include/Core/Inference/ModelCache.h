#ifndef MODELCACHE_H
#define MODELCACHE_H

#include "Core/Inference/IInferenceBackend.h"
#include "Core/Inference/ModelDesc.h"
#include "Core/Device/DeviceSpec.h"
#include <mutex>
#include <unordered_map>
#include <memory>
#include <functional>
#include <chrono>

namespace XVisual {

/**
 * CacheKey: 缓存键
 * 
 * 用于唯一标识一个已加载的模型实例：
 * backend + modelPath + deviceSpec + optionsHash
 */
struct CacheKey
{
    std::string backendName;    // 后端名称
    std::string modelPath;      // 模型路径
    DeviceSpec deviceSpec;      // 设备配置
    std::string optionsHash;    // 选项哈希
    
    // 比较运算符
    bool operator==(const CacheKey& other) const;
    
    // 转换为字符串（用于日志）
    std::string toString() const;
};

} // namespace XVisual

// std::hash 特化（用于 unordered_map）
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
    ModelHandle handle = INVALID_MODEL_HANDLE;   // 模型句柄
    IInferenceBackend* backend = nullptr;        // 所属后端
    int refCount = 0;                            // 引用计数
    std::chrono::steady_clock::time_point lastUsed;  // 最后使用时间
    
    CachedModel() : lastUsed(std::chrono::steady_clock::now()) {}
};

/**
 * ModelCache: 模型缓存管理器
 * 
 * 单例模式，负责：
 * 1. 缓存已加载的模型，避免重复加载
 * 2. 管理模型生命周期（引用计数）
 * 3. 提供线程安全的访问
 * 
 * 使用方式:
 *   CacheKey key{backendName, modelPath, deviceSpec, optionsHash};
 *   ModelHandle handle = ModelCache::instance().getOrCreate(key, [&]() {
 *       return backend->load(modelDesc, deviceSpec, options);
 *   });
 */
class ModelCache
{
public:
    /**
     * 获取单例实例
     */
    static ModelCache& instance();
    
    /**
     * 获取或创建模型
     * 
     * 如果缓存中存在，返回缓存的句柄并增加引用计数；
     * 否则调用 factory 创建新模型并缓存。
     * 
     * @param key 缓存键
     * @param backend 后端指针（用于存储和后续卸载）
     * @param factory 创建函数
     * @return 模型句柄
     */
    ModelHandle getOrCreate(
        const CacheKey& key,
        IInferenceBackend* backend,
        std::function<ModelHandle()> factory);
    
    /**
     * 释放模型引用
     * 
     * 减少引用计数，当引用计数为0时模型仍保留在缓存中（懒卸载）。
     */
    void release(const CacheKey& key);
    
    /**
     * 清理未使用的缓存
     * 
     * 卸载超过指定空闲时间且引用计数为0的模型。
     */
    void cleanup(std::chrono::seconds maxIdleTime = std::chrono::seconds(300));
    
    /**
     * 清空所有缓存
     * 
     * 卸载所有模型，通常在程序退出时调用。
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
