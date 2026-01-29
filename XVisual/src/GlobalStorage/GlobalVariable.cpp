#include "GlobalStorage/GlobalVariable.h"

namespace XVisual {
	std::vector<std::string> nodeIdTopoSequence;
	std::string globalWorkSpaceDir;
	std::atomic_bool g_graphExecuting{ false };
	std::stop_source g_graphStopSource{};
}
