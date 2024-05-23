#ifndef JsonFileUtils_H
#define JsonFileUtils_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

// 读取文件内容到字符串
char* readJsonFile(const char* filename);
bool isJsonFile(const std::string& path);

#endif // JsonFileUtils_H