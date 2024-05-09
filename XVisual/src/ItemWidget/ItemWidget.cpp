#include <QHBoxLayout>
#include "ItemWidget/XTextEdit.h"
#include "ItemWidget/ItemWidget.h"

ItemWidget::ItemWidget(QWidget* parent, Qt::WindowFlags)
	:QWidget(parent)
{
	//QTextEdit��������Ϊ͸�������ر�Ե
	setStyleSheet("background:transparent;border:none");
	initWidget();
}
void ItemWidget::initWidget()
{
	edit = new XTextEdit();
	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(edit);
	setLayout(layout);
}
XTextEdit* ItemWidget::getEdit()
{
	return edit;
}