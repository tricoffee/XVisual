# Step15: PR-5.1 设备队列化（占位版本）实施方案 ✅ 已完成

## 概述

在不引入真实 GPU/NPU 计算的前提下，**跑通设备路由和队列化的架构形状**。

### 目标
1. 建立 `DeviceType` 枚举和 `IDeviceQueue` 接口
2. 实现 CPUQueue（多 worker）和 SerialQueue（单 worker）
3. 节点声明设备亲和性，调度器按亲和性路由
4. 验证路由形状和串行队列语义

### 不做的事（留给 PR-5.2）
- 真实 GPU/NPU 计算
- Value 数据位置感知
- 跨设备数据搬运（CopyToCPU/CopyToGPU）
- 设备 fallback 策略

---

## 设计决策（已确认）

| 决策项 | PR-5.1 方案 | 理由 |
|--------|-------------|------|
| GPU/NPU 占位 | 任务仍在 CPU 执行，但保证串行 | 验证路由形状 + 资源独占 |
| 默认设备 | `Any`，调度器固定映射到 CPU | 留出未来智能调度空间 |
| 队列数量 | 全局单 GPUQueue + 单 NPUQueue | 避免多 GPU 复杂度 |
| future 暴露 | 不暴露到上层，完成性以 nodeStates 为准 | 避免两套完成语义 |
| fallback | 不 fallback，GPU 节点仍走 GPUQueue | 验证路由形状正确性 |

---

## 工程化硬约束（必须遵守）

### A. 队列 shutdown / 析构顺序

**SerialQueue 必须支持：**
- `shutdown()`：停止接收新任务
- worker 线程退出条件：`shutdown_ && tasks_.empty()`
- 唤醒机制：`cv_.notify_all()` 确保 worker 不死等

**DeviceQueueManager shutdown 顺序：**
1. 停止接收（所有 queue 标记 shutdown）
2. 唤醒 worker（cv.notify_all）
3. join worker 线程
4. 清空队列（丢弃未执行任务，记录日志）

**关键语义：**
- cancel/fail-fast 后，队列仍可能执行已出队的那个 task
- 但不再出队新的 task
- GraphExecutor 析构顺序：先 cancel → wait → shutdown queues

### B. 路由必须是纯函数（稳定可测）

```cpp
// PR-5.1 的路由逻辑必须是确定的
DeviceType resolvedDevice = node->preferredDevice();
if (resolvedDevice == DeviceType::Any) {
    resolvedDevice = DeviceType::CPU;  // 固定映射
}
// TFDetect -> GPU（占位），其他 -> CPU
```

**禁止引入：**
- 随机性
- 基于负载的动态策略
- 任何运行时变化的行为

### C. 事件线程模型不变

```
[Worker Thread]           [QtEventBridge]           [UI Thread]
     │                          │                        │
     │  emitEvent(NodeFinished) │                        │
     │ ─────────────────────────>│                        │
     │                          │  Qt::QueuedConnection  │
     │                          │ ───────────────────────>│
     │                          │                        │  处理事件
```

**约束：**
- GraphExecutor 内 emitEvent 仍在 worker 线程发
- QtEventBridge 负责 marshal 到 UI 线程
- UI 线程仅消费事件，不直接访问 executor 状态

### D. 取消语义落在"派发层"而非"队列层"

| 层级 | 职责 | 取消行为 |
|------|------|----------|
| 调度器层 | DAG 管理、节点派发 | 不再 dispatch 新节点 |
| 节点执行层 | 单节点执行 | 执行前检查 stop_token |
| 队列层 | 排队、串行/并行 | **只负责排队，不理解 DAG cancel** |

**队列层禁止：**
- 检查 canceled_ 标志
- 主动跳过任务
- 理解上下游依赖

### E. 完成性收口唯一

**任务提交形式：**
```cpp
queue->submit([this, nodeId]() {
    executeNodeAndFinalize(nodeId);  // 统一收口
});
```

**禁止：**
- 队列返回 future 让上层 get()
- 在队列层维护完成计数
- 分叉的 finalize 路径

---

## 文件结构

```
XVisual/include/Core/Device/
├── DeviceType.h           # 设备类型枚举
├── IDeviceQueue.h         # 队列接口
├── CPUQueue.h             # CPU 线程池队列
├── SerialQueue.h          # 串行队列（GPU/NPU 共用）
└── DeviceQueueManager.h   # 队列管理器

XVisual/src/Core/Device/
├── CPUQueue.cpp
├── SerialQueue.cpp
└── DeviceQueueManager.cpp
```

