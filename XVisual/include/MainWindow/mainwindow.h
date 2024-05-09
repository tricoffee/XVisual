// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TableWidget/TableData.h"
#include "Common/ErrorCode.h"

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QFont;
class QToolButton;
class QAbstractButton;
class GraphicsWidget;
class SideWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	std::map<int, QString> idnames;
	void inintActionConnection();
	XVisual::ErrorCode lastError() const
	{
		return m_lastError;
	}
signals:
	void errorOccurred(XVisual::ErrorCode errorCode);
public slots:
	void listenForError();
private slots:
	void backgroundButtonGroupClicked(QAbstractButton* button);
	void buttonGroupClicked(QAbstractButton* button);
	void linePointerButtonClicked(bool checked);
	void runButtonClicked(bool checked);
	void exportButtonClicked(bool checked);
	void loadButtonClicked(bool checked);
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
	QWidget* createBackgroundCellWidget(const QString& text,int type,const QString& image);
	QWidget* createCellWidget(const QString& text);
	int itemtype = 0;
	QMenu* createColorMenu(const char* slot, QColor defaultColor);
	QIcon createColorToolButtonIcon(const QString& image, QColor color);
	QIcon createColorIcon(QColor color);

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
	QToolBar* pointerToolBar;
	QToolBar* sceneScaleBar;
	QToolBar* runButtonToolBar;
	QToolBar* exportButtonToolBar;
	QToolBar* loadButtonToolBar;

	QComboBox* sceneScaleCombo;
	QComboBox* itemColorCombo;
	QComboBox* textColorCombo;
	QComboBox* fontSizeCombo;
	QFontComboBox* fontCombo;

	QToolBox* toolBox;
	QToolButton* linePointerButton;
	QButtonGroup* buttonGroup;
	QToolButton* runButton;
	QToolButton* exportButton;
	QToolButton* loadButton;
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

	GraphicsWidget* graphicsWidget;
	SideWidget* sideWidget;

	XVisual::ErrorCode m_lastError = XVisual::ErrorCode::Success;

	QString latestJsonPath;
};

#endif // MAINWINDOW_H
