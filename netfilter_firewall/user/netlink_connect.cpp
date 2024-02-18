#include "netlink_connect.h"
#include<string.h>
#include<iostream>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<QTextEdit>
extern QTextEdit* global_edit;
void netlink_connect::init()
{
    if(netlink_fd)return;
    netlink_fd=socket(AF_NETLINK,SOCK_RAW,NETLINK_PROTOCOL);
    if(netlink_fd<0)
    {
        netlink_fd=0;
        global_edit->append("create netlink fd error");
        return;
    }
    memset(&netlink_addr_s,0,sizeof(sockaddr_nl));
    netlink_addr_s.nl_family=AF_NETLINK;
    netlink_addr_s.nl_pid=NETLINK_PORT;
    netlink_addr_s.nl_groups=0;
    if(bind(netlink_fd,(sockaddr*)&netlink_addr_s,sizeof(sockaddr_nl))!=0)
    {
        global_edit->append("bind netlink fd error");
        close(netlink_fd);
        netlink_fd=0;
        return;
    }
    memset(&netlink_addr_d,0,sizeof(sockaddr_nl));
    netlink_addr_d.nl_family=AF_NETLINK;
    netlink_addr_d.nl_pid=0;
    netlink_addr_d.nl_groups=0;
    netlink_header=(struct nlmsghdr *)malloc(NLMSG_SPACE(NETLINK_MAX_PAYLOAD));
    memset(netlink_header, 0, sizeof(struct nlmsghdr));
    netlink_header->nlmsg_len = NLMSG_SPACE(sizeof(struct msg));
    netlink_header->nlmsg_flags = 0;
    netlink_header->nlmsg_type = 0;
    netlink_header->nlmsg_seq = 0;
    netlink_header->nlmsg_pid=netlink_addr_s.nl_pid;
}
int netlink_connect::write_msg(msg* msg)
{
    if(netlink_fd<=0)return -1;
    memcpy(NLMSG_DATA(netlink_header),msg,sizeof(class msg));
    int ret;
    ret=sendto(netlink_fd,netlink_header,netlink_header->nlmsg_len,0,(sockaddr*)&netlink_addr_d\
    ,sizeof(struct sockaddr_nl));
    if(ret<0)
    {

      global_edit->append("send msg error");
    }
    return ret;
}
int netlink_connect::read_msg(msg* msg)
{
    if(netlink_fd<=0)return -1;
    int ret;
    netlink_header_and_msg r_msg;
    socklen_t len=sizeof(struct sockaddr);
    ret=recvfrom(netlink_fd,&r_msg,sizeof(netlink_header_and_msg),0,(sockaddr*)&netlink_addr_d,&len);

    if(ret<0)
    {
          msg->action=INFORMATION_NED;
       global_edit->append("read msg error");
      return -1;
    }
    else if(ret==0)
    {
    msg->action=INFORMATION_NED;
      return 0;
    }
    memcpy(msg,&(r_msg.m),sizeof(struct msg));
    return ret;
}
netlink_connect* netlink_connect::get_individual()
{
    static netlink_connect* individual=nullptr;
    if(!individual)
    {
       individual=new netlink_connect;
    }
    return individual;
}
