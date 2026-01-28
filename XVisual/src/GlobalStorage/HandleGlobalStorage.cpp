// HandleGlobalStorage.cpp
#include "GlobalStorage/HandleGlobalStorage.h"

namespace XVisual{
	std::unordered_map<std::string, XBaseHandle*> globalHandleMap;
	std::mutex handleMapMutex;

	std::vector<std::shared_ptr<GraphNode>> xHandleGraph;
	std::mutex xHandleGraphMutex;
} //namespace XVisual