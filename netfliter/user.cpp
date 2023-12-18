#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <iostream>
#include <linux/netlink.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include<arpa/inet.h>
#include<unordered_set>
#define NL_BUFFER 100
#define PORT 100
struct user_msg_info
{
    struct nlmsghdr hdr;
    char buffer[100];
};
class Connect
{
public:
Connect()
{

}
~Connect()
{
    close(sockfd);
    delete nlhdr;
}
    void init()
    {
        sockfd = socket(AF_NETLINK, SOCK_RAW,NETLINK_FIREWALL);
        if (sockfd <= 0)
        { 
            //
            std::cout<<"error sockfd"<<std::endl;
        }
        bzero(&address_s, sizeof(sockaddr_nl));
        bzero(&address_d, sizeof(sockaddr_nl));
        address_s.nl_family = AF_NETLINK;
        address_s.nl_pid = PORT;
        address_s.nl_groups = 0;
        address_d.nl_family = AF_NETLINK;
        address_d.nl_groups = 0;
        address_d.nl_pid = 0;
        if (bind(sockfd, (sockaddr *)&address_s, sizeof(sockaddr_nl)) != 0)
        {
             std::cout<<"error bind"<<std::endl;
        }
        nlhdr = (nlmsghdr *)malloc(NLMSG_SPACE(NL_BUFFER));
        bzero(nlhdr,NLMSG_SPACE(NL_BUFFER));
        nlhdr->nlmsg_flags = 0;
        nlhdr->nlmsg_seq = 0;
        nlhdr->nlmsg_type = 0;
        nlhdr->nlmsg_pid = PORT;
        nlhdr->nlmsg_len = NLMSG_SPACE(NL_BUFFER);
        skaddress_len = sizeof(sockaddr_nl);
    }
    void send_msg_to_kernel(char *msg,int len)
    {
        memcpy(NLMSG_DATA(nlhdr),msg,len);
        int size=sendto(sockfd,nlhdr,nlhdr->nlmsg_len,0,(sockaddr*)&address_d,skaddress_len);
        if(size<0)
        {
             std::cout<<"error size"<<std::endl;
        }
        bzero(NLMSG_DATA(nlhdr),len);
    }

private:
    int sockfd = 0;
    sockaddr_nl address_s, address_d;
    user_msg_info usr;
    nlmsghdr *nlhdr = nullptr;
    socklen_t skaddress_len;
    std::unordered_set<uint32_t> set_ips;
     std::unordered_set<uint32_t> set_ports;
};
struct msg
{
    char ctl[2];
    uint32_t value;
};
int main()
{
 	  Connect *t;
      t=new Connect;
      t->init();
      msg m;
      m.ctl[0]='r';
      m.ctl[1]='i';
      m.value=inet_addr("127.0.0.1");
      t->send_msg_to_kernel((char*)&m,sizeof(msg));
    return 0;
}
