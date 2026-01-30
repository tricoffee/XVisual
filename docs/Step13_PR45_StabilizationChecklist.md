# Step13: PR-4.5 稳定化测试清单

## 概述

本文档定义 PR-4.5（节点状态 UI 可视化 & 心跳事件）的稳定性验收标准。
在进入 PR-5（设备队列化）之前，必须通过以下所有测试用例。

---

## 里程碑 M1：PR-4.5 稳定性验收

### 1. 构建验证

| 配置 | 平台 | 预期结果 |
|------|------|----------|
| Debug | x64 | ✅ 编译成功，0 error |
| Release | x64 | ✅ 编译成功，0 error |

**检查点：**
- 新增文件 `NodeStateOverlay.h/.cpp` 正确加入项目
- Qt MOC 正确处理 `NodeStateOverlay`（因为继承 `QGraphicsObject`）
- 所有 include 路径正确

---

### 2. 连续运行测试（Run 3 次）

**测试步骤：**
1. 启动 XVisual
2. 加载 `solutions.json`（包含 LoadImage → ImagePre → TFDetect 流程）
3. 点击 Run 按钮
4. 等待执行完成
5. 重复步骤 3-4 共 3 次

**预期行为：**

| 阶段 | 预期日志关键字 | UI 状态 |
|------|----------------|---------|
| 第1次 Run | `Job started`, `Job finished: result=Success` | 所有节点绿色边框 |
| 第2次 Run | 同上 | 同上（状态正确重置） |
| 第3次 Run | 同上 | 同上 |

**失败模式检测：**
- ❌ 第2/3次运行时 overlay 状态残留（节点保持绿色/红色）
- ❌ UI 卡死或无响应
- ❌ 日志出现 `JobFinished` 两次或缺失

---

### 3. Cancel 取消测试

**测试步骤：**
1. 加载包含 TFDetect 的流程（耗时 > 3 秒）
2. 点击 Run 按钮
3. 在 TFDetect 执行期间点击 Cancel 按钮
4. 观察日志和 UI

**预期行为：**

| 检查项 | 预期结果 |
|--------|----------|
| 日志 | `Job finished: result=Canceled` |
| 心跳停止 | Cancel 后不再有 `Node heartbeat` 日志 |
| UI 解锁 | Run 按钮可用，Cancel 按钮禁用 |
| 后续节点 | 未执行的节点状态为 `Skipped` 或 `Canceled` |

**失败模式检测：**
- ❌ Cancel 后仍有心跳事件发出
- ❌ Cancel 后 UI 未解锁
- ❌ 下游节点仍被执行

---

### 4. Fail-fast 错误传播测试

**测试步骤：**
1. 配置 LoadImage 节点使用不存在的图片路径
2. 点击 Run 按钮
3. 观察日志和 UI

**预期行为：**

| 检查项 | 预期结果 |
|--------|----------|
| LoadImage 状态 | 红色边框 + ✗ 角标 |
| 下游节点状态 | 灰色虚线边框（Skipped） |
| 日志 | `Node finished: state=Failed`, `Node skipped` |
| UI | 自动解锁，弹出错误对话框 |

**失败模式检测：**
- ❌ 错误未传播，下游节点仍被执行
- ❌ UI 未解锁
- ❌ 没有错误对话框

---

### 5. 并行执行测试

**测试图构造：**
```
LoadImage_A ─┬─→ ImagePre_A ─┬─→ DrawBox
             │               │
LoadImage_B ─┴─→ ImagePre_B ─┘
```

**测试步骤：**
1. 构建上述两个独立分支的流程
2. 点击 Run
3. 观察日志中的线程 ID 和时间戳

**预期行为：**
- `LoadImage_A` 和 `LoadImage_B` 几乎同时开始（时间差 < 100ms）
- `ImagePre_A` 和 `ImagePre_B` 几乎同时开始
- 日志显示不同的线程 ID

