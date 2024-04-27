// GlobalStorage.cpp
#include "GlobalStorage/GlobalStorage.h"

std::unordered_map<std::string, XBaseItem*> globalItemMap;
std::mutex itemMapMutex;
std::unordered_map<std::string, XBaseHandle*> globalHandleMap;
std::mutex handleMapMutex;
std::vector<std::shared_ptr<GraphNode>> xGraph;
std::mutex xGraphMutex;
std::vector<std::shared_ptr<GraphNode>> xHandleGraph;
std::mutex xHandleGraphMutex;


// 全局的 ItemMap 和 HandleMap
GlobalMap<XBaseItem> GglobalItemMap;
GlobalMap<XBaseHandle> GglobalHandleMap;


//unused
//std::unordered_map<XBaseItem*, XBaseHandle*> itemHandleUMap;
//std::mutex itemHandleUMapMutex;
//std::unordered_map<std::string, std::string> itemHandleIdMap;
//std::mutex itemHandleIdMapMutex;
