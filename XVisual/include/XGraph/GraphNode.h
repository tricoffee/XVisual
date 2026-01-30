#ifndef GraphNode_H
#define GraphNode_H
#include<string>
#include<vector>
#include<memory>

namespace XVisual {

struct GraphNode
{
	std::string nodeId;
	std::string className;                                   // PR-3: node class name (e.g. "LoadImage")
	std::vector<std::shared_ptr<GraphNode>> neighbors;
	GraphNode(const std::string& xNodeId, const std::string& xClassName = {})
		: nodeId(xNodeId), className(xClassName) {}
};

} // namespace XVisual

#endif //GraphNode_H
