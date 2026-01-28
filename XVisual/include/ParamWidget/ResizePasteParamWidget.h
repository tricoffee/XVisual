#ifndef ResizePasteParamWidget_H
#define ResizePasteParamWidget_H

#include<QLabel>
#include<QLineEdit>
#include<QHBoxLayout>
#include<QVBoxLayout>

#include "PreParamWidget.h"

namespace XVisual {

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

} // namespace XVisual

#endif // ResizePasteParamWidget_H

