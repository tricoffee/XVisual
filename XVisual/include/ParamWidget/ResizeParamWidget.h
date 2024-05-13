#ifndef ResizeParamWidget_H
#define ResizeParamWidget_H

#include "PreParamWidget.h"

class QLabel;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;

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

#endif // ResizeParamWidget_H

