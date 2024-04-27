#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include <unordered_map>
#include <queue>
#include "XGraph/XGraph.h"
#include "GlobalStorage/GlobalStorage.h"
#include "Common/LoggerInstance.h"
#include "Exception/UmapKeyNoFoundException.h"
#include "Common/StrUtils.h"

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
            if (inDegrees.find(neighbor->nodeId) == inDegrees.end())
            {
                inDegrees[neighbor->nodeId] = 0;
            }
            inDegrees[neighbor->nodeId]++;
        }
    }

    // �����Ϊ0�Ľڵ�������q
    for (const auto& nodePtr : graph)
    {
        if (inDegrees[nodePtr->nodeId] == 0)
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
        result.push_back(node->nodeId);

        /*
         ���������Ե�ǰ�ڵ�Ϊ���ıߣ����ٱ�ָ��ڵ����ȣ�
         ������ٺ�ĳ���ڵ����ȱ�Ϊ0�����ýڵ�������q�С�
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


void XGraph::executeGraphNode(std::shared_ptr<GraphNode>& graphNode)
{
	XLOG_INFO("XGraph::executeGraphNode ... ", CURRENT_THREAD_ID);
	std::string nId = graphNode->nodeId;
    std::string typeStr = extractSubstrBeforeDelimiter(nId, "_");
    if ("Item" == typeStr)
    {
		auto it = globalItemMap.find(nId);
		if (it != globalItemMap.end())
		{
			XBaseItem* item = it->second;
			item->initOperands();
			try
			{
				item->xOperate();
			}
			catch (UmapKeyNoFoundException& e)
			{
				//        std::cout << "=== === ===" << e.what() << "=== === ===" << std::endl;
				XLOG_INFO("====== void XGraph::executeGraphNode, UmapKeyNoFoundException  ======" + std::string(e.what()), CURRENT_THREAD_ID);
				throw UmapKeyNoFoundException(e.what());
			}
		}
		else
		{
			XLOG_ERROR("XGraph::executeGraphNode, No found handleId in globalHandleMap", CURRENT_THREAD_ID);
		}
    }
	else if ("Handle" == typeStr)
	{
		auto it = globalHandleMap.find(nId);
		if (it != globalHandleMap.end())
		{
			XBaseHandle* handle = it->second;
			handle->initOperands();
			try
			{
				handle->xOperate();
			}
			catch (UmapKeyNoFoundException& e)
			{
				//        std::cout << "=== === ===" << e.what() << "=== === ===" << std::endl;
				XLOG_INFO("====== void XGraph::executeGraphNode, UmapKeyNoFoundException  ======" + std::string(e.what()), CURRENT_THREAD_ID);
				throw UmapKeyNoFoundException(e.what());
			}
		}
		else
		{
			XLOG_ERROR("XGraph::executeGraphNode, No found handleId in globalHandleMap", CURRENT_THREAD_ID);
		}
	}

}


/*
������ȱ����������������˳����ʽڵ�
1������ÿ���ڵ����ȣ�
2�������Ϊ0�Ľڵ�������q��
3����ʼ�������У�ִ�����²��裺
    a��������һ���ڵ�(�����Ϊ0�Ľڵ�)��
    b��ִ�иýڵ�ļ��㣻
    c�����������Ե�ǰ�ڵ�Ϊ���ıߣ����ٱ�ָ��ڵ����ȣ�
    d��������ٺ�ĳ���ڵ����ȱ�Ϊ0�����ýڵ�������q�С�
4���ظ�����3ֱ������qΪ�ա�
*/
void XGraph::executeXGraph(std::vector<std::shared_ptr<GraphNode>>& graph)
{
    std::unordered_map<std::string, int> inDegrees;
    std::queue<std::shared_ptr<GraphNode>> q; // ע������Ķ������͸�Ϊ std::shared_ptr<GraphNode>

    // ��ʼ�����Ϊ0������ÿ���ڵ�����
    for (const auto& nodePtr : graph)
    {
        for (const auto& neighbor : nodePtr->neighbors)
        {
            if (inDegrees.find(neighbor->nodeId) == inDegrees.end())
            {
                inDegrees[neighbor->nodeId] = 1;
            }
            else
            {
                inDegrees[neighbor->nodeId]++;
            }
        }
    }

    // �����Ϊ0�Ľڵ�������q
    for (const auto& nodePtr : graph)
    {
        if (inDegrees[nodePtr->nodeId] == 0)
        {
            q.push(nodePtr);
        }
    }

    // ִ�й�����ȱ���
    while (!q.empty())
    {
        std::shared_ptr<GraphNode> node = q.front();
        q.pop();

        try
        {
			// ִ�нڵ�nodeIdF
			executeGraphNode(node);
        }
		catch (UmapKeyNoFoundException& e)
		{
			//        std::cout << "=== === ===" << e.what() << "=== === ===" << std::endl;
			XLOG_INFO("====== void XGraph::executeXGraph, UmapKeyNoFoundException  ======" + std::string(e.what()), CURRENT_THREAD_ID);
			throw UmapKeyNoFoundException(e.what());
		}

        /*
         ���������Ե�ǰ�ڵ�Ϊ���ıߣ����ٱ�ָ��ڵ����ȣ�
         ������ٺ�ĳ���ڵ����ȱ�Ϊ0�����ýڵ�������q�С�
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
}

// ���Ҿ����ض� ID �Ľڵ��� vector �е�����
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
	return -1; // ���δ�ҵ����򷵻� -1
}