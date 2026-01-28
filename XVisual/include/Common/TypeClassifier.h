#ifndef TypeClassifier_H
#define TypeClassifier_H

#include "Common/XType.h"

namespace XVisual {

class TypeClassifier
{
	public:
		template<typename T>
		static XType classify(const T& variable)
		{
			if (typeid(variable) == typeid(std::string))
			{
				return XType::Str;
			}
			else if (typeid(variable) == typeid(std::vector<int>) ||
				typeid(variable) == typeid(std::vector<float>) ||
				typeid(variable) == typeid(std::vector<double>))
			{
				return XType::Arr;
			}
			else if (typeid(variable) == typeid(cv::Rect))
			{
				return XType::CVRect;
			}
			else
			{
				return XType::Unknown;
		}
	}
};

} // namespace XVisual

#endif //TypeClassifier_H
