#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QPushButton>
#include <QWidget>
#include<QTextEdit>
#include<thread>
#include"new_window.h"
#include"Close.h"
#include"nat.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void init_cb_func();
    void read_rules();
private:
    new_window* window_create_rule;
    class Close* close_;
    class nat* nat_;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
