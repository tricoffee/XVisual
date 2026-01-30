#ifndef XVISUAL_CORE_EXECUTOR_INODEFACTORY_H
#define XVISUAL_CORE_EXECUTOR_INODEFACTORY_H

#include <memory>
#include <string>

#include "Core/Executor/INode.h"

namespace XVisual {

/**
 * NodeConfig: 传递给工厂的节点描述信息
 */
struct NodeConfig
{
	std::string nodeId;
	std::string className;
	// innerParam / outerParam 可按需扩展
};

/**
 * INodeFactory: 抽象工厂接口，由调用侧实现以构造具体节点。
 *
 * 典型实现:
 *   - HandleNodeFactory: 复用现有 HandleFactory::createObject()
 */
class INodeFactory
{
public:
	virtual ~INodeFactory() = default;

	/**
	 * 根据配置创建节点实例
	 * @return unique_ptr<INode>，失败时返回 nullptr
	 */
	virtual std::unique_ptr<INode> createNode(const NodeConfig& cfg) = 0;
};

} // namespace XVisual

#endif // XVISUAL_CORE_EXECUTOR_INODEFACTORY_H
