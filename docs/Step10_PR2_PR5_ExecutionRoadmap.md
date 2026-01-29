# XVisual Async Inference Roadmap (PR-2 ~ PR-5) — 施工级设计（不改代码版本）

> 状态：PR-1 已完成并验证可编译运行（后台异步串行 + UI LockAll + 节点边界 Cancel）。
>
> 本文固化后续 PR-2/PR-3/PR-4/PR-5 的“接口边界/目录结构/线程模型/事件协议/验收标准”，便于直接开分支施工。
>
> 原则：
> - Cancel 粒度：节点边界生效（不强杀节点内部黑盒推理）
> - UI 行为：运行期间 Lock All（工业软件一致性）
> - Core/UI 解耦目标：Core 零 Qt 依赖，未来可 CLI/服务端复用

---

## 0. 已定的 4 个决策（作为路线前提）

### 决策 1：PR 顺序
**结论：先 PR-2（执行引擎抽离）再 PR-3（消灭全局依赖）。**

原因：
- PR-2 固化入口/线程/取消/事件模型，后续改动都落在 executor/context，而不是 UI 入口散落修改
- PR-3 会牵涉 globalMap + Source/Dest/Param 单例等“结构性手术”，需要一个稳定的 Executor 承接

### 决策 2：PR-4 并发范围
**结论：PR-4 先只做 CPU DAG 并发；GPU/NPU 只做队列化占位（接口先铺，提交仍串行）。**

### 决策 3：节点级事件
**最小事件集合（PR-2/PR-4 推荐一次性支持）：**
1. `job_started(job_id, graph_id)`
2. `node_started(node_id, timestamp)`
3. `node_finished(node_id, status, duration_us)`
4. `job_finished(status)`

可选增强（同框架即可顺手加，但不强制第一版就做 artifact 数据搬运）：
- `log(level, node_id, message)`
- `artifact(node_id, port, ArtifactRef)`（图像/框/中间张量等）

### 决策 4：Core/UI 分离强度
**结论：目标定为“未来可无 UI CLI/服务端运行”。**（Core 代码零 Qt include）

---

## 1. 总体分层（从 PR-2 开始遵循）

### 1.1 Core（无 Qt）
- 图执行控制流：串行执行、取消、事件、未来并发调度器
- 运行期上下文：节点实例、数据通道、资源缓存（模型等）
- 设备抽象（占位）：CPU/GPU/NPU 队列、数据搬运

### 1.2 UI（Qt）
- 编辑/展示/锁定策略
- 组装 GraphSnapshot / ExecutionContext（PR-2 仅 snapshot）
- 将 Core 事件 marshal 到 UI 主线程，并更新高亮/日志/状态

---

## 2. PR-2（Phase B1）：抽出 GraphExecutor（仍串行、节点边界取消）

### 2.1 PR-2 目标
- MainWindow 不再直接调用 `XGraph::executeXGraph`
- UI 只负责：
  - snapshot（graphSnapshot）
  - LockAll/UnlockAll
  - start/cancel
  - 消费事件（节点高亮、耗时、错误展示）
- GraphExecutor 负责：
  - `std::jthread + stop_source/stop_token`
  - 串行执行图
  - 节点边界 cancel
  - 产生标准事件

### 2.2 PR-2 的接口边界（你选择的“更干净”方案）
**GraphExecutor 不依赖 `XBaseItem` / Qt。**  
它只认识：
1) `graphSnapshot`（拓扑结构）
2) `NodeResolver`（如何执行一个 nodeId）
3) `IEventSink`（如何上报事件）

#### 2.2.1 INode：节点执行接口（Core）
```cpp
namespace XVisual {

class INode {
public:
  virtual ~INode() = default;
  virtual void initOperands() = 0;
  virtual void run(std::stop_token st) = 0;
};

} // namespace XVisual
```

> 说明：`run()` 内部仍可以调用现有 Item/Handle 的逻辑。  
> Cancel 粒度仍为节点边界：`run()` 不要求响应 stop_token（可选支持），Executor 只在节点前检查 stop。

