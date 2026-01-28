#ifndef WorkSpaceDialog_H
#define WorkSpaceDialog_H

#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>

#include "Common/SettingsReader.h"

namespace XVisual {

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

} // namespace XVisual

#endif // WorkSpaceDialog_H
