// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QApplication>
#include "MainWindow/mainwindow.h"

int main(int argv, char* args[])
{
	Q_INIT_RESOURCE(XVisual);
	QApplication app(argv, args);
	MainWindow mainWindow;
	//mainWindow.setGeometry(100, 100, 800, 500);
	// ���ô��ڳ�ʼ״̬Ϊ���
	mainWindow.setWindowState(mainWindow.windowState() | Qt::WindowMaximized);
	mainWindow.show();

	return app.exec();
}

