#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TableWidget/TableData.h"
#include "Common/ErrorCode.h"
#include "Common/SettingsReader.h"

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
	MainWindow(const QString& mProjectRootDir, QWidget* parent = nullptr);
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
	void handleFontChanged();
	void handleWorkspaceChanged();
	void about();
private:
	void checkAndCreateDirectory(const QString& path);
	void makeDefaultSettings();
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
	QAction* workspaceAction;
	QAction* aboutAction;

	QMenu* fileMenu;
	QMenu* itemMenu;
	QMenu* settingsMenu;
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

	// LatestJsonPath maintains the path of the solution JSON file you loaded
	QString latestJsonPath;

	QString settingsFilePath;
	WorkSpaceData workspaceData;
};

#endif // MAINWINDOW_H
