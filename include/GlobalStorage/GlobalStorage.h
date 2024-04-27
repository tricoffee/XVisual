#ifndef GlobalStorage_H
#define GlobalStorage_H
#include <unordered_map>
#include <mutex>
#include "XGraph/XGraph.h"
#include "ItemBase/XBaseItem.h"
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
//һ��ȫ�ֵ�std::unordered_map�����洢itemId�Ͷ�Ӧ��XBaseItem*
extern std::unordered_map<std::string, XBaseItem*> globalItemMap;
//������(mutex)������ȫ��globalItemMap����������ȷ��ͬʱֻ��һ���̷߳��������Ա�����Դ���������ݲ�һ��
extern std::mutex itemMapMutex;
//һ��ȫ�ֵ�std::unordered_map�����洢handleId�Ͷ�Ӧ��XBaseHandle*
extern std::unordered_map<std::string, XBaseHandle*> globalHandleMap;
//������(mutex)������ȫ��globalHandleMap����������ȷ��ͬʱֻ��һ���̷߳��������Ա�����Դ���������ݲ�һ��
extern std::mutex handleMapMutex;
//һ��ȫ�ֵ�xGraph�����洢itemId��ʶ��GraphNode
extern std::vector<std::shared_ptr<GraphNode>> xGraph;
//������(mutex)������ȫ�ֵ�xGraph����������ȷ��ͬʱֻ��һ���̷߳��������Ա�����Դ���������ݲ�һ��
extern std::mutex xGraphMutex;
//һ��ȫ�ֵ�xHandleGraph�����洢handleId��ʶ��GraphNode
extern std::vector<std::shared_ptr<GraphNode>> xHandleGraph;
//������(mutex)������ȫ�ֵ�xHandleGraph����������ȷ��ͬʱֻ��һ���̷߳��������Ա�����Դ���������ݲ�һ��
extern std::mutex xHandleGraphMutex;


template<typename T>
struct GlobalMap
{
	std::unordered_map<std::string, T*> map;
	std::mutex mutex;
};


// ȫ�ֵ� ItemMap �� HandleMap
extern GlobalMap<XBaseItem> GglobalItemMap;
extern GlobalMap<XBaseHandle> GglobalHandleMap;


// Unused
////һ��ȫ�ֵ�itemHandleUMap�����洢XBaseItem*��XHandle*֮���ӳ���ϵ
//extern std::unordered_map<XBaseItem*, XBaseHandle*> itemHandleUMap;
////������(mutex)������ȫ�ֵ�itemHandleUMap����������ȷ��ͬʱֻ��һ���̷߳��������Ա�����Դ���������ݲ�һ��
//extern std::mutex itemHandleUMapMutex;
////һ��ȫ�ֵ�itemHandleIdMap�����洢xItemId��xHandleId֮���ӳ���ϵ
//extern std::unordered_map<std::string, std::string> itemHandleIdMap;
////������(mutex)������ȫ�ֵ�itemHandleIdMap����������ȷ��ͬʱֻ��һ���̷߳��������Ա�����Դ���������ݲ�һ��
//extern std::mutex itemHandleIdMapMutex;

#endif //GlobalStorage_H