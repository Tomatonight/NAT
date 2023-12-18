#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(800,600);

    button_ban_ip=new QPushButton("ban ip",this);
    button_ban_port=new QPushButton("ban port",this);
    button_recover_ip=new QPushButton("recover ip",this);
    button_recover_port=new QPushButton("recover port",this);
    button_show_ip= new QPushButton("show ip",this);
    button_show_port=new QPushButton("show port",this);
    reset_rules=new QPushButton("reset rules",this);
    line_ip=new QLineEdit(this);
    line_port=new QLineEdit(this);
    ban_ip_text=new QTextEdit(nullptr);
    ban_port_text=new QTextEdit(nullptr);
    control_text=new QTextEdit(this);
    control_text->document()->setMaximumBlockCount(100);
    edit_danger_ip=new QTextEdit(nullptr);

    menubar =new QMenuBar(this);
    menu=new QMenu("test");

    menubar->addMenu(menu);
    menu->resize(50,30);
    menubar->move(600,0);
    menubar->resize(50,30);

    QAction *action_1=new QAction("show danger ip");
    connect(action_1,&QAction::triggered,this,[=](){
        edit_danger_ip->show();
    });
    QAction *action_2=new QAction("set protect port");
    connect(action_2,&QAction::triggered,this,[=](){
        bool ok=false;
        QString re=QInputDialog::getText(this,"input","port", QLineEdit::Normal,"",&ok);
        int port=Connect::get()->judge_port(re.toStdString());
        if(port<0)
        {
            control_text->append("input port error");
        }
        send_msg m('c','p',port);
        Connect::get()->send_msg_to_kernel((char*)&m,sizeof(send_msg));
    });
    QAction *action_3=new QAction("start ddos and scan defence");
    connect(action_3,&QAction::triggered,this,[=](){
        send_msg m('c','o',0);
        Connect::get()->send_msg_to_kernel((char*)&m,sizeof(send_msg));
    });
    QAction *action_4=new QAction("stop ddos and scan defence");
    connect(action_4,&QAction::triggered,this,[=](){
        send_msg m('c','c',0);
        Connect::get()->send_msg_to_kernel((char*)&m,sizeof(send_msg));
    });
    menu->addAction(action_1);
    menu->addAction(action_2);
    menu->addAction(action_3);
    menu->addAction(action_4);
    connect(button_ban_ip,&QPushButton::clicked,this,[=](){
        uint32_t ip=inet_addr((line_ip->text().toStdString()).c_str());
        if(ip==INADDR_NONE)
        {
            control_text->append("ban ip error");
            return;
        }
        if(Connect::get()->find_ip(ip))return;
        Connect::get()->add_ip(ip);
        send_msg m('b','i',ip);
        Connect::get()->send_msg_to_kernel((char*)&m,sizeof(send_msg));

    });
    connect(button_ban_port,&QPushButton::clicked,this,[=](){
        std::string str=line_port->text().toStdString();
        int a=0,b=0;
        int index=0;
        while(index<str.size()&&str[index]!='-')index++;
        if(str.size()<3||index>=str.size())
        {
            control_text->append("ban port error");
            return;
        }
        a=Connect::get()->judge_port(std::string(&str[0],&str[index]));
        b=Connect::get()->judge_port(std::string(&str[index+1],&str[str.size()]));
        if(a<0||b<0||a>b)
        {
            \
                control_text->append("ban port error");
            return;
        }
        if(Connect::get()->find_port(a,b))return;
        Connect::get()->add_port(a,b);
        send_msg m('b','p',a,b);
        Connect::get()->send_msg_to_kernel((char*)&m,sizeof(send_msg));
    });
    connect(button_recover_ip,&QPushButton::clicked,this,[=](){
        uint32_t ip=inet_addr((line_ip->text().toStdString()).c_str());
        if(ip==INADDR_NONE)
        {
            control_text->append("ban ip error");
            return;
        }
        Connect::get()->delete_ip(ip);
        send_msg m('r','i',ip);
        Connect::get()->send_msg_to_kernel((char*)&m,sizeof(send_msg));
    });
    connect(button_recover_port,&QPushButton::clicked,this,[=](){
        std::string str=line_port->text().toStdString();
        int a=0,b=0;
        int index=0;
        while(index<str.size()&&str[index]!='-')index++;
        if(str.size()<3||index>=str.size())
        {
            control_text->append("recover port error");
            return;
        }
        a=Connect::get()->judge_port(std::string(&str[0],&str[index]));
        b=Connect::get()->judge_port(std::string(&str[index+1],&str[str.size()]));
        if(a<0||b<0||a>b)
        {

            control_text->append("recover port error");
            return;
        }
        Connect::get()->delete_port(a,b);
        send_msg m('r','p',a,b);
        Connect::get()->send_msg_to_kernel((char*)&m,sizeof(send_msg));
    });
    connect(reset_rules,&QPushButton::clicked,this,[=](){
        Connect::get()->reset_rules();
        send_msg m('r','e',0);
        Connect::get()->send_msg_to_kernel((char*)&m,sizeof(send_msg));
    });
    connect(button_show_ip,&QPushButton::clicked,this,[=](){
        ban_ip_text->clear();
        for(std::unordered_set<uint32_t>::iterator it=Connect::get()->ips.begin();it!=Connect::get()->ips.end();it++)
        {
            struct in_addr network;
            network.s_addr=*it;
            ban_ip_text->append(inet_ntoa(network));
        }
        ban_ip_text->show();
    });
    connect(button_show_port,&QPushButton::clicked,this,[=](){
        ban_port_text->clear();

        for(std::set<std::pair<uint16_t,uint16_t>>::iterator it=Connect::get()->ports.begin();it!=Connect::get()->ports.end();it++)
        {
            char buffer[10]={0};
            sprintf(buffer,"%s-%s",std::to_string(it->first).c_str(),std::to_string(it->second).c_str());
            ban_port_text->append(buffer);
        }

        ban_port_text->show();
    });
    button_ban_ip->move(0,0);
    button_ban_port->move(100,0);
    button_recover_ip->move(0,100);
    button_recover_port->move(100,100);
    button_show_ip->move(300,0);
    button_show_port->move(300,100);
    line_ip->move(0,50);
    line_port->move(200,50);
    ban_ip_text->resize(300,800);
    ban_port_text->resize(300,800);
    reset_rules->move(200,0);
    Connect::get()->set_out(control_text,edit_danger_ip);
    control_text->move(0,150);
    control_text->resize(600,400);
    control_text->setReadOnly(true);
    ban_ip_text=new QTextEdit(nullptr);
    ban_port_text=new QTextEdit(nullptr);
    Connect::get()->init();
    reset_rules->click();
};

MainWindow::~MainWindow()
{
    reset_rules->click();
    delete line_ip;
    delete line_port;
    delete ui;
}

