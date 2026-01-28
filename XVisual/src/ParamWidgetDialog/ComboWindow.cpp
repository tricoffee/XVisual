#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "ParamWidgetDialog/ComboWindow.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <string>
#include "ParamWidget/ResizeParamWidget.h"
#include "ParamWidget/ResizePasteParamWidget.h"
#include "ParamWidget/Normalize01ParamWidget.h"
#include "Common/DeletePtrMacro.h"

namespace XVisual {

ComboWindow::~ComboWindow()
{
	//qDebug() << " === ComboWindow::~ComboWindow() === ";
	XLOG_INFO(" === ComboWindow::~ComboWindow() === ", CURRENT_THREAD_ID);
	
	// 删除所有动态分配的对象
	for (PreParamWidget* widget : widgets)
	{
		SAFE_DELETE(widget);
	}

	// 清空向量
	widgets.clear();
	SAFE_DELETE(comboBox);
	SAFE_DELETE(mLayout);
}

ComboWindow::ComboWindow(QWidget* parent) : QWidget(parent)
{
	//qDebug() << " @@@ ComboWindow::ComboWindow() @@@ ";
	XLOG_INFO(" @@@ ComboWindow::ComboWindow() @@@ ", CURRENT_THREAD_ID);

	comboBox = new QComboBox();
	mLayout = new QHBoxLayout();
	mLayout->addWidget(comboBox);
	int widgetWidth = 180;
	int i = 0;
	for (const auto& itemStr : itemStrList)
	{
		comboBox->addItem(QString::fromStdString(itemStr));
		widgets.push_back(ParamWidgetRegistry::createObject(itemStr));
		widgets[i]->setVisible(false); // 初始状态下隐藏第i个widget
		widgets[i]->setFixedWidth(widgetWidth); // 设置第i个widget为固定宽度
		mLayout->addWidget(widgets[i]);
		i += 1;
	}
	comboBox->setCurrentIndex(0); // 将默认选中项设置为第一项
	selectedWidgetName = comboBox->currentText(); // update selectedWidgetName
	widgets[0]->setVisible(true); // 初始状态下显示comboBox第一项对应的widget

	// 设置布局的对齐方式为居中
	mLayout->setAlignment(Qt::AlignCenter);
	mLayout->setContentsMargins(0, 0, 0, 0); // 设置外边距为0
	setLayout(mLayout);

	// 设置大小策略为固定
	comboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	// 设置固定大小
	comboBox->setFixedSize(110, 30);  // 这里的110和30可以根据需要调整

	connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &ComboWindow::comboBoxIndexChanged);
}

void ComboWindow::setSelectItem(const PreParam& params)
{
	std::string itemStr = params.preTag;

	// 将std::string转换为QString
	QString qItemStr = QString::fromStdString(itemStr);

	// 查找字符串对应的索引
	int index = comboBox->findText(qItemStr);

	// 如果找到了对应的索引
	if (index != -1)
	{
		// 设置当前选中的索引
		comboBox->setCurrentIndex(index);
		// update ComboWindow
		comboBoxIndexChanged(index);
		// 获取 comboBox 当前选中项的文本信息
		selectedWidgetName = comboBox->currentText();
		if( (selectedWidgetName == "Resize") || (selectedWidgetName == "ResizePaste") || (selectedWidgetName == "Normalize01"))
		{
			// update widget corresponding to current selected comboBox-Item
			widgets[comboBox->currentIndex()]->setPreParam(params);
		}
	}
}

void ComboWindow::comboBoxIndexChanged(int index)
{
	// 打印 comboBox 选中项对应的 widget 的索引
	qDebug() << "Widget Index : " << index; // 索引从0开始

	// 获取 comboBox 当前选中项的文本信息
	selectedWidgetName = comboBox->currentText();

	// 根据索引切换QWidget的可见性
	for (int i = 0; i < widgets.size(); ++i)
	{
		widgets[i]->setVisible(i == index);
	}

	//printSelectedParam();
}

void ComboWindow::getSelectedParam(PreParam& params)
{

	// Implementing function "getPreParam" call through virtual function table
	if (("Resize" == selectedWidgetName) || ("ResizePaste" == selectedWidgetName) || ("Normalize01" == selectedWidgetName))
	{

		widgets[comboBox->currentIndex()]->getPreParam(params);
	}
	else
	{
		XLOG_INFO(" @@@ ComboWindow::getSelectedParam @@@ " + selectedWidgetName.toStdString(), CURRENT_THREAD_ID);
	}

}

void ComboWindow::printSelectedParam()
{
	PreParam params;
	if (selectedWidgetName == "Resize")
	{
		static_cast<ResizePasteParamWidget*>(widgets[comboBox->currentIndex()])->getPreParam(params);
		std::string preTagStr = params.preTag;
		//qDebug() << " *** *** *** " << QString::fromStdString(preTagStr) << " *** *** *** ";
		XLOG_INFO(" *** *** *** " + preTagStr + " *** *** *** ", CURRENT_THREAD_ID);
		for (auto& e : params.params)
		{
			int width_or_height = std::any_cast<int>(e.second);
			//qDebug() << " *** *** *** " << QString::fromStdString(e.first) << ", " << width_or_height << " *** *** *** ";
			XLOG_INFO(" *** *** *** " + e.first + ", " + std::to_string(width_or_height) + " *** *** *** ", CURRENT_THREAD_ID);
		}
	}
	else if (selectedWidgetName == "ResizePaste")
	{
		static_cast<ResizeParamWidget*>(widgets[comboBox->currentIndex()])->getPreParam(params);
		std::string preTagStr = std::any_cast<std::string>(params.preTag);
		//qDebug() << " *** *** *** " << QString::fromStdString(preTagStr) << " *** *** *** ";
		XLOG_INFO(" *** *** *** " + preTagStr + " *** *** *** ", CURRENT_THREAD_ID);
		for (auto& e : params.params)
		{
			int width_or_height = std::any_cast<int>(e.second);
			//qDebug() << " *** *** *** " << QString::fromStdString(e.first) << ", " << width_or_height << " *** *** *** ";
			XLOG_INFO(" *** *** *** " + e.first + ", " + std::to_string(width_or_height) + " *** *** *** ", CURRENT_THREAD_ID);
		}
	}
	else if (selectedWidgetName == "Normalize01")
	{
		static_cast<Normalize01ParamWidget*>(widgets[comboBox->currentIndex()])->getPreParam(params);
		std::string preTagStr = params.preTag;
		//qDebug() << " *** *** *** " << QString::fromStdString(preTagStr) << " *** *** *** ";
		XLOG_INFO(" *** *** *** " + preTagStr + " *** *** *** ", CURRENT_THREAD_ID);
		for (auto& e : params.params)
		{
			float scale = std::any_cast<float>(e.second);
			//qDebug() << " *** *** *** " << QString::fromStdString(e.first) << ", " << scale << " *** *** *** ";
			XLOG_INFO(" *** *** *** " + e.first + ", " + std::to_string(scale) + " *** *** *** ", CURRENT_THREAD_ID);
		}
	}
}

} // namespace XVisual