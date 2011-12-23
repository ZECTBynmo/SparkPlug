/********************************************************************************
** Form generated from reading UI file 'Sparkplug.ui'
**
** Created: Thu Dec 22 23:23:16 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPARKPLUG_H
#define UI_SPARKPLUG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SparkPlugClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *SparkPlugClass)
    {
        if (SparkPlugClass->objectName().isEmpty())
            SparkPlugClass->setObjectName(QString::fromUtf8("SparkPlugClass"));
        SparkPlugClass->resize(600, 400);
        menuBar = new QMenuBar(SparkPlugClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        SparkPlugClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(SparkPlugClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        SparkPlugClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(SparkPlugClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        SparkPlugClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(SparkPlugClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        SparkPlugClass->setStatusBar(statusBar);

        retranslateUi(SparkPlugClass);

        QMetaObject::connectSlotsByName(SparkPlugClass);
    } // setupUi

    void retranslateUi(QMainWindow *SparkPlugClass)
    {
        SparkPlugClass->setWindowTitle(QApplication::translate("SparkPlugClass", "SparkPlug", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SparkPlugClass: public Ui_SparkPlugClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPARKPLUG_H
