#ifndef Normalize01ParamWidget_H
#define Normalize01ParamWidget_H

#include "PreParamWidget.h"

class QLabel;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
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

#endif // Normalize01ParamWidget_H

