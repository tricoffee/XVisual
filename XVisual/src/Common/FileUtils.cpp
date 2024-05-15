#include "Common/FileUtils.h"

int readClassNames(const std::string& classNamesPath, std::vector<std::string>& classNames)
{
	// 打开文件
	std::ifstream file(classNamesPath);
	if (!file)
	{
		std::cerr << "Can NOT open file " << classNamesPath << std::endl;
		return -1;
	}

	// 读取文件中的每一行，并将其添加到向量中
	std::string line;
	while (std::getline(file, line))
	{
		if (!line.empty())
		{
			classNames.push_back(line);
		}
	}

	// 关闭文件
	file.close();

	return 0;
}
