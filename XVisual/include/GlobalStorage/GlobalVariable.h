#ifndef GlobalVariable_H
#define GlobalVariable_H

#include <iostream>
#include <vector>
#include <string>

//一个全局的nodeIdTopoSequence用来存储nodeId的拓扑顺序
extern std::vector<std::string> nodeIdTopoSequence;
//一个维护WorkSpace的全局变量
extern std::string globalWorkSpaceDir;

#endif // GlobalVariable_H