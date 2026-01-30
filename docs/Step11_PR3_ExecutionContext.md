# Step 11 — PR-3: ExecutionContext + 去全局 map + 节点实例私有化

> 状态: **草案（待审阅 & 实施）**  
> 前置: PR-2 已完成 (GraphExecutor / INode / IEventSink / QtEventBridge)

---

## 1. PR-3 目标

| # | 目标 | 说明 |
|---|------|------|
| 1 | **GraphExecutor 不再依赖全局 map** | `start()` 不再访问 `globalItemMap` / `globalHandleMap` |
| 2 | **per-run ExecutionContext** | 每次 `start()` 构建独立上下文，支持后续多实例并行（PR-4） |
| 3 | **sources/dests 私有化** | 拆掉 singleton：`XBaseHandle::sources_` / `dests_` 改为 per-instance |

---

## 2. 类图 (简化)

```
┌─────────────────────────────────────────────────────────────────────────┐
│                              Core (无 Qt)                               │
├──────────────────────────────────┬──────────────────────────────────────┤
│ Core/Runtime/                    │ Core/Executor/                       │
│  ├─ Value.h   (using Value=any)  │  ├─ INode.h         (PR-2 已有)      │
│  ├─ VarBag.h  (per-inst 容器)    │  ├─ IEventSink.h    (PR-2 已有)      │
│  └─ ExecutionContext.h           │  ├─ GraphExecutor.h (PR-2→3 升级)    │
│                                  │  └─ INodeFactory.h  (NEW)            │
└──────────────────────────────────┴──────────────────────────────────────┘

                              ┌───────────────────────────┐
                              │ Adapter/Qt (仅 UI bridge) │
                              │  └─ QtEventBridge.h       │
                              └───────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│                     Legacy (待改造但保持兼容)                           │
│  XVariable/Source.h  ─┬─> 仅保留静态帮助函数或删除 singleton            │
│  XVariable/Dest.h     │                                                 │
│  XVariable/Param.h    │  接口迁移到 VarBag；旧宏可继续使用              │
│  HandleBase/XBaseHandle.h                                               │
│    sources_ : unique_ptr<VarBag>   (替换 Source&)                       │
│    dests_   : unique_ptr<VarBag>   (替换 Dest&)                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 3. 关键接口

### 3.1 VarBag (替代 Param singleton)

```cpp
// Core/Runtime/VarBag.h
#pragma once
#include <any>
#include <string>
#include <unordered_map>
#include <vector>
#include "XVariable/SourceFrom.h"

namespace XVisual {

class VarBag
{
public:
    template<typename T>
    void registerMember(const std::string& name, const T& value, bool fromOutside = false);

    template<typename T>
    T& getMemberWithT(const std::string& name);

    std::any& getMember(const std::string& name);
    std::vector<std::string> acquireNames() const;

    void setSourceFrom(const std::string& name, const SourceFrom& sf);
    SourceFrom& getSourceFrom(const std::string& name);
    bool isFromOutside(const std::string& name) const;

private:
    std::unordered_map<std::string, std::any> members_;
    std::unordered_map<std::string, bool> fromOutside_;
    std::unordered_map<std::string, SourceFrom> sourceMap_;
};

} // namespace XVisual
```

> 实现放 `Core/Runtime/VarBag.cpp`；接口 100% 兼容旧 `Param`，旧宏无需改动。

### 3.2 ExecutionContext

```cpp
// Core/Runtime/ExecutionContext.h
#pragma once
#include <memory>
#include <stop_token>
#include <string>
#include <unordered_map>
#include "Core/Executor/INode.h"

namespace XVisual {

struct ExecutionContext
{
    std::stop_token st;
    std::unordered_map<std::string, std::unique_ptr<INode>> nodes;
    // PR-5 可选: TensorStore store;
};

} // namespace XVisual
```

### 3.3 INodeFactory

```cpp
// Core/Executor/INodeFactory.h
#pragma once
#include <memory>
#include <string>
#include "Core/Executor/INode.h"

namespace XVisual {

struct NodeConfig
{
    std::string nodeId;
    std::string className;
    // innerParam, outerParam 可按需扩展
};

class INodeFactory
{
public:
    virtual ~INodeFactory() = default;
    virtual std::unique_ptr<INode> createNode(const NodeConfig& cfg) = 0;
};

} // namespace XVisual
```

### 3.4 GraphExecutor::start() 签名变更

```cpp
// PR-2 签名 (保留兼容)
bool start(std::string graphId,
           std::vector<std::shared_ptr<GraphNode>> graphSnapshot,
           NodeResolver resolver,           // callback
           IEventSink* sink,
           FinishedCallback onFinished = {},
           Options opt = Options{});

// PR-3 新增重载 (推荐)
bool start(std::string graphId,
           std::vector<std::shared_ptr<GraphNode>> graphSnapshot,
           INodeFactory& factory,           // NEW
           IEventSink* sink,
           FinishedCallback onFinished = {},
           Options opt = Options{});
