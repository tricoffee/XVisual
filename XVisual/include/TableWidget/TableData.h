#ifndef TableData_H
#define TableData_H 
#include<iostream>
#include<string>
#include<any>

#define TableData_UnSelectedItemId  "---"
#define TableData_UnSelectedName  "---"
#define TableData_UnSelectedType  "---"

struct TableData
{
	std::string itemId;
	std::string name;
	std::string type;
};

#endif // TableData_H
