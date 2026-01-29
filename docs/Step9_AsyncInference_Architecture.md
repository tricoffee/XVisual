# Step9 - 节点级并行异步推理 + 异构设备（CPU/GPU/NPU）+ 前后端分离（UI进程 + 后端服务）设计

> 目标：在不推翻现有 XVisual 图编辑/导出能力的前提下，扩展 XVisual 支持：
> 1) **节点级并行异步推理**；
> 2) **CPU/GPU/NPU 混合节点**（含 TensorRT/CUDA）；
> 3) **UI 与后端执行彻底解耦**（UI 进程仅负责编辑/展示；后端是独立推理服务）。

本设计基于当前代码现状：
- UI 里 `MainWindow::runButtonClicked()` 同步执行 `XGraph::executeXGraph<XBaseItem>(globalItemMap, xGraph)`，导致 UI 阻塞；
- UI Item 的 `xOperate()` 内直接 `emit showImageSignal` 更新界面，造成 **执行与 UI 强耦合**；
- `XInfer` 已经具备“脱离 UI 的 Handle 图构建 + 执行”的雏形：`XGraph::executeXGraph<XBaseHandle>(globalHandleMap, xHandleGraph)`。

---

## 1. 总体架构（进程边界）

### 1.1 进程模型

```
┌──────────────────────────┐          gRPC/Protobuf (Control Plane)
│        XVisual UI         │  ─────────────────────────────────────►  RunGraph / Cancel / QueryStatus / SubscribeEvents
│  Qt Editor + Visualizer   │  ◄─────────────────────────────────────  GraphEvent stream (node状态、耗时、bbox等元信息)
└───────────┬──────────────┘
            │
            │ Shared Memory (Data Plane)
            │  image/tensor 大数据不走 gRPC：
            │  UI 写 shm -> 发送 ShmRef；后端读 shm/写 shm -> 返回 ShmRef
            ▼
┌──────────────────────────┐
│   XVisual Backend Service │
│  Core Runtime + Scheduler │
│  CPU/GPU/NPU Executors    │
│  Model/Engine Cache       │
└──────────────────────────┘
```

### 1.2 Control Plane 与 Data Plane 分工

**Control Plane（gRPC）**负责：
- RunGraph / LoadModel（可选）/ QueryStatus / Cancel
- 订阅事件（节点开始/结束/错误/耗时/拓扑进度）
- 传输小数据（参数、bbox、classNames 等）

**Data Plane（Shared Memory）**负责：
- cv::Mat、PNG/JPEG bytes、大 tensor、feature map 等
- gRPC 只传“引用”：shm_name + offset + bytes + shape/stride/dtype + frame_id

---

## 2. 后端 Core Runtime 设计

### 2.1 核心对象

#### (1) GraphSpec（纯后端图描述）
- `nodes[]`：node_id、op_type（原 className/HandleName）、参数（inner/outer）、输入端口定义、输出端口定义
- `edges[]`：from(node_id, out_port) -> to(node_id, in_port)

> 迁移策略：**第一阶段可直接复用现有 solutions.json**，后端服务内部复用 `XParser`/`XInfer::build()` 的解析逻辑；第二阶段再升级成更强的 proto GraphSpec。

#### (2) Kernel（节点计算实现，替代“Item::xOperate()”的执行角色）
接口建议（伪代码）：

```cpp
struct KernelContext {
  ExecutionContext& exec;
  NodeId node;
  CancellationToken& cancel;
  IDevice& device;
};

class IKernel {
public:
  virtual ~IKernel() = default;
  virtual std::string opType() const = 0;
  virtual std::vector<DeviceType> supportedDevices() const = 0;
  virtual DeviceType preferredDevice() const = 0;  // 可配置 override
  virtual Status run(KernelContext& ctx) = 0;
};
```

#### (3) ExecutionContext（每次 run 独立，避免全局/单例污染）
必须包含：
- `VariableStore`：按 `(node_id, port)` 存储 outputs，并支持引用计数释放
- `RunMetrics`：耗时、profiling、trace id
- `CancellationToken`

