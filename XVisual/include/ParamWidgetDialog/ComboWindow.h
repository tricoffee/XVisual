#ifndef ComboWindow_H
#define ComboWindow_H

#include <QWidget>
#include <iostream>
#include <vector>
#include <any>
#include <string>
#include "Common/PreParam.h"

class QComboBox;
class QHBoxLayout;
class ResizeParamWidget;
class ResizePasteParamWidget;
class Normalize01ParamWidget;
class ComboDialog;
class PreParamWidget;

class ComboWindow : public QWidget
{
	Q_OBJECT

	public:
		~ComboWindow();
		ComboWindow(QWidget* parent = nullptr);
		void getSelectedParam(PreParam& params);
		void printSelectedParam();
		void setSelectItem(const PreParam& params);
		friend class ComboDialog;
	private slots:
		void comboBoxIndexChanged(int index);
	private:
		QComboBox* comboBox;
		QHBoxLayout* mLayout;
		QString selectedWidgetName;
		std::vector<std::string> itemStrList = { "Resize", "ResizePaste", "Normalize01" };
		std::vector<PreParamWidget*> widgets;
};

#endif // ComboWindow_H

