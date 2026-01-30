# PR-4: 异步执行与并行调度基础

## 1. 概述

PR-4 是在 PR-3 完成"运行期隔离（VarBag per-node）+ 执行期去全局依赖"的基础上，将 GraphExecutor 从串行执行改为并行执行，实现 DAG 中独立节点的并发运行。

### 1.1 目标

- DAG 中 `inDegree==0` 的节点可并行提交到 CPU 线程池
- 节点状态可视化（Pending/Running/Completed/Failed）
- 保持 stop_token 的"节点边界取消"语义
- 事件系统增强，UI 可实时显示节点执行状态

### 1.2 里程碑拆分

| 里程碑 | 目标 | 依赖 |
|--------|------|------|
| PR-4.1 | 并行 Kahn 调度器骨架 | PR-3 |
| PR-4.2 | 节点状态机 | PR-4.1 |
| PR-4.3 | 事件增强 | PR-4.2 |
| PR-4.4 | 并发安全检查清单 | PR-4.1~4.3 |

---

## 2. 类图

```
┌─────────────────────────────────────────────────────────────────────┐
│                         GraphExecutor                                │
├─────────────────────────────────────────────────────────────────────┤
│ - threadPool_: std::unique_ptr<ThreadPool>                          │
│ - readyQueue_: ThreadSafeQueue<std::string>                         │
│ - inDegree_: std::unordered_map<std::string, std::atomic<int>>      │
│ - downstream_: std::unordered_map<std::string, std::vector<string>> │
│ - nodeStates_: std::unordered_map<std::string, NodeStateInfo>       │
│ - remaining_: std::atomic<int>                                      │
│ - mutex_: std::mutex                                                │
├─────────────────────────────────────────────────────────────────────┤
│ + start(...) -> bool                                                │
│ + cancel() -> void                                                  │
│ + wait() -> void                                                    │
│ + isRunning() -> bool                                               │
│ - buildDAG() -> void                                                │
│ - executeNode(nodeId) -> void                                       │
│ - onNodeComplete(nodeId, status) -> void                            │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              │ uses
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                          ThreadPool                                  │
├─────────────────────────────────────────────────────────────────────┤
│ - workers_: std::vector<std::jthread>                               │
│ - tasks_: ThreadSafeQueue<std::function<void()>>                    │
│ - stop_: std::atomic<bool>                                          │
├─────────────────────────────────────────────────────────────────────┤
│ + ThreadPool(size_t numThreads)                                     │
│ + ~ThreadPool()                                                     │
│ + submit(task: std::function<void()>) -> void                       │
│ + shutdown() -> void                                                │
└─────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────┐
│                     ThreadSafeQueue<T>                               │
├─────────────────────────────────────────────────────────────────────┤
│ - queue_: std::queue<T>                                             │
│ - mutex_: std::mutex                                                │
│ - cv_: std::condition_variable                                      │
│ - stopped_: std::atomic<bool>                                       │
├─────────────────────────────────────────────────────────────────────┤
│ + push(item: T) -> void                                             │
│ + pop() -> std::optional<T>                                         │
│ + tryPop() -> std::optional<T>                                      │
│ + stop() -> void                                                    │
│ + size() -> size_t                                                  │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 3. 数据结构

### 3.1 NodeState 枚举

```cpp
namespace XVisual {

enum class NodeState : uint8_t {
    Pending,    // 尚未就绪（有未完成的上游依赖）
    Ready,      // 就绪，等待线程池调度
    Running,    // 正在执行
    Completed,  // 执行成功
    Failed,     // 执行失败
    Canceled,   // 被取消（用户主动取消）
    Skipped     // 被跳过（上游失败导致）
};

} // namespace XVisual
```

### 3.2 NodeStateInfo 结构

```cpp
namespace XVisual {

struct NodeStateInfo {
    NodeState state = NodeState::Pending;
    int64_t startTime = 0;      // 微秒时间戳
    int64_t endTime = 0;        // 微秒时间戳
    int64_t duration = 0;       // 执行耗时（微秒）
    std::string errorMessage;   // 失败时的错误信息
};

} // namespace XVisual
```

### 3.3 增强后的 NodeEvent

```cpp
namespace XVisual {

enum class EventType : uint8_t {
    JobStarted,
    JobFinished,
    NodeStarted,    // 新增
    NodeFinished,   // 新增
    NodeSkipped,    // 新增
    ProgressUpdate
};

struct NodeEvent {
    EventType type;
    std::string nodeId;         // 节点 ID
    NodeState state;            // 节点状态
    int code;                   // ErrorCode 或进度值
    int64_t timestamp;          // 微秒时间戳
    int64_t duration;           // 执行耗时（仅 NodeFinished 有效）
    std::string message;        // 错误信息或描述
};

} // namespace XVisual
```

---

## 4. 执行流程

### 4.1 并行 Kahn 算法

```
1. 构建 DAG
   - 遍历 graphSnapshot，计算每个节点的 inDegree
   - 构建 downstream 邻接表
   - 初始化所有节点状态为 Pending

