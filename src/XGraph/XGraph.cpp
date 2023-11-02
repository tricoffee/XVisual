#include "XGraph/XGraph.h"
#include <unordered_map>
#include <queue>

/*
������ȱ����������������˳����ʽڵ�
1������ÿ���ڵ����ȣ�
2�������Ϊ0�Ľڵ�������q��
3����ʼ�������У�ִ�����²��裺
    a. ������һ���ڵ�(�����Ϊ0�Ľڵ�)��
    b. ������ڵ���ӵ�����У�
    c. ���������Ե�ǰ�ڵ�Ϊ���ıߣ����ٱ�ָ��ڵ����ȣ�
    d. ������ٺ�ĳ���ڵ����ȱ�Ϊ0�����ýڵ�������q�С�
4���ظ�����3ֱ������qΪ�ա�
*/
std::vector<std::string> XGraph::BFSWithTopologicalSort(std::vector<std::shared_ptr<GraphNode>>& graph)
{
    std::vector<std::string> result;
    std::unordered_map<std::string, int> inDegrees;
    std::queue<std::shared_ptr<GraphNode>> q; // ע������Ķ������͸�Ϊ std::shared_ptr<GraphNode>

    // ��ʼ�����Ϊ0������ÿ���ڵ�����
    for (const auto& nodePtr : graph)
    {
        for (const auto& neighbor : nodePtr->neighbors)
        {
            if (inDegrees.find(neighbor->label) == inDegrees.end())
            {
                inDegrees[neighbor->label] = 0;
            }
            inDegrees[neighbor->label]++;
        }
    }

    // �����Ϊ0�Ľڵ�������q
    for (const auto& nodePtr : graph)
    {
        if (inDegrees[nodePtr->label] == 0)
        {
            q.push(nodePtr);
        }
    }

    // ִ�й�����ȱ���
    while (!q.empty())
    {
        std::shared_ptr<GraphNode> node = q.front();
        q.pop();

        // ���ʽڵ�
        result.push_back(node->label);

        /*
         ���������Ե�ǰ�ڵ�Ϊ���ıߣ����ٱ�ָ��ڵ����ȣ�
         ������ٺ�ĳ���ڵ����ȱ�Ϊ0�����ýڵ�������q�С�
         */
        for (const auto& neighbor : node->neighbors)
        {
            inDegrees[neighbor->label]--;
            if (inDegrees[neighbor->label] == 0)
            {
                q.push(std::shared_ptr<GraphNode>(neighbor));

            }
        }
    }

    return result;
}

// ���Ҿ����ض� ID �Ľڵ��� vector �е�����
int XGraph::findNodeIndex(const std::vector<std::shared_ptr<GraphNode>>& graph, const std::string& nodeId)
{
	for (int i = 0; i < graph.size(); i++)
	{
		if (graph[i]->label == nodeId)
		{
			return i;
		}
	}
	return -1; // ���δ�ҵ����򷵻� -1
}