#ifndef FILTER_H
#define FILTER_H
#include<linux/tcp.h>
#include<linux/string.h>
#include<linux/udp.h>
#include <linux/slab.h>
#include<linux/icmp.h>
#include<linux/in.h>
#include<linux/ip.h>
#include <linux/rwlock.h>
#include <linux/inet.h>
#include"msg.h"
#include"connect.h"
#define MAX_RULES_ENTRY 1000
void init_rule_entrys(void);
void deinit_rule_entrys(void);
int search_rule_entrys(struct sk_buff *skb,int hook_point);
int add_rule_entry(struct MSG_RULE* msg);
int delete_rule_entry(struct MSG_RULE* msg);
void send_all_rules(void);
#endif