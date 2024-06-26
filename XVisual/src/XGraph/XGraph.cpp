#include "XGraph/XGraph.h"

/*
广度优先遍历按照拓扑排序的顺序访问节点
1、计算每个节点的入度；
2、将入度为0的节点加入队列q；
3、开始遍历队列，执行以下步骤：
    a. 出队列一个节点(即入度为0的节点)；
    b. 将这个节点添加到结果中；
    c. 遍历所有以当前节点为起点的边，减少被指向节点的入度；
    d. 如果减少后某个节点的入度变为0，将该节点加入队列q中。
4、重复步骤3直到队列q为空。
*/
std::vector<std::string> XGraph::BFSWithTopologicalSort(std::vector<std::shared_ptr<GraphNode>>& graph)
{
    std::vector<std::string> result;
    std::unordered_map<std::string, int> inDegrees;
    std::queue<std::shared_ptr<GraphNode>> q; // 注意这里的队列类型改为 std::shared_ptr<GraphNode>

    // 初始化入度为0并计算每个节点的入度
    for (const auto& nodePtr : graph)
    {
        for (const auto& neighbor : nodePtr->neighbors)
        {
            if (inDegrees.find(neighbor->nodeId) == inDegrees.end())
            {
                inDegrees[neighbor->nodeId] = 0;
            }
            inDegrees[neighbor->nodeId]++;
        }
    }

    // 将入度为0的节点加入队列q
    for (const auto& nodePtr : graph)
    {
        if (inDegrees[nodePtr->nodeId] == 0)
        {
            q.push(nodePtr);
        }
    }

    // 执行广度优先遍历
    while (!q.empty())
    {
        std::shared_ptr<GraphNode> node = q.front();
        q.pop();

        // 访问节点
        result.push_back(node->nodeId);

        /*
         遍历所有以当前节点为起点的边，减少被指向节点的入度；
         如果减少后某个节点的入度变为0，将该节点加入队列q中。
         */
        for (const auto& neighbor : node->neighbors)
        {
            inDegrees[neighbor->nodeId]--;
            if (inDegrees[neighbor->nodeId] == 0)
            {
                q.push(std::shared_ptr<GraphNode>(neighbor));

            }
        }
    }

    return result;
}


// 查找具有特定 ID 的节点在 vector 中的索引
int XGraph::findNodeIndex(const std::vector<std::shared_ptr<GraphNode>>& graph, const std::string& nodeId)
{
	for (int i = 0; i < graph.size(); i++)
	{
		if (graph[i]->nodeId == nodeId)
		{
            XLOG_INFO("XGraph::findNodeIndex, nodeId = " + nodeId + ", index = " + std::to_string(i), CURRENT_THREAD_ID);
            return i;
		}
	}
    XLOG_INFO("XGraph::findNodeIndex, nodeId = " + nodeId + ", index = " + std::to_string(-1), CURRENT_THREAD_ID);
	return -1; // 如果未找到，则返回 -1
}