#ifndef HOOK_FUNC_H
#define HOOK_FUNC_H
#include <linux/netfilter_ipv4.h>
#include <linux/netlink.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/types.h>
#include"filter.h"
#include"nat.h"
int init_hookfunc(void);
void deinit_hookfunc(void);
unsigned int hook_func_filter(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
unsigned int hook_func_nat(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

#endif
