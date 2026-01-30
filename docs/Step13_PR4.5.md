## PR-4.5 工程化设计方案（设计先行，确认后再落代码）

感谢您的详细反馈，我完全同意您的两处关键调整建议。下面给出具体的文件清单、接口草案和改动点位。

---

## PR-4.5a：节点状态 Overlay 可视化

### 设计原则

1. **不修改 XBaseItem/XBaseFItem 的 paint() 逻辑** - 避免 UI 回归
2. **使用独立的 Overlay 装饰器** - 改动集中、易于移除/迭代
3. **状态更新只在 UI 线程** - QtEventBridge 已保证

### 文件清单

| 文件 | 操作 | 说明 |
|------|------|------|
| `include/ItemWidget/NodeStateOverlay.h` | 新建 | Overlay 装饰器类定义 |
| `src/ItemWidget/NodeStateOverlay.cpp` | 新建 | Overlay 绘制实现 |
| `include/MainWindow/diagramscene.h` | 修改 | 添加 overlay 管理 |
| `src/MainWindow/diagramscene.cpp` | 修改 | 创建/更新 overlay |
| `src/MainWindow/mainwindow.cpp` | 修改 | 事件处理中更新 overlay |

### 类/接口草案

```cpp
// NodeStateOverlay.h
namespace XVisual {

class NodeStateOverlay : public QGraphicsItem {
public:
    explicit NodeStateOverlay(QGraphicsItem* targetItem, QGraphicsItem* parent = nullptr);
    
    void setState(NodeState state);
    NodeState state() const;
    
    // 动画控制
    void startPulseAnimation();
    void stopPulseAnimation();
    
protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
private:
    QGraphicsItem* targetItem_;
    NodeState state_ = NodeState::Pending;
    QTimer* pulseTimer_ = nullptr;
    qreal pulsePhase_ = 0.0;
    
    QColor stateColor() const;
    Qt::PenStyle statePenStyle() const;
    QString stateIcon() const;  // "✓" / "✗" / ""
};

} // namespace XVisual
```

### Overlay 绑定方式

```cpp
// DiagramScene 中
class DiagramScene {
private:
    // nodeId -> overlay
    std::unordered_map<std::string, NodeStateOverlay*> nodeOverlays_;
    
public:
    void setNodeState(const std::string& nodeId, NodeState state);
    void clearAllOverlays();  // JobFinished 后清理
};
```

### 状态颜色/样式表

| NodeState | 边框颜色 | 边框样式 | 角标 | 动画 |
|-----------|----------|----------|------|------|
| Pending | 灰色 #808080 | 实线 | 无 | 无 |
| Ready | 蓝色 #4A90D9 | 实线 | 无 | 无 |
| Running | 橙色 #FFA500 | 实线 | 无 | 脉冲 |
| Completed | 绿色 #28A745 | 实线 | ✓ | 无 |
| Failed | 红色 #DC3545 | 实线 | ✗ | 无 |
| Skipped | 灰色 #808080 | 虚线 | — | 无 |

### Z-order 设计

- Overlay 的 `zValue()` 设为目标 Item 的 zValue + 0.5
- 不覆盖选中/hover 反馈（选中由 Item 自身处理）

---

## PR-4.5b：调度器心跳事件

### 设计原则

1. **心跳 ≠ 进度** - 心跳只表示"仍在运行"，不承诺百分比
2. **调度器统一发送** - 不侵入各 Handle 实现
3. **生命周期绑定 Job** - 心跳线程必须在 JobFinished 前 join

### 文件清单

| 文件 | 操作 | 说明 |
|------|------|------|
| `include/Core/Executor/IEventSink.h` | 修改 | 添加 NodeHeartbeat 事件类型 |
| `include/Core/Executor/GraphExecutor.h` | 修改 | 添加心跳线程相关成员 |
| `src/Core/Executor/GraphExecutor.cpp` | 修改 | 实现心跳循环 |
| `src/MainWindow/mainwindow.cpp` | 修改 | 处理心跳事件 |

### 事件定义

```cpp
// IEventSink.h 扩展
enum class EventType {
    // ... existing ...
    NodeHeartbeat,  // 新增
};

struct NodeEvent {
    // ... existing fields ...
    std::uint64_t elapsedUs = 0;  // 节点已运行时长（心跳用）
};
```

