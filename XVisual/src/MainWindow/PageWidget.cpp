#include "MainWindow/PageWidget.h"

namespace XVisual {

PageWidget::PageWidget(QWidget* parent) :QWidget(parent)
{
	vLayout = new QVBoxLayout();
}
void PageWidget::addWidgetsToLayout(QList<QWidget*> widgetList)
{
	for (QWidget* widget : widgetList)
	{
		vLayout->addWidget(widget);
	}
	setLayout(vLayout);
}

} // namespace XVisual
