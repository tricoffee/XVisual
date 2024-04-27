#ifndef CVCropHandle_H
#define CVCropHandle_H

#include <iostream>
#include "HandleBase/XBaseHandle.h"

class Colleague;
class CVCropHandle : public XBaseHandle
{
	public:
		CVCropHandle();
		~CVCropHandle();
		void initParams() override;
		void xOperate() override;
		void setRoI(const cv::Rect& regionOfInterest);
		cv::Rect getRoI();
	private:
		cv::Rect regionOfInterest;
};

#endif // CVCropHandle_H
