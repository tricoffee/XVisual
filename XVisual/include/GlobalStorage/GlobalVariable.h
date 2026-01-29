#ifndef GlobalVariable_H
#define GlobalVariable_H

#include <iostream>
#include <vector>
#include <string>
// PR-1 async run/cancel
#include <atomic>

// C++20 stop_token / jthread support
#include <stop_token>

namespace XVisual {

//一个全局的nodeIdTopoSequence用来存储nodeId的拓扑顺序
extern std::vector<std::string> nodeIdTopoSequence;
//一个维护WorkSpace的全局变量
extern std::string globalWorkSpaceDir;

// =========================
// PR-1: Async serial execution + cancel at node boundary
// =========================
// If true, UI should be locked (no editing) and execution is considered running.
extern std::atomic_bool g_graphExecuting;

// Per-run cancel source. Executor checks stop_token at node boundary.
extern std::stop_source g_graphStopSource;

} // namespace XVisual

#endif // GlobalVariable_H
