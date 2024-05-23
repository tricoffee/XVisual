#include <QApplication>
#include "MainWindow/mainwindow.h"
#include "Common/constants.h"

int main(int argv, char* args[])
{
	Q_INIT_RESOURCE(XVisual);
	QApplication app(argv, args);
	// 获取可执行文件所在的目录
	QString executableDir = QCoreApplication::applicationDirPath();
	// qDebug() << "Executable Directory:" << executableDir;

	MainWindow* mainWindow = new MainWindow(executableDir, nullptr);
	mainWindow->listenForError(); // 启动错误监听
	//mainWindow.setGeometry(100, 100, 800, 500);
	// 设置窗口初始状态为最大化
	mainWindow->setWindowState(mainWindow->windowState() | Qt::WindowMaximized);
	mainWindow->setWindowIcon(QIcon(ImageSources::XVisualLogo));

	mainWindow->show();

	return app.exec();
}