```

> 执行流程:
> 1. `start()` 内部遍历 `graphSnapshot`，调用 `factory.createNode(...)` 构建所有节点 → 存入 `ExecutionContext::nodes`
> 2. 串行 Kahn BFS 执行（PR-2 逻辑不变），每节点 `initOperands()` + `run(st)`
> 3. job 结束后 `ExecutionContext` 析构，所有节点随之释放

---

## 4. XBaseHandle 改造

### 4.1 头文件改动 (`include/HandleBase/XBaseHandle.h`)

```diff
- #include "XVariable/Source.h"
- #include "XVariable/Dest.h"
+ #include "Core/Runtime/VarBag.h"
  ...
  protected:
-     Source sources;
-     Dest dests;
+     std::unique_ptr<VarBag> sources_;
+     std::unique_ptr<VarBag> dests_;
```

### 4.2 构造函数改动 (`src/HandleBase/XBaseHandle.cpp`)

```diff
- XBaseHandle::XBaseHandle()
-     : Colleague()
-     , sources(Source::getInstance())
-     , dests(Dest::getInstance())
+ XBaseHandle::XBaseHandle()
+     : Colleague()
+     , sources_(std::make_unique<VarBag>())
+     , dests_(std::make_unique<VarBag>())
```

### 4.3 getter 改动

```diff
- Source& XBaseHandle::getSources() { return sources; }
- Dest&   XBaseHandle::getDests()   { return dests;   }
+ VarBag& XBaseHandle::getSources() { return *sources_; }
+ VarBag& XBaseHandle::getDests()   { return *dests_;   }
```

### 4.4 宏体系兼容

旧宏 `REGISTER_MEMBER_STR(sources, xName, val)` 展开为 `sources.registerMember(xName, val)`。  
改造后 `sources` 变为 `*sources_`，展开为 `sources_->registerMember(xName, val)`，调用链一致；只需在宏定义里把 `.` 改成 `->` 或直接把成员变量名改为 `sources` 指针别名即可。  
推荐做法：保持成员名 `sources_`，定义 inline `VarBag& sources() { return *sources_; }`，宏继续用 `sources()` 函数访问。

---

## 5. MainWindow 瘦身

| 删除 | 新增 |
|------|------|
| `runButtonClicked` 里构造 `ItemNodeAdapter` + `adapters` map | 改为注册一个 `HandleNodeFactory : INodeFactory`（复用 `HandleFactory::createObject`） |
| `resolver` lambda | 删除（factory 替代） |

示例:
```cpp
void MainWindow::runButtonClicked(bool checked)
{
    ...
    // PR-3: 使用 factory 而非 resolver
    HandleNodeFactory factory;  // 实现 INodeFactory
    m_graphExecutor.start("XGraph", graphSnapshot, factory, bridge.get(), onFinished, opts);
}
```

---

## 6. 文件变更清单

| 操作 | 文件 |
|------|------|
| 新增 | `include/Core/Runtime/Value.h` |
| 新增 | `include/Core/Runtime/VarBag.h` |
| 新增 | `src/Core/Runtime/VarBag.cpp` |
| 新增 | `include/Core/Runtime/ExecutionContext.h` |
| 新增 | `include/Core/Executor/INodeFactory.h` |
| 修改 | `include/HandleBase/XBaseHandle.h` |
| 修改 | `src/HandleBase/XBaseHandle.cpp` |
| 修改 | `include/Core/Executor/GraphExecutor.h` (新增 start 重载) |
| 修改 | `src/Core/Executor/GraphExecutor.cpp` (实现新 start) |
| 修改 | `src/MainWindow/mainwindow.cpp` (使用 factory 替代 resolver) |
| 修改 | `XVisual.vcxproj` (添加新文件) |

---

## 7. 验收用例

| # | 用例 | 预期 |
|---|------|------|
| 1 | 单次运行 LoadImage → ImagePre → TFDetect | 正常输出，无崩溃 |
| 2 | 连续点击 Run 两次（第一次完成后再点） | 两次输出独立，不残留上一次 dests 数据 |
| 3 | Run 执行中 Cancel，再次 Run | 取消生效，第二次 run 正常 |
| 4 | 验证 `globalItemMap` / `globalHandleMap` 仅用于 UI 展示，执行路径不访问 | grep / 静态分析 |

---

## 8. 风险 & 待讨论

1. **宏体系**：若子节点 Handle 直接写 `sources.xxx`，需批量替换为 `sources().xxx` 或 `sources_->xxx`。
2. **TFDetect 模型缓存**：PR-3 保证"每 run 不互相污染"，模型 per-run 重建；PR-5 再做 ModelCache（按 modelPath + device 缓存）。
3. **NodeFactory 放置位置**：建议 PR-3 结束前移到 Core；短期可先放 MainWindow 侧。

---

## 9. 下一阶段（PR-4）

PR-3 完成后，`ExecutionContext` + per-instance `VarBag` 可支撑 PR-4 的**并行 Kahn 调度**：
- 每个节点有私有 `sources_`/`dests_`，无竞争
- Scheduler 只需把 ready 节点提交到 ThreadPool，无需加锁全局 map

---

**审阅后若无异议，可开始实施本 PR-3 改造。**
