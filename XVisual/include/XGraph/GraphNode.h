#ifndef GraphNode_H
#define GraphNode_H
#include<string>
#include<vector>
#include<memory>

namespace XVisual {

struct GraphNode
{
	std::string nodeId;
	std::vector<std::shared_ptr<GraphNode>> neighbors;
	GraphNode(const std::string& xNodeId) : nodeId(xNodeId) {}
};

} // namespace XVisual

#endif //GraphNode_H
