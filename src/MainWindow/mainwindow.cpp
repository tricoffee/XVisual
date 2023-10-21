// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include <QtWidgets>
#include <QString>
#include <string>

#include "ItemBase/XArrow.h"
#include "ItemBase/XBaseItem.h"
#include "MainWindow/diagramscene.h"
#include "MainWindow/mainwindow.h"
#include "Common/constants.h"
#include "TableWidget/LabelDelegate.h"
#include "TableWidget/ComboBoxDelegate.h"
#include "TableWidget/CustomTableView.h"

MainWindow::MainWindow()
{
	createActions();
	createMenus();
	createToolbars();
	createToolBox();
	scene_width = 5000;
	scene_height = 5000;
	scene = new DiagramScene(itemMenu, this);
	scene->setSceneRect(QRectF(0, 0, scene_width, scene_height));

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(toolBox);

	view = new QGraphicsView(scene);
	view->setMouseTracking(true);
	layout->addWidget(view);
	scene->setView(view);

	QWidget* widget = new QWidget;
	widget->setLayout(layout);

	setCentralWidget(widget);
	setWindowTitle(tr("Diagramscene"));
	setUnifiedTitleAndToolBarOnMac(true);

}

void MainWindow::backgroundButtonGroupClicked(QAbstractButton* button)
{
	button->setChecked(false);
	const int id = backgroundButtonGroup->id(button);
	if (BackGroundType::BlueGrid == id)
	{
		scene->setBackgroundBrush(QPixmap(ImageSources::Background1));
	}
	else if (BackGroundType::WhiteGrid == id)
	{
		scene->setBackgroundBrush(QPixmap(ImageSources::Background2));
	}
	else if (BackGroundType::GrayGrid == id)
	{
		scene->setBackgroundBrush(QPixmap(ImageSources::Background3));
	}
	else
	{
		scene->setBackgroundBrush(QPixmap(ImageSources::Background4));
	}
	scene->update();
	view->update();
}

void MainWindow::buttonGroupClicked(QAbstractButton* button)
{
	button->setChecked(false);
	const int id = buttonGroup->id(button);
	scene->setDiagramState(DiagramState::Insert);
	scene->setDiagramType(DiagramType::Item);
	scene->setDiagramName(idnames[id]);
}

void MainWindow::deleteItem()
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

void MainWindow::linePointerButtonClicked(bool checked)
{
	linePointerButton->setChecked(false);
	scene->setDiagramState(DiagramState::Insert);
	scene->setDiagramType(DiagramType::Line);
}

void MainWindow::bringToFront()
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

void MainWindow::sendToBack()
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

void MainWindow::currentFontChanged(const QFont&)
{
	handleFontChange();
}

void MainWindow::fontSizeChanged(const QString&)
{
	handleFontChange();
}

void MainWindow::sceneScaleChanged(const QString& scale)
{
	double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
	QTransform oldMatrix = view->transform();
	view->resetTransform();
	view->translate(oldMatrix.dx(), oldMatrix.dy());
	view->scale(newScale, newScale);
}

/*
单击fontColorToolButton的弹出菜单的某一个QAction时,
slot函数MainWindow::textColorChanged响应, fontColorToolButton的图标(Icon)有相应变化,
同时场景scene中text的背景色有相应变化
*/
void MainWindow::textColorChanged()
{
	textAction = qobject_cast<QAction*>(sender());
	fontColorToolButton->setIcon(createColorToolButtonIcon(
		ImageSources::TextPointer,
		qvariant_cast<QColor>(textAction->data())));
	//去掉原来的textButtonTriggered();直接把textButtonTriggered()里面的这一句拿过来，如下：
	//这段代码表示设置场景scene中text的背景色
	scene->setTextColor(qvariant_cast<QColor>(textAction->data()));
}

/*
单击fillColorToolButton的弹出菜单的某一个QAction时,
slot函数MainWindow::itemColorChanged响应, fillColorToolButton的图标(Icon)有相应变化,
同时场景scene中item的颜色有相应变化
*/
void MainWindow::itemColorChanged()
{
	fillAction = qobject_cast<QAction*>(sender());
	fillColorToolButton->setIcon(createColorToolButtonIcon(
		ImageSources::FloodFill,
		qvariant_cast<QColor>(fillAction->data())));
	//去掉原来的fillButtonTriggered();直接把fillButtonTriggered里面的这一句拿过来，如下:
	//这段代码表示设置场景scene中item的颜色
	scene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
}

