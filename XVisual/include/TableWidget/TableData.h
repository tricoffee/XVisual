#ifndef TableData_H
#define TableData_H 
#include<iostream>
#include<string>
#include<any>

#define TableData_UnSelectedItemId  "---"
#define TableData_UnSelectedName  "---"
#define TableData_UnSelectedType  "---"

namespace XVisual {

struct TableData
{
	std::string itemId;
	std::string name;
	std::string type;
};

} // namespace XVisual

#endif // TableData_H
