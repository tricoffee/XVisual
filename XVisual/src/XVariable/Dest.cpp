#include "XVariable/Dest.h"

Dest& Dest::getInstance()
{
	static Dest instance;
	return instance;
}
Dest::Dest() {}