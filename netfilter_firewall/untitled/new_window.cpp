#include "new_window.h"
#include "ui_new_window.h"
#include"../user/msg.h"
#include"../user/netlink_connect.h"
#include<arpa/inet.h>
#include<iostream>
#include<string>
#include<QMessageBox>
static msg* make_new_msg(std::string hook_point,std::string protocol,std::string s_ip,std::string d_ip,std::string s_mask,
                         std::string d_mask,std::string s_port,std::string d_port);
new_window::new_window(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::new_window)
{
     ui->setupUi(this);
    List=new QTextEdit();
     List->setText("ip&mask 为ip的匹配规则\n对port不做匹配则置为0\n例：\n禁止源123.124.125.126"
                  "的进入\n sip=123.124.125.126 s_mask=32 dip=0,mask=0,sp=dp=0");
    List->resize(200,300);
     List->setReadOnly(true);
    List->setEnabled(true);
     List->setAlignment(Qt::AlignLeft);
    connect(ui->PUSH,&QPushButton::clicked,this,[=]()
            {
                std::string hook_point=ui->HOOK_POINT->currentText().toStdString();
                std::string protocol=ui->PROTOCOL->currentText().toStdString();
                std::string s_ip=ui->S_IP_LINE->text().toStdString();
                std::string d_ip=ui->D_IP_LINE->text().toStdString();
                std::string s_mask=ui->S_MASK_LINE->text().toStdString();

                std::string d_mask=ui->D_MASK_LINE->text().toStdString();
                std::string s_port=ui->S_PORT_LINE->text().toStdString();
                std::string d_port=ui->D_PORT_LINE->text().toStdString();
                msg* new_msg=make_new_msg(hook_point,protocol,s_ip,d_ip,s_mask,d_mask,s_port,d_port);
                netlink_connect::get_individual()->write_msg(new_msg);
                delete new_msg;
            });
    connect(ui->CLICK,&QPushButton::clicked,this,[=](){

        List->show();

    });
}

new_window::~new_window()
{
    delete ui;
}
static uint32_t int_to_mask(int mask)
{

    uint32_t ret=0;
    while(mask-->0)
    {
         ret=ret>>1;
         ret|=0x8000;
    }

    return ret;
}
static msg* make_new_msg(std::string hook_point,std::string protocol,std::string s_ip,std::string d_ip,std::string s_mask,
std::string d_mask,std::string s_port,std::string d_port)
{

    uint16_t s_port_=atoi(s_port.data());
    uint16_t d_port_=atoi(d_port.data());
    uint32_t d_mask_=int_to_mask(atoi(d_mask.data()));
    uint32_t s_mask_=int_to_mask(atoi(s_mask.data()));
    uint32_t d_ip_=ntohl(inet_addr(d_ip.data()));
    uint32_t s_ip_=ntohl(inet_addr(s_ip.data()));
    enum msg_hook_point hook;
    enum msg_protocol protocol_;
    switch(hook_point[0])
    {
    case 'i':
    {
         hook=INPUT;
         break;
    }
    case 'o':
    {
         hook=OUTPUT;
         break;
    }
    case 'f':
    {
         hook=FORWARD;
         break;
    }
    }
    switch(protocol[0])
    {
    case 'T':
    {
         protocol_=TCP;
         break;
    }
    case 'U':
    {
         protocol=UDP;
         break;
    }
    case 'I':
    {
         protocol=ICMP;
         break;
    }
    }

    msg* new_msg=new msg(SIGN_UP,hook,protocol_,REJECT,d_ip_,s_ip_,d_port_,s_port_,d_mask_,s_mask_);

    return new_msg;
}
