#include "Common/JsonFileUtils.h"
#include <filesystem>

// ��ȡ�ļ����ݵ��ַ���
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

// �ж� Json �ļ��Ƿ����
bool isJsonFile(const std::string& path)
{

	if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path) && std::filesystem::path(path).extension() != ".json")
	{
		//std::cout << path << " does not exists, or is not json file. " << path << std::endl;
		return true;
	}
	else
	{
		//std::cout << path << " is Json File." << std::endl;
		return false;
	}
}