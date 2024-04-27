// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "Common/LoggerInstance.h"
#include "Common/XThreadMacro.h"
#include "GlobalStorage/GlobalStorage.h"
#include "XGraph/XGraph.h"

#include <QtWidgets>
#include <QString>
#include <string>
#include <QSplitter>

#include "ItemBase/XArrow.h"
#include "ItemBase/XBaseItem.h"
#include "MainWindow/diagramscene.h"
#include "MainWindow/mainwindow.h"
#include "Common/constants.h"
#include "MainWindow/GraphicsWidget.h"
#include "MainWindow/SideWidget.h"
#include "MainWindow/ImagePageWidget.h"
#include "Exception/UmapKeyNoFoundException.h"

MainWindow::MainWindow()
{
	createActions();
	createMenus();
    graphicsWidget = new GraphicsWidget(itemMenu);
	createToolbars();
	createToolBox();
	inintActionConnection();

	sideWidget = new SideWidget();

	// ����ˮƽ����splitter
	QSplitter* splitter = new QSplitter(Qt::Horizontal);
	// ����QSplitter�ָ��ߵ���ʽ��
	splitter->setStyleSheet("QSplitter::handle { background: lightgray; border: 1px gray; }");
    // ��toolBox��ӵ�splitter
	splitter->addWidget(toolBox);
	// ��graphicsWidget��ӵ�splitter
	splitter->addWidget(graphicsWidget);
	// ��sideWidget��ӵ�splitter
	splitter->addWidget(sideWidget);
	//�����������
	splitter->setStretchFactor(0, 1);
	splitter->setStretchFactor(1, 1);
	splitter->setStretchFactor(2, 0);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(splitter);
	// ����layout��С�ؼ�֮��ľ���, �������ڱ߾�
	layout->setSpacing(0);
	// ����layout����߾�
	layout->setContentsMargins(0, 0, 0, 0);

	QWidget* widget = new QWidget;
	widget->setLayout(layout);

	setCentralWidget(widget);
	setWindowTitle(tr("Diagramscene"));
	setUnifiedTitleAndToolBarOnMac(true);

	connect(graphicsWidget, &GraphicsWidget::showImageInTabSignal, 
		sideWidget,&SideWidget::showImageInTabSlot);
}

void MainWindow::backgroundButtonGroupClicked(QAbstractButton* button)
{
	button->setChecked(false);
	const int id = backgroundButtonGroup->id(button);
	graphicsWidget->setBackgroundChange(id);
}

void MainWindow::buttonGroupClicked(QAbstractButton* button)
{
	button->setChecked(false);
	const int id = buttonGroup->id(button);
	graphicsWidget->setDiagramState(DiagramState::Insert);
	graphicsWidget->setDiagramType(DiagramType::Item);
	graphicsWidget->setDiagramName(idnames[id]);
}

void MainWindow::linePointerButtonClicked(bool checked)
{
	linePointerButton->setChecked(false);
	graphicsWidget->setDiagramState(DiagramState::Insert);
	graphicsWidget->setDiagramType(DiagramType::Line);
}

void MainWindow::runButtonClicked(bool checked)
{
	XLOG_INFO("void MainWindow::runButtonClicked to do executeXGraph", CURRENT_THREAD_ID);
	{
		std::lock_guard<std::mutex> lock(xGraphMutex);
		// ����xGraph��ÿ��Ԫ��
		//for(auto it = xGraph.begin(); it != xGraph.end(); ++it) 
		//{
		//	// ��ȡ��ǰ�ڵ�node
		//	std::shared_ptr<GraphNode> node = *it;
		//	// ��ȡ��ǰ�ڵ��nodeId
		//	std::string nodeId = node->nodeId;
		//	XLOG_INFO("void MainWindow::runButtonClicked, nodeId = "+nodeId, CURRENT_THREAD_ID);
		//}

		try 
		{
			XGraph::executeXGraph(xGraph);
		}
		catch (UmapKeyNoFoundException& e)
		{
			//        std::cout << "=== === ===" << e.what() << "=== === ===" << std::endl;
			XLOG_INFO("====== void MainWindow::runButtonClicked, UmapKeyNoFoundException  ======" + std::string(e.what()), CURRENT_THREAD_ID);
			//throw UmapKeyNoFoundException(e.what());
			// �����쳣����¼������
			m_lastError = XVisual::ErrorCode::UmapKeyNoFound;
			// ��������źţ�֪ͨ������
			emit errorOccurred(m_lastError);
		}

	}// ���������ʱ��lock_guard ���Զ�����������xGraphMutex
	XLOG_INFO("void MainWindow::runButtonClicked xGraph is executed.", CURRENT_THREAD_ID);
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
	graphicsWidget->setSceneScaleChange(scale);
}

