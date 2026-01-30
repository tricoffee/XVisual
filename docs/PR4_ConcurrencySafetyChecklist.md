# PR-4 并发安全检查清单

本文档列出了 PR-4（异步执行与并行调度）实施后需要遵守的并发安全规则。

## 1. 核心原则

### 1.1 数据隔离规则

| 规则 | 说明 |
|------|------|
| ✅ 每个节点只写自己的 `dests_` | 节点执行时只能修改自己的输出变量 |
| ✅ 每个节点只读上游的 `dests_` | 读取输入数据时，只能读取已完成节点的输出 |
| ❌ 禁止跨节点写别人的 VarBag | 任何节点不得修改其他节点的数据 |
| ❌ 禁止在节点内写全局变量 | 除非该全局变量有锁保护 |

### 1.2 数据流向

```
Node A (完成)                Node B (运行中)
┌─────────────┐            ┌─────────────┐
│  sources_   │            │  sources_   │
│  dests_  ◀──│────读取────│───▶ dests_  │
│  (只读)     │            │  (只写)     │
└─────────────┘            └─────────────┘
```

## 2. 特殊节点处理

### 2.1 TFDetectHandle

**问题**：如果 TensorFlow/ONNX session 是进程级单例，并发执行会导致数据竞争。

**解决方案**：

```cpp
// 方案 A：每节点实例私有 session（推荐）
class TFDetectHandle : public XBaseHandle {
private:
    std::unique_ptr<TFSession> session_;  // 私有 session
};

// 方案 B：全局 session 加锁
class TFDetectHandle : public XBaseHandle {
private:
    static std::mutex sessionMutex_;
    static std::shared_ptr<TFSession> sharedSession_;
    
    void xOperate() override {
        std::lock_guard<std::mutex> lock(sessionMutex_);
        // 使用 sharedSession_
    }
};
```

### 2.2 文件操作节点

**问题**：多个节点同时写入同一文件会导致数据损坏。

**解决方案**：
- 每个节点使用唯一的输出文件名
- 或使用文件锁（不推荐，影响性能）

### 2.3 GPU 资源节点

**问题**：CUDA 操作默认在 default stream，多节点并发会导致乱序。

**解决方案**：
- 每个节点使用独立的 CUDA stream
- 或使用 CUDA graph 管理依赖

## 3. 日志系统

### 3.1 当前状态

`XLOG_INFO` 等宏需要确保线程安全。

### 3.2 检查项

- [ ] Logger 内部使用 mutex 保护
- [ ] 日志格式化不依赖全局状态
- [ ] 日志文件写入是原子操作（或使用缓冲区）

### 3.3 示例修复

```cpp
class LoggerInstance {
private:
    std::mutex mutex_;
    std::ofstream logFile_;
    
public:
    void log(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        logFile_ << msg << std::endl;
    }
};
```

## 4. GraphExecutor 内部安全

### 4.1 已实现的保护

| 数据结构 | 保护方式 |
|----------|----------|
| `nodeStates_` | `stateMutex_` |
| `readyQueue_` | `ThreadSafeQueue` 内部锁 |
| `inDegree_` | 原子操作（`--inDegree_[id]`） |
| `remaining_` | `std::atomic<int>` |
| `canceled_` / `failed_` | `std::atomic<bool>` |

### 4.2 潜在风险点

| 风险 | 缓解措施 |
|------|----------|
| `graphSnapshot_` 被多线程读取 | 只在构建 DAG 时写入，执行期只读 |
| `downstream_` 被多线程读取 | 同上 |
| `sink_->onEvent()` 被多线程调用 | QtEventBridge 内部使用 Qt 事件队列 marshal |

## 5. 验证方法

### 5.1 静态分析

```bash
# Clang Thread Safety Analysis
clang++ -Wthread-safety ...
```

### 5.2 动态检测

```bash
# ThreadSanitizer (TSan) - Linux/macOS
clang++ -fsanitize=thread -g ...

# 运行测试
./test_parallel_execution
```

### 5.3 人工检查清单

执行前检查：
- [ ] 所有 Handle 子类的 `xOperate()` 只写 `*dests_`
- [ ] 所有 Item 子类的 `initOperands()` 只读上游数据
- [ ] 没有使用未保护的全局变量
- [ ] 没有使用 `static` 局部变量存储状态

## 6. 代码注释模板

在并发敏感的代码处添加注释：

```cpp
// THREAD-SAFETY: This method is called from ThreadPool workers.
// It only writes to this->dests_ and reads from upstream dests_ (completed nodes).
// Do NOT access other nodes' VarBag or unprotected global variables.
void SomeHandle::xOperate() {
    // ...
}

// THREAD-SAFETY: This data structure is protected by stateMutex_.
// Always acquire lock before read/write.
std::unordered_map<std::string, NodeStateInfo> nodeStates_;
```

## 7. 已知安全的操作

| 操作 | 原因 |
|------|------|
| 读取 `sources_` 中的数据 | 上游已完成，数据不可变 |
| 写入 `dests_` 中的数据 | 只有本节点写入，无竞争 |
| 调用 `ACQUIRE_NAMES()` | 只读操作 |
| 调用 `GET_MEMBER_WITH_TYPE_STR()` | 只读操作 |
| 发送 Qt 信号 | 信号/槽机制线程安全 |

## 8. 禁止的操作

| 操作 | 原因 |
|------|------|
| 在 `xOperate()` 中修改 `sources_` | 违反只读规则 |
| 在 `xOperate()` 中访问其他节点的 Handle | 可能未完成，导致数据竞争 |
| 在 `xOperate()` 中修改 `globalItemMap` | 全局变量无锁保护 |
| 在 `xOperate()` 中修改 `xGraph` | 全局变量无锁保护 |

## 9. 后续改进

- [ ] 为 Logger 添加线程安全保护
- [ ] 检查 TFDetectHandle 的 session 管理
- [ ] 添加编译时线程安全注解
- [ ] 在 CI 中集成 TSan 测试
