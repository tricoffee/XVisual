#ifndef HandleGlobalStorage_H
#define HandleGlobalStorage_H
#include <unordered_map>
#include <mutex>
#include "XGraph/XGraph.h"
#include "HandleBase/XBaseHandle.h"


/*
extern�ؼ�����������ȫ�ֱ���������������洢�ռ䡣
����Ŀ�����ñ�����֪���ñ���������һ��Դ�ļ��ж���ġ�
����Ĵ����У�globalItemMap��itemMapMutex�Ķ���ܿ�������һ��Դ�ļ��У�
��������ֻ�������ͷ�ļ������������ǡ�
��ˣ�������ȷ���÷�����Ϊ�����߱�������Щ�����Ķ����������ط���
��ֻ��Ҫ�������ǡ�ȥ��extern���ܻᵼ���ظ�����Ĵ�����Ϊ���������ڶ��Դ�ļ��б����塣
���ԣ�������ȥ��extern��
*/

//һ��ȫ�ֵ�std::unordered_map�����洢handleId�Ͷ�Ӧ��XBaseHandle*
extern std::unordered_map<std::string, XBaseHandle*> globalHandleMap;
//������(mutex)������ȫ��globalHandleMap����������ȷ��ͬʱֻ��һ���̷߳��������Ա�����Դ���������ݲ�һ��
extern std::mutex handleMapMutex;


//һ��ȫ�ֵ�xHandleGraph�����洢handleId��ʶ��GraphNode
extern std::vector<std::shared_ptr<GraphNode>> xHandleGraph;
//������(mutex)������ȫ�ֵ�xHandleGraph����������ȷ��ͬʱֻ��һ���̷߳��������Ա�����Դ���������ݲ�һ��
extern std::mutex xHandleGraphMutex;

#endif //HandleGlobalStorage_H