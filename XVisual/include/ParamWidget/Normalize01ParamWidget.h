#ifndef Normalize01ParamWidget_H
#define Normalize01ParamWidget_H

#include<QLabel>
#include<QLineEdit>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QGridLayout>

#include "PreParamWidget.h"

namespace XVisual {

class Normalize01ParamWidget : public PreParamWidget
{
	Q_OBJECT
	public:
		~Normalize01ParamWidget();
		Normalize01ParamWidget(QWidget* parent = nullptr);
		void getPreParam(PreParam& params) override;
		void setPreParam(const PreParam& params) override;
	private:
		QLabel* label;
		QHBoxLayout* mLayout;
};

} // namespace XVisual

#endif // Normalize01ParamWidget_H

