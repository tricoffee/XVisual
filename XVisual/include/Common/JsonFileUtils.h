#ifndef JsonFileUtils_H
#define JsonFileUtils_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

// ��ȡ�ļ����ݵ��ַ���
char* readJsonFile(const char* filename);
bool isJsonFile(const std::string& path);

#endif // JsonFileUtils_H