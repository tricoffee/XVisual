#ifndef HandleGlobalStorage_H
#define HandleGlobalStorage_H
#include <unordered_map>
#include <mutex>
#include "XGraph/XGraph.h"
#include "HandleBase/XBaseHandle.h"


/*
extern关键字用于声明全局变量，但它不分配存储空间。
它的目的是让编译器知道该变量是在另一个源文件中定义的。
在你的代码中，globalItemMap和itemMapMutex的定义很可能在另一个源文件中，
而这里你只是在这个头文件中声明了它们。
因此，这是正确的用法，因为它告诉编译器这些变量的定义在其他地方，
你只需要声明它们。去掉extern可能会导致重复定义的错误，因为变量可能在多个源文件中被定义。
所以，不建议去掉extern。
*/

namespace XVisual {

//一个全局的std::unordered_map用来存储handleId和对应的XBaseHandle*
extern std::unordered_map<std::string, XBaseHandle*> globalHandleMap;
//互斥锁(mutex)来保护全局globalHandleMap，这样可以确保同时只有一个线程访问它，以避免资源竞争和数据不一致
extern std::mutex handleMapMutex;


//一个全局的xHandleGraph用来存储handleId标识的GraphNode
extern std::vector<std::shared_ptr<GraphNode>> xHandleGraph;
//互斥锁(mutex)来保护全局的xHandleGraph，这样可以确保同时只有一个线程访问它，以避免资源竞争和数据不一致
extern std::mutex xHandleGraphMutex;

} // namespace XVisual

#endif //HandleGlobalStorage_H
