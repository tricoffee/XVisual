#include "Common/LoggerInstance.h"
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include "Common/OpenCVHeaders.h"
#include "Common/XThreadMacro.h"
#include "MainWindow/ImagePageWidget.h"
#include "ItemBase/XBaseItem.h"

ImagePageWidget::ImagePageWidget(QWidget* parent) :QWidget(parent)
{
	tabWidget = new QTabWidget();
	tabWidget->setTabShape(QTabWidget::Triangular);
	tabWidget->setTabPosition(QTabWidget::South);
	tabWidget->setMovable(true);
	tabWidget->setTabsClosable(true);

	gridLayout = new QGridLayout();
	gridLayout->addWidget(tabWidget);

	setLayout(gridLayout);

	connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	connect(tabWidget, &QTabWidget::currentChanged, this, &ImagePageWidget::tabCurrentSlot);
}

QTabWidget* ImagePageWidget::getTabWidget()
{
	return tabWidget;
}

void ImagePageWidget::closeTab(int index)
{
	qDebug() << "closeTab" << index;
	qDebug() << qobject_cast<QTabWidget*>(tabWidget);
	tabWidget->removeTab(index);
}

void ImagePageWidget::tabSelectedSlot(const QString& itemName)
{
	int tabIndex = -1;

	// ������ǩҳ�������� itemName ƥ��ı�ǩҳ
	for (int i = 0; i < tabWidget->count(); ++i) 
	{
		if (tabWidget->tabText(i) == itemName) 
		{
			tabIndex = i;
			break;
		}
	}

	if (tabIndex != -1) 
	{
		// ��������Խ�һ�������Ѿ����ڵı�ǩҳ
		tabWidget->setCurrentIndex(tabIndex);
		//qDebug() << "Tab with name " << itemName << " exists at index " << tabIndex;

	}
	else 
	{
		// ��������Դ����ǩҳ�����ڵ����
		tabWidget->setCurrentIndex(-1);
		//qDebug() << "Tab with name " << itemName << " does not exist";
	}
}

void ImagePageWidget::tabCurrentSlot(int index)
{
	//QWidget* tempWidget = tabWidget->widget(index);
	//if (tempWidget != nullptr)
	//{
	//}

	// ʹ��������ȡ��Ӧ��ǩҳ�ı�ǩ
	QString currentTabLabel = tabWidget->tabText(index);
	emit tabChangedSignal(currentTabLabel);
}

void ImagePageWidget::tabToggledSlot(const QString& itemName)
{
	tabSelectedSlot(itemName);
}