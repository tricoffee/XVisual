#ifndef XVISUAL_CORE_DEVICE_DEVICETYPE_H
#define XVISUAL_CORE_DEVICE_DEVICETYPE_H

#include <string>

namespace XVisual {

/**
 * DeviceType: 设备类型枚举
 * 
 * 定义计算任务可以执行的设备类型。
 * 不同设备类型有不同的并发模型：
 * - CPU: 线程池（多 worker 并行）
 * - GPU: 串行队列（模拟单 CUDA stream）
 * - NPU: 串行队列（模拟单 NPU context）
 */
enum class DeviceType {
    CPU,     // CPU 计算（多线程并行）
    GPU,     // GPU 计算（单 stream 串行）
    NPU,     // NPU 计算（单 context 串行）
    Any      // 无设备偏好，由调度器决定
};

/**
 * DeviceType 转字符串（用于日志）
 */
inline std::string deviceTypeToString(DeviceType type) {
    switch (type) {
        case DeviceType::CPU: return "CPU";
        case DeviceType::GPU: return "GPU";
        case DeviceType::NPU: return "NPU";
        case DeviceType::Any: return "Any";
        default: return "Unknown";
    }
}

/**
 * 字符串转 DeviceType
 */
inline DeviceType stringToDeviceType(const std::string& str) {
    if (str == "CPU") return DeviceType::CPU;
    if (str == "GPU") return DeviceType::GPU;
    if (str == "NPU") return DeviceType::NPU;
    if (str == "Any") return DeviceType::Any;
    return DeviceType::Any;  // 默认
}

} // namespace XVisual

#endif // XVISUAL_CORE_DEVICE_DEVICETYPE_H