/*
����fontColorToolButton�ĵ����˵���ĳһ��QActionʱ,
slot����MainWindow::textColorChanged��Ӧ, fontColorToolButton��ͼ��(Icon)����Ӧ�仯,
ͬʱ����scene��text�ı���ɫ����Ӧ�仯
*/
void MainWindow::textColorChanged()
{
	textAction = qobject_cast<QAction*>(sender());
	fontColorToolButton->setIcon(createColorToolButtonIcon(
		ImageSources::TextPointer,
		qvariant_cast<QColor>(textAction->data())));
	//ȥ��ԭ����textButtonTriggered();ֱ�Ӱ�textButtonTriggered()�������һ���ù��������£�
	//��δ����ʾ���ó���scene��text�ı���ɫ
	QColor color = qvariant_cast<QColor>(textAction->data());
	graphicsWidget->setItemTextColor(color);
}

/*
����fillColorToolButton�ĵ����˵���ĳһ��QActionʱ,
slot����MainWindow::itemColorChanged��Ӧ, fillColorToolButton��ͼ��(Icon)����Ӧ�仯,
ͬʱ����scene��item����ɫ����Ӧ�仯
*/
void MainWindow::itemColorChanged()
{
	fillAction = qobject_cast<QAction*>(sender());
	fillColorToolButton->setIcon(createColorToolButtonIcon(
		ImageSources::FloodFill,
		qvariant_cast<QColor>(fillAction->data())));
	//ȥ��ԭ����fillButtonTriggered();ֱ�Ӱ�fillButtonTriggered�������һ���ù���������:
	//��δ����ʾ���ó���scene��item����ɫ
	QColor color = qvariant_cast<QColor>(fillAction->data());
	graphicsWidget->setSceneItemColor(color);
}

/*
����lineColorToolButton�ĵ����˵���ĳһ��QActionʱ,
slot����MainWindow::lineColorChanged��Ӧ, lineColorToolButton��ͼ��(Icon)����Ӧ�仯,
ͬʱ����scene��line����ɫ����Ӧ�仯
*/
void MainWindow::lineColorChanged()
{
	lineAction = qobject_cast<QAction*>(sender());
	lineColorToolButton->setIcon(createColorToolButtonIcon(
		ImageSources::LineColor,
		qvariant_cast<QColor>(lineAction->data())));
	//ȥ��ԭ����lineButtonTriggered();ֱ�Ӱ�lineButtonTriggered�������һ���ù���������:
	//��δ����ʾ���ó���scene��line����ɫ
	QColor color = qvariant_cast<QColor>(lineAction->data());
	graphicsWidget->setSceneLineColor(color);
}