#### 2.2.2 NodeResolver：节点解析器（Core）
```cpp
using NodeResolver = std::function<XVisual::INode*(const std::string& nodeId)>;
```

#### 2.2.3 EventSink：事件接口（Core）
```cpp
namespace XVisual {

enum class EventType { JobStarted, NodeStarted, NodeFinished, JobFinished, Log };

struct NodeEvent {
  EventType type;
  std::string jobId;
  std::string graphId;
  std::string nodeId;
  int code = 0;                // ErrorCode or Status code
  std::string message;
  uint64_t tsUs = 0;
  uint64_t durationUs = 0;
};

class IEventSink {
public:
  virtual ~IEventSink() = default;
  virtual void onEvent(const NodeEvent& e) = 0;
};

} // namespace XVisual
```

#### 2.2.4 GraphExecutor（Core）
```cpp
namespace XVisual {

class GraphExecutor {
public:
  struct Options {
    bool nodeBoundaryCancel = true;
  };

  using FinishedCallback = std::function<void(XVisual::ErrorCode)>;

  GraphExecutor();
  ~GraphExecutor();

  bool start(std::string graphId,
             std::vector<std::shared_ptr<GraphNode>> graphSnapshot,
             NodeResolver resolver,
             IEventSink* sink,
             Options opt = {});

  void cancel();       // request_stop
  bool isRunning() const;

private:
  std::atomic_bool running_{false};
  std::stop_source stopSource_;
  std::unique_ptr<std::jthread> worker_;
};

} // namespace XVisual
```

### 2.3 执行策略（PR-2）
- 串行拓扑执行仍可复用现有 `XGraph`，但执行单元由 resolver 提供：
  - `INode* node = resolver(nodeId)`
  - `node->initOperands(); node->run(stopToken /*可传*/);`
- Cancel：每个节点执行前 `if (stopToken.stop_requested()) return Canceled;`

### 2.4 UI 适配（PR-2 设计，不动代码）
UI 层（MainWindow）持有一个 `QtEventBridge`：
- 实现 `IEventSink::onEvent`
- 在 `onEvent` 内用 `QMetaObject::invokeMethod(...QueuedConnection)` 切回主线程
- UI 主线程执行：
  - node_started：节点高亮、时间戳记录
  - node_finished：耗时显示、失败标红
  - job_finished：UnlockAll、提示错误

**注意**：GraphExecutor 不做 UI 逻辑；UI 也不做线程管理细节（线程由 GraphExecutor 管）。

### 2.5 PR-2 文件结构建议
（可放在现有 XVisual 工程内，先不拆库；后续再抽 Core library）
```
XVisual/include/Core/
  Executor/
    GraphExecutor.h
    IEventSink.h
    INode.h
XVisual/src/Core/
  Executor/
    GraphExecutor.cpp
XVisual/include/Adapter/
  Qt/
    QtEventBridge.h
XVisual/src/Adapter/Qt/
  QtEventBridge.cpp
```

### 2.6 PR-2 验收标准
1. MainWindow 不再创建/管理 `std::jthread`（迁移到 GraphExecutor）
2. 行为保持 PR-1：LockAll、节点边界取消、执行结束 UI 解锁
3. 具备最小事件集：job_started/node_started/node_finished/job_finished（日志可选）
4. GraphExecutor/INode/IEventSink 不包含 Qt 头文件

---

## 3. PR-3（Phase B2）：ExecutionContext（消灭 globalMap + 拆 singleton）

### 3.1 PR-3 目标
- 执行期不依赖 `globalItemMap/globalHandleMap`
- 引入 `ExecutionContext`：本次运行的节点实例集合 + 数据通道 + 资源缓存
- **关键必须项**：拆掉 `Source::getInstance()/Dest::getInstance()` 单例共享（否则无法证明并发正确）