/*
单击lineColorToolButton的弹出菜单的某一个QAction时,
slot函数MainWindow::lineColorChanged响应, lineColorToolButton的图标(Icon)有相应变化,
同时场景scene中line的颜色有相应变化
*/
void MainWindow::lineColorChanged()
{
	lineAction = qobject_cast<QAction*>(sender());
	lineColorToolButton->setIcon(createColorToolButtonIcon(
		ImageSources::LineColor,
		qvariant_cast<QColor>(lineAction->data())));
	//去掉原来的lineButtonTriggered();直接把lineButtonTriggered里面的这一句拿过来，如下:
	//这段代码表示设置场景scene中line的颜色
	scene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
}

void MainWindow::handleFontChange()
{
	QFont font = fontCombo->currentFont();
	font.setPointSize(fontSizeCombo->currentText().toInt());
	font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
	font.setItalic(italicAction->isChecked());
	font.setUnderline(underlineAction->isChecked());
	scene->setFont(font);
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About Diagram Scene"),
		tr("The <b>Diagram Scene</b> example shows "
			"use of the graphics framework."));
}

void MainWindow::createToolBox()
{
	buttonGroup = new QButtonGroup(this);
	buttonGroup->setExclusive(false);
	connect(buttonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
		this, &MainWindow::buttonGroupClicked);

	QGridLayout* layout = new QGridLayout;
	layout->addWidget(createCellWidget(tr("Condition")), 0, 0);
	layout->addWidget(createCellWidget(tr("Step")), 0, 1);
	layout->addWidget(createCellWidget(tr("Input")), 1, 0);
	layout->addWidget(createCellWidget(tr("Output")), 1, 1);
	layout->addWidget(createCellWidget(tr("LoadImage")), 2, 0);
	layout->addWidget(createCellWidget(tr("CVCrop")), 2, 1);
	layout->setRowStretch(3, 10);
	layout->setColumnStretch(2, 10);

	QWidget* itemWidget = new QWidget;
	itemWidget->setLayout(layout);

	backgroundButtonGroup = new QButtonGroup(this);
	connect(backgroundButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
		this, &MainWindow::backgroundButtonGroupClicked);

	QGridLayout* backgroundLayout = new QGridLayout;

	backgroundLayout->addWidget(createBackgroundCellWidget(tr("Blue Grid"), BackGroundType::BlueGrid,
		ImageSources::Background1), 0, 0);
	backgroundLayout->addWidget(createBackgroundCellWidget(tr("White Grid"), BackGroundType::WhiteGrid,
		ImageSources::Background2), 0, 1);
	backgroundLayout->addWidget(createBackgroundCellWidget(tr("Gray Grid"), BackGroundType::GrayGrid,
		ImageSources::Background3), 1, 0);
	backgroundLayout->addWidget(createBackgroundCellWidget(tr("No Grid"), BackGroundType::NoGrid,
		ImageSources::Background4), 1, 1);


	backgroundLayout->setRowStretch(2, 10);
	backgroundLayout->setColumnStretch(2, 10);

	QWidget* backgroundWidget = new QWidget;
	backgroundWidget->setLayout(backgroundLayout);

	toolBox = new QToolBox;
	toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	toolBox->setMinimumWidth(itemWidget->sizeHint().width());
	toolBox->addItem(itemWidget, tr("Basic Flowchart Shapes"));
	toolBox->addItem(backgroundWidget, tr("Backgrounds"));

}

