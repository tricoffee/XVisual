#ifndef COMBODIALOG_H
#define COMBODIALOG_H

#include <QDialog>
#include <iostream>
#include <unordered_map>
#include <string>
#include <any>
#include "Common/PreParam.h"

class ComboWindow;
class QPushButton;
class QFormLayout;
class QVBoxLayout;

class ComboDialog : public QDialog
{
	Q_OBJECT
	public:
		~ComboDialog();
		ComboDialog(QWidget* parent = nullptr);
		void setParams(const PreParam& mParams);
		void getParams(PreParam& mParams);
	public slots:
		void accept();
	protected:
		void closeEvent(QCloseEvent* event) override;
	private:
		ComboWindow* comboWindow;
		QPushButton* okButton;
		QPushButton* cancelButton;
		QFormLayout* btnLayout;
		QWidget* btnWidget;
		QVBoxLayout* mLayout;
		PreParam params;
};

#endif // COMBODIALOG_H
