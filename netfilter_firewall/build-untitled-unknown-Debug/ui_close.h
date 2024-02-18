/********************************************************************************
** Form generated from reading UI file 'close.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLOSE_H
#define UI_CLOSE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Close
{
public:
    QLabel *label;
    QPushButton *delete_;
    QLineEdit *lineEdit;

    void setupUi(QFrame *Close)
    {
        if (Close->objectName().isEmpty())
            Close->setObjectName("Close");
        Close->resize(225, 246);
        label = new QLabel(Close);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 50, 67, 21));
        delete_ = new QPushButton(Close);
        delete_->setObjectName("delete_");
        delete_->setGeometry(QRect(60, 160, 89, 25));
        lineEdit = new QLineEdit(Close);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(60, 50, 113, 25));

        retranslateUi(Close);

        QMetaObject::connectSlotsByName(Close);
    } // setupUi

    void retranslateUi(QFrame *Close)
    {
        Close->setWindowTitle(QCoreApplication::translate("Close", "Frame", nullptr));
        label->setText(QCoreApplication::translate("Close", "<html><head/><body><p align=\"justify\"><span style=\" font-size:14pt;\">id</span></p></body></html>", nullptr));
        delete_->setText(QCoreApplication::translate("Close", "delete", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Close: public Ui_Close {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLOSE_H
