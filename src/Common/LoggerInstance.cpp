#include "Common/LoggerInstance.h"

// 定义全局变量，使得在不同 cpp 文件中可以共享
LoggerInstance& logger = LoggerInstance::GetInstance();