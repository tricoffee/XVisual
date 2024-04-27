#ifndef ExceptionUtils_H
#define ExceptionUtils_H
#include <iostream>
#include <unordered_map>
#include <any>
#include "UmapKeyNoFoundException.h"

template<typename T>
void keyExists(const std::unordered_map<std::string, T>& myMap, const std::string& key, bool& isKeyExist)
{
	isKeyExist = myMap.find(key) != myMap.end();
	if (!isKeyExist)
	{
		std::string message = " +++ +++ +++ \"" + key + "\" No Found +++ +++ +++ \n";
		throw UmapKeyNoFoundException(message);
	}
}

#endif // ExceptionUtils_H