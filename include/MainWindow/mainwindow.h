// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MainWindow/diagramscene.h"
#include "TableWidget/TableData.h"
#include "TableWidget/CustomTableView.h"

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
class DiagramScene;
class XArrow;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	std::map<int, QString> idnames;
public slots:
		void showTableViewSlot(const QString& xName,const QString& yName,
			XArrow*, QList<TableData>, QList<TableData>);
private slots:
	void backgroundButtonGroupClicked(QAbstractButton* button);
	void buttonGroupClicked(QAbstractButton* button);
	void deleteItem();
	void linePointerButtonClicked(bool checked);
	void bringToFront();
	void sendToBack();
	void currentFontChanged(const QFont& font);
	void fontSizeChanged(const QString& size);
	void sceneScaleChanged(const QString& scale);
	void textColorChanged();
	void itemColorChanged();
	void lineColorChanged();
	void handleFontChange();
	void about();
private:
	void initSceneBackground();
	void createToolBox();
	void createActions();
	void createMenus();
	void createToolbars();
	QWidget* createBackgroundCellWidget(const QString& text,BackGroundType type,const QString& image);
	QWidget* createCellWidget(const QString& text);
	int itemtype = 0;
	QMenu* createColorMenu(const char* slot, QColor defaultColor);
	QIcon createColorToolButtonIcon(const QString& image, QColor color);
	QIcon createColorIcon(QColor color);
	
	DiagramScene* scene;
	QGraphicsView* view;

	QAction* exitAction;
	QAction* addAction;
	QAction* deleteAction;

	QAction* toFrontAction;
	QAction* sendBackAction;
	QAction* aboutAction;

	QMenu* fileMenu;
	QMenu* itemMenu;
	QMenu* aboutMenu;

	QToolBar* textToolBar;
	QToolBar* editToolBar;
	QToolBar* colorToolBar;
	QToolBar* pointerToolbar;

	QComboBox* sceneScaleCombo;
	QComboBox* itemColorCombo;
	QComboBox* textColorCombo;
	QComboBox* fontSizeCombo;
	QFontComboBox* fontCombo;

	QToolBox* toolBox;
	QButtonGroup* buttonGroup;
	QToolButton* linePointerButton;
	QButtonGroup* backgroundButtonGroup;
	QToolButton* fontColorToolButton;
	QToolButton* fillColorToolButton;
	QToolButton* lineColorToolButton;
	QAction* boldAction;
	QAction* underlineAction;
	QAction* italicAction;
	QAction* textAction;
	QAction* fillAction;
	QAction* lineAction;
	int scene_width;
	int scene_height;

	CustomTableView* tableView = nullptr;
};

#endif // MAINWINDOW_H
