#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netlink.h>
#include <linux/types.h>
#include <net/sock.h>
#include "hook_func.h"
#include "connect.h"
#include"filter.h"
#include"hook_func.h"
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linux Firewall Kernal Module");
MODULE_AUTHOR("YYQ");
int init_module()
{
    if (init_hookfunc() < 0 || init_netlink() < 0)
        printk("init firewall error\n");
    else
        printk("init firewall done\n");
    return 0;
}
void cleanup_module()
{
    deinit_hookfunc();
    deinit_netlink();
    printk("deinit firewall done\n");
}