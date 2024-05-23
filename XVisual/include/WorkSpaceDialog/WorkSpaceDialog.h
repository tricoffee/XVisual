#ifndef WorkSpaceDialog_H
#define WorkSpaceDialog_H


#include <QDialog>
class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QRadioButton;
class QLineEdit;
class QPushButton;
#include "Common/SettingsReader.h"

class WorkSpaceDialog : public QDialog
{
	Q_OBJECT
	public:
		WorkSpaceDialog(WorkSpaceData data, QWidget* parent = nullptr);
		QString getWorkSpacePath() const;
		bool isCustomWorkSpace() const;

	private slots:
		void defaultRadioButtonResponse();
		void customRadioButtonResponse();
		void browse();

	private:
		QRadioButton* defaultRadioButton;
		QLineEdit* defaultLineEdit;
		QHBoxLayout* defaultLayout;

		QRadioButton* customRadioButton;
		QLineEdit* customLineEdit;
		QPushButton* browserButton;
		QHBoxLayout* customLayout;

		QPushButton* okButton;
		QPushButton* cancelButton;
		QHBoxLayout* buttonLayout;

		QVBoxLayout* mainLayout;

};


#endif // WorkSpaceDialog_H
