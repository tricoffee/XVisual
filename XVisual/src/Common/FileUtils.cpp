#include "Common/FileUtils.h"

int readClassNames(const std::string& classNamesPath, std::vector<std::string>& classNames)
{
	// ���ļ�
	std::ifstream file(classNamesPath);
	if (!file)
	{
		std::cerr << "Can NOT open file " << classNamesPath << std::endl;
		return -1;
	}

	// ��ȡ�ļ��е�ÿһ�У���������ӵ�������
	std::string line;
	while (std::getline(file, line))
	{
		if (!line.empty())
		{
			classNames.push_back(line);
		}
	}

	// �ر��ļ�
	file.close();

	return 0;
}
