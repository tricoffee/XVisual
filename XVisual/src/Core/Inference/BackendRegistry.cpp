#include "Core/Inference/BackendRegistry.h"
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"

namespace XVisual {

BackendRegistry& BackendRegistry::instance()
{
    static BackendRegistry registry;
    return registry;
}

void BackendRegistry::registerBackend(std::unique_ptr<IInferenceBackend> backend)
{
    if (!backend)
    {
        XLOG_INFO("BackendRegistry: Attempted to register null backend", CURRENT_THREAD_ID);
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const std::string name = backend->name();
    
    if (backends_.find(name) != backends_.end())
    {
        XLOG_INFO("BackendRegistry: Backend '" + name + "' already registered, replacing", CURRENT_THREAD_ID);
    }
    
    backends_[name] = std::move(backend);
    XLOG_INFO("BackendRegistry: Registered backend '" + name + "'", CURRENT_THREAD_ID);
}

IInferenceBackend* BackendRegistry::get(const std::string& name)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = backends_.find(name);
    if (it != backends_.end())
    {
        return it->second.get();
    }
    
    return nullptr;
}

IInferenceBackend* BackendRegistry::getForModel(const ModelDesc& modelDesc)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& kv : backends_)
    {
        if (kv.second->supports(modelDesc))
        {
            return kv.second.get();
        }
    }
    
    return nullptr;
}

std::vector<std::string> BackendRegistry::getRegisteredNames() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> names;
    names.reserve(backends_.size());
    
    for (const auto& kv : backends_)
    {
        names.push_back(kv.first);
    }
    
    return names;
}

bool BackendRegistry::hasBackend(const std::string& name) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return backends_.find(name) != backends_.end();
}

size_t BackendRegistry::size() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return backends_.size();
}

// BackendOptions::toHash 实现
std::string BackendOptions::toHash() const
{
    std::string hash;
    hash += std::to_string(enableProfiling);
    hash += std::to_string(allowGrowthMemory);
    hash += std::to_string(static_cast<int>(memoryFraction * 100));
    hash += std::to_string(intraOpParallelism);
    hash += std::to_string(interOpParallelism);
    
    for (const auto& kv : extra)
    {
        hash += kv.first + "=" + kv.second + ";";
    }
    
    return hash;
}

} // namespace XVisual
