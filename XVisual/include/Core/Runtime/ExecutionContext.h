#ifndef XVISUAL_CORE_RUNTIME_EXECUTIONCONTEXT_H
#define XVISUAL_CORE_RUNTIME_EXECUTIONCONTEXT_H

#include <memory>
#include <stop_token>
#include <string>
#include <unordered_map>

#include "Core/Executor/INode.h"

namespace XVisual {

/**
 * ExecutionContext: 每次 GraphExecutor::start() 创建的运行期上下文。
 *
 * PR-3 作用:
 * - 持有所有节点实例（unique_ptr），job 结束后自动释放
 * - 持有 stop_token 供取消检查
 * - PR-5 可扩展 TensorStore
 */
struct ExecutionContext
{
	std::stop_token st;
	std::unordered_map<std::string, std::unique_ptr<INode>> nodes;

	// 后续可添加:
	// TensorStore store;
};

} // namespace XVisual

#endif // XVISUAL_CORE_RUNTIME_EXECUTIONCONTEXT_H
