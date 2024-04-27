#ifndef LoadImageHandle_H
#define LoadImageHandle_H

#include <iostream>
#include "HandleBase/XBaseHandle.h"

class Colleague;
class LoadImageHandle : public XBaseHandle
{
	public:
		LoadImageHandle();
		~LoadImageHandle();
		void initParams() override;
		void xOperate() override;
};

#endif // LoadImageHandle_H