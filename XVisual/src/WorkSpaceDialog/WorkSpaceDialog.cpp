#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QDir>
#include <QFileDialog>

#include "WorkSpaceDialog/WorkSpaceDialog.h"

namespace XVisual {

WorkSpaceDialog::WorkSpaceDialog(WorkSpaceData data, QWidget* parent) : QDialog(parent)
{
	setWindowTitle("WorkSpace Selection Dialog");

	mainLayout = new QVBoxLayout();

	// 默认工作区, Default WorkSpace
	defaultLayout = new QHBoxLayout();
	defaultRadioButton = new QRadioButton("Default WorkSpace");
	defaultLineEdit = new QLineEdit(data.defaultWorkSpace);
	defaultLineEdit->setReadOnly(true);
	defaultLayout->addWidget(defaultRadioButton);
	defaultLayout->addWidget(defaultLineEdit);

	// 自定义工作区, Custom WorkSpace
	customLayout = new QHBoxLayout();
	customRadioButton = new QRadioButton("Custom WorkSpace");
	customLineEdit = new QLineEdit(data.customWorkSpace);
	browserButton = new QPushButton("Browing ...");
	customLayout->addWidget(customRadioButton);
	customLayout->addWidget(customLineEdit);
	customLayout->addWidget(browserButton);

	// 确认和取消按钮
	buttonLayout = new QHBoxLayout();
	okButton = new QPushButton("OK");
	cancelButton = new QPushButton("Cancel");
	buttonLayout->addWidget(okButton);
	buttonLayout->addWidget(cancelButton);

	mainLayout->addLayout(defaultLayout);
	mainLayout->addLayout(customLayout);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);

	// 默认选中默认工作区
	defaultRadioButton->setChecked(true);
	customRadioButton->setChecked(false);
	customLineEdit->setEnabled(false);
	browserButton->setEnabled(false);

	// 连接信号和槽
	connect(defaultRadioButton, &QRadioButton::toggled, this, &WorkSpaceDialog::defaultRadioButtonResponse);
	connect(customRadioButton, &QRadioButton::toggled, this, &WorkSpaceDialog::customRadioButtonResponse);
	connect(browserButton, &QPushButton::clicked, this, &WorkSpaceDialog::browse);
	connect(okButton, &QPushButton::clicked, this, &WorkSpaceDialog::accept);
	connect(cancelButton, &QPushButton::clicked, this, &WorkSpaceDialog::reject);

	setMinimumSize(400, 300);
}

QString WorkSpaceDialog::getWorkSpacePath() const
{
	if (defaultRadioButton->isChecked())
	{
		return defaultLineEdit->text();
	}
	else
	{
		return customLineEdit->text();
	}
}

bool WorkSpaceDialog::isCustomWorkSpace() const
{
	return customRadioButton->isChecked();
}

void WorkSpaceDialog::defaultRadioButtonResponse()
{
	bool isC = defaultRadioButton->isChecked();
	customLineEdit->setEnabled(!isC);
	browserButton->setEnabled(!isC);
}

void WorkSpaceDialog::customRadioButtonResponse()
{
	bool isC = customRadioButton->isChecked();
	customLineEdit->setEnabled(isC);
	browserButton->setEnabled(isC);
}

void WorkSpaceDialog::browse()
{
	QString dir = QFileDialog::getExistingDirectory(this, "Select WorkSpace", QString());
	if (!dir.isEmpty())
	{
		customLineEdit->setText(dir);
	}
}

} // namespace XVisual
