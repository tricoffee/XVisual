#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "MainWindow/RowWidget.h"
RowWidget::RowWidget(const QString& xText, QWidget* parent) :QWidget(parent),text(xText)
{
	XLOG_INFO("RowWidget::RowWidget, text = " + text.toStdString(), CURRENT_THREAD_ID);
	label = new QLabel(text);
	addButton1 = new QPushButton("+");
	deleteButton = new QPushButton("-");
	rowLayout = new QHBoxLayout();

	// ��ȡ��ťlabel�ı��Ŀ��
	QFontMetrics labelFontMetrics(label->font());
	// labelFontMetrics.horizontalAdvance��������Ϊ��λ���ַ����
	int labelSize = labelFontMetrics.horizontalAdvance(label->text());
	// ����label�Ŀ��ΪlabelSize+20
	label->setFixedWidth(labelSize + 20);

	// ��ȡ��ťaddButton1�ı��Ŀ��
	QFontMetrics addFontMetrics(addButton1->font());
	// addFontMetrics.horizontalAdvance��������Ϊ��λ���ַ����
	int addButtonSize = addFontMetrics.horizontalAdvance(addButton1->text());
	// ����addButton1�Ŀ��ΪaddButtonSize+20
	addButton1->setFixedWidth(addButtonSize + 20);

	// ��ȡ��ťdeleteButton�ı��Ŀ��
	QFontMetrics deleteFontMetrics(deleteButton->font());
	// deleteFontMetrics.horizontalAdvance��������Ϊ��λ���ַ����
	int deleteButtonSize = deleteFontMetrics.horizontalAdvance(deleteButton->text());
	// ����deleteButton�Ŀ��ΪdeleteButtonSize+20
	deleteButton->setFixedWidth(deleteButtonSize + 20);

	//��label, addButton1, deleteButton��ӵ�rowLayout
	rowLayout->addWidget(label);
	rowLayout->addWidget(addButton1);
	rowLayout->addWidget(deleteButton);
	// ��rowLayout����ΪrowWidget�Ĳ���
	setLayout(rowLayout);
	// ����rowLayout���ⲿ�߾�
	rowLayout->setContentsMargins(0, 0, 0, 0);
	// ����rowLayout���ڲ��߾�
	rowLayout->setSpacing(0);
	// �����źźͲ�
	connect(addButton1, &QPushButton::clicked, this, &RowWidget::onAddButton1Clicked);
	connect(deleteButton, &QPushButton::clicked, this, &RowWidget::onDeleteButtonClicked);
	getText();
}

void RowWidget::onAddButton1Clicked()
{
	XLOG_INFO("RowWidget::onAddButton1Clicked ...", CURRENT_THREAD_ID);
	emit tabInsertSignal(text);
}

void RowWidget::onDeleteButtonClicked()
{
	XLOG_INFO("RowWidget::onDeleteButtonClicked ...", CURRENT_THREAD_ID);
	emit tabDeleteSignal(text);
}

QString RowWidget::getText() const
{
	XLOG_INFO("RowWidget::getText(), text = " + text.toStdString(), CURRENT_THREAD_ID);
	return text;
}

QLabel* RowWidget::getLabel()
{
	XLOG_INFO("RowWidget::getLabel(), label->text() = " + label->text().toStdString(), CURRENT_THREAD_ID);
	return label;
}