---

## 接口设计

### 1. DeviceType.h

```cpp
#ifndef XVISUAL_CORE_DEVICE_DEVICETYPE_H
#define XVISUAL_CORE_DEVICE_DEVICETYPE_H

#include <string>

namespace XVisual {

/**
 * 设备类型枚举
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

} // namespace XVisual

#endif // XVISUAL_CORE_DEVICE_DEVICETYPE_H
```

### 2. IDeviceQueue.h

```cpp
#ifndef XVISUAL_CORE_DEVICE_IDEVICEQUEUE_H
#define XVISUAL_CORE_DEVICE_IDEVICEQUEUE_H

#include <functional>
#include <string>
#include "DeviceType.h"

namespace XVisual {

/**
 * IDeviceQueue: 设备计算队列抽象
 * 
 * 不同设备类型有不同的并发模型：
 * - CPU: 线程池（多 worker 并行）
 * - GPU: 串行队列（模拟单 CUDA stream）
 * - NPU: 串行队列（模拟单 NPU context）
 */
class IDeviceQueue {
public:
    virtual ~IDeviceQueue() = default;
    
    /**
     * 提交任务到队列
     * 
     * 注意：不返回 future，完成性由调用方通过 callback 或外部计数管理。
     * 
     * @param task 任务函数，执行完成后应调用 onComplete 回调
     */
    virtual void submit(std::function<void()> task) = 0;
    
    /**
     * 获取队列关联的设备类型
     */
    virtual DeviceType deviceType() const = 0;
    
    /**
     * 获取队列名称（用于日志）
     */
    virtual std::string name() const = 0;
    
    /**
     * 关闭队列，等待所有任务完成
     */
    virtual void shutdown() = 0;
};

} // namespace XVisual

#endif // XVISUAL_CORE_DEVICE_IDEVICEQUEUE_H
```

### 3. CPUQueue.h

```cpp
#ifndef XVISUAL_CORE_DEVICE_CPUQUEUE_H
#define XVISUAL_CORE_DEVICE_CPUQUEUE_H

#include "IDeviceQueue.h"
#include "BS_thread_pool.hpp"
#include <memory>

namespace XVisual {

/**
 * CPUQueue: 多 worker 线程池队列
 * 
 * 支持并行执行多个任务。
 */
class CPUQueue : public IDeviceQueue {
public:
    /**
     * @param numWorkers 工作线程数，默认 = hardware_concurrency
     */
    explicit CPUQueue(size_t numWorkers = 0);
    ~CPUQueue() override;
    
    void submit(std::function<void()> task) override;
    DeviceType deviceType() const override { return DeviceType::CPU; }
    std::string name() const override { return "CPUQueue"; }
    void shutdown() override;
    
private:
    std::unique_ptr<BS::thread_pool> pool_;
};

} // namespace XVisual

#endif // XVISUAL_CORE_DEVICE_CPUQUEUE_H
```

### 4. SerialQueue.h

```cpp
#ifndef XVISUAL_CORE_DEVICE_SERIALQUEUE_H
#define XVISUAL_CORE_DEVICE_SERIALQUEUE_H

#include "IDeviceQueue.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace XVisual {

/**
 * SerialQueue: 单 worker 串行队列
 * 
 * 用于 GPU/NPU 设备，保证同一时间只有一个任务在执行。
 * PR-5.1：任务实际仍在 CPU 上执行（占位）。
 */
class SerialQueue : public IDeviceQueue {
public:
    /**
     * @param type 设备类型（GPU 或 NPU）
     * @param queueName 队列名称（用于日志）
     */
    SerialQueue(DeviceType type, const std::string& queueName);
    ~SerialQueue() override;
    
    void submit(std::function<void()> task) override;
    DeviceType deviceType() const override { return type_; }
    std::string name() const override { return name_; }
    void shutdown() override;
    
private:
    void workerLoop();
    
    DeviceType type_;
    std::string name_;
    
    std::thread worker_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> shutdown_{false};
};

} // namespace XVisual

#endif // XVISUAL_CORE_DEVICE_SERIALQUEUE_H
```

### 5. DeviceQueueManager.h

