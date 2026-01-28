#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "MainWindow/NamesWidget.h"
#include "MainWindow/RowWidget.h"

namespace XVisual {

NamesWidget::NamesWidget(QWidget* parent) :QWidget(parent)
{
	qDebug() << "CustomListWidget::CustomListWidget Initializating ... ";
	titleLabel = new QLabel("File Name List");
	// 设置titleLabel的字体为粗体
	QFont boldFont = titleLabel->font();
	boldFont.setBold(true);
	titleLabel->setFont(boldFont);
	// 创建QListWidget
	listWidget = new QListWidget();
	// 创建水平布局
	layout = new QVBoxLayout();
	layout->addWidget(titleLabel, 0, Qt::AlignTop | Qt::AlignLeft);
	layout->addWidget(listWidget);
	// 设置layout的外部边距
	layout->setContentsMargins(0, 0, 0, 0);
	// 设置layout的内部边距
	layout->setSpacing(0);
	// 设置布局到主窗口
	setLayout(layout);
}

void NamesWidget::addRow(const QString& text)
{
	rowWidget = new RowWidget(text);
	connect(rowWidget, &RowWidget::tabInsertSignal, this, &NamesWidget::tabInsertSlot);
	connect(rowWidget, &RowWidget::tabDeleteSignal, this, &NamesWidget::tabDeleteSlot);
	//在listWidget中添加一个新创建的QListWidgetItem项，并将该项与rowWidget关联起来。
	widgetItem = new QListWidgetItem();
	listWidget->addItem(widgetItem);
	listWidget->setItemWidget(widgetItem, rowWidget);
	XLOG_INFO("NamesWidget::addRow,text = " + text.toStdString(), CURRENT_THREAD_ID);
	connect(listWidget, &QListWidget::itemClicked, this, &NamesWidget::onListWidgetItemClicked);
}

void NamesWidget::onListWidgetItemClicked(QListWidgetItem* item)
{
	QWidget* tempWidget = nullptr;
	RowWidget* rWidget = nullptr;
	tempWidget = listWidget->itemWidget(item);
	if(tempWidget!=nullptr)
	{
		//const char* class_name = tempWidget->metaObject()->className();
		//std::string classNameStr(class_name);
		//XLOG_INFO("tempWidget type: " + classNameStr , CURRENT_THREAD_ID);
	    rWidget = qobject_cast<RowWidget*>(tempWidget);
		if (rWidget != nullptr) 
		{
			//const char* class_name2 = rWidget->metaObject()->className();
			//std::string classNameStr2(class_name2);
			//XLOG_INFO("rowWidget type: " + classNameStr2, CURRENT_THREAD_ID);
			QString itemName = rWidget->getText();
			//QString itemName = rWidget->getLabel()->text();
			XLOG_INFO("NamesWidget::onListWidgetItemClicked, current selected is " + itemName.toStdString(), CURRENT_THREAD_ID);
			emit itemSelectedSignal(itemName);
		}
	}
}

void NamesWidget::tabInsertSlot(const QString& itemName)
{
	emit tabAddSignal(itemName);
}

void NamesWidget::tabDeleteSlot(const QString& itemName)
{
	emit tabRemoveSignal(itemName);
}

void NamesWidget::itemChangedSlot(const QString& itemName)
{
	// 遍历QListWidget中的每个item
	for (int i = 0; i < listWidget->count(); ++i)
	{
		QWidget* indexWidget = nullptr;
		RowWidget* rrWidget = nullptr;
		QListWidgetItem* indexItem = listWidget->item(i);
		indexWidget = listWidget->itemWidget(indexItem);
		rrWidget = qobject_cast<RowWidget*>(indexWidget);
		QString rrItemName = rrWidget->getText();
		// 检查当前rrItemName的文本是否与目标文本itemName匹配
		if (itemName == rrItemName)
		{
			listWidget->setCurrentRow(i);
			break;  
		}
	}
}

void NamesWidget::itemToggledSlot(const QString& itemName)
{
	itemChangedSlot(itemName);
}

} // namespace XVisual
