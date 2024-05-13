#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "ParamWidgetDialog/ComboDialog.h"
#include "ParamWidgetDialog/ComboWindow.h"
#include <QVBoxLayout>
#include "Common/DeletePtrMacro.h"
#include <QPushButton>
#include <QFormLayout>

ComboDialog::~ComboDialog()
{
	//qDebug() << " === ComboDialog::~ComboDialog() === ";
	XLOG_INFO(" === ComboDialog::~ComboDialog() === ", CURRENT_THREAD_ID);
	SAFE_DELETE(comboWindow);
	SAFE_DELETE(mLayout);
}

ComboDialog::ComboDialog(QWidget* parent) : QDialog(parent)
{
	//qDebug() << " @@@ ComboDialog::ComboDialog() @@@ ";
	XLOG_INFO(" @@@ ComboDialog::ComboDialog() @@@ ", CURRENT_THREAD_ID);

	setWindowTitle("Set Image Preprocessing Parameters");
	/*
	setModal(true) 是设置对话框为模态对话框的意思。
	模态对话框是一种用户界面组件，
	当它打开时，它会阻止用户与应用程序的其他部分进行交互，
	直到用户关闭了这个对话框。这意味着用户必须首先处理这个对话框，
	然后才能继续与应用程序进行交互
	*/
	setModal(true);

	comboWindow = new ComboWindow();
	mLayout = new QVBoxLayout();

	// Create OK and Cancel buttons
	okButton = new QPushButton("OK");
	cancelButton = new QPushButton("Cancel");

	connect(okButton, &QPushButton::clicked, this, &ComboDialog::accept);
	connect(cancelButton, &QPushButton::clicked, this, &ComboDialog::reject);

	// Create a QWidget with QFormLayout for holding okButton and cancelButton
	btnLayout = new QFormLayout();
	btnLayout->addRow(okButton, cancelButton);
	btnWidget = new QWidget();
	btnWidget->setLayout(btnLayout);

	mLayout->addWidget(comboWindow);
	mLayout->addWidget(btnWidget);
	mLayout->setAlignment(Qt::AlignCenter);
	mLayout->setContentsMargins(0, 0, 0, 0); // 设置外边距为0
	setLayout(mLayout);

	// 获取comboWindow的大小
	//QSize comboSize = comboWindow->size();
	//qDebug() << comboSize;

	// 设置对话框的固定大小
	setFixedSize(300, 100); // 例如设置为宽度为300，高度为100
}

void ComboDialog::closeEvent(QCloseEvent* event)
{
	XLOG_INFO(" == @@@ ==  ComboDialog::closeEvent == @@@ == ", CURRENT_THREAD_ID);
	// 获取当前选中项的文本信息
	QString selectedText = comboWindow->selectedWidgetName;
	// 打印选中项的信息到控制台
	// qDebug() << " == ComboDialog::closeEvent ==, Selected ComboBox item: " << selectedText;
	XLOG_INFO(" == ComboDialog::closeEvent ==, Selected ComboBox item: " + selectedText.toStdString(), CURRENT_THREAD_ID);

	comboWindow->getSelectedParam(params);

	// 让父类处理关闭事件
	QDialog::closeEvent(event);
}

void ComboDialog::setParams(const PreParam& mParams)
{
	params = mParams;
	//Done, 保证嵌入在ComboDialog里面的ComboWindow也做相应改变
	comboWindow->setSelectItem(params);
}
void ComboDialog::getParams(PreParam& mParams)
{
	XLOG_INFO("@@@ == ComboDialog::getParams,  " + params.preTag + " == @@@", CURRENT_THREAD_ID);
	mParams = params;
}

void ComboDialog::accept()
{
	// 当使用ComboDialog::exec()方法时，对话框的关闭行为通常是通过 accept() 或 reject() 来控制的。
	// 这些方法会设置对话框的返回状态并结束模态循环，但不会直接调用 closeEvent()。
	// 需要在关闭对话框前显式调用 close() 方法来确保 closeEvent() 被调用。
	close();
	QDialog::accept();
}
