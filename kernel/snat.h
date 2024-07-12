#ifndef SNAT_H
#define SNAT_H
#include <linux/types.h>
#include <linux/hashtable.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/net.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <net/checksum.h>
#include <linux/tcp.h>
#include <linux/string.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include "bitmap.h"
#define NAT_PROXY_IP "192.168.72.140"
#define TIME_1S HZ
#define CLEAR_TIME 10 * TIME_1S     // 10s 10000
#define ENTRY_TIMEOUT TIME_1S * 100 // 100s
#define NAT_PORT_TCP_START 10000
#define NAT_PORT_TCP_NUMBER 10000
#define NAT_PORT_UDP_START 10000
#define NAT_PORT_UDP_NUMBER 10000
struct NAT_ENTRY
{

    u_int64_t key_in;
    u_int64_t key_out;
    u_int32_t proxy_ip;
    u_int16_t proxy_port;
    u_int32_t sip;
    u_int16_t sport;
    u_int32_t dip;
    u_int16_t dport;
    u_int64_t timeout;
    u_int8_t protocol;
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
void start_snat(u_int32_t);
void stop_snat(void);
void init_snat(void);
void deinit_snat(void);
bool search_snat_out(struct sk_buff *skb);
bool search_snat_in(struct sk_buff *skb);
void print_ip_address(struct in_addr *in);
void print_ip(u_int32_t sip, u_int16_t sport, u_int32_t dip, u_int16_t dport);
u_int64_t get_key(u_int32_t ip, u_int16_t port,u_int8_t protocol);
struct NAT_ENTRY *search_entry_in(u_int32_t sip, u_int32_t dip, u_int16_t sport, u_int16_t dport,u_int8_t protocol);
struct NAT_ENTRY *search_entry_out(u_int32_t sip, u_int32_t dip, u_int16_t sport, u_int16_t dport,u_int8_t protocol);
#endif