void MainWindow::createActions()
{
	toFrontAction = new QAction(QIcon(ImageSources::BringFront),
		tr("Bring to &Front"), this);
	toFrontAction->setShortcut(tr("Ctrl+F"));
	toFrontAction->setStatusTip(tr("Bring item to front"));
	connect(toFrontAction, &QAction::triggered, this, &MainWindow::bringToFront);

	sendBackAction = new QAction(QIcon(ImageSources::SendBack), tr("Send to &Back"), this);
	sendBackAction->setShortcut(tr("Ctrl+T"));
	sendBackAction->setStatusTip(tr("Send item to back"));
	connect(sendBackAction, &QAction::triggered, this, &MainWindow::sendToBack);

	deleteAction = new QAction(QIcon(ImageSources::Delete), tr("&Delete"), this);
	deleteAction->setShortcut(tr("Delete"));
	deleteAction->setStatusTip(tr("Delete item from diagram"));
	connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteItem);

	exitAction = new QAction(tr("E&xit"), this);
	exitAction->setShortcuts(QKeySequence::Quit);
	exitAction->setStatusTip(tr("Quit Scenediagram example"));
	connect(exitAction, &QAction::triggered, this, &QWidget::close);

	boldAction = new QAction(QIcon(ImageSources::Bold), tr("Bold"), this);
	boldAction->setCheckable(true);
	boldAction->setShortcut(tr("Ctrl+B"));
	connect(boldAction, &QAction::triggered, this, &MainWindow::handleFontChange);

	italicAction = new QAction(QIcon(ImageSources::Italic), tr("Italic"), this);
	italicAction->setCheckable(true);
	italicAction->setShortcut(tr("Ctrl+I"));
	connect(italicAction, &QAction::triggered, this, &MainWindow::handleFontChange);

	underlineAction = new QAction(QIcon(ImageSources::Underline), tr("Underline"), this);
	underlineAction->setCheckable(true);
	underlineAction->setShortcut(tr("Ctrl+U"));
	connect(underlineAction, &QAction::triggered, this, &MainWindow::handleFontChange);

	aboutAction = new QAction(tr("A&bout"), this);
	aboutAction->setShortcut(tr("F1"));
	connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(exitAction);

	itemMenu = menuBar()->addMenu(tr("&Item"));
	itemMenu->addAction(deleteAction);
	itemMenu->addSeparator();
	itemMenu->addAction(toFrontAction);
	itemMenu->addAction(sendBackAction);

	aboutMenu = menuBar()->addMenu(tr("&Help"));
	aboutMenu->addAction(aboutAction);
}

void MainWindow::createToolbars()
{

	editToolBar = addToolBar(tr("Edit"));
	editToolBar->addAction(deleteAction);
	editToolBar->addAction(toFrontAction);
	editToolBar->addAction(sendBackAction);

	fontCombo = new QFontComboBox();
	connect(fontCombo, &QFontComboBox::currentFontChanged,
		this, &MainWindow::currentFontChanged);

	fontSizeCombo = new QComboBox;
	fontSizeCombo->setEditable(true);
	for (int i = 8; i < 30; i = i + 2)
		fontSizeCombo->addItem(QString().setNum(i));
	fontSizeCombo->setCurrentIndex(5);
	QIntValidator* validator = new QIntValidator(2, 64, this);
	fontSizeCombo->setValidator(validator);
	connect(fontSizeCombo, &QComboBox::currentTextChanged,
		this, &MainWindow::fontSizeChanged);

	fontColorToolButton = new QToolButton;
	fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
	textAction = fontColorToolButton->menu()->defaultAction();
	fontColorToolButton->setIcon(createColorToolButtonIcon(ImageSources::TextPointer, Qt::black));
	fontColorToolButton->setAutoFillBackground(true);

	fillColorToolButton = new QToolButton;
	fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
	fillAction = fillColorToolButton->menu()->defaultAction();
	fillColorToolButton->setIcon(createColorToolButtonIcon(ImageSources::FloodFill, Qt::white));

	lineColorToolButton = new QToolButton;
	lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
	lineAction = lineColorToolButton->menu()->defaultAction();
	lineColorToolButton->setIcon(createColorToolButtonIcon(ImageSources::LineColor, Qt::black));
	textToolBar = addToolBar(tr("Font"));
	textToolBar->addWidget(fontCombo);
	textToolBar->addWidget(fontSizeCombo);
	textToolBar->addAction(boldAction);
	textToolBar->addAction(italicAction);
	textToolBar->addAction(underlineAction);

	colorToolBar = addToolBar(tr("Color"));
	colorToolBar->addWidget(fontColorToolButton);
	colorToolBar->addWidget(fillColorToolButton);
	colorToolBar->addWidget(lineColorToolButton);

	linePointerButton = new QToolButton;
	linePointerButton->setCheckable(true);
	linePointerButton->setChecked(false);
	linePointerButton->setIcon(QIcon(ImageSources::LinePointer));
	connect(linePointerButton, &QAbstractButton::toggled,this,&MainWindow::linePointerButtonClicked);

	sceneScaleCombo = new QComboBox;
	QStringList scales;
	scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
	sceneScaleCombo->addItems(scales);
	sceneScaleCombo->setCurrentIndex(2);
	connect(sceneScaleCombo, &QComboBox::currentTextChanged,
		this, &MainWindow::sceneScaleChanged);

	pointerToolbar = addToolBar(tr("Pointer type"));
	pointerToolbar->addWidget(linePointerButton);
	pointerToolbar->addWidget(sceneScaleCombo);
}