> 这是支持“节点级并行”的关键：当前实现 `Source::getInstance()/Dest::getInstance()` + `globalHandleMap` 会导致并发写入冲突。

#### (4) Scheduler（节点级并行 DAG 调度器）
目标：DAG 入度计数 + ready 队列 + 设备选择 + 任务投递。

建议结构：
- 预处理：对每个 node 计算 `in_degree`、`dependents[]`
- `ready_queue`：线程安全队列（可按设备分队列）
- 节点状态机：`Pending -> Running -> Succeeded/Failed/Canceled`

#### (5) Executors（设备执行器）
- `CPUExecutor`：线程池（N worker），可并行执行多个 CPU kernel
- `GPUExecutor`：
  - **默认建议：单队列/单线程串行提交**（TensorRT/TF session 并发风险更低）
  - 可扩展：多 stream 并行（需要 kernel 声明 thread-safe / stream-safe）
- `NPUExecutor`：对接厂商 runtime（通常也是队列模型）

---

## 3. 异构设备（CPU/GPU/NPU）策略

### 3.1 Device 抽象

```cpp
enum class DeviceType { CPU, GPU, NPU };

class IDevice {
public:
  virtual ~IDevice() = default;
  virtual DeviceType type() const = 0;
  virtual Buffer alloc(size_t bytes) = 0;
  virtual Status memcpyH2D(Buffer dst, const void* src, size_t bytes) = 0;
  virtual Status memcpyD2H(void* dst, Buffer src, size_t bytes) = 0;
  virtual Status synchronize() = 0;
};
```

### 3.2 数据位置与搬运（必须有“location”概念）

引入统一数据容器：
- `XValue`：variant( ImageRef / TensorRef / Detections / Scalars ... )
- `TensorRef` 包含：dtype、shape、stride、bytes、location(Host/GPU/NPU)、以及实际存储（shared_ptr + deleter）

调度器可在需要时插入隐式 copy：
- Host->GPU（H2D）
- GPU->Host（D2H）
- GPU->NPU / NPU->GPU（视厂商能力，通常走 Host 作为中转）

> 实践建议：先实现 **Host<->GPU**，并将 NPU 作为后续 executor 插件。

---

## 4. GPU（TensorRT/CUDA）执行器与 Kernel 设计

### 4.1 TensorRT Kernel 建议形态

以当前 `TFDetectHandle` 为例，建议增加 `TrtDetectKernel`：
- 输入：image tensor（或 cv::Mat -> preprocess -> tensor）
- 输出：detections（bbox/classId/score）、可选输出 overlay image

模型/engine 管理：
- 后端服务维护 `EngineCache`：key = (model_id / engine_path / device_id / precision)
- engine 的创建与反序列化耗时大，必须缓存

### 4.2 CUDA stream 策略（并行度）

建议分层支持：
1) **默认：GPUExecutor 单 stream 串行**（最稳妥）
2) 可选：GPUExecutor 多 stream（需要：
   - 每个 inflight task 拥有独立 stream / bindings / workspace
   - kernel 声明 `streamSafe=true`）

---

## 5. gRPC API（Control Plane）建议

### 5.1 关键 RPC

#### RunGraph
- 入参：GraphSpec（或 solutions.json 文本/bytes）、Inputs（含 ShmRef）、运行配置（并行度、device policy）
- 出参：run_id

#### SubscribeEvents（Server streaming）
- 入参：run_id
- 出参：GraphEvent stream

GraphEvent 建议包含：
- node_id / state(Pending/Running/Succeeded/Failed/Canceled)
- start_ts/end_ts/duration_ms
- outputs 元信息（比如 bbox、classNames、以及可视化 artifact 的 ShmRef）
- error message / error code

#### CancelRun
- 入参：run_id
- 语义：停止投递未执行节点；正在执行节点尽可能协作式取消（GPU/NPU 可能只能等待完成）

