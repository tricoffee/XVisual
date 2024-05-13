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
	setModal(true) �����öԻ���Ϊģ̬�Ի������˼��
	ģ̬�Ի�����һ���û����������
	������ʱ��������ֹ�û���Ӧ�ó�����������ֽ��н�����
	ֱ���û��ر�������Ի�������ζ���û��������ȴ�������Ի���
	Ȼ����ܼ�����Ӧ�ó�����н���
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
	mLayout->setContentsMargins(0, 0, 0, 0); // ������߾�Ϊ0
	setLayout(mLayout);

	// ��ȡcomboWindow�Ĵ�С
	//QSize comboSize = comboWindow->size();
	//qDebug() << comboSize;

	// ���öԻ���Ĺ̶���С
	setFixedSize(300, 100); // ��������Ϊ���Ϊ300���߶�Ϊ100
}

void ComboDialog::closeEvent(QCloseEvent* event)
{
	XLOG_INFO(" == @@@ ==  ComboDialog::closeEvent == @@@ == ", CURRENT_THREAD_ID);
	// ��ȡ��ǰѡ������ı���Ϣ
	QString selectedText = comboWindow->selectedWidgetName;
	// ��ӡѡ�������Ϣ������̨
	// qDebug() << " == ComboDialog::closeEvent ==, Selected ComboBox item: " << selectedText;
	XLOG_INFO(" == ComboDialog::closeEvent ==, Selected ComboBox item: " + selectedText.toStdString(), CURRENT_THREAD_ID);

	comboWindow->getSelectedParam(params);

	// �ø��ദ��ر��¼�
	QDialog::closeEvent(event);
}

void ComboDialog::setParams(const PreParam& mParams)
{
	params = mParams;
	//Done, ��֤Ƕ����ComboDialog�����ComboWindowҲ����Ӧ�ı�
	comboWindow->setSelectItem(params);
}
void ComboDialog::getParams(PreParam& mParams)
{
	XLOG_INFO("@@@ == ComboDialog::getParams,  " + params.preTag + " == @@@", CURRENT_THREAD_ID);
	mParams = params;
}

void ComboDialog::accept()
{
	// ��ʹ��ComboDialog::exec()����ʱ���Ի���Ĺر���Ϊͨ����ͨ�� accept() �� reject() �����Ƶġ�
	// ��Щ���������öԻ���ķ���״̬������ģ̬ѭ����������ֱ�ӵ��� closeEvent()��
	// ��Ҫ�ڹرնԻ���ǰ��ʽ���� close() ������ȷ�� closeEvent() �����á�
	close();
	QDialog::accept();
}
