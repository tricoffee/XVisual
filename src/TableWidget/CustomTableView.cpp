#include "TableWidget/CustomTableView.h"
#include "ItemBase/XArrow.h"
CustomTableView::CustomTableView(QWidget* parent) : QTableView(parent)
{
}
CustomTableView::CustomTableView(QGraphicsLineItem* lineArrow, QWidget* parent) : QTableView(parent)
{
	XArrow* xLineArrow = static_cast<XArrow*>(lineArrow);
	connect(this, &CustomTableView::changeLineArrowColorSignal,
		xLineArrow, &XArrow::changeLineArrowColorSlots);
}
void CustomTableView::closeEvent(QCloseEvent* event)
{
	qDebug() << "CustomTableView::handleCustomCloseEvent";
	QColor color = Qt::black;
	emit changeLineArrowColorSignal(color);
	QAbstractItemModel* model = this->model();
	if (!model)
	{
		qDebug() << "Model is not set.";
		return;
	}
	// 第二列的索引
	int column = 1;
	for (int row = 0; row < model->rowCount(); ++row)
	{
		QModelIndex index = model->index(row, column);
		QVariant data = model->data(index);
		if (data.canConvert<QString>())
		{
			QString text = data.toString();
			qDebug() << "Row " << row << " - Selected Item Text: " << text;
		}
	}
}
