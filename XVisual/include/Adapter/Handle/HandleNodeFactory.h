#ifndef XVISUAL_ADAPTER_HANDLE_HANDLENODEFACTORY_H
#define XVISUAL_ADAPTER_HANDLE_HANDLENODEFACTORY_H

#include "Core/Executor/INodeFactory.h"
#include "HandleBase/HandleFactory.h"
#include "HandleBase/XBaseHandle.h"
#include <memory>

namespace XVisual {

/**
 * HandleNodeFactory: 适配器，将现有 HandleFactory::createObject 转为 INodeFactory 接口
 */
class HandleNodeFactory : public INodeFactory
{
public:
	HandleNodeFactory() = default;
	~HandleNodeFactory() override = default;

	std::unique_ptr<INode> createNode(const NodeConfig& cfg) override
	{
		// 用 HandleRegistry 创建裸指针，再包装成 unique_ptr
		XBaseHandle* raw = HandleRegistry::createObject(cfg.className + "Handle");
		if (!raw)
			return nullptr;
		// XBaseHandle already inherits INode
		return std::unique_ptr<INode>(raw);
	}
};

} // namespace XVisual

#endif // XVISUAL_ADAPTER_HANDLE_HANDLENODEFACTORY_H
