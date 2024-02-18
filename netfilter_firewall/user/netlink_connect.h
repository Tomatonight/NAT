#ifndef USER_NETLINK_CONNECT_H
#define USER_NETLINK_CONNECT_H
#include<sys/socket.h>
#include <linux/netlink.h>
#include <sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include"msg.h"
#define NETLINK_PROTOCOL 22
#define NETLINK_MAX_PAYLOAD sizeof(class msg)
#define NETLINK_PORT 100
struct netlink_header_and_msg
{
    nlmsghdr hdr;
    msg m;
};
class netlink_connect
{
    public:
    void init();
    static netlink_connect* get_individual();
    int write_msg(msg*);
    int read_msg(msg*);
    void inline closefd(){if(netlink_fd>0)close(netlink_fd);}
    ~netlink_connect(){closefd();}
    int netlink_fd=0;
    private:

    sockaddr_nl netlink_addr_s;
    sockaddr_nl netlink_addr_d;
    nlmsghdr* netlink_header;
};
#endif
