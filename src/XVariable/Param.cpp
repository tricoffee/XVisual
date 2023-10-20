#include "XVariable/Param.h"

Param& Param::getInstance()
{
	static Param instance;
	return instance;
}
Param::Param() {}
