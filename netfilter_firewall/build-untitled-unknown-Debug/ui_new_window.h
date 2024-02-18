/********************************************************************************
** Form generated from reading UI file 'new_window.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEW_WINDOW_H
#define UI_NEW_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_new_window
{
public:
    QLabel *HOOK;
    QLabel *label_3;
    QLabel *S_IP;
    QLabel *D_IP;
    QLabel *S_MASK;
    QLabel *D_MASK;
    QLineEdit *S_IP_LINE;
    QLineEdit *D_IP_LINE;
    QLineEdit *S_MASK_LINE;
    QLineEdit *D_MASK_LINE;
    QPushButton *PUSH;
    QComboBox *HOOK_POINT;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *S_PORT_LINE;
    QLineEdit *D_PORT_LINE;
    QPushButton *CLICK;
    QLabel *label_4;
    QComboBox *PROTOCOL;

    void setupUi(QFrame *new_window)
    {
        if (new_window->objectName().isEmpty())
            new_window->setObjectName("new_window");
        new_window->resize(280, 439);
        HOOK = new QLabel(new_window);
        HOOK->setObjectName("HOOK");
        HOOK->setGeometry(QRect(10, 70, 101, 20));
        label_3 = new QLabel(new_window);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(50, 150, 67, 17));
        S_IP = new QLabel(new_window);
        S_IP->setObjectName("S_IP");
        S_IP->setGeometry(QRect(60, 140, 67, 17));
        D_IP = new QLabel(new_window);
        D_IP->setObjectName("D_IP");
        D_IP->setGeometry(QRect(60, 180, 67, 17));
        S_MASK = new QLabel(new_window);
        S_MASK->setObjectName("S_MASK");
        S_MASK->setGeometry(QRect(40, 220, 67, 17));
        D_MASK = new QLabel(new_window);
        D_MASK->setObjectName("D_MASK");
        D_MASK->setGeometry(QRect(40, 260, 67, 17));
        S_IP_LINE = new QLineEdit(new_window);
        S_IP_LINE->setObjectName("S_IP_LINE");
        S_IP_LINE->setGeometry(QRect(120, 140, 113, 25));
        D_IP_LINE = new QLineEdit(new_window);
        D_IP_LINE->setObjectName("D_IP_LINE");
        D_IP_LINE->setGeometry(QRect(120, 180, 113, 25));
        S_MASK_LINE = new QLineEdit(new_window);
        S_MASK_LINE->setObjectName("S_MASK_LINE");
        S_MASK_LINE->setGeometry(QRect(120, 220, 113, 25));
        D_MASK_LINE = new QLineEdit(new_window);
        D_MASK_LINE->setObjectName("D_MASK_LINE");
        D_MASK_LINE->setGeometry(QRect(120, 260, 113, 25));
        PUSH = new QPushButton(new_window);
        PUSH->setObjectName("PUSH");
        PUSH->setGeometry(QRect(80, 380, 89, 25));
        HOOK_POINT = new QComboBox(new_window);
        HOOK_POINT->addItem(QString());
        HOOK_POINT->addItem(QString());
        HOOK_POINT->addItem(QString());
        HOOK_POINT->setObjectName("HOOK_POINT");
        HOOK_POINT->setGeometry(QRect(120, 70, 111, 25));
        label = new QLabel(new_window);
        label->setObjectName("label");
        label->setGeometry(QRect(40, 300, 67, 17));
        label_2 = new QLabel(new_window);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(40, 340, 67, 17));
        S_PORT_LINE = new QLineEdit(new_window);
        S_PORT_LINE->setObjectName("S_PORT_LINE");
        S_PORT_LINE->setGeometry(QRect(120, 300, 113, 25));
        D_PORT_LINE = new QLineEdit(new_window);
        D_PORT_LINE->setObjectName("D_PORT_LINE");
        D_PORT_LINE->setGeometry(QRect(120, 340, 113, 25));
        CLICK = new QPushButton(new_window);
        CLICK->setObjectName("CLICK");
        CLICK->setGeometry(QRect(60, 30, 131, 31));
        label_4 = new QLabel(new_window);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(26, 110, 81, 20));
        PROTOCOL = new QComboBox(new_window);
        PROTOCOL->addItem(QString());
        PROTOCOL->addItem(QString());
        PROTOCOL->addItem(QString());
        PROTOCOL->setObjectName("PROTOCOL");
        PROTOCOL->setGeometry(QRect(120, 110, 111, 25));

        retranslateUi(new_window);

        QMetaObject::connectSlotsByName(new_window);
    } // setupUi

    void retranslateUi(QFrame *new_window)
    {
        new_window->setWindowTitle(QCoreApplication::translate("new_window", "add rule", nullptr));
        HOOK->setText(QCoreApplication::translate("new_window", "HOOK_POINT", nullptr));
        label_3->setText(QString());
        S_IP->setText(QCoreApplication::translate("new_window", "S_IP", nullptr));
        D_IP->setText(QCoreApplication::translate("new_window", "D_IP", nullptr));
        S_MASK->setText(QCoreApplication::translate("new_window", "S_MASK", nullptr));
        D_MASK->setText(QCoreApplication::translate("new_window", "D_MASK", nullptr));
        PUSH->setText(QCoreApplication::translate("new_window", "PUSH", nullptr));
        HOOK_POINT->setItemText(0, QCoreApplication::translate("new_window", "input", nullptr));
        HOOK_POINT->setItemText(1, QCoreApplication::translate("new_window", "output", nullptr));
        HOOK_POINT->setItemText(2, QCoreApplication::translate("new_window", "forward", nullptr));

        label->setText(QCoreApplication::translate("new_window", "S_PORT", nullptr));
        label_2->setText(QCoreApplication::translate("new_window", "D_PORT", nullptr));
        CLICK->setText(QCoreApplication::translate("new_window", "\347\202\271\345\207\273\346\237\245\347\234\213\344\275\277\347\224\250\350\257\264\346\230\216", nullptr));
        label_4->setText(QCoreApplication::translate("new_window", "PROTOCOL", nullptr));
        PROTOCOL->setItemText(0, QCoreApplication::translate("new_window", "TCP", nullptr));
        PROTOCOL->setItemText(1, QCoreApplication::translate("new_window", "UDP", nullptr));
        PROTOCOL->setItemText(2, QCoreApplication::translate("new_window", "ICMP", nullptr));

    } // retranslateUi

};

namespace Ui {
    class new_window: public Ui_new_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEW_WINDOW_H
