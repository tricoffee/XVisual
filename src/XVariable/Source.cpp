#include "XVariable/Source.h"

Source& Source::getInstance()
{
	static Source instance;
	return instance;
}
Source::Source() {}