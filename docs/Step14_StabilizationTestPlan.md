# Step14: PR-4.5 稳定化测试计划

## 概述

本文档定义 PR-4/4.5 的稳定化测试计划。
**通过所有测试后，方可进入 PR-5.1（设备队列化）开发。**

---

## 状态语义定义（固定约定）

| 状态 | 含义 | 触发条件 |
|------|------|----------|
| **Pending** | 等待调度 | 初始状态 |
| **Ready** | 依赖满足，等待执行 | 入度 = 0 |
| **Running** | 正在执行 | 线程池分配执行 |
| **Completed** | 执行成功 | xOperate 返回成功 |
| **Failed** | 执行失败 | xOperate 抛异常或返回错误 |
| **Skipped** | **依赖失败被跳过** | 上游 Failed/Skipped |
| **Canceled** | **用户取消** | 用户点击 Cancel，尚未执行的节点 |

### 关键规则
- **依赖失败 → Skipped**（不是 Failed，不是 Canceled）
- **用户 Cancel → Canceled**（仅影响尚未执行的节点）
- **只有一个 Failed**（真实故障源），便于排错

---

## 总目标（M1 验收门槛）

| # | 验收项 | 要求 |
|---|--------|------|
| 1 | 可重复运行 | 连续 Run 3 次，结果一致，overlay 不残留 |
| 2 | 可取消 | Cancel 后不派发新节点，UI 解锁，心跳停止 |
| 3 | 可 fail-fast | 失败节点 → Failed，下游 → Skipped，UI 解锁 |
| 4 | UI 不假死 | 长节点运行期间，心跳/流水灯持续刷新 |

---

## 测试矩阵

### 1. Build 验证

| 测试项 | 操作 | 预期结果 | 通过 |
|--------|------|----------|------|
| Debug x64 | MSBuild Debug | 0 error | ☐ |
| Release x64 | MSBuild Release | 0 error | ☐ |
| 空闲退出 | 启动→加载→关闭 | 无崩溃 | ☐ |
| 运行中退出 | 启动→Run→立即关闭 | 无崩溃（cancel+wait） | ☐ |

---

### 2. 连续运行测试（Run×3）

**测试图**：LoadImage → ImagePre → TFDetect → RevertBox → DrawBox

| 测试项 | 操作 | 预期结果 | 通过 |
|--------|------|----------|------|
| Run #1 | 加载 solutions.json → Run | 5/5 Completed，UI 解锁 | ☐ |
| Run #2 | 立即再 Run | 同上 | ☐ |
| Run #3 | 立即再 Run | 同上 | ☐ |
| Overlay 检查 | 三次间观察 | 无残留（每次从 Pending 开始） | ☐ |

**关键日志验证**：
```
JobStarted → NodeStarted×5 → NodeFinished×5 → JobFinished(Success)
```

---

### 3. Cancel 测试

**测试场景**：TFDetect 运行期间 Cancel

| 测试项 | 操作 | 预期结果 | 通过 |
|--------|------|----------|------|
| Cancel 早期 | Run 后 1s Cancel | UI 解锁，已派发节点跑完，其余 Canceled | ☐ |
| Cancel 中期 | Run 后 3s Cancel | 同上 | ☐ |
| Cancel 晚期 | Run 后 5s Cancel | 同上 | ☐ |
| 心跳停止 | Cancel 后观察 | 不再有 NodeHeartbeat | ☐ |
| 无新节点 | Cancel 后观察 | 不再有 NodeStarted | ☐ |

**关键日志验证**：
```
JobStarted → NodeStarted(1~N) → [Cancel] → NodeFinished(Running节点) → JobFinished(Canceled)
```

**预期状态分布**：
- 已完成节点：Completed
- 正在运行节点：Completed 或 Canceled（取决于是否支持中断）
- 未执行节点：Canceled

---

### 4. Fail-fast 测试

**故障注入方式**（不改代码）：
- 修改 solutions.json 中 LoadImage 的 imagePath 为不存在的文件
- 或修改 TFDetect 的 savedModelPath 为不存在的目录

| 测试项 | 操作 | 预期结果 | 通过 |
|--------|------|----------|------|
| LoadImage 失败 | imagePath 指向不存在文件 → Run | LoadImage=Failed，下游=Skipped | ☐ |
| UI 解锁 | 观察 | Run 按钮可用 | ☐ |
| 错误提示 | 观察 | 日志有错误信息 | ☐ |

**关键日志验证**：
```
JobStarted → NodeStarted(LoadImage) → NodeFinished(Failed) → NodeSkipped×4 → JobFinished(Failed)
```

**预期状态分布**：
- LoadImage：**Failed**（红色边框 + ✗）
- ImagePre/TFDetect/RevertBox/DrawBox：**Skipped**（灰色虚线 + —）

---

### 5. UI 响应测试（长节点心跳）

**测试场景**：TFDetect 执行 7+ 秒

| 测试项 | 操作 | 预期结果 | 通过 |
|--------|------|----------|------|
| 心跳日志 | 观察日志 | 每秒有 NodeHeartbeat | ☐ |
| 流水灯动画 | 观察 TFDetect 节点 | 橙色亮点持续移动 | ☐ |
| UI 响应 | 拖动窗口/点击菜单 | 无卡顿 | ☐ |

---

## 签核表

| 测试类别 | 测试项数 | 通过数 | 验收人 | 日期 |
|----------|----------|--------|--------|------|
| Build 验证 | 4 | _ | | |
| 连续运行 | 4 | _ | | |
| Cancel | 5 | _ | | |
| Fail-fast | 3 | _ | | |
| UI 响应 | 3 | _ | | |
| **合计** | **19** | _ | | |

**验收标准**：19/19 通过后，进入 PR-5.1 开发。

---

## 已知问题记录

| # | 问题描述 | 复现步骤 | 状态 | 修复版本 |
|---|----------|----------|------|----------|
| | | | | |

---

## 下一步

通过 M1 后，进入 **PR-5.1 设备队列化（占位版本）**：
- `DeviceType` 枚举（CPU/GPU/NPU）
- `DeviceQueue` 接口
- CPUQueue = ThreadPool
- GPU/NPUQueue = SerialQueue（单 worker）
- 节点先全标 CPU，跑通路由形状