### 3.2 Core 运行期数据模型（建议）
#### 3.2.1 Value（先 CPU-only，后续扩展 device）
```cpp
struct Value {
  std::any payload;          // Phase B2: cv::Mat, vector<DetectResult>, etc.
  // PR-5 预留：
  // DeviceDesc device;
  // std::string dtype;
};
```

#### 3.2.2 TensorStore（运行期 KV 数据通道）
```cpp
class TensorStore {
public:
  void put(const std::string& key, Value v);
  std::optional<Value> get(const std::string& key) const;
};
```

#### 3.2.3 ExecutionContext
```cpp
struct ExecutionContext {
  TensorStore store;
  // ResourceCache resources; // PR-5: model/session cache
  std::unordered_map<std::string, std::unique_ptr<INode>> nodes; // nodeId -> node instance
  std::shared_ptr<const GraphIR> graph; // 后续替换 GraphNode 结构
};
```

### 3.3 PR-3 的关键工程策略
- 每个节点应拥有私有参数/输入输出容器
- 如果短期无法彻底改掉 `Param` 的 any-map 结构，也必须做到**不共享静态实例**
- 对模型对象（如 TensorFlowModel）建议引入 ResourceCache，避免每次 run 重复 load（后续优化）

---

## 4. PR-4（Phase C）：CPU DAG 并发调度器（GPU/NPU 占位）

### 4.1 PR-4 目标
- 真异步：同图内无依赖节点并行执行（CPU）
- cancel：停止派发新节点（节点边界）
- 失败传播：任一节点失败 -> 标记 job 失败并停止派发下游
- 事件：node_started/node_finished/job_finished

### 4.2 核心调度算法（Kahn + readyQueue + thread pool）
- 维护 `inDegree[node]`
- 初始化 `readyQueue`（inDegree==0）
- `ThreadPool cpuPool(N)`
- dispatch：
  - 从 readyQueue 取 node
  - 提交到 cpuPool 执行
- 完成回调：
  - 更新下游 inDegree
  - inDegree==0 入队
- cancel：
  - stop_token 请求后不再派发；已在跑的节点不强杀

### 4.3 设备队列占位（PR-4 只启 CPU）
- 抽象接口先铺：
  - `DeviceQueue`（CPU/GPU/NPU）
  - `dispatch(node, device)` 先都走 CPU
- PR-5 再填 GPU/NPU queue 逻辑

---

## 5. PR-5（Phase D）：异构设备调度 + 数据搬运

### 5.1 PR-5 目标
- 节点声明 capability：支持哪些设备、preferred device
- 调度器按 device queue 派发
- 跨设备依赖时：
  - 自动插入 `CopyToX` 节点或隐式搬运（建议显式 CopyNode 便于 debug）

### 5.2 关键抽象（PR-3/4 就要预留）
- `DeviceManager::getQueue(DeviceType)`
- `Value` 增加 device/location 信息
- `Value.to(DeviceType)`（必要时 copy/convert）

---

## 6. 里程碑建议（给项目管理用）
- PR-2：入口解耦 + 事件框架（串行）
- PR-3：ExecutionContext + per-node 私有参数（并发前提）
- PR-4：CPU DAG 并发（真异步）
- PR-5：GPU/NPU 队列化 + 数据搬运（异构）

---

## 7. 风险清单（提前规避）
1. 现有 Source/Dest/Param 单例：必须在 PR-3 处理，否则 PR-4 无法正确并发
2. 推理框架线程安全：GPU/NPU 并发不要在 PR-4 直接打开
3. 事件线程归属：Core 事件在后台线程产生，UI 侧必须 marshal 到主线程
4. 产物回传（artifact）慎做：跨线程/跨设备的数据生命周期需明确（建议先只做 metadata/log）

---

## 8. PR-2 施工前的“定稿点”（后续如需微调）
- nodeId 的规范：现阶段以 `Item_...` 为 nodeId 还是切到 `Handle_...`
- 事件扩展：是否第一版就加 `log`（推荐加，成本小价值大）
