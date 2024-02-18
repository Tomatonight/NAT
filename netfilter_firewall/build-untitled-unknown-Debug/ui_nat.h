/********************************************************************************
** Form generated from reading UI file 'nat.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NAT_H
#define UI_NAT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_nat
{
public:
    QLineEdit *nat_ip_line;
    QLabel *label;
    QLabel *label_2;
    QPushButton *start;
    QPushButton *stop;

    void setupUi(QFrame *nat)
    {
        if (nat->objectName().isEmpty())
            nat->setObjectName("nat");
        nat->resize(225, 300);
        nat_ip_line = new QLineEdit(nat);
        nat_ip_line->setObjectName("nat_ip_line");
        nat_ip_line->setGeometry(QRect(80, 100, 113, 25));
        label = new QLabel(nat);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 100, 67, 17));
        label_2 = new QLabel(nat);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 140, 191, 20));
        start = new QPushButton(nat);
        start->setObjectName("start");
        start->setGeometry(QRect(20, 210, 89, 25));
        stop = new QPushButton(nat);
        stop->setObjectName("stop");
        stop->setGeometry(QRect(120, 210, 89, 25));

        retranslateUi(nat);

        QMetaObject::connectSlotsByName(nat);
    } // setupUi

    void retranslateUi(QFrame *nat)
    {
        nat->setWindowTitle(QCoreApplication::translate("nat", "Frame", nullptr));
        label->setText(QCoreApplication::translate("nat", "nat_ip", nullptr));
        label_2->setText(QCoreApplication::translate("nat", "\347\253\257\345\217\243\350\265\204\346\272\220\351\273\230\350\256\244\344\270\2722000-32000", nullptr));
        start->setText(QCoreApplication::translate("nat", "start", nullptr));
        stop->setText(QCoreApplication::translate("nat", "stop", nullptr));
    } // retranslateUi

};

namespace Ui {
    class nat: public Ui_nat {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NAT_H