2. 初始化就绪队列
   - 将所有 inDegree == 0 的节点标记为 Ready
   - 将 Ready 节点推入 readyQueue

3. 主循环
   while (remaining > 0 && !canceled && !failed) {
       nodeId = readyQueue.pop()
       if (nodeId.has_value()) {
           threadPool.submit([this, nodeId] {
               executeNode(nodeId);
           });
       }
   }

4. 执行节点
   executeNode(nodeId):
       - 检查是否已取消 -> 标记 Canceled, return
       - 标记 Running, 发送 NodeStarted 事件
       - 调用 resolver(nodeId)->execute(stop_token)
       - 成功 -> 标记 Completed, 发送 NodeFinished
       - 失败 -> 标记 Failed, 发送 NodeFinished, 设置 failed flag

5. 节点完成回调
   onNodeComplete(nodeId, status):
       - 对每个 downstream 节点:
           if (--inDegree[downstream] == 0) {
               if (failed) {
                   标记 downstream 为 Skipped
               } else {
                   标记 downstream 为 Ready
                   readyQueue.push(downstream)
               }
           }
       - --remaining
       - if (remaining == 0) 触发 JobFinished

6. 取消处理
   cancel():
       - 设置 canceled flag
       - 停止 readyQueue（不再 pop）
       - 已 Running 的节点通过 stop_token 检测取消
       - 未执行的 Ready/Pending 节点标记为 Canceled
```

### 4.2 序列图

```
User        MainWindow     GraphExecutor    ThreadPool     INode
 │              │               │                │            │
 │──runButton───▶               │                │            │
 │              │──start()─────▶│                │            │
 │              │               │──buildDAG()───▶│            │
 │              │               │◀───────────────│            │
 │              │               │                │            │
 │              │               │══ 并行循环 ════│            │
 │              │               │──pop()────────▶│            │
 │              │               │◀──nodeA────────│            │
 │              │               │──submit(A)────▶│            │
 │              │               │                │──execute()─▶│
 │              │               │──pop()────────▶│            │
 │              │               │◀──nodeB────────│            │
 │              │               │──submit(B)────▶│            │
 │              │               │                │──execute()─▶│
 │              │               │                │◀────────────│
 │              │               │◀──A完成────────│            │
 │              │               │──onComplete(A)─│            │
 │              │               │                │◀────────────│
 │              │               │◀──B完成────────│            │
 │              │               │──onComplete(B)─│            │
 │              │               │                │            │
 │              │               │══ C 就绪 ══════│            │
 │              │               │──pop()────────▶│            │
 │              │               │◀──nodeC────────│            │
 │              │               │──submit(C)────▶│            │
 │              │               │                │──execute()─▶│
 │              │               │                │◀────────────│
 │              │               │◀──C完成────────│            │
 │              │               │──onComplete(C)─│            │
 │              │               │                │            │
 │              │◀──callback────│                │            │
 │◀──UI解锁─────│               │                │            │
