#include "netlink.h"
#include"hookfuncs.h"
#include"nat.h"
#include<linux/in.h>
struct sock *netlink_sock;
struct netlink_kernel_cfg cfg = {
    .input = read_msg, /* set recv callback */
};
extern bool open_nat;
extern u_int32_t mid_ip;
extern struct nat_entry *tcp_entrys;
extern struct nat_entry *udp_entrys;
int init_netlink()
{
    netlink_sock=netlink_kernel_create(&init_net, NETLINK_PROTOCOL, &cfg);
    if(!netlink_sock)
    {
        printk("netlink_sock error\n");
        return -1;
    }
    return 0;
}
void deinit_netlink()
{
    if(netlink_sock)
    {
         netlink_kernel_release(netlink_sock);
    }
}
void read_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = nlmsg_hdr(skb);
    struct msg *msg;
    msg = NLMSG_DATA(nlh);
    if(msg->action==INFORMATION)
    {
        send_all_rules();
        return;
    }
    else if(msg->action==NAT_START)
    {
        if(open_nat)return;
        mid_ip=msg->s_ip;
        open_nat=true;
        return;
    }
    else if(msg->action==NAT_STOP)
    {
        open_nat=false;
        memset(tcp_entrys,0,sizeof(struct nat_entry)*MAX_NAT_ENTRYS);
        memset(udp_entrys,0,sizeof(struct nat_entry)*MAX_NAT_ENTRYS);
        return;
    }
    parse_msg(msg);
}
int send_msg(struct msg *m)
{
   // printk("send msg %d\n",m->action);
    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;
    int ret;
    int len=sizeof(struct msg);
    nl_skb = nlmsg_new(len, GFP_ATOMIC);
    if (!nl_skb)
    {
        printk("nlmsg_new error\n");
        return -1;
    }
    nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_PROTOCOL, len, 0);
    if (nlh == NULL)
    {
        printk("nlmsg_put error\n");
        nlmsg_free(nl_skb); 
        return -1;
    }
    memcpy(nlmsg_data(nlh), m, len);
    ret = netlink_unicast(netlink_sock, nl_skb, NETLINK_PORT, MSG_DONTWAIT);
    if(ret<0)
    {
        printk("netlink send error\n");
    }
    return ret;
}