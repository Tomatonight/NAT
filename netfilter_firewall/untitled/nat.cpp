#include "nat.h"
#include"../user/msg.h"
#include"../user/netlink_connect.h"
#include "ui_nat.h"
#include<arpa/inet.h>
#include<QMessageBox>
nat::nat(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::nat)
{

    ui->setupUi(this);
    connect(ui->start,&QPushButton::clicked,this,[=](){
        struct msg m;
        std::string proxy_ip_str=ui->nat_ip_line->text().toStdString();
        uint32_t proxy_ip=ntohl(inet_addr(proxy_ip_str.data()));
        if(!proxy_ip)
        {
            QMessageBox::information(NULL, "error", "check ip", QMessageBox::Yes , QMessageBox::Yes);
            return;
        }
        m.s_ip=proxy_ip;
        m.action=NAT_START;
        netlink_connect::get_individual()->write_msg(&m);

    });
    connect(ui->stop,&QPushButton::clicked,this,[](){
    struct msg m;
        m.action=NAT_STOP;
        netlink_connect::get_individual()->write_msg(&m);
    });
}

nat::~nat()
{
    delete ui;
}
