#ifndef NAT_H
#define NAT_H
#include <linux/types.h>
#include <linux/hashtable.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/types.h>
#include<linux/net.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <net/checksum.h>
#include <linux/tcp.h>
#include <linux/string.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include "bitmap.h"
#define MAX_NAT_ENTRY 10000
#define TIME_1S HZ
#define CLEAR_TIME 10*TIME_1S    // 10s 10000
#define ENTRY_TIMEOUT TIME_1S*100 // 100s
#define NAT_PORT_START 2000
#define NAT_PORT_NUMBER 10000
struct NAT_ENTRY
{
    union
    {
        u_int64_t key_in;
        struct
        {
            u_int32_t proxy_ip;
            u_int16_t proxy_port;
            u_int16_t proxy_zero;
        };
    };
    union
    {
        u_int64_t key_out;
        struct
        {
            u_int32_t sip;
            u_int16_t sport;
            u_int16_t s_zero;
        };
    };
    u_int32_t dip;
    u_int16_t dport;
    u_int64_t timeout;
    struct hlist_node hash_node_out;
    struct hlist_node hash_node_in;
    struct list_head list_node;
    //***************
};
/*
struct hash_node
{
    u_int32_t key;
    struct NAT_ENTRY *entry;
    struct hlist_node list_node;
};
struct list_node
{
    struct NAT_ENTRY *entry;
    struct list_head list;
};*/
void init_nat(void);
void deinit_nat(void);
bool search_nat_out(struct sk_buff *skb);
bool search_nat_in(struct sk_buff *skb);
struct NAT_ENTRY *search_entry_in(u_int32_t sip, u_int32_t dip, u_int16_t sport, u_int16_t dport);
struct NAT_ENTRY *search_entry_out(u_int32_t sip, u_int32_t dip, u_int16_t sport, u_int16_t dport);
#endif