/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *install_kernel_block;
    QPushButton *uninstall_kernel_block;
    QPushButton *create_rule;
    QPushButton *show_all_rules;
    QPushButton *connect;
    QTextEdit *textEdit;
    QPushButton *delete_rule;
    QPushButton *nat;
    QMenuBar *menubar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(908, 701);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        install_kernel_block = new QPushButton(centralwidget);
        install_kernel_block->setObjectName("install_kernel_block");
        install_kernel_block->setGeometry(QRect(30, 50, 171, 51));
        uninstall_kernel_block = new QPushButton(centralwidget);
        uninstall_kernel_block->setObjectName("uninstall_kernel_block");
        uninstall_kernel_block->setGeometry(QRect(240, 50, 171, 51));
        create_rule = new QPushButton(centralwidget);
        create_rule->setObjectName("create_rule");
        create_rule->setGeometry(QRect(30, 140, 171, 51));
        show_all_rules = new QPushButton(centralwidget);
        show_all_rules->setObjectName("show_all_rules");
        show_all_rules->setGeometry(QRect(240, 140, 171, 51));
        connect = new QPushButton(centralwidget);
        connect->setObjectName("connect");
        connect->setGeometry(QRect(450, 50, 161, 51));
        textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName("textEdit");
        textEdit->setGeometry(QRect(0, 290, 911, 371));
        textEdit->setReadOnly(true);
        delete_rule = new QPushButton(centralwidget);
        delete_rule->setObjectName("delete_rule");
        delete_rule->setGeometry(QRect(450, 140, 161, 51));
        nat = new QPushButton(centralwidget);
        nat->setObjectName("nat");
        nat->setGeometry(QRect(30, 220, 171, 51));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 908, 27));
        MainWindow->setMenuBar(menubar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName("toolBar");
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        install_kernel_block->setText(QCoreApplication::translate("MainWindow", "install_kernel_block", nullptr));
        uninstall_kernel_block->setText(QCoreApplication::translate("MainWindow", "uninstall_kernel_block", nullptr));
        create_rule->setText(QCoreApplication::translate("MainWindow", "create_rule", nullptr));
        show_all_rules->setText(QCoreApplication::translate("MainWindow", "show_all_rules", nullptr));
        connect->setText(QCoreApplication::translate("MainWindow", "connect init", nullptr));
        delete_rule->setText(QCoreApplication::translate("MainWindow", "delete_rule", nullptr));
        nat->setText(QCoreApplication::translate("MainWindow", "nat", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
