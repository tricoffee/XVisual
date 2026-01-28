#ifndef SOURCE_H
#define SOURCE_H

#include "Param.h"

namespace XVisual {

class Source :public Param
{
	public:
		static Source& getInstance();
	protected:
		Source();
};

} // namespace XVisual

#endif //SOURCE_H