```cpp
#ifndef XVISUAL_CORE_DEVICE_DEVICEQUEUEMANAGER_H
#define XVISUAL_CORE_DEVICE_DEVICEQUEUEMANAGER_H

#include "IDeviceQueue.h"
#include "CPUQueue.h"
#include "SerialQueue.h"
#include <memory>

namespace XVisual {

/**
 * DeviceQueueManager: 设备队列管理器
 * 
 * 管理所有设备队列，提供按设备类型路由的能力。
 */
class DeviceQueueManager {
public:
    DeviceQueueManager();
    ~DeviceQueueManager();
    
    /**
     * 获取指定设备类型的队列
     * 
     * @param type 设备类型
     * @return 对应的队列指针
     * 
     * 注意：PR-5.1 中 Any 固定映射到 CPU。
     */
    IDeviceQueue* getQueue(DeviceType type);
    
    /**
     * 关闭所有队列
     */
    void shutdown();
    
private:
    std::unique_ptr<CPUQueue> cpuQueue_;
    std::unique_ptr<SerialQueue> gpuQueue_;
    std::unique_ptr<SerialQueue> npuQueue_;
    bool shutdown_ = false;
};

} // namespace XVisual

#endif // XVISUAL_CORE_DEVICE_DEVICEQUEUEMANAGER_H
```

---

## 节点设备亲和性

### INode 接口扩展

```cpp
// 在 INode.h 中添加
class INode {
public:
    // ... 现有接口
    
    /**
     * 返回节点首选的设备类型
     * 
     * 默认返回 Any，由调度器决定（PR-5.1 固定映射到 CPU）。
     * 节点可 override 返回 GPU/NPU 表示设备偏好。
     */
    virtual DeviceType preferredDevice() const { return DeviceType::Any; }
};
```

### HandleNodeAdapter 透传

```cpp
// HandleNodeAdapter.cpp
DeviceType HandleNodeAdapter::preferredDevice() const {
    return handle_->preferredDevice();
}
```

### XBaseHandle 默认实现

```cpp
// XBaseHandle.h
virtual DeviceType preferredDevice() const { return DeviceType::CPU; }
```

### TFDetectHandle 覆写

```cpp
// TFDetectHandle.h
DeviceType preferredDevice() const override { return DeviceType::GPU; }
```

---

## GraphExecutor 集成

### 关键修改点

```cpp
class GraphExecutor {
    // 替换：BS::thread_pool pool_;
    DeviceQueueManager deviceManager_;
    
    void dispatchNode(const std::string& nodeId) {
        INode* node = resolver_(nodeId);
        DeviceType device = node->preferredDevice();
        
        // PR-5.1：Any 固定映射到 CPU
        if (device == DeviceType::Any) {
            device = DeviceType::CPU;
        }
        
        IDeviceQueue* queue = deviceManager_.getQueue(device);
        
        // 日志：显示路由目标
        XLOG_INFO("Dispatching node=" + nodeId + " to " + queue->name(), 
                  CURRENT_THREAD_ID);
        
        queue->submit([this, nodeId]() {
            executeNode(nodeId);  // 内部调用 onNodeFinished
        });
    }
};
```

### 销毁顺序

```cpp
GraphExecutor::~GraphExecutor() {
    // 1. 请求取消（如果正在运行）
    cancel();
    
    // 2. 等待所有任务完成
    // （通过 remaining_ == 0 或 job 线程 join）
    
    // 3. 关闭设备队列
    deviceManager_.shutdown();
}
```

---

## 线程模型图

```
                    ┌─────────────────────────────────────────────────────┐
                    │                  GraphExecutor                       │
                    │                                                      │
                    │   resolver_ ─────────────────────────────────────────┼──▶ INode::preferredDevice()
                    │       │                                              │
                    │       ▼                                              │
                    │   dispatchNode(nodeId)                               │
                    │       │                                              │
                    │       │  device = node->preferredDevice()            │
                    │       │  queue = deviceManager_.getQueue(device)     │
                    │       │  queue->submit(executeNode)                  │
                    │       │                                              │
                    └───────┼──────────────────────────────────────────────┘
                            │
            ┌───────────────┼───────────────┬───────────────┐
            │               │               │               │
            ▼               ▼               ▼               ▼
    ┌──────────────┐ ┌──────────────┐ ┌──────────────┐
    │   CPUQueue    │ │   GPUQueue   │ │   NPUQueue   │
    │  (thread pool)│ │(serial queue)│ │(serial queue)│
    │               │ │              │ │              │
    │ ┌─────┐┌─────┐│ │   ┌─────┐   │ │   ┌─────┐   │
    │ │ W1  ││ W2  ││ │   │  W  │   │ │   │  W  │   │
    │ └─────┘└─────┘│ │   └─────┘   │ │   └─────┘   │
    └──────────────┘ └──────────────┘ └──────────────┘
         ↓               ↓               ↓
    LoadImage        TFDetect          (未来)
    ImagePre         (占位)
    DrawBox
```