### 心跳线程实现草案

```cpp
// GraphExecutor.h
class GraphExecutor {
private:
    std::unique_ptr<std::jthread> heartbeatThread_;
    std::chrono::milliseconds heartbeatInterval_{1000};
    
    void heartbeatLoop(std::stop_token st);
};

// GraphExecutor.cpp
void GraphExecutor::heartbeatLoop(std::stop_token st) {
    while (!st.stop_requested() && remaining_.load() > 0) {
        std::this_thread::sleep_for(heartbeatInterval_);
        
        std::lock_guard<std::mutex> lock(stateMutex_);
        for (const auto& [nodeId, stateInfo] : nodeStates_) {
            if (stateInfo.state == NodeState::Running) {
                NodeEvent evt;
                evt.type = EventType::NodeHeartbeat;
                evt.jobId = jobId_;
                evt.nodeId = nodeId;
                evt.elapsedUs = nowMicros() - stateInfo.startTime;
                evt.tsUs = nowUs();
                emitEvent(evt);
            }
        }
    }
}
```

### 心跳线程生命周期

```cpp
// runParallel() 中
void GraphExecutor::runParallel(...) {
    // 启动心跳线程
    heartbeatThread_ = std::make_unique<std::jthread>(
        [this](std::stop_token st) { heartbeatLoop(st); }
    );
    
    // ... 主调度循环 ...
    
    // 等待所有任务完成后，停止心跳
    heartbeatThread_->request_stop();
    heartbeatThread_->join();
    heartbeatThread_.reset();
    
    // 发送 JobFinished
    // ...
}
```

### UI 心跳处理

```cpp
// mainwindow.cpp
case EventType::NodeHeartbeat:
    // 更新状态栏或侧边栏
    statusBar()->showMessage(
        QString("%1 running... %2s")
            .arg(QString::fromStdString(evt.nodeId))
            .arg(evt.elapsedUs / 1000000.0, 0, 'f', 1)
    );
    break;
```

---

## 改动点位汇总

### PR-4.5a 改动点

1. **新建** `NodeStateOverlay` 类（绘制边框/角标/动画）
2. **DiagramScene** 添加 `nodeOverlays_` map 和 `setNodeState()` 方法
3. **MainWindow** 事件处理中调用 `scene->setNodeState()`

### PR-4.5b 改动点

1. **IEventSink.h** 添加 `NodeHeartbeat` 事件类型，`NodeEvent` 添加 `elapsedUs` 字段
2. **GraphExecutor** 添加心跳线程，在 `runParallel` 中启动/join
3. **MainWindow** 处理 `NodeHeartbeat` 更新 UI

---

## 确认项

请确认以下内容后，我将开始落代码：

1. ✅ Overlay 方案（不改 XBaseItem paint）
2. ✅ 心跳事件语义（仅表示"仍在运行"，不承诺进度百分比）
3. ✅ 心跳间隔 1 秒
4. ✅ 文件清单和接口草案

确认后请回复"开始实施"，我将按此方案生成代码。


## 心跳包快照锁外emit 
方案里唯一需要调整的点：避免 stateMutex_ 影响 worker
你现在的草案是：
std::lock_guard<std::mutex> lock(stateMutex_);
for (...) { ... emitEvent(evt); }
这里有两个风险：
1. 持锁期间 emitEvent 可能较慢（尤其 QtEventBridge 会排队/分配内存），会延长锁持有时间
2. worker 完成节点时也会锁 stateMutex_ 更新状态/时间戳 → 可能被心跳线程阻塞
推荐的写法（小改动，性能更稳）
心跳线程只在锁内做“快照拷贝”，锁外 emit：

需要调整的点：避免 stateMutex_ 影响 worker 你现在的草案是： cpp std::lock_guard<std::mutex> lock(stateMutex_); for (...) { ... emitEvent(evt); }  这里有两个风险： 1. 持锁期间 emitEvent 可能较慢（尤其 QtEventBridge 会排队/分配内存），会延长锁持有时间 2. worker 完成节点时也会锁 stateMutex_ 更新状态/时间戳 → 可能被心跳线程阻塞 ### 推荐的写法（小改动，性能更稳） 心跳线程只在锁内做“快照拷贝”，锁外 emit： 
std::vector<std::pair<std::string, uint64_t>> running;
{
  std::lock_guard<std::mutex> lk(stateMutex_);
  for (auto& [nodeId, s] : nodeStates_) {
    if (s.state == NodeState::Running) {
      running.emplace_back(nodeId, nowUs() - s.startTimeUs);
    }
  }
}
// 锁外 emitEvent
for (auto& [nodeId, elapsed] : running) {
  emitHeartbeat(nodeId, elapsed);
}
这样心跳线程对 worker 的影响趋近于 0。给出"心跳包快照锁外emit"的实施方案, 等指令再实施

