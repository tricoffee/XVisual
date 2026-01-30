#ifndef XVISUAL_CORE_EXECUTOR_INODE_H
#define XVISUAL_CORE_EXECUTOR_INODE_H

#include <stop_token>

namespace XVisual {

/**
 * Core execution node interface (no Qt dependency).
 *
 * Cancel granularity: node-boundary by default.
 * The stop_token is provided for optional cooperative cancellation inside node,
 * but nodes are not required to check it (black-box inference safety).
 */
class INode
{
public:
	virtual ~INode() = default;

	// Execute the node (includes initOperands internally if needed).
	virtual void execute(std::stop_token st) = 0;
};

} // namespace XVisual

#endif // XVISUAL_CORE_EXECUTOR_INODE_H
