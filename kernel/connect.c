#include "connect.h"
struct sock *netlink_sock;
struct netlink_kernel_cfg cfg = {
    .input = read_msg, /* set recv callback */
};
int init_netlink(void)
{
    netlink_sock = netlink_kernel_create(&init_net, NETLINK_PROTOCOL, &cfg);
    if (!netlink_sock)
    {
        printk("create netlink err\n");
        return -1;
    }
    return 0;
}
void deinit_netlink()
{
    if (netlink_sock)
    {
        netlink_kernel_release(netlink_sock);
    }
}
void read_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = nlmsg_hdr(skb);
    struct MSG_RULE *msg;
    msg = NLMSG_DATA(nlh);
    parse_msg(msg);
}
int send_msg(struct MSG_RULE *m)
{
  //   printk("send msg %d %d\n",m->hook_point,m->protocol);
    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;
    int ret;
    int len = sizeof(struct MSG_RULE);
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
    if (ret < 0)
    {
        printk("netlink send error\n");
    }
    return ret;
}
void parse_msg(struct MSG_RULE *msg)
{
    switch (msg->action)
    {
    case MSG_ACTION_ADD_ACL_RULE:
        if(add_rule_entry(msg)>0)
        {
           printk("add rule\n");
        }
        else printk("add rule err\n");
        break;
    case MSG_ACTION_DELETE_ACL_RULE:
        delete_rule_entry(msg);
        break;
    case MSG_ACTION_START_NAT:

        break;
    case MSG_ACTION_SHOW_RULES:
        send_all_rules();
        break;
    default:
        printk("recv msg err\n");
        break;
    }
}