您的反馈非常正确。当前实现存在性能风险。下面是"心跳包快照锁外 emit"的优化方案：
优化方案：心跳包快照锁外 emit
问题分析
当前实现：
void GraphExecutor::heartbeatLoop(std::stop_token st)
{
    while (!st.stop_requested() && remaining_.load() > 0)
    {
        std::this_thread::sleep_for(currentOptions_.heartbeatInterval);
        
        // ❌ 风险：持锁期间 emitEvent
        {
            std::lock_guard<std::mutex> lock(stateMutex_);
            int64_t now = nowMicros();
            for (const auto& [nodeId, stateInfo] : nodeStates_)
            {
                if (stateInfo.state == NodeState::Running)
                {
                    NodeEvent evt;
                    // ... 构建事件 ...
                    emitEvent(evt);  // ❌ 可能较慢
                }
            }
        }
    }
}
优化后的实现
void GraphExecutor::heartbeatLoop(std::stop_token st)
{
    while (!st.stop_requested() && remaining_.load() > 0)
    {
        // 等待心跳间隔
        std::this_thread::sleep_for(currentOptions_.heartbeatInterval);

        // 检查是否应该停止
        if (st.stop_requested() || remaining_.load() <= 0)
            break;

        // ✅ 锁内只做快照拷贝
        std::vector<std::pair<std::string, std::uint64_t>> runningNodes;
        {
            std::lock_guard<std::mutex> lock(stateMutex_);
            int64_t now = nowMicros();
            for (const auto& [nodeId, stateInfo] : nodeStates_)
            {
                if (stateInfo.state == NodeState::Running)
                {
                    runningNodes.emplace_back(
                        nodeId, 
                        static_cast<std::uint64_t>(now - stateInfo.startTime)
                    );
                }
            }
        }
        // ✅ 锁已释放

        // ✅ 锁外 emitEvent
        for (const auto& [nodeId, elapsedUs] : runningNodes)
        {
            NodeEvent evt;
            evt.type = EventType::NodeHeartbeat;
            evt.jobId = jobId_;
            evt.graphId = graphId_;
            evt.nodeId = nodeId;
            evt.state = NodeState::Running;
            evt.elapsedUs = elapsedUs;
            evt.tsUs = static_cast<std::uint64_t>(nowMicros());
            emitEvent(evt);
        }
    }
}
改动点
| 位置 | 改动 | |------|------| | GraphExecutor.cpp | heartbeatLoop() 方法 - 锁内只做快照拷贝，锁外 emit |
性能收益
1. 锁持有时间最小化 - 只做 map 遍历 + 拷贝（O(n) 字符串拷贝 + int64）
2. emitEvent 在锁外 - QtEventBridge 的内存分配、排队操作不影响 worker
3. worker 不被阻塞 - 节点完成时更新状态的锁等待时间趋近于 0
等您指令后实施此优化。
心跳包快照锁外 emit 优化方案已准备好：
当前问题：
- 持锁期间 emitEvent 可能较慢（QtEventBridge 排队/分配内存）
- worker 完成节点时更新状态可能被心跳线程阻塞
优化方案：
// 锁内只做快照拷贝
std::vector<std::pair<std::string, std::uint64_t>> runningNodes;
{
    std::lock_guard<std::mutex> lock(stateMutex_);
    for (auto& [nodeId, s] : nodeStates_) {
        if (s.state == NodeState::Running)
            runningNodes.emplace_back(nodeId, nowUs() - s.startTime);
    }
}
// 锁外 emitEvent
for (auto& [nodeId, elapsed] : runningNodes) {
    emitHeartbeat(nodeId, elapsed);
}
性能收益：
- 锁持有时间最小化
- emitEvent 在锁外执行
- worker 不被心跳线程阻塞
是否开始实施此优化？   
开始实施