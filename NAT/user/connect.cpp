#include "connect.h"
#include <arpa/inet.h>
#include <string>
#include "msg.h"
#include "parse_rule.h"
void netlink_connect::init()
{
    netlink_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_PROTOCOL);
    if (netlink_fd < 0)
    {
        printf("create netlink err\n");
        exit(-1);
    }

    memset(&netlink_addr_s, 0, sizeof(sockaddr_nl));
    netlink_addr_s.nl_family = AF_NETLINK;
    netlink_addr_s.nl_pid = NETLINK_PORT;
    netlink_addr_s.nl_groups = 0;
    if (bind(netlink_fd, (sockaddr *)&netlink_addr_s, sizeof(sockaddr_nl)) != 0)
    {
        printf("bind saddr err\n");
        exit(-1);
    }
    memset(&netlink_addr_d, 0, sizeof(netlink_addr_d));
    netlink_addr_d.nl_family = AF_NETLINK;
    netlink_addr_d.nl_pid = 0;
    netlink_addr_d.nl_groups = 0;
    read_thread = std::move(std::thread([this]()
                                        {
        while(true)
        {
            MSG_RULE msg;
            int re=read((char*)&msg,sizeof(msg));
            if(re<=0)
            {
                printf("read msg err\n");
                exit(-1);
            }
            print_msg(&msg);
        } }));
}
int netlink_connect::write(char *buffer, int len)
{

    nlmsghdr *netlink_header = (nlmsghdr *)malloc(NLMSG_SPACE(len));
    memset(netlink_header, 0, sizeof(nlmsghdr));
    netlink_header->nlmsg_len = NLMSG_SPACE(len);
    netlink_header->nlmsg_flags = 0;
    netlink_header->nlmsg_type = 0;
    netlink_header->nlmsg_seq = 0;
    netlink_header->nlmsg_pid = netlink_addr_s.nl_pid;
    memcpy(NLMSG_DATA(netlink_header), buffer, len);
    int re = sendto(netlink_fd, netlink_header, netlink_header->nlmsg_len, 0,
                    (sockaddr *)&netlink_addr_d, sizeof(sockaddr_nl));
    free(netlink_header);
}
int netlink_connect::read(char *buffer, int len)
{
    char buf[1024] = {0};
   // socklen_t size = sizeof(sockaddr);
    int ret = recvfrom(netlink_fd, buf, sizeof(nlmsghdr) + len, 0,
                       NULL, NULL);
    if (ret < 0)
    {
        printf("read msg err\n");
        exit(-1);
    }
    memcpy(buffer, buf + sizeof(nlmsghdr), sizeof(MSG_RULE));
    return ret;
}