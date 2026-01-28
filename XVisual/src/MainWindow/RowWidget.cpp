#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "MainWindow/RowWidget.h"

namespace XVisual {

RowWidget::RowWidget(const QString& xText, QWidget* parent) :QWidget(parent),text(xText)
{
	XLOG_INFO("RowWidget::RowWidget, text = " + text.toStdString(), CURRENT_THREAD_ID);
	label = new QLabel(text);
	addButton1 = new QPushButton("+");
	deleteButton = new QPushButton("-");
	rowLayout = new QHBoxLayout();

	// 获取按钮label文本的宽度
	QFontMetrics labelFontMetrics(label->font());
	// labelFontMetrics.horizontalAdvance是以像素为单位的字符宽度
	int labelSize = labelFontMetrics.horizontalAdvance(label->text());
	// 设置label的宽度为labelSize+20
	label->setFixedWidth(labelSize + 20);

	// 获取按钮addButton1文本的宽度
	QFontMetrics addFontMetrics(addButton1->font());
	// addFontMetrics.horizontalAdvance是以像素为单位的字符宽度
	int addButtonSize = addFontMetrics.horizontalAdvance(addButton1->text());
	// 设置addButton1的宽度为addButtonSize+20
	addButton1->setFixedWidth(addButtonSize + 20);

	// 获取按钮deleteButton文本的宽度
	QFontMetrics deleteFontMetrics(deleteButton->font());
	// deleteFontMetrics.horizontalAdvance是以像素为单位的字符宽度
	int deleteButtonSize = deleteFontMetrics.horizontalAdvance(deleteButton->text());
	// 设置deleteButton的宽度为deleteButtonSize+20
	deleteButton->setFixedWidth(deleteButtonSize + 20);

	//将label, addButton1, deleteButton添加到rowLayout
	rowLayout->addWidget(label);
	rowLayout->addWidget(addButton1);
	rowLayout->addWidget(deleteButton);
	// 将rowLayout设置为rowWidget的布局
	setLayout(rowLayout);
	// 设置rowLayout的外部边距
	rowLayout->setContentsMargins(0, 0, 0, 0);
	// 设置rowLayout的内部边距
	rowLayout->setSpacing(0);
	// 连接信号和槽
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

} // namespace XVisual
