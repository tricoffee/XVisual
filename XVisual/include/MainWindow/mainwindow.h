#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QToolBox>
#include <QSpinBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QButtonGroup>
#include <QLineEdit>
#include <QFont>
#include <QToolButton>
#include <QAbstractButton>

#include <memory>
#include <thread>
#include <unordered_map>
#include <map>

// PR-2: Core executor (no Qt dependency)
#include "Core/Executor/GraphExecutor.h"
#include "Core/Executor/IEventSink.h"
#include "Core/Executor/INode.h"

#include "TableWidget/TableData.h"
#include "Common/ErrorCode.h"
#include "Common/SettingsReader.h"
#include "GraphicsWidget.h"
#include "SideWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace XVisual {

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(const QString& mProjectRootDir, QWidget* parent = nullptr);
	~MainWindow();
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
	void cancelRunButtonClicked(bool checked);
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

	QAction* textAction;
	QAction* fillAction;
	QAction* lineAction;

	QComboBox* sceneScaleCombo;
	QComboBox* itemColorCombo;
	QComboBox* textColorCombo;
	QComboBox* fontSizeCombo;
	QFontComboBox* fontCombo;

	QToolButton* linePointerButton;
	QButtonGroup* buttonGroup;
	QToolButton* runButton;
	QToolButton* cancelRunButton;
	QToolButton* exportButton;
	QToolButton* loadButton;

	QToolButton* fontColorToolButton;
	QToolButton* fillColorToolButton;
	QToolButton* lineColorToolButton;

	XVisual::ErrorCode m_lastError = XVisual::ErrorCode::Success;

	// LatestJsonPath maintains a path of the solution JSON file you loaded
	QString latestJsonPath;

	QString settingsFilePath;
	WorkSpaceData workspaceData;

    // Pointer to UI class
    ::Ui::MainWindow *ui;

    // Pointers to UI widgets for easier access (or refactor cpp to use ui->)
    QToolBox* toolBox;
    GraphicsWidget* graphicsWidget;
    SideWidget* sideWidget;

    // Actions (populated from UI)
    QAction* toFrontAction;
    QAction* sendBackAction;
    QAction* deleteAction;
    QAction* exitAction;
    QAction* boldAction;
    QAction* italicAction;
    QAction* underlineAction;
    QAction* workspaceAction;
    QAction* aboutAction;

    // Menus (populated from UI)
    QMenu* fileMenu;
    QMenu* itemMenu;
    QMenu* settingsMenu;
    QMenu* aboutMenu;

    // Toolbars (populated from UI)
    QToolBar* editToolBar;
    QToolBar* textToolBar;
    QToolBar* colorToolBar;
    QToolBar* sceneScaleBar;
    QToolBar* pointerToolBar;
    QToolBar* runButtonToolBar;
	QToolBar* cancelRunButtonToolBar;
    QToolBar* exportButtonToolBar;
    QToolBar* loadButtonToolBar;

	// PR-2: Core graph executor (serial) + UI bridge
	XVisual::GraphExecutor m_graphExecutor;

	void setUiLocked(bool locked);
};

} // namespace XVisual

#endif // MAINWINDOW_H
