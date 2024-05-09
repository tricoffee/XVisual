#ifndef XGraph_H
#define XGraph_H

#include <vector>
#include <string>
#include <memory>
#include "XGraph/GraphNode.h"
#include "Common/LoggerInstance.h"
#include "Exception/UmapKeyNoFoundException.h"
#include "Common/StrUtils.h"
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include <unordered_map>
#include <queue>
#include "GlobalStorage/GlobalVariable.h"

class XGraph
{
	public:
		static std::vector<std::string> BFSWithTopologicalSort(std::vector<std::shared_ptr<GraphNode>>& graph);

        static int findNodeIndex(const std::vector<std::shared_ptr<GraphNode>>& graph, const std::string& nodeId);


        template<typename T>
        static void executeGraphNode(std::unordered_map<std::string, T*>& uMap, std::shared_ptr<GraphNode>& graphNode)
        {
            XLOG_INFO("XGraph::executeGraphNode ... ", CURRENT_THREAD_ID);
            std::string nId = graphNode->nodeId;
            std::string typeStr = extractSubstrBeforeDelimiter(nId, "_");

            auto it = uMap.find(nId);
            if (it != uMap.end())
            {
                T* iElement = it->second;
                iElement->initOperands();
                try
                {
                    iElement->xOperate();
                }
                catch (UmapKeyNoFoundException& e)
                {
                    XLOG_INFO("====== void XGraph::executeGraphNode, UmapKeyNoFoundException  ======" + std::string(e.what()), CURRENT_THREAD_ID);
                    throw UmapKeyNoFoundException(e.what());
                }
            }
            else
            {
                XLOG_ERROR("XGraph::executeGraphNode, No found handleId in globalHandleMap", CURRENT_THREAD_ID);
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

        template<typename T>
        static void executeXGraph(std::unordered_map<std::string, T*>& uMap, std::vector<std::shared_ptr<GraphNode>>& graph)
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
                    std::string nId = node->nodeId;
                    nodeIdTopoSequence.push_back(nId);
                    // ִ�нڵ�nodeIdF
                    executeGraphNode(uMap, node);
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
};

#endif // XGraph_H