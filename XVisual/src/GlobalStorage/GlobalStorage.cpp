// GlobalStorage.cpp
#include "GlobalStorage/GlobalStorage.h"

namespace XVisual {

std::unordered_map<std::string, XBaseItem*> globalItemMap;
std::mutex itemMapMutex;

std::vector<std::shared_ptr<GraphNode>> xGraph;
std::mutex xGraphMutex;

} // namespace XVisual
