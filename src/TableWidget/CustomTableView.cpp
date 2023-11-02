#include "TableWidget/CustomTableView.h"
#include "ItemBase/XArrow.h"
#include "ItemBase/XBaseItem.h"
#include <QString>
CustomTableView::CustomTableView(QWidget* parent) : QTableView(parent)
{
}
CustomTableView::CustomTableView(QGraphicsLineItem* lineArrow, QWidget* parent) : QTableView(parent)
{
	XArrow* aLineArrow = static_cast<XArrow*>(lineArrow);
	yItem = aLineArrow->getStartItem();
	xItem = aLineArrow->getEndItem();
	yItemId = QString::fromStdString(yItem->getUuid());
	xItemId = QString::fromStdString(xItem->getUuid());
	connect(this, &CustomTableView::changeLineArrowColorSignal,
		aLineArrow, &XArrow::changeLineArrowColorSlots);
}
void CustomTableView::closeEvent(QCloseEvent* event)
{
	qDebug() << "CustomTableView::closeEvent(QCloseEvent* event)";
	qDebug() << "yItemId" << yItemId;
	qDebug() << "xItemId" << xItemId;
	QColor color = Qt::black;
	emit changeLineArrowColorSignal(color);
	QAbstractItemModel* model = this->model();
	if (!model)
	{
		qDebug() << "Model is not set.";
		return;
	}
	// 第一列的索引
	int xColumn = 0;
	// 第二列的索引
	int yColumn = 1;
	for (int row = 0; row < model->rowCount(); ++row)
	{
		QModelIndex yIndex = model->index(row, yColumn);
		QVariant yData = model->data(yIndex);
		QString yVaribleName = "";
		QString yVariableType = "";
		if (yData.canConvert<QString>())
		{
			QString yText = yData.toString();
			qDebug() << "Cloumn-1,Row-" << row << " - Selected Item Text: " << yText;
			QStringList parts = yText.split('\n');
			if (parts.size() >= 2 && !parts[0].isEmpty() && !parts[1].isEmpty()) 
			{
				yVaribleName = parts[0];
				yVariableType = parts[1];
				qDebug() << "yVaribleName :" << yVaribleName;
				qDebug() << "yVariableType :" << yVariableType;
			}
			else 
			{
				qDebug() << "Input does not contain expected parts.";
			}
		}

		QModelIndex xIndex = model->index(row, xColumn);
		QVariant xData = model->data(xIndex);
		//variable name
		QString xVariableName = "";
		//variable type
		QString xVariableType = "";
		if (xData.canConvert<QString>())
		{
			QString xText = xData.toString();
			qDebug() << "Cloumn-0,Row-" << row << " - Selected Item Text: " << xText;
			QStringList parts = xText.split('\n');
			if (parts.size() >= 2 && !parts[0].isEmpty() && !parts[1].isEmpty())
			{
				xVariableName = parts[0];
				xVariableType = parts[1];
				qDebug() << "xVariableName :" << xVariableName;
				qDebug() << "xVariableType :" << xVariableType;
			}
			else
			{
				qDebug() << "Input does not contain expected parts.";
			}
		}
		qDebug() << "xVariableName.toStdString(), yItemId, yVariableName.toStdString():" << xVariableName << yItemId << yVaribleName;
		SourceFrom soureFrom{yItemId.toStdString(),yVaribleName.toStdString()};
		xItem->setSourceFrom(xVariableName.toStdString(), soureFrom);
	}
}