```

---

## 5. 文件清单

### 5.1 新增文件

| 文件路径 | 说明 |
|----------|------|
| `XVisual/include/Core/Executor/ThreadPool.h` | 线程池头文件 |
| `XVisual/src/Core/Executor/ThreadPool.cpp` | 线程池实现 |
| `XVisual/include/Core/Executor/ThreadSafeQueue.h` | 线程安全队列 |
| `XVisual/include/Core/Executor/NodeState.h` | 节点状态定义 |

### 5.2 修改文件

| 文件路径 | 修改内容 |
|----------|----------|
| `XVisual/include/Core/Executor/IEventSink.h` | 增强 NodeEvent 结构 |
| `XVisual/include/Core/Executor/GraphExecutor.h` | 添加并行调度成员 |
| `XVisual/src/Core/Executor/GraphExecutor.cpp` | 实现并行 Kahn 算法 |
| `XVisual/src/Adapter/Qt/QtEventBridge.cpp` | 处理新增事件类型 |
| `XVisual/src/MainWindow/mainwindow.cpp` | UI 响应节点状态变化 |

---

## 6. 线程安全规则

### 6.1 核心原则

1. **每个节点实例只写自己的 `dests_`，只读上游 `dests_`**
2. **禁止跨节点写别人的 VarBag**
3. **任何共享资源（模型缓存、全局日志、文件写入）要么串行，要么加锁**

### 6.2 特殊节点处理

**TFDetectHandle**：
- 如果内部有"进程级静态缓存/单例 session"，PR-4 需改为：
  - 方案 A：每节点实例私有（推荐）
  - 方案 B：资源加锁串行

**日志系统**：
- `XLOG_INFO` 等宏需确保线程安全
- 建议在 Logger 内部使用 mutex 保护

### 6.3 数据竞争检测

推荐在开发阶段使用 ThreadSanitizer (TSan) 检测：
```bash
# MSVC 不支持 TSan，可在 Linux/Clang 环境下测试
clang++ -fsanitize=thread ...
```

---

## 7. 验收用例

### 7.1 并行执行验证

**测试用例**：构造图 A、B 独立，C 依赖 A+B

```
    A ──┐
        ├──▶ C
    B ──┘
```

**预期**：
- 串行执行：A(200ms) + B(200ms) + C(100ms) ≈ 500ms
- 并行执行：max(A,B)(200ms) + C(100ms) ≈ 300ms

**实现**：在 A、B 的 `xOperate()` 中添加 `std::this_thread::sleep_for(200ms)`

### 7.2 失败传播验证

**测试用例**：A 失败，B 正常，C 依赖 A+B

**预期**：
- A 标记为 Failed
- C 标记为 Skipped（不执行）
- B 正常完成
- Job 整体标记为 Failed

### 7.3 取消传播验证

**测试用例**：运行中点击取消按钮

**预期**：
- 已 Running 的节点通过 stop_token 检测取消（优雅退出）
- 未执行的节点标记为 Canceled
- Job 整体标记为 Canceled

### 7.4 UI 状态显示验证

**预期**：
- 节点运行时 UI 可显示状态（颜色/图标变化）
- 节点完成后显示耗时
- 节点失败后显示错误信息

---

## 8. 默认配置

### 8.1 线程池大小

```cpp
// 默认并发度
const size_t defaultPoolSize = std::max(1u, std::thread::hardware_concurrency() - 1);
```

### 8.2 失败策略

- **默认**：fail-fast（任一节点失败，停止派发新节点）
- **已运行节点**：不强杀，允许跑完
- **下游节点**：标记为 Skipped

---

## 9. 实施步骤

### Step 1: PR-4.1 线程池与调度器骨架

1. 创建 `ThreadSafeQueue.h`
2. 创建 `ThreadPool.h/cpp`
3. 修改 `GraphExecutor` 添加并行调度逻辑
4. 验证并行执行

### Step 2: PR-4.2 节点状态机

1. 创建 `NodeState.h`
2. 在 `GraphExecutor` 中添加状态跟踪
3. 实现失败传播和 Skipped 语义
4. 验证失败传播

### Step 3: PR-4.3 事件增强

1. 修改 `IEventSink.h` 中的 `NodeEvent`
2. 在 `GraphExecutor` 中发送详细事件
3. 修改 `QtEventBridge` 处理新事件
4. 验证 UI 状态显示

### Step 4: PR-4.4 并发安全

1. 检查并修复 TFDetectHandle 等节点
2. 确保 Logger 线程安全
3. 添加代码注释说明线程安全规则
4. （可选）使用 TSan 测试

---

## 10. 风险与缓解

| 风险 | 缓解措施 |
|------|----------|
| 数据竞争 | 严格遵循"只写自己 dests_"规则 |
| 死锁 | 使用统一的锁顺序，避免嵌套锁 |
| 线程池饥饿 | 限制单节点执行时间，支持超时 |
| UI 卡顿 | 所有 UI 更新通过 Qt 事件队列 marshal |

---

## 11. 后续演进

- **PR-5**: 设备抽象（在 ThreadPool 基础上添加设备队列选择）
- **PR-6**: 前后端分离（将 Core 层做成独立库）
- **PR-7**: 动态负载均衡（根据节点历史耗时调整调度优先级）
