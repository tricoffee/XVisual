#ifndef ResizePasteParamWidget_H
#define ResizePasteParamWidget_H

#include "PreParamWidget.h"

class QLabel;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;

class ResizePasteParamWidget : public PreParamWidget
{
	Q_OBJECT
	public:
		~ResizePasteParamWidget();
		ResizePasteParamWidget(QWidget* parent = nullptr);
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

#endif // ResizePasteParamWidget_H

