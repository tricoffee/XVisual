#ifndef DEST_H
#define DEST_H

#include "Param.h"

namespace XVisual {

class Dest :public Param
{
	public:
		static Dest& getInstance();
	protected:
		Dest();
};

} // namespace XVisual

#endif //DEST_H

