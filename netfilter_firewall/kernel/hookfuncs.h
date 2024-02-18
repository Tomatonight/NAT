#ifndef KERNEL_HOOKFUNCS_H
#define KERNEL_HOOKFUNCS_H
#include <linux/netfilter_ipv4.h>
#include <linux/netlink.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <net/sock.h>
#include<linux/types.h>
#include"msg.h"
#include"netlink.h"
#define HOOK_MAX_RULES 30
int init_hook_functions(void);
void deinit_hook_functions(void);
unsigned int hook_func_input(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
unsigned int hook_func_forward(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
unsigned int hook_func_output(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
unsigned int hook_func_nat_post_routing(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
unsigned int hook_func_nat_pre_routing(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
void send_all_rules(void);
void parse_msg(struct msg* m);

#endif