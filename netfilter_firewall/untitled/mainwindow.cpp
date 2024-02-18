#include<iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"../user/msg.h"
#include"../user/netlink_connect.h"
QTextEdit* global_edit;
static uint32_t mask_to_int(uint32_t number);
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    global_edit=ui->textEdit;
   window_create_rule=new new_window();
    close_=new Close();
   nat_=new nat();
    init_cb_func();
}
MainWindow::~MainWindow()
{
    delete close_;
    delete window_create_rule;
    delete ui;
}
void MainWindow::init_cb_func()
{
    connect(ui->install_kernel_block,&QPushButton::clicked,this,[=]()
            {
        system("sudo insmod ../kernel/Firewall.ko");
    });
    connect(ui->uninstall_kernel_block,&QPushButton::clicked,this,[=]()
            {
        netlink_connect::get_individual()->closefd();
                system("sudo rmmod ../kernel/Firewall.ko");
            });
    connect(ui->create_rule,&QPushButton::clicked,this,[=]()
            {
 \
        window_create_rule->show();
            });
   connect(ui->show_all_rules,&QPushButton::clicked,this,[=]()
            {
        msg m;
        memset(&m,0,sizeof(m));
        m.action=INFORMATION;
        netlink_connect::get_individual()->write_msg(&m);
        read_rules();
            });
    connect(ui->connect,&QPushButton::clicked,this,[=](){
         netlink_connect::get_individual()->init();

   });
   connect(ui->delete_rule,&QPushButton::clicked,this,[=](){
        close_->show();
    });
    connect(ui->nat,&QPushButton::clicked,this,[=](){
       nat_->show();
    });
}
void MainWindow::read_rules()
{

   while(true)
    {
    msg m;
    netlink_connect::get_individual()->read_msg(&m);
    if(m.action==INFORMATION_NED)return;
    int id=m.id;
    uint32_t s_ip=m.s_ip;
    uint32_t d_ip=m.d_ip;
    uint32_t s_mask=m.s_ip_mask;
    uint32_t d_mask=m.d_ip_mask;
    uint16_t s_port=m.s_port;
    uint16_t d_port=m.d_port;
    uint32_t protocol=m.protocol;
    uint32_t hook_point=m.hook_point;
    std::string str;
    str+="id: "+std::to_string(id)+" hook: ";
    switch(hook_point)
    {
    case INPUT:
        str+="input protocol: ";
        break;
    case OUTPUT:
        str+="output protocol: ";
        break;
    case FORWARD:
        str+="forward protocol: ";
        break;
    }
    switch(protocol)
    {
    case TCP:
        str+="TCP ";
        break;
    case UDP:
        str+="UDP ";
        break;
    case ICMP:
        str+="ICMP ";
        break;
    }
    in_addr addr;
    addr.s_addr=htonl(s_ip);
    str+="s_ip: "+std::string(inet_ntoa(addr));
    addr.s_addr=htonl(d_ip);
    str+=" d_ip: "+std::string(inet_ntoa(addr));
    str+=" s_mask: "+std::to_string(mask_to_int(s_mask));
   str+=" d_mask: "+std::to_string(mask_to_int(d_mask));
    str+=" s_port: "+std::to_string(s_port);
    str+=" d_port: "+std::to_string(d_port);

    global_edit->append(str.data());
   }

}
static uint32_t mask_to_int(uint32_t number)
{

   uint32_t ret=0;
   while(number&0x8000)
   {
    number=number<<1;
    ret++;
   }

   return ret;

}
