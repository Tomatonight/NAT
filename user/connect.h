#pragma once
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <linux/netlink.h>
#include <thread>
#include"msg.h"
#define NETLINK_PROTOCOL 22
// #define NETLINK_MAX_PAYLOAD sizeof(class msg)
#define NETLINK_PORT 100
class netlink_connect
{
public:
    void init();
    int write(char *buffer, int len);
    int read(char *buffer, int len);

private:
    std::thread read_thread;
    int netlink_fd;
    sockaddr_nl netlink_addr_s;
    sockaddr_nl netlink_addr_d;
};