#### QueryStatus
- 入参：run_id
- 出参：总体进度、已完成节点数、失败原因、耗时统计

### 5.2 ShmRef（Data Plane 引用）

建议 proto 结构（概念层）：
- `string shm_name`：跨进程可打开的共享内存名称（Windows: CreateFileMapping name；Linux: shm_open name）
- `uint64 offset` / `uint64 bytes`
- `string frame_id`：便于 UI 管理生命周期/引用
- 对于 image：width/height/channels/stride、format(BGR/RGB/GRAY)
- 对于 tensor：dtype、shape、stride

生命周期策略：
- **Run 级别 allocator**：run 结束统一释放；
- 或基于 ref-count：当 UI ack “已消费 artifact” 后释放。

---

## 6. 如何从当前代码演进（推荐分阶段实施）

### Phase 0：利用现有 XInfer 作为后端执行雏形
当前仓库已经有 `XInfer`：
- `build()`：解析 solutions.json，构建 `globalHandleMap + xHandleGraph`
- `exec()`：调用 `XGraph::executeXGraph<XBaseHandle>` 执行（同步串行）

这非常适合作为后端服务的第一版“执行引擎”。

### Phase 1：UI/后端进程分离（gRPC + shared memory 通道打通）
1) 新增 `XVisualBackendService`（可放在新工程 `XBackend/`）：
   - gRPC server + ShmManager
   - RunGraph：接收 solutions.json（或 GraphSpec）+ 输入 ShmRef
   - 调用现有 `XInfer` 同步执行（先不做并行）
2) UI：Run 按钮改为调用 gRPC client，订阅事件并显示结果。

### Phase 2：移除全局/单例（为并行做准备）
重点改造点：
- 将 `globalHandleMap / xHandleGraph / nodeIdTopoSequence / Source::getInstance / Dest::getInstance` 从“全局共享”改为“run 内部对象”。
- 引入 `ExecutionContext`：每次 Run 创建独立 variable store。

### Phase 3：节点级并行 Scheduler + CPUExecutor
- 将 `executeXGraph` 替换为 Scheduler：ready 队列 + worker pool
- 完成事件流（node start/finish）
- 支持 Cancel（至少可停止投递）

### Phase 4：GPU/TensorRT kernel & GPUExecutor
- 增加 TensorRT backend（ONNX->engine 或直接加载 engine）
- GPUExecutor 先单队列，再逐步多 stream

### Phase 5：NPU executor 插件化
- 增加 IDevice/Executor 插件接口
- 适配厂商 runtime

---

## 7. 风险点与建议（结合当前实现）

1) **并行与共享状态冲突**：目前 `XBaseHandle` 的 sources/dests 可能来自单例（风险很高），必须改。
2) **globalHandleMap 访问**：`XBaseHandle::initOperands()` 依赖全局 map + mutex；并行会放大锁竞争。
3) **GPU 并发安全**：TensorRT engine/context 是否可并发，取决于 context/bindings 管理；建议默认单队列。
4) **SharedMemory 跨平台细节**：Windows 的 named mapping、权限、安全、清理策略要设计好；建议统一封装 `ShmManager`。

---

## 8. 建议的“默认组合”（最稳妥可落地）

- IPC：gRPC(Protobuf) 控制面 + Named SharedMemory 数据面（大数据不走 gRPC）
- 调度：CPU 线程池（N = std::thread::hardware_concurrency-1） + GPU 单队列 executor
- 后端：C++
- 模型：
  - 首选 TensorRT（ONNX->Engine），用于 GPU 节点
  - CPU 节点可继续 OpenCV/传统处理
  - TF/PyTorch/ONNXRuntime 作为“后续可插拔 backend”，先把 Kernel/Executor 抽象打通
- 可视化：
  - gRPC event 返回 bbox/keypoints 等小数据
  - 可选返回 overlay image 的 ShmRef

