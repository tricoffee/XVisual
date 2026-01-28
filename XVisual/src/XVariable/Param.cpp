#include "XVariable/Param.h"

namespace XVisual {

Param& Param::getInstance()
{
	static Param instance;
	return instance;
}

Param::Param() {}

} // namespace XVisual
