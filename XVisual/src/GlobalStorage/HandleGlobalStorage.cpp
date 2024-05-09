// HandleGlobalStorage.cpp
#include "GlobalStorage/HandleGlobalStorage.h"

std::unordered_map<std::string, XBaseHandle*> globalHandleMap;
std::mutex handleMapMutex;

std::vector<std::shared_ptr<GraphNode>> xHandleGraph;
std::mutex xHandleGraphMutex;