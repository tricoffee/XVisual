#ifndef DEVICESPEC_H
#define DEVICESPEC_H

#include "Core/Device/DeviceType.h"
#include <string>
#include <unordered_map>
#include <functional>

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
    
    // 默认构造
    DeviceSpec() = default;
    
    // 带参构造
    DeviceSpec(DeviceType t, int idx = 0, const std::string& n = "")
        : type(t), index(idx), name(n) {}
    
    // 便捷构造
    static DeviceSpec CPU();
    static DeviceSpec GPU(int index = 0);
    static DeviceSpec NPU(int index = 0);
    static DeviceSpec Any();
    
    // 从 DeviceType 构造
    static DeviceSpec FromType(DeviceType type, int index = 0);
    
    // 比较运算符 (用于 map key)
    bool operator==(const DeviceSpec& other) const;
    bool operator!=(const DeviceSpec& other) const;
    
    // 生成唯一标识字符串 (用于缓存 key)
    std::string toKeyString() const;
    
    // 转换为可读字符串
    std::string toString() const;
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
