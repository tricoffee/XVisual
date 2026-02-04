#ifndef XVISUAL_CORE_EXECUTOR_INODE_H
#define XVISUAL_CORE_EXECUTOR_INODE_H

#include <stop_token>
#include "Core/Device/DeviceType.h"

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

	/**
	 * Returns the preferred device type for this node.
	 * 
	 * Default returns Any, meaning the scheduler decides (PR-5.1: mapped to CPU).
	 * Nodes can override to indicate device preference (e.g., GPU for inference).
	 * 
	 * PR-5.1 behavior:
	 * - Any -> CPU (fixed mapping in scheduler)
	 * - GPU/NPU -> route to SerialQueue (placeholder, still executes on CPU)
	 */
	virtual DeviceType preferredDevice() const { return DeviceType::Any; }
};

} // namespace XVisual

#endif // XVISUAL_CORE_EXECUTOR_INODE_H