**日志示例：**
```
[Thread-1234] Node started: LoadImage_A
[Thread-1235] Node started: LoadImage_B
[Thread-1234] Node finished: LoadImage_A, duration: 50ms
[Thread-1235] Node finished: LoadImage_B, duration: 52ms
```

---

### 6. 心跳事件测试

**测试步骤：**
1. 确保 TFDetect 节点执行时间 > 3 秒
2. 点击 Run
3. 观察日志中的心跳事件

**预期行为：**

| 检查项 | 预期结果 |
|--------|----------|
| 心跳间隔 | 约 1 秒一次 |
| 心跳内容 | `Node heartbeat: TFDetect running... Xs` |
| 心跳停止 | 节点完成后立即停止 |

**性能检查：**
- 心跳期间其他节点调度不受阻塞
- 日志无明显延迟（< 50ms）

---

### 7. Overlay 视觉测试

**测试步骤：**
1. 运行流程
2. 观察每个节点的边框和角标变化

**预期视觉效果：**

| 状态 | 边框 | 角标 | 动画 |
|------|------|------|------|
| Pending | 灰色实线 | 无 | 无 |
| Ready | 蓝色实线 | 无 | 无 |
| Running | 橙色实线 | 无 | 脉冲闪烁 |
| Completed | 绿色实线 | ✓ | 无 |
| Failed | 红色实线 | ✗ | 无 |
| Skipped | 灰色虚线 | — | 无 |

**检查点：**
- [ ] Running 脉冲动画流畅（约 20fps）
- [ ] 角标位置正确（右上角）
- [ ] 边框不与节点选中状态冲突
- [ ] 节点移动时 overlay 跟随

---

## 常见失败模式与定位提示

### 问题 1：第二次运行时状态残留

**症状：** 节点保持上次运行的颜色
**原因：** `clearAllNodeStates()` 未被调用或 overlay 未正确删除
**定位：**
```cpp
// 检查 JobStarted 事件处理
case XVisual::EventType::JobStarted:
    diagramScene->clearAllNodeStates();  // 确保这行执行
```

### 问题 2：Cancel 后心跳继续

**症状：** Cancel 后日志仍有 `Node heartbeat`
**原因：** `stopHeartbeatThread()` 未在 cancel 路径调用
**定位：**
```cpp
// 检查 runParallel 中的 cancel 分支
// heartbeatThread_->request_stop() 必须在 waitAll() 之前或同时
```

### 问题 3：UI 死锁

**症状：** UI 完全无响应
**原因：** 可能在 UI 线程等待锁，而 worker 线程持锁等待 UI
**定位：**
- 检查 `emitEvent` 是否在锁内调用
- 检查 `QtEventBridge` 是否正确使用 `Qt::QueuedConnection`

### 问题 4：Overlay 闪烁或消失

**症状：** 边框时有时无
**原因：** `boundingRect()` 返回空或 `zValue` 设置不当
**定位：**
```cpp
// 检查 NodeStateOverlay::boundingRect()
if (!targetItem_)
    return QRectF();  // 确保 targetItem_ 有效
```

---

## 验收签核

| 测试项 | 通过 | 备注 |
|--------|------|------|
| 1. 构建验证 (Debug) | ☐ | |
| 1. 构建验证 (Release) | ☐ | |
| 2. 连续运行测试 (x3) | ☐ | |
| 3. Cancel 取消测试 | ☐ | |
| 4. Fail-fast 错误传播 | ☐ | |
| 5. 并行执行测试 | ☐ | |
| 6. 心跳事件测试 | ☐ | |
| 7. Overlay 视觉测试 | ☐ | |

**验收标准：** 所有 8 项测试通过后，方可进入 PR-5 开发。

---

## 下一步

通过 M1 后，进入 **里程碑 M2：PR-5.1 设备队列化占位**：
- `DeviceType` 枚举定义
- `DeviceQueue` 接口设计
- CPUQueue = ThreadPool, GPU/NPUQueue = SerialQueue
- 节点设备亲和性声明
