#include "close.h"
#include "ui_close.h"
#include"../user/msg.h"
#include"../user/netlink_connect.h"
Close::Close(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Close)
{
    ui->setupUi(this);
    connect(ui->delete_,&QPushButton::clicked,this,[=](){
        msg m;
        m.action=REMOVE;
        m.id=atoi(ui->lineEdit->text().toStdString().data());
        netlink_connect::get_individual()->write_msg(&m);
    });
}

Close::~Close()
{
    delete ui;
}
