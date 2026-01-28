#include "Common/UuidGenerator.h"

namespace XVisual {

	std::string generateUUID()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 15);

		std::stringstream ss;
		ss << std::hex;

		for (int i = 0; i < 8; ++i)
		{
			ss << dis(gen);
		}
		ss << "-";
		for (int i = 0; i < 4; ++i)
		{
			ss << dis(gen);
		}
		ss << "-";
		for (int i = 0; i < 4; ++i)
		{
			ss << dis(gen);
		}
		ss << "-";
		for (int i = 0; i < 4; ++i)
		{
			ss << dis(gen);
		}
		ss << "-";
		for (int i = 0; i < 12; ++i)
		{
			ss << dis(gen);
		}

		return ss.str();
	}
} //namespace XVisual