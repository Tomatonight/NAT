#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include"connect.h"
#include <QMainWindow>
#include<QPushButton>
#include<QTextEdit>
#include<QMenu>
#include<QMenuBar>
#include<QLineEdit>
#include<QInputDialog>
#include<string>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPushButton *button_ban_ip;
    QPushButton *button_ban_port;
    QPushButton *button_recover_ip;
    QPushButton *button_recover_port;
     QPushButton *button_show_ip;
     QPushButton *button_show_port;
    QPushButton *reset_rules;
    QLineEdit *line_ip;
    QLineEdit *line_port;
    QTextEdit *control_text;
    QTextEdit* edit_danger_ip;
    QTextEdit *ban_ip_text;
    QTextEdit *ban_port_text;
    QMenu *menu;
    QMenuBar* menubar;

};
#endif // MAINWINDOW_H
