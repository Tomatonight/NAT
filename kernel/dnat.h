#ifndef DNAT_H
#define DNAT_H
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
struct entry
{
    u_int64_t key_in;
    u_int64_t key_out;
    u_int32_t private_ip;//net
    u_int16_t private_port;//net
    u_int32_t public_ip;//net
    u_int16_t public_port;//net
    struct hlist_node hash_node_out;
    struct hlist_node hash_node_in;
};
void dnat_init(void);
void dnat_deinit(void);
void add_entry(u_int32_t private_ip, u_int16_t private_port, u_int32_t public_ip, u_int16_t public_port);
void delete_entry(u_int32_t private_ip, u_int16_t private_port, u_int32_t public_ip, u_int16_t public_port);
void search_dnat_in(struct sk_buff *skb);
void search_dnat_out(struct sk_buff *skb);
#endif