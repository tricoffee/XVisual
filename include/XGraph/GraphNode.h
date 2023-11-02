#ifndef GraphNode_H
#define GraphNode_H
#include<string>
#include<vector>
#include<memory>
struct GraphNode
{
	std::string label;
	std::vector<std::shared_ptr<GraphNode>> neighbors;
	GraphNode(const std::string& x) : label(x) {}
};
#endif //GraphNode_H