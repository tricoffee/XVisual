#ifndef XGraph_H
#define XGraph_H

#include <vector>
#include <string>
#include <memory>
#include "XGraph/GraphNode.h"

class XGraph
{
	public:
		static std::vector<std::string> BFSWithTopologicalSort(std::vector<std::shared_ptr<GraphNode>>& graph);
		static int findNodeIndex(const std::vector<std::shared_ptr<GraphNode>>& graph, const std::string& nodeId);
};

#endif // XGraph_H