#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include "MainWindow/VariableWidget.h"
#include "Core/Runtime/VarBag.h"
#include "HandleBase/XBaseHandle.h"
#include "ItemBase/XBaseItem.h"

namespace XVisual {

VariableWidget::VariableWidget(QWidget* parent) : QWidget(parent)
{
	// 创建树形控件
	treeWidget = new QTreeWidget(this);

	// 设置树形控件的列数
	treeWidget->setColumnCount(3);

	// 设置树形控件的头部信息
	QStringList headers;
	headers << "Variable Name" << "Type" << "Value";
	treeWidget->setHeaderLabels(headers);

	// 设置表头字体加粗
	QFont font = treeWidget->header()->font();
	font.setBold(true);
	treeWidget->header()->setFont(font);

	// 根据内容自适应变化
	treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

	// 创建垂直布局
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(treeWidget);

	// 设置布局到主窗口
	setLayout(layout);

	connect(treeWidget, &QTreeWidget::itemSelectionChanged, this, &VariableWidget::nodeSelectionChanged);
}

void VariableWidget::createTree(XBaseItem* xItem)
{
	XBaseHandle* xHandle = xItem->getXHandle();
	VarBag& sources = xHandle->getSources();
	VarBag& dests = xHandle->getDests();
	std::vector<std::string> sNames =  ACQUIRE_NAMES(sources);
	std::vector<std::string> dNames = ACQUIRE_NAMES(dests);
	XLOG_INFO("VariableWidget::createTree  sNames = " + std::to_string(sNames.size()), CURRENT_THREAD_ID);
	XLOG_INFO("VariableWidget::createTree  dNames = " + std::to_string(dNames.size()), CURRENT_THREAD_ID);

	// 获取当前 xItem 的名称
	const QString itemName = QString::fromStdString(xItem->getUniqueName());

	// 添加节点(第一级节点)，节点名称为 xItem 的名称
	itemNode = new QTreeWidgetItem(treeWidget);
	itemNode->setText(0, itemName);
	// 设置节点字体加粗
	QFont itemFont = itemNode->font(0);
	itemFont.setBold(true);
	itemNode->setFont(0, itemFont);

	// 添加 "Sources" 节点(第二级节点)
	sourcesNode = new QTreeWidgetItem(itemNode);
	sourcesNode->setText(0, "Sources");
	// 设置节点字体加粗
	QFont sourcesFont = sourcesNode->font(0);
	sourcesFont.setBold(true);
	sourcesNode->setFont(0, sourcesFont);

	// 添加 "Dests" 节点(第二级节点)
	destsNode = new QTreeWidgetItem(itemNode);
	destsNode->setText(0, "Dests");
	// 设置节点字体加粗
	QFont destsFont = destsNode->font(0);
	destsFont.setBold(true);
	destsNode->setFont(0, destsFont);

	// 添加记录到 "Sources" 节点
	for (int i = 0; i < sNames.size(); ++i)
	{
		std::string sName = sNames[i];
		const std::type_info* typeInfo = GET_MEMBER_TYPE_STR(sources, sName);
		std::string sTypeName = (*typeInfo).name();
		std::any sValue = GET_MEMBER_STR(sources, sName);
		QTreeWidgetItem* item = new QTreeWidgetItem(sourcesNode);
		item->setText(0, QString::fromStdString(sName));
		item->setText(1, QString::fromStdString(sTypeName));
	}

	// 添加记录到 "Dests" 节点
	for (int i = 0; i < dNames.size(); ++i)
	{
		std::string dName = dNames[i];
		const std::type_info* typeInfo = GET_MEMBER_TYPE_STR(dests, dName);
		std::string dTypeName = (*typeInfo).name();
		std::any dValue = GET_MEMBER_STR(dests, dName);
		QTreeWidgetItem* item = new QTreeWidgetItem(destsNode);
		item->setText(0, QString::fromStdString(dName));
		item->setText(1, QString::fromStdString(dTypeName));
	}
}

void VariableWidget::nodeSelectedSlot(const QString& itemName)
{
	// 使用findItems方法查找匹配的项，第一个参数是搜索的文本，第三个参数是搜索的列
	QList<QTreeWidgetItem*> items = treeWidget->findItems(itemName, Qt::MatchExactly, 0);
	// 遍历找到的项，展开它们
	for (QTreeWidgetItem* item : items) 
	{
		treeWidget->expandItem(item);
	}
}

void VariableWidget::nodeChangedSlot(const QString& itemName)
{
	nodeSelectedSlot(itemName);
}

void VariableWidget::nodeSelectionChanged()
{
	//treeWidget->selectedItems()返回一个包含当前选中项的QList<QTreeWidgetItem*>对象。
	//这个函数会返回所有当前选中的项，因为QTreeWidget可以允许多个项同时被选中
	QList<QTreeWidgetItem*> selectedItems = treeWidget->selectedItems();
	if (!selectedItems.isEmpty())
	{
		QTreeWidgetItem* currentItem = selectedItems.first();
		QString itemName = currentItem->text(0);
		emit nodeToggledSignal(itemName);
	}
}

} // namespace XVisual
