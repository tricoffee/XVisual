#include "MainWindow/GraphicsWidget.h"
#include "MainWindow/diagramscene.h"
#include "TableWidget/LabelDelegate.h"
#include "TableWidget/ComboBoxDelegate.h"
#include "TableWidget/CustomTableView.h"
#include "ItemBase/XArrow.h"
#include "ItemBase/XBaseItem.h"
#include <QWidget>
#include <QGridLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QHeaderView>

namespace XVisual {

GraphicsWidget::GraphicsWidget(QWidget* parent, Qt::WindowFlags)
	:QWidget(parent), itemMenu(nullptr)
{
	//setStyleSheet("background:transparent;border:none");

	scene_width = 5000;
	scene_height = 5000;
	// Scene initialized with nullptr menu, menu must be set later
	scene = new DiagramScene(itemMenu, this);
	scene->setSceneRect(QRectF(0, 0, scene_width, scene_height));

	view = new QGraphicsView(scene);
	view->setMouseTracking(true);
	scene->setView(view);

	// set layout for this GraphicsWidget and QGraphicsView
	QGridLayout* layout = new QGridLayout();
	layout->addWidget(view);
	setLayout(layout);
}

GraphicsWidget::GraphicsWidget(QMenu* myItemMenu, QWidget* parent, Qt::WindowFlags)
	:QWidget(parent), itemMenu(myItemMenu)
{
	//setStyleSheet("background:transparent;border:none");

	scene_width = 5000;
	scene_height = 5000;
	scene = new DiagramScene(itemMenu, this);
	scene->setSceneRect(QRectF(0, 0, scene_width, scene_height));

	view = new QGraphicsView(scene);
	view->setMouseTracking(true);
	scene->setView(view);

	// set layout for this GraphicsWidget and QGraphicsView
	QGridLayout* layout = new QGridLayout();
	layout->addWidget(view);
	setLayout(layout);
}

void GraphicsWidget::setItemMenu(QMenu* myItemMenu)
{
	itemMenu = myItemMenu;
	if(scene) {
		scene->setItemMenu(itemMenu);
	}
}

void GraphicsWidget::setBackgroundBrushChange(const QBrush& brush)
{
	scene->setBackgroundBrush(brush);
}

void GraphicsWidget::setSceneScaleChange(const QString& scale)
{
	double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
	QTransform oldMatrix = view->transform();
	view->resetTransform();
	view->translate(oldMatrix.dx(), oldMatrix.dy());
	view->scale(newScale, newScale);
}

void GraphicsWidget::setItemTextColor(const QColor& color)
{
	scene->setTextColor(color);
}

void GraphicsWidget::setSceneItemColor(const QColor& color)
{
	scene->setItemColor(color);
}

void GraphicsWidget::setSceneLineColor(const QColor& color)
{
	scene->setLineColor(color);
}

void GraphicsWidget::setSceneFont(const QFont& font)
{
	scene->setFont(font);
}

void GraphicsWidget::bringToFront()
{
	if (scene->selectedItems().isEmpty())
		return;

	QGraphicsItem* selectedItem = scene->selectedItems().first();
	const QList<QGraphicsItem*> overlapItems = selectedItem->collidingItems();

	qreal zValue = 0;
	for (const QGraphicsItem* item : overlapItems)
	{
		if (item->zValue() >= zValue && item->type() == XBaseItem::Type)
			zValue = item->zValue() + 0.1;
	}
	selectedItem->setZValue(zValue);
}

void GraphicsWidget::sendToBack()
{
	if (scene->selectedItems().isEmpty())
		return;

	QGraphicsItem* selectedItem = scene->selectedItems().first();
	const QList<QGraphicsItem*> overlapItems = selectedItem->collidingItems();

	qreal zValue = 0;
	for (const QGraphicsItem* item : overlapItems)
	{
		if (item->zValue() <= zValue && item->type() == XBaseItem::Type)
			zValue = item->zValue() - 0.1;
	}
	selectedItem->setZValue(zValue);
}

void GraphicsWidget::deleteItem()
{
	QList<QGraphicsItem*> selectedItems = scene->selectedItems();
	for (QGraphicsItem* item : std::as_const(selectedItems))
	{
		if (XArrow::Type == item->type())
		{
			scene->removeItem(item);
			XArrow* arrow = qgraphicsitem_cast<XArrow*>(item);
			arrow->getStartItem()->removeArrow(arrow);
			arrow->getEndItem()->removeArrow(arrow);
			delete item;
		}
		else if (XBaseItem::Type == item->type())
		{
			qgraphicsitem_cast<XBaseItem*>(item)->removeArrows();
			scene->removeItem(item);
			delete item;
		}
	}
}

void GraphicsWidget::setDiagramName(const QString& name)
{
	scene->setDiagramName(name);
}

void GraphicsWidget::setDiagramType(DiagramType type)
{
	scene->setDiagramType(type);
}

void GraphicsWidget::setDiagramState(DiagramState state)
{
	scene->setDiagramState(state);
}

DiagramScene*& GraphicsWidget::getScene()
{
	return scene;
}

void GraphicsWidget::showTableViewSlot(const QString& xName, const QString& yName, XArrow* xLineArrow,
	QList<TableData> variablesX, QList<TableData> variablesY)
{
	QStandardItemModel* lastModel = xLineArrow->getLatestTableViewModel();

	// 必须确保 defaultValues 的长度等于 variablesX 的长度
	// must ensure the length of defaultValues is equal to the length of variablesX
	std::unordered_map<std::string, TableData>& defaultValues = xLineArrow->getDefaultYforVariablesX();

	XLOG_INFO("GraphicsWidget::showTableViewSlot, defaultValues.size() = " + std::to_string(defaultValues.size()), CURRENT_THREAD_ID);

	tableView = new CustomTableView(xLineArrow);
	// 设置对象名称为 "tableView"
	tableView->setObjectName("tableView");

	// Create a QStandardItemModel
	QStandardItemModel* model = new QStandardItemModel(variablesX.length(), 2);

	// Set the header for the table
	model->setHeaderData(0, Qt::Horizontal, xName);
	model->setHeaderData(1, Qt::Horizontal, yName);

	// Add variables of node X to the table
	for (int i = 0; i < variablesX.length(); i++)
	{
		model->setData(model->index(i, 0), QString::fromStdString(variablesX[i].name + "\n" + variablesX[i].type));
	}

	// Set the custom delegate for the first column
	LabelDelegate* labelDelegate = new LabelDelegate();
	tableView->setItemDelegateForColumn(0, labelDelegate);

	// Set the ComboBoxDelegate for column 1
	ComboBoxDelegate* comboBoxDelegate = new ComboBoxDelegate(variablesY);
	tableView->setItemDelegateForColumn(1, comboBoxDelegate);

	// Set the Model for tableView
	tableView->setModel(model);

	tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	// Set default values for the second column (variablesX)
	for (int rowIndex = 0; rowIndex < variablesX.length(); ++rowIndex)
	{
		TableData xVar = variablesX[rowIndex];
		std::string xName = xVar.name;
		if (!defaultValues.empty())
		{
			// 检查字符串 xName 是否是一个键
			auto it = defaultValues.find(xName);
			if (it != defaultValues.end())
			{
				TableData yVar = defaultValues[xName];
				QString defaultText = QString::fromStdString(yVar.name + "\n" + yVar.type);
				model->setData(model->index(rowIndex, 1), defaultText);
			}
		}
	}

	// Clear defaultValues before loading the JSON rendering visualization scheme
	if (!defaultValues.empty())
	{
		defaultValues.clear();
	}

	// 恢复上次使用的数据模型状态
	if (lastModel) // 因为第一次加载时 lastModel 为 nullptr, 所以这个片段的代码不会被执行
	{
		// 复制上次模型的数据到当前模型
		for (int row = 0; row < lastModel->rowCount(); ++row)
		{
			QModelIndex lastModelIndex = lastModel->index(row, 1); // 第二列的索引
			if (lastModelIndex.isValid())
			{
				QString selectedText = lastModelIndex.data().toString();
				QModelIndex currentIndex = model->index(row, 1); // 当前模型对应的索引
				model->setData(currentIndex, selectedText); // 设置当前模型的 ComboBox 选中项
			}
		}
	}

	// 保存当前数据模型到 lastModel, 设置 xLineArrow 的 latestTableViewModel 就是设置这里的局部变量 lastModel
	xLineArrow->setLatestTableViewModel(model); // 等同于更新局部变量 lastModel

	tableView->show();
}

void GraphicsWidget::showImageSlot(const std::string& filename, const cv::Mat& image, XBaseItem* item)
{
	emit showImageInTabSignal(filename, image, item);
}

} // namespace XVisual
