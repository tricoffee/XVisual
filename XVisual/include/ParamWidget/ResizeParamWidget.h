#ifndef ResizeParamWidget_H
#define ResizeParamWidget_H

#include<QLabel>
#include<QLineEdit>
#include<QHBoxLayout>
#include<QVBoxLayout>

#include "PreParamWidget.h"

namespace XVisual {

class ResizeParamWidget : public PreParamWidget
{
	Q_OBJECT
	public:
		~ResizeParamWidget();
		ResizeParamWidget(QWidget* parent = nullptr);
		void getPreParam(PreParam& params) override;
		void setPreParam(const PreParam& params) override;
	private:
		QHBoxLayout* dstWidthLayout;
		QLabel* dstWidthLabel;
		QLineEdit* dstWidthLineEdit;

		QHBoxLayout* dstHeightLayout;
		QLabel* dstHeightLabel;
		QLineEdit* dstHeightLineEdit;

		QVBoxLayout* mLayout;
};

} // namespace XVisual

#endif // ResizeParamWidget_H

