#include "Common/JsonFileUtils.h"
#include <filesystem>

namespace XVisual {

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

// 判断 Json 文件是否存在
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

} // namespace XVisual
