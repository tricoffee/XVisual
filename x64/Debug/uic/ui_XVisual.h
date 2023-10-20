/********************************************************************************
** Form generated from reading UI file 'XVisual.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XVISUAL_H
#define UI_XVISUAL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_XVisualClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *XVisualClass)
    {
        if (XVisualClass->objectName().isEmpty())
            XVisualClass->setObjectName("XVisualClass");
        XVisualClass->resize(600, 400);
        menuBar = new QMenuBar(XVisualClass);
        menuBar->setObjectName("menuBar");
        XVisualClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(XVisualClass);
        mainToolBar->setObjectName("mainToolBar");
        XVisualClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(XVisualClass);
        centralWidget->setObjectName("centralWidget");
        XVisualClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(XVisualClass);
        statusBar->setObjectName("statusBar");
        XVisualClass->setStatusBar(statusBar);

        retranslateUi(XVisualClass);

        QMetaObject::connectSlotsByName(XVisualClass);
    } // setupUi

    void retranslateUi(QMainWindow *XVisualClass)
    {
        XVisualClass->setWindowTitle(QCoreApplication::translate("XVisualClass", "XVisual", nullptr));
    } // retranslateUi

};

namespace Ui {
    class XVisualClass: public Ui_XVisualClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XVISUAL_H
