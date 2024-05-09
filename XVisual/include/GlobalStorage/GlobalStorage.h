#ifndef GlobalStorage_H
#define GlobalStorage_H
#include <unordered_map>
#include <mutex>
#include "XGraph/XGraph.h"
#include "ItemBase/XBaseItem.h"


/*
extern�ؼ�����������ȫ�ֱ���������������洢�ռ䡣
����Ŀ�����ñ�����֪���ñ���������һ��Դ�ļ��ж���ġ�
����Ĵ����У�globalItemMap��itemMapMutex�Ķ���ܿ�������һ��Դ�ļ��У�
��������ֻ�������ͷ�ļ������������ǡ�
��ˣ�������ȷ���÷�����Ϊ�����߱�������Щ�����Ķ����������ط���
��ֻ��Ҫ�������ǡ�ȥ��extern���ܻᵼ���ظ�����Ĵ�����Ϊ���������ڶ��Դ�ļ��б����塣
���ԣ�������ȥ��extern��
*/

//һ��ȫ�ֵ�std::unordered_map�����洢itemId�Ͷ�Ӧ��XBaseItem*
extern std::unordered_map<std::string, XBaseItem*> globalItemMap;
//������(mutex)������ȫ��globalItemMap����������ȷ��ͬʱֻ��һ���̷߳��������Ա�����Դ���������ݲ�һ��
extern std::mutex itemMapMutex;


//һ��ȫ�ֵ�xGraph�����洢itemId��ʶ��GraphNode
extern std::vector<std::shared_ptr<GraphNode>> xGraph;
//������(mutex)������ȫ�ֵ�xGraph����������ȷ��ͬʱֻ��һ���̷߳��������Ա�����Դ���������ݲ�һ��
extern std::mutex xGraphMutex;

#endif //GlobalStorage_H