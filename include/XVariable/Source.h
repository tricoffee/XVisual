#ifndef SOURCE_H
#define SOURCE_H

#include "Param.h"

class Source :public Param
{
	public:
		static Source& getInstance();
	protected:
		Source();
};

#endif //SOURCE_H
