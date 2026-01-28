#include "XVariable/Dest.h"

namespace XVisual {

Dest& Dest::getInstance()
{
	static Dest instance;
	return instance;
}

Dest::Dest() {}

} // namespace XVisual