//BackGroundType
QWidget* MainWindow::createBackgroundCellWidget(const QString& text,
	BackGroundType type, const QString& image)
{
	QToolButton* button = new QToolButton;
	button->setIcon(QIcon(image));
	button->setIconSize(QSize(50, 50));
	button->setCheckable(true);
	backgroundButtonGroup->addButton(button, int(type));

	QGridLayout* layout = new QGridLayout;
	layout->addWidget(button, 0, 0, Qt::AlignHCenter);
	layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

	QWidget* widget = new QWidget;
	widget->setLayout(layout);

	return widget;
}

QWidget* MainWindow::createCellWidget(const QString& text)
{
	idnames[itemtype] = text;
	std::string itemclass = text.toStdString();
	XBaseItem  *item = ItemRegistry::createObject(itemclass, itemMenu, nullptr);
	QIcon icon(item->image());

	QToolButton* button = new QToolButton;
	button->setIcon(icon);
	button->setIconSize(QSize(50, 50));
	button->setCheckable(true);
	buttonGroup->addButton(button, itemtype);

	itemtype += 1;

	QGridLayout* layout = new QGridLayout;
	layout->addWidget(button, 0, 0, Qt::AlignHCenter);
	layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

	QWidget* widget = new QWidget;
	widget->setLayout(layout);

	return widget;
}

QMenu* MainWindow::createColorMenu(const char* slot, QColor defaultColor)
{
	QList<QColor> colors;
	colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
	QStringList names;
	names << tr("black") << tr("white") << tr("red") << tr("blue")
		<< tr("yellow");

	QMenu* colorMenu = new QMenu(this);
	for (int i = 0; i < colors.count(); ++i)
	{
		QAction* action = new QAction(names.at(i), this);
		action->setData(colors.at(i));
		action->setIcon(createColorIcon(colors.at(i)));
		connect(action, SIGNAL(triggered()), this, slot);
		colorMenu->addAction(action);
		if (defaultColor == colors.at(i))
		{
			colorMenu->setDefaultAction(action);
		}
	}
	return colorMenu;
}

QIcon MainWindow::createColorToolButtonIcon(const QString& imageFile, QColor color)
{
	QPixmap pixmap(50, 80);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setPen(Qt::NoPen);
	// Draw icon centred horizontally on button.
	QRect target(4, 0, 42, 43);
	QPixmap image(imageFile);
	QRect source(0, 0, 42, 43);
	// ToolButton的图标大小42x43, 左上角(4,0), 右下角(46,43)
	painter.drawPixmap(target, image, source);
	//颜色块大小50x20, 左上角(0,60), 右下角(50,80)
	painter.fillRect(QRect(0, 60, 50, 20), color);
	return QIcon(pixmap);
}

QIcon MainWindow::createColorIcon(QColor color)
{
	QPixmap pixmap(20, 20);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setPen(Qt::NoPen);
	painter.fillRect(QRect(0, 0, 20, 20), color);
	return QIcon(pixmap);
}

void MainWindow::initSceneBackground()
{
	const int TILE_SIZE = 50;
	QPixmap bg(TILE_SIZE, TILE_SIZE);
	bg.fill(Qt::transparent);
	QPainter p(&bg);
	p.setBrush(QBrush(Qt::gray));
	p.drawRect(0, 0, TILE_SIZE, TILE_SIZE);
	scene->setBackgroundBrush(QBrush(bg));
	//    qDebug() << "MainWindow::initSceneBackground... ";
}

void MainWindow::showTableViewSlot(const QString& xName,const QString& yName,XArrow* xLineArrow, 
	QList<TableData> variablesX, QList<TableData> variablesY)
{

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

	tableView->show();
}

