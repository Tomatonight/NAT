#ifndef CONNECT_H
#define CONNECT_H
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include "msg.h"
#include "filter.h"
#define NETLINK_PORT 100
#define NETLINK_PROTOCOL 22
int init_netlink(void);
void deinit_netlink(void);
void read_msg(struct sk_buff *skb);
int send_msg(struct MSG_RULE *);
void parse_msg(struct MSG_RULE *msg);

#endif