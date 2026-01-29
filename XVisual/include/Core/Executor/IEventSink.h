#ifndef XVISUAL_CORE_EXECUTOR_IEVENTSINK_H
#define XVISUAL_CORE_EXECUTOR_IEVENTSINK_H

#include <cstdint>
#include <string>

namespace XVisual {

enum class EventType
{
	JobStarted,
	NodeStarted,
	NodeFinished,
	JobFinished,
	Log
};

struct NodeEvent
{
	EventType type = EventType::Log;

	// identifiers
	std::string jobId;
	std::string graphId;
	std::string nodeId;

	// status / payload
	int code = 0; // Typically XVisual::ErrorCode cast to int
	std::string message;

	// timing
	std::uint64_t tsUs = 0;
	std::uint64_t durationUs = 0;
};

class IEventSink
{
public:
	virtual ~IEventSink() = default;
	virtual void onEvent(const NodeEvent& e) = 0;
};

} // namespace XVisual

#endif // XVISUAL_CORE_EXECUTOR_IEVENTSINK_H
