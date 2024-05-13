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

ComboWindow::~ComboWindow()
{
	//qDebug() << " === ComboWindow::~ComboWindow() === ";
	XLOG_INFO(" === ComboWindow::~ComboWindow() === ", CURRENT_THREAD_ID);
	
	// ɾ�����ж�̬����Ķ���
	for (PreParamWidget* widget : widgets)
	{
		SAFE_DELETE(widget);
	}

	// �������
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
		widgets[i]->setVisible(false); // ��ʼ״̬�����ص�i��widget
		widgets[i]->setFixedWidth(widgetWidth); // ���õ�i��widgetΪ�̶����
		mLayout->addWidget(widgets[i]);
		i += 1;
	}
	comboBox->setCurrentIndex(0); // ��Ĭ��ѡ��������Ϊ��һ��
	selectedWidgetName = comboBox->currentText(); // update selectedWidgetName
	widgets[0]->setVisible(true); // ��ʼ״̬����ʾcomboBox��һ���Ӧ��widget

	// ���ò��ֵĶ��뷽ʽΪ����
	mLayout->setAlignment(Qt::AlignCenter);
	mLayout->setContentsMargins(0, 0, 0, 0); // ������߾�Ϊ0
	setLayout(mLayout);

	// ���ô�С����Ϊ�̶�
	comboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	// ���ù̶���С
	comboBox->setFixedSize(110, 30);  // �����110��30���Ը�����Ҫ����

	connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &ComboWindow::comboBoxIndexChanged);
}

void ComboWindow::setSelectItem(const PreParam& params)
{
	std::string itemStr = params.preTag;

	// ��std::stringת��ΪQString
	QString qItemStr = QString::fromStdString(itemStr);

	// �����ַ�����Ӧ������
	int index = comboBox->findText(qItemStr);

	// ����ҵ��˶�Ӧ������
	if (index != -1)
	{
		// ���õ�ǰѡ�е�����
		comboBox->setCurrentIndex(index);
		// update ComboWindow
		comboBoxIndexChanged(index);
		// ��ȡ comboBox ��ǰѡ������ı���Ϣ
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
	// ��ӡ comboBox ѡ�����Ӧ�� widget ������
	qDebug() << "Widget Index : " << index; // ������0��ʼ

	// ��ȡ comboBox ��ǰѡ������ı���Ϣ
	selectedWidgetName = comboBox->currentText();

	// ���������л�QWidget�Ŀɼ���
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
