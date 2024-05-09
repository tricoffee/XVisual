#include "Common/StrUtils.h"

/*
Strip off the space characters at the beginning and end
*/
std::string strip(const std::string& str)
{
	size_t start = str.find_first_not_of(" \t\n\r");
	size_t end = str.find_last_not_of(" \t\n\r");
	if (start == std::string::npos || end == std::string::npos)
	{
		// �ַ���ȫΪ�ո�
		return "";
	}
	else
	{
		return str.substr(start, end - start + 1);
	}
}

/*
Stripping the specified string at the beginning
*/
std::string stripPrefix(const std::string& str, const std::string& prefix)
{
	size_t pos = str.find(prefix);
	if (pos == std::string::npos)
	{
		return str; // ���δ�ҵ�ǰ׺���򷵻�ԭʼ�ַ���
	}
	return str.substr(pos + prefix.length()); // ����ȥ��ǰ׺������ַ���
}

/*
Stripping the specified string at the end
*/
std::string stripSuffix(const std::string& str, const std::string& suffix)
{
	if (str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix)
	{
		return str.substr(0, str.size() - suffix.size());
	}
	else
	{
		return str;
	}
}

/*
Extract substring before delimiter at the beginning
*/
std::string extractSubstrBeforeDelimiter(std::string input, std::string delimiter)
{
	size_t pos = input.find(delimiter);
	if (pos != std::string::npos)
	{
		return input.substr(0, pos);
	}
	else
	{
		return input;  // ����Ҳ����ָ��������������ַ���
	}
}

/*
Extract substring between two colons
*/
std::string extractSubstrBetweenColons(const std::string& str)
{
	size_t start = str.find(":") + 1; // ���ҵ�һ��ð�ŵ�λ�ã���1��Ϊ������ð�ű���
	size_t end = str.rfind(":"); // �������һ��ð�ŵ�λ��
	if (start == std::string::npos || end == std::string::npos || start >= end)
	{
		return ""; // ���û���ҵ�����ð�Ż������ǵ�˳����ȷ���򷵻ؿ��ַ���
	}
	return str.substr(start, end - start); // ��ȡ����ð��֮������ַ���
}

void printShape(const std::string& str1)
{
	std::vector<int> shape1 = parseShape<int>(str1);
	int size1 = shape1.size();
	std::cout << "Parsed shape for \"" << str1 << "\": [";
	int i = 0;
	for (int num : shape1)
	{
		++i;
		std::cout << num;
		if (i != size1)
		{
			std::cout << ", ";
		}
	}
	std::cout << "]" << std::endl;
}

bool startsWith(const std::string& str, const std::string& prefix)
{
	return str.find(prefix) == 0;
}

std::string extractSubstrAfterDelimiter(std::string input, std::string delimiter)
{
	int strLen = input.size();
	size_t pos = input.find(delimiter);
	if (pos != std::string::npos)
	{
		return input.substr(pos + 1, strLen - pos);
	}
	else
	{
		return input;  // ����Ҳ����ָ��������������ַ���
	}
}
