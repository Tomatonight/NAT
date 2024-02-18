#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netlink.h>
#include <linux/types.h>
#include <net/sock.h>
#include "hookfuncs.h"
#include "netlink.h"
extern struct net init_net;
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linux Firewall Kernal Module");
MODULE_AUTHOR("YYQ");
int init_module()
{
    if (init_hook_functions() < 0 || init_netlink() < 0)
        printk("init firewall error\n");
    else
        printk("init firewall done\n");
    return 0;
}
void cleanup_module()
{
    deinit_hook_functions();
    deinit_netlink();
    printk("deinit firewall done\n");
}