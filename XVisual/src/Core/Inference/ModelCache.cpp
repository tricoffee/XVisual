#include "Core/Inference/ModelCache.h"
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include <sstream>

namespace XVisual {

// CacheKey 实现
bool CacheKey::operator==(const CacheKey& other) const
{
    return backendName == other.backendName &&
           modelPath == other.modelPath &&
           deviceSpec == other.deviceSpec &&
           optionsHash == other.optionsHash;
}

std::string CacheKey::toString() const
{
    std::ostringstream oss;
    oss << backendName << ":" << modelPath << ":" << deviceSpec.toKeyString();
    if (!optionsHash.empty())
    {
        oss << ":" << optionsHash;
    }
    return oss.str();
}

// ModelCache 实现
ModelCache& ModelCache::instance()
{
    static ModelCache cache;
    return cache;
}

ModelCache::~ModelCache()
{
    clear();
}

ModelHandle ModelCache::getOrCreate(
    const CacheKey& key,
    IInferenceBackend* backend,
    std::function<ModelHandle()> factory)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = cache_.find(key);
    if (it != cache_.end())
    {
        // 缓存命中
        ++hitCount_;
        it->second.refCount++;
        it->second.lastUsed = std::chrono::steady_clock::now();
        XLOG_INFO("ModelCache: HIT - using cached model (key=" + key.toString() + 
                  ", refCount=" + std::to_string(it->second.refCount) + ")", CURRENT_THREAD_ID);
        return it->second.handle;
    }
    
    // 缓存未命中，创建新模型
    ++missCount_;
    XLOG_INFO("ModelCache: MISS - loading model (key=" + key.toString() + ")", CURRENT_THREAD_ID);
    
    ModelHandle handle = INVALID_MODEL_HANDLE;
    try
    {
        handle = factory();
    }
    catch (const std::exception& e)
    {
        XLOG_INFO("ModelCache: Failed to load model: " + std::string(e.what()), CURRENT_THREAD_ID);
        return INVALID_MODEL_HANDLE;
    }
    
    if (handle == INVALID_MODEL_HANDLE)
    {
        XLOG_INFO("ModelCache: Factory returned invalid handle", CURRENT_THREAD_ID);
        return INVALID_MODEL_HANDLE;
    }
    
    // 存入缓存
    CachedModel cached;
    cached.handle = handle;
    cached.backend = backend;
    cached.refCount = 1;
    cached.lastUsed = std::chrono::steady_clock::now();
    
    cache_[key] = cached;
    XLOG_INFO("ModelCache: Model loaded and cached (key=" + key.toString() + ")", CURRENT_THREAD_ID);
    
    return handle;
}

void ModelCache::release(const CacheKey& key)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = cache_.find(key);
    if (it != cache_.end())
    {
        if (it->second.refCount > 0)
        {
            it->second.refCount--;
        }
        it->second.lastUsed = std::chrono::steady_clock::now();
        XLOG_INFO("ModelCache: Released reference (key=" + key.toString() + 
                  ", refCount=" + std::to_string(it->second.refCount) + ")", CURRENT_THREAD_ID);
    }
}

void ModelCache::cleanup(std::chrono::seconds maxIdleTime)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::steady_clock::now();
    
    for (auto it = cache_.begin(); it != cache_.end(); )
    {
        auto idleTime = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.lastUsed);
        
        if (it->second.refCount == 0 && idleTime >= maxIdleTime)
        {
            // 卸载模型
            if (it->second.backend && it->second.handle != INVALID_MODEL_HANDLE)
            {
                XLOG_INFO("ModelCache: Unloading idle model (key=" + it->first.toString() + 
                          ", idle=" + std::to_string(idleTime.count()) + "s)", CURRENT_THREAD_ID);
                try
                {
                    it->second.backend->unload(it->second.handle);
                }
                catch (const std::exception& e)
                {
                    XLOG_INFO("ModelCache: Error unloading model: " + std::string(e.what()), CURRENT_THREAD_ID);
                }
            }
            it = cache_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void ModelCache::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& kv : cache_)
    {
        if (kv.second.backend && kv.second.handle != INVALID_MODEL_HANDLE)
        {
            XLOG_INFO("ModelCache: Unloading model on clear (key=" + kv.first.toString() + ")", CURRENT_THREAD_ID);
            try
            {
                kv.second.backend->unload(kv.second.handle);
            }
            catch (const std::exception& e)
            {
                XLOG_INFO("ModelCache: Error unloading model: " + std::string(e.what()), CURRENT_THREAD_ID);
            }
        }
    }
    
    cache_.clear();
    XLOG_INFO("ModelCache: Cleared all cached models", CURRENT_THREAD_ID);
}

size_t ModelCache::size() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return cache_.size();
}

} // namespace XVisual

// std::hash 特化实现
namespace std {
    size_t hash<XVisual::CacheKey>::operator()(const XVisual::CacheKey& key) const
    {
        size_t h1 = std::hash<std::string>{}(key.backendName);
        size_t h2 = std::hash<std::string>{}(key.modelPath);
        size_t h3 = std::hash<XVisual::DeviceSpec>{}(key.deviceSpec);
        size_t h4 = std::hash<std::string>{}(key.optionsHash);
        
        // 组合哈希
        size_t result = h1;
        result ^= h2 + 0x9e3779b9 + (result << 6) + (result >> 2);
        result ^= h3 + 0x9e3779b9 + (result << 6) + (result >> 2);
        result ^= h4 + 0x9e3779b9 + (result << 6) + (result >> 2);
        return result;
    }
}