void MainWindow::handleFontChange()
{
	QFont font = fontCombo->currentFont();
	font.setPointSize(fontSizeCombo->currentText().toInt());
	font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
	font.setItalic(italicAction->isChecked());
	font.setUnderline(underlineAction->isChecked());
	graphicsWidget->setFont(font);
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

	//layout->addWidget(createCellWidget(tr("Condition")), 0, 0);
	//layout->addWidget(createCellWidget(tr("Step")), 0, 1);
	//layout->addWidget(createCellWidget(tr("Input")), 1, 0);
	//layout->addWidget(createCellWidget(tr("Output")), 1, 1);
	//layout->addWidget(createCellWidget(tr("LoadImage")), 2, 0);
	//layout->addWidget(createCellWidget(tr("CVCrop")), 2, 1);

	layout->addWidget(createCellWidget(tr("LoadImage")), 0, 0);
	layout->addWidget(createCellWidget(tr("CVCrop")), 0, 1);

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

void MainWindow::inintActionConnection()
{
	connect(toFrontAction, &QAction::triggered, graphicsWidget, &GraphicsWidget::bringToFront);
	connect(sendBackAction, &QAction::triggered, graphicsWidget, &GraphicsWidget::sendToBack);
	connect(deleteAction, &QAction::triggered, graphicsWidget, &GraphicsWidget::deleteItem);
	connect(exitAction, &QAction::triggered, this, &QWidget::close);
	connect(boldAction, &QAction::triggered, this, &MainWindow::handleFontChange);
	connect(italicAction, &QAction::triggered, this, &MainWindow::handleFontChange);
	connect(underlineAction, &QAction::triggered, this, &MainWindow::handleFontChange);
	connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createActions()
{
	toFrontAction = new QAction(QIcon(ImageSources::BringFront),
		tr("Bring to &Front"), this);
	toFrontAction->setShortcut(tr("Ctrl+F"));
	toFrontAction->setStatusTip(tr("Bring item to front"));
	
	sendBackAction = new QAction(QIcon(ImageSources::SendBack), tr("Send to &Back"), this);
	sendBackAction->setShortcut(tr("Ctrl+T"));
	sendBackAction->setStatusTip(tr("Send item to back"));
	
	deleteAction = new QAction(QIcon(ImageSources::Delete), tr("&Delete"), this);
	deleteAction->setShortcut(tr("Delete"));
	deleteAction->setStatusTip(tr("Delete item from diagram"));
	
	exitAction = new QAction(tr("E&xit"), this);
	exitAction->setShortcuts(QKeySequence::Quit);
	exitAction->setStatusTip(tr("Quit Scenediagram example"));
	
	boldAction = new QAction(QIcon(ImageSources::Bold), tr("Bold"), this);
	boldAction->setCheckable(true);
	boldAction->setShortcut(tr("Ctrl+B"));
	
	italicAction = new QAction(QIcon(ImageSources::Italic), tr("Italic"), this);
	italicAction->setCheckable(true);
	italicAction->setShortcut(tr("Ctrl+I"));
	
	underlineAction = new QAction(QIcon(ImageSources::Underline), tr("Underline"), this);
	underlineAction->setCheckable(true);
	underlineAction->setShortcut(tr("Ctrl+U"));
	
	aboutAction = new QAction(tr("A&bout"), this);
	aboutAction->setShortcut(tr("F1"));
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
	connect(linePointerButton, &QAbstractButton::toggled, this, &MainWindow::linePointerButtonClicked);

	runButton = new QToolButton;
	runButton->setCheckable(true);
	runButton->setChecked(false);
	runButton->setIcon(QIcon(ImageSources::RunButton));
	connect(runButton, &QAbstractButton::toggled, this, &MainWindow::runButtonClicked);

	sceneScaleCombo = new QComboBox;
	QStringList scales;
	scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
	sceneScaleCombo->addItems(scales);
	sceneScaleCombo->setCurrentIndex(2);
	connect(sceneScaleCombo, &QComboBox::currentTextChanged,
		this, &MainWindow::sceneScaleChanged);

	sceneScaleBar = addToolBar(tr("Scale"));
	sceneScaleBar->addWidget(sceneScaleCombo);

	pointerToolBar = addToolBar(tr("linePointer"));
	pointerToolBar->addWidget(linePointerButton);

	runButtonToolBar = addToolBar(tr("Run"));
	runButtonToolBar->addWidget(runButton);
}


//BackGroundType
QWidget* MainWindow::createBackgroundCellWidget(const QString& text,
	int type, const QString& image)
{
	QToolButton* button = new QToolButton;
	button->setIcon(QIcon(image));
	button->setIconSize(QSize(50, 50));
	button->setCheckable(true);
	backgroundButtonGroup->addButton(button, type);

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
	XBaseItem  *item = ItemRegistry::createObject(itemclass, graphicsWidget, itemMenu, nullptr);
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
	// ToolButton��ͼ���С42x43, ���Ͻ�(4,0), ���½�(46,43)
	painter.drawPixmap(target, image, source);
	//��ɫ���С50x20, ���Ͻ�(0,60), ���½�(50,80)
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
	graphicsWidget->setBackgroundBrushChange(QBrush(bg));
}

void MainWindow::listenForError()
{
	// ���Ӵ������źŵ��ۺ�������
	QObject::connect(this, &MainWindow::errorOccurred, [&](XVisual::ErrorCode errorCode) 
	{
		std::string errorCodeStr = XVisual::errorCodeToStr(errorCode);
		std::string warningStr = "\"" + errorCodeStr + "\" occurred. Program cannot continue.";
		QString warningQStr = QString::fromStdString(warningStr);
		// ��ʾ����Ի���
		QMessageBox::critical(nullptr, "Error", warningQStr);

		//// ���� QMessageBox::information
		//// ��ʾ�û���ֹ����
		//QMessageBox::StandardButton button = QMessageBox::information(nullptr, "Error", "Terminate program?",
		//QMessageBox::Yes, QMessageBox::NoButton);
		//// ����û�ѡ����ֹ�������˳�Ӧ��
		//if (button == QMessageBox::Yes) 
		//{
		//	qApp->quit();
		//}


		// ���� QMessageBox::question
		// QMessageBox::Yes �� QMessageBox::No ������ťͬʱ��ʾ
		// ��ʾ�û���ֹ����
		QMessageBox::StandardButton button = QMessageBox::question(nullptr, "Error", "Terminate program?",
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// ����û�ѡ����ֹ�������˳�Ӧ��
		if (button == QMessageBox::Yes) 
		{
			qApp->quit();
		}


		//// ���� QMessageBox::warning
		//// ��ʾ�û���ֹ����
		// QMessageBox::StandardButton button = QMessageBox::warning(nullptr, "Error", "Terminate program?");
		//// ����û�ѡ����ֹ�������˳�Ӧ��
		//if (button == QMessageBox::Ok) 
		//{
		//	qApp->quit();
		//}


	});
}