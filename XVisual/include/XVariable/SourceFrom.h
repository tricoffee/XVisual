#ifndef SourceFrom_H
#define SourceFrom_H
#include <iostream>

#define NULL_ITEMID ""
#define NULL_VariableName "" 
struct SourceFrom
{
	std::string itemId = NULL_ITEMID; //ItemId
	std::string variableName = NULL_VariableName; // Dest对象的或Source对象的某个变量的name
};

#endif //SourceFrom_H
