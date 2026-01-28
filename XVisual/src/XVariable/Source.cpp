#include "XVariable/Source.h"

namespace XVisual {

Source& Source::getInstance()
{
	static Source instance;
	return instance;
}

Source::Source() {}

} // namespace XVisual
