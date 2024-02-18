#ifndef KERNEL_NETLINK_H
#define KERNEL_NETLINK_H
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include"msg.h"
#define NETLINK_PORT 100
#define NETLINK_PROTOCOL 22
int init_netlink(void);
void deinit_netlink(void);
void read_msg(struct sk_buff *skb);
int send_msg(struct msg*);
#endif