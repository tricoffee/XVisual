#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "TableWidget/CustomTableView.h"
#include "ItemBase/XArrow.h"
#include "ItemBase/XBaseItem.h"
#include "HandleBase/XBaseHandle.h"
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


	yHandle = yItem->getXHandle();
	xHandle = xItem->getXHandle();
	yHandleId = QString::fromStdString(yHandle->getUuid());
	xHandleId = QString::fromStdString(xHandle->getUuid());


	connect(this, &CustomTableView::changeLineArrowColorSignal,
		aLineArrow, &XArrow::changeLineArrowColorSlots);
}
void CustomTableView::closeEvent(QCloseEvent* event)
{
	//qDebug() << "CustomTableView::closeEvent(QCloseEvent* event)";
	//qDebug() << "yHandleId" << yItemId;
	//qDebug() << "xHandleId" << xItemId;
	XLOG_INFO("CustomTableView::closeEvent(QCloseEvent* event)", CURRENT_THREAD_ID);
	XLOG_INFO("CustomTableView::closeEvent, yHandleId = " + yHandleId.toStdString(), CURRENT_THREAD_ID);
	XLOG_INFO("CustomTableView::closeEvent, xHandleId = " + xHandleId.toStdString(), CURRENT_THREAD_ID);
	QColor color = Qt::black;
	emit changeLineArrowColorSignal(color);
	QAbstractItemModel* model = this->model();
	if (!model)
	{
		//qDebug() << "Model is not set.";
		XLOG_INFO("Model is not set.", CURRENT_THREAD_ID);
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
		QString yVariableName = "";
		QString yVariableType = "";

		if (yData.canConvert<QString>())
		{
			QString yText = yData.toString();
			//qDebug() << "Cloumn-1,Row-" << row << " - Selected Item Text: " << yText;
			XLOG_INFO("Cloumn-1,Row- " + std::to_string(row) + " - Selected Item Text: " + yText.toStdString(), CURRENT_THREAD_ID);
			QStringList parts = yText.split('\n');
			if (parts.size() >= 2 && !parts[0].isEmpty() && !parts[1].isEmpty()) 
			{
				yVariableName = parts[0];
				yVariableType = parts[1];
				//qDebug() << "yVariableName :" << yVariableName;
				//qDebug() << "yVariableType :" << yVariableType;
				XLOG_INFO("yVariableName : " + yVariableName.toStdString(), CURRENT_THREAD_ID);
				XLOG_INFO("yVariableType : " + yVariableType.toStdString(), CURRENT_THREAD_ID);
			}
			else 
			{
				//qDebug() << "Input does not contain expected parts.";
				XLOG_INFO("Input does not contain expected parts.", CURRENT_THREAD_ID);
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
			//qDebug() << "Cloumn-0,Row-" << row << " - Selected Item Text: " << xText;
			XLOG_INFO("Cloumn-0,Row-"+ std::to_string(row) +" - Selected Item Text: " + xText.toStdString(), CURRENT_THREAD_ID);
			QStringList parts = xText.split('\n');
			if (parts.size() >= 2 && !parts[0].isEmpty() && !parts[1].isEmpty())
			{
				xVariableName = parts[0];
				xVariableType = parts[1];
				//qDebug() << "xVariableName :" << xVariableName;
				//qDebug() << "xVariableType :" << xVariableType;
				XLOG_INFO("xVariableName : " + xVariableName.toStdString(), CURRENT_THREAD_ID);
				XLOG_INFO("xVariableType : " + xVariableType.toStdString(), CURRENT_THREAD_ID);
			}
			else
			{
				//qDebug() << "Input does not contain expected parts.";
				XLOG_INFO("Input does not contain expected parts.", CURRENT_THREAD_ID);
			}
		}

		if ((yVariableName.toStdString() == TableData_UnSelectedName) || (yVariableType.toStdString() == TableData_UnSelectedType))
		{
			// do nothing
			XLOG_INFO("@@@ CustomTableView::closeEvent, xVariableName.toStdString() = " + xVariableName.toStdString(), CURRENT_THREAD_ID);
			XLOG_INFO("@@@ CustomTableView::closeEvent, yHandleId.toStdString() = " + yHandleId.toStdString(), CURRENT_THREAD_ID);
		}
		else
		{
			XLOG_INFO("CustomTableView::closeEvent, xVariableName.toStdString() = " + xVariableName.toStdString(), CURRENT_THREAD_ID);
			XLOG_INFO("CustomTableView::closeEvent, yHandleId.toStdString() = " + yHandleId.toStdString(), CURRENT_THREAD_ID);
			XLOG_INFO("CustomTableView::closeEvent, yVariableName.toStdString() = " + yVariableName.toStdString(), CURRENT_THREAD_ID);
			XLOG_INFO("CustomTableView::closeEvent, yVariableType.toStdString() = " + yVariableType.toStdString(), CURRENT_THREAD_ID);
			SourceFrom soureFrom{ yHandleId.toStdString(),yVariableName.toStdString() };
			xHandle->setSourceFrom(xVariableName.toStdString(), soureFrom);
		}

	}
}
