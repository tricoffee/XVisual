#ifndef DEST_H
#define DEST_H

#include "Param.h"

class Dest :public Param
{
	public:
		static Dest& getInstance();
	protected:
		Dest();
};

#endif //DEST_H

