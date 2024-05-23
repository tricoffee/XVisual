#include <QApplication>
#include "MainWindow/mainwindow.h"
#include "Common/constants.h"

int main(int argv, char* args[])
{
	Q_INIT_RESOURCE(XVisual);
	QApplication app(argv, args);
	// ��ȡ��ִ���ļ����ڵ�Ŀ¼
	QString executableDir = QCoreApplication::applicationDirPath();
	// qDebug() << "Executable Directory:" << executableDir;

	MainWindow* mainWindow = new MainWindow(executableDir, nullptr);
	mainWindow->listenForError(); // �����������
	//mainWindow.setGeometry(100, 100, 800, 500);
	// ���ô��ڳ�ʼ״̬Ϊ���
	mainWindow->setWindowState(mainWindow->windowState() | Qt::WindowMaximized);
	mainWindow->setWindowIcon(QIcon(ImageSources::XVisualLogo));

	mainWindow->show();

	return app.exec();
}