#ifndef XVISUAL_CORE_DEVICE_DEVICEINSTANCE_H
#define XVISUAL_CORE_DEVICE_DEVICEINSTANCE_H

#include "Core/Device/DeviceType.h"
#include <string>
#include <functional>

namespace XVisual {

/**
 * DeviceInstance: 具体设备实例
 * 
 * 与 DeviceType 的区别：
 * - DeviceType: 设备类型（CPU/GPU/NPU/Any）
 * - DeviceInstance: 具体实例（cpu:0, gpu:0, gpu:1, npu:0）
 * 
 * PR-5.3: 多队列路由的基础标识
 */
struct DeviceInstance
{
    DeviceType type = DeviceType::CPU;
    int index = 0;
    
    /**
     * 默认构造函数
     */
    DeviceInstance() = default;
    
    /**
     * 构造函数
     * 
     * @param t 设备类型
     * @param i 设备索引（默认 0）
     */
    DeviceInstance(DeviceType t, int i = 0) : type(t), index(i) {}
    
    /**
     * 获取名称（用于日志和显示）
     * 
     * @return 如 "cpu:0", "gpu:0", "gpu:1", "npu:0"
     */
    std::string name() const;
    
    /**
     * 比较运算符
     */
    bool operator==(const DeviceInstance& other) const
    {
        return type == other.type && index == other.index;
    }
    
    bool operator!=(const DeviceInstance& other) const
    {
        return !(*this == other);
    }
    
    /**
     * 小于运算符（用于 map/set 排序）
     */
    bool operator<(const DeviceInstance& other) const
    {
        if (type != other.type)
            return static_cast<int>(type) < static_cast<int>(other.type);
        return index < other.index;
    }
    
    /**
     * 静态便捷方法：创建 CPU 实例
     */
    static DeviceInstance CPU(int index = 0)
    {
        return DeviceInstance(DeviceType::CPU, index);
    }
    
    /**
     * 静态便捷方法：创建 GPU 实例
     */
    static DeviceInstance GPU(int index = 0)
    {
        return DeviceInstance(DeviceType::GPU, index);
    }
    
    /**
     * 静态便捷方法：创建 NPU 实例
     */
    static DeviceInstance NPU(int index = 0)
    {
        return DeviceInstance(DeviceType::NPU, index);
    }
};

} // namespace XVisual

// 为 unordered_map/unordered_set 提供 hash 特化
namespace std {

template<>
struct hash<XVisual::DeviceInstance>
{
    size_t operator()(const XVisual::DeviceInstance& d) const noexcept
    {
        // 组合 type 和 index 生成 hash
        // type * 1000 + index 保证在合理范围内唯一
        return hash<int>()(static_cast<int>(d.type) * 1000 + d.index);
    }
};

} // namespace std

#endif // XVISUAL_CORE_DEVICE_DEVICEINSTANCE_H
