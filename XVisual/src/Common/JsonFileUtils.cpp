#include "Common/JsonFileUtils.h"

// 读取文件内容到字符串
char* readJsonFile(const char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (!file)
	{
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = (char*)malloc(length + 1);
	if (!buffer)
	{
		fclose(file);
		return NULL;
	}

	fread(buffer, 1, length, file);
	fclose(file);
	buffer[length] = '\0';
	return buffer;
}