---

## 实施步骤

### Phase 1：基础设施（~2h） ✅ 已完成

| # | 任务 | 文件 | 状态 |
|---|------|------|------|
| 1 | 创建 DeviceType.h | `include/Core/Device/DeviceType.h` | ✅ |
| 2 | 创建 IDeviceQueue.h | `include/Core/Device/IDeviceQueue.h` | ✅ |
| 3 | 创建 CPUQueue.h/.cpp | `include/Core/Device/CPUQueue.h`, `src/Core/Device/CPUQueue.cpp` | ✅ |
| 4 | 创建 SerialQueue.h/.cpp | `include/Core/Device/SerialQueue.h`, `src/Core/Device/SerialQueue.cpp` | ✅ |
| 5 | 创建 DeviceQueueManager.h/.cpp | `include/Core/Device/DeviceQueueManager.h`, `src/Core/Device/DeviceQueueManager.cpp` | ✅ |

### Phase 2：节点亲和性（~1h） ✅ 已完成

| # | 任务 | 文件 | 状态 |
|---|------|------|------|
| 6 | INode 添加 preferredDevice() | `include/Core/Executor/INode.h` | ✅ |
| 7 | HandleNodeAdapter 透传 | 不需要（XBaseHandle 直接实现 INode） | ✅ |
| 8 | XBaseHandle 默认实现 | 继承 INode 默认实现（返回 Any） | ✅ |
| 9 | TFDetectHandle 覆写 | `include/Handle/TFDetectHandle.h` | ✅ |

### Phase 3：调度器集成（~1.5h） ✅ 已完成

| # | 任务 | 文件 | 状态 |
|---|------|------|------|
| 10 | GraphExecutor 使用 DeviceQueueManager | `src/Core/Executor/GraphExecutor.cpp` | ✅ |
| 11 | 添加路由日志 | 同上 | ✅ |
| 12 | 更新销毁顺序 | 同上 | ✅ |

### Phase 4：验证（~0.5h）

| # | 任务 | 状态 |
|---|------|------|
| 13 | Build 验证 | 待验证 |
| 14 | 运行 Step14 测试计划 | 待验证 |
| 15 | 验证路由日志 | 待验证 |
| 16 | 验证串行队列语义 | 待验证 |

---

## 验收标准

### 1. 路由日志正确

```
Dispatching node=Item_xxx(LoadImage) to CPUQueue
Dispatching node=Item_xxx(ImagePre) to CPUQueue
Dispatching node=Item_xxx(TFDetect) to GPUQueue    ← 关键
Dispatching node=Item_xxx(RevertBox) to CPUQueue
Dispatching node=Item_xxx(DrawBox) to CPUQueue
```

### 2. 串行队列语义正确

如果有两个无依赖的 GPU 节点（需要构造测试图）：

| 节点 | 开始时间 | 结束时间 | 验证 |
|------|----------|----------|------|
| GPU_A | 0ms | 100ms | |
| GPU_B | 100ms | 200ms | 不重叠 ✓ |

### 3. 无回归

- Step14 测试计划 19/19 通过
- Cancel/Fail-fast/UI 响应 均正常

---

## 补丁记录

### 2026-02-04: 修复 ItemNodeAdapter 设备透传

**问题**：PR-5.1 实现后，日志显示 TFDetect 路由到 CPU queue 而非 GPU queue。

**根因**：`mainwindow.cpp` 中的 `ItemNodeAdapter` 没有覆写 `preferredDevice()`，使用了 `INode` 的默认实现（返回 `Any`），最终被映射到 `CPU`。

**修复**：
```cpp
struct ItemNodeAdapter final : public XVisual::INode
{
    // ... 
    // PR-5.1: 透传底层 Handle 的设备偏好
    XVisual::DeviceType preferredDevice() const override
    {
        if (item && item->getXHandle())
            return item->getXHandle()->preferredDevice();
        return XVisual::DeviceType::Any;  // fallback
    }
};
```

**文件改动**：
- `XVisual/src/MainWindow/mainwindow.cpp`：添加 `preferredDevice()` 覆写和 `DeviceType.h` include

---

## 下一步（PR-5.2）

- Value 添加 `location`/`device` 属性
- `CopyToCPU()`/`CopyToGPU()` 最小实现
- 真实 GPU 计算接入（TensorRT/CUDA）
- 设备 fallback 策略
