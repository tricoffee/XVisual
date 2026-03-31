#ifndef XVISUAL_CORE_DEVICE_DEVICEPOLICY_H
#define XVISUAL_CORE_DEVICE_DEVICEPOLICY_H

#include "Core/Device/DeviceInstance.h"
#include "Core/Device/DeviceType.h"
#include <atomic>

namespace XVisual {

// 前向声明
class DeviceRuntime;
class INode;

/**
 * IDevicePolicy: 设备选择策略接口
 * 
 * 负责将节点的 preferredDevice() 映射到具体的设备实例。
 * 
 * PR-5.3: 把"如何选择设备"的决策从 GraphExecutor 抽出去，
 * 使策略可插拔/可替换。
 */
class IDevicePolicy
{
public:
    virtual ~IDevicePolicy() = default;
    
    /**
     * 选择设备实例
     * 
     * @param node 待执行的节点（用于获取 preferredDevice()）
     * @param runtime 设备运行时（用于查询可用设备）
     * @return 选择的设备实例
     */
    virtual DeviceInstance pick(const INode& node, const DeviceRuntime& runtime) = 0;
    
    /**
     * 获取策略名称（用于日志）
     */
    virtual std::string name() const = 0;
};

/**
 * FixedPolicy: 固定策略
 * 
 * 最简单的策略，总是选择第一个可用设备：
 * - CPU → cpu:0
 * - GPU → gpu:0
 * - NPU → npu:0
 * - Any → cpu:0（保持 PR-5.1 兼容性）
 * 
 * PR-5.3 默认策略。
 */
class FixedPolicy : public IDevicePolicy
{
public:
    DeviceInstance pick(const INode& node, const DeviceRuntime& runtime) override;
    std::string name() const override { return "FixedPolicy"; }
};

/**
 * RoundRobinPolicy: 轮询策略
 * 
 * 对于多 GPU/NPU 场景，在可用设备间轮询：
 * - CPU → cpu:0
 * - GPU → gpu:0, gpu:1, gpu:0, ... 轮询
 * - NPU → npu:0, npu:1, npu:0, ... 轮询
 * - Any → cpu:0
 * 
 * 用于多 GPU 验证。
 */
class RoundRobinPolicy : public IDevicePolicy
{
public:
    DeviceInstance pick(const INode& node, const DeviceRuntime& runtime) override;
    std::string name() const override { return "RoundRobinPolicy"; }

private:
    std::atomic<int> gpuRoundRobin_{0};
    std::atomic<int> npuRoundRobin_{0};
};

/**
 * GpuPreferPolicy: GPU 优先策略
 * 
 * 当 Any 时优先选择 GPU（如果有），否则 fallback 到 CPU。
 * 
 * - CPU → cpu:0
 * - GPU → gpu:0
 * - NPU → npu:0
 * - Any → gpu:0（如果有 GPU），否则 cpu:0
 * 
 * 注意：PR-5.3 默认不使用此策略，保持兼容性。
 */
class GpuPreferPolicy : public IDevicePolicy
{
public:
    DeviceInstance pick(const INode& node, const DeviceRuntime& runtime) override;
    std::string name() const override { return "GpuPreferPolicy"; }
};

} // namespace XVisual

#endif // XVISUAL_CORE_DEVICE_DEVICEPOLICY_H
