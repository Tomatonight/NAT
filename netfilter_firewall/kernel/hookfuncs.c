#include "hookfuncs.h"
#include "nat.h"
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/icmp.h>
#include <linux/in.h>
#include <linux/net.h>
#include "header.h"

#define NIPQUAD(addr)                \
    ((unsigned char *)&addr)[0],     \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

static bool pass_rule(struct msg *rules, struct sk_buff *skb);
static void add_rule(struct msg *m);
static void remove_rule(struct msg *m);
static inline bool ip_judge(__u32 ip, __u32 ip_, __u32 mask);
static inline bool port_judge(__u16 port_rule, __u16 port_);
bool open_nat = 0;
u_int32_t mid_ip = 0;
static u_int32_t id = 0;
struct nf_hook_ops hook_input, hook_forward, hook_output;
struct nf_hook_ops hook_nat_post_routing, hook_nat_pre_routing;
struct msg input_msg[HOOK_MAX_RULES];
struct msg forward_msg[HOOK_MAX_RULES];
struct msg output_msg[HOOK_MAX_RULES];
extern struct nat_entry tcp_entrys[MAX_NAT_ENTRYS];
extern struct nat_entry udp_entrys[MAX_NAT_ENTRYS];
extern char ports_bitmap[POOL_BYTES];
int init_hook_functions(void)
{

    hook_input.hook = hook_func_input;
    hook_forward.hook = hook_func_forward;
    hook_output.hook = hook_func_output;
    hook_nat_post_routing.hook = hook_func_nat_post_routing;
    hook_nat_pre_routing.hook = hook_func_nat_pre_routing;
    hook_input.hooknum = NF_INET_LOCAL_IN;
    hook_forward.hooknum = NF_INET_FORWARD;
    hook_output.hooknum = NF_INET_LOCAL_OUT;
    hook_nat_post_routing.hooknum = NF_INET_POST_ROUTING;
    hook_nat_pre_routing.hooknum = NF_INET_PRE_ROUTING;
    hook_input.pf = PF_INET;
    hook_forward.pf = PF_INET;
    hook_output.pf = PF_INET;
    hook_nat_post_routing.pf = PF_INET;
    hook_nat_pre_routing.pf = PF_INET;
    hook_input.priority = NF_IP_PRI_FIRST;
    hook_forward.priority = NF_IP_PRI_FIRST;
    hook_output.priority = NF_IP_PRI_FIRST;
    hook_nat_post_routing.priority = NF_IP_PRI_FIRST;
    hook_nat_pre_routing.priority = NF_IP_PRI_FIRST;
    memset((void *)input_msg, 0, sizeof(struct msg) * HOOK_MAX_RULES);
    memset((void *)forward_msg, 0, sizeof(struct msg) * HOOK_MAX_RULES);
    memset((void *)output_msg, 0, sizeof(struct msg) * HOOK_MAX_RULES);
    memset((void *)tcp_entrys, 0, sizeof(struct nat_entry) * MAX_NAT_ENTRYS);
    memset((void *)udp_entrys, 0, sizeof(struct nat_entry) * MAX_NAT_ENTRYS);
    nf_register_net_hook(&init_net, &hook_input);
    nf_register_net_hook(&init_net, &hook_forward);
    nf_register_net_hook(&init_net, &hook_output);
    nf_register_net_hook(&init_net, &hook_nat_post_routing);
    nf_register_net_hook(&init_net, &hook_nat_pre_routing);
    memset(ports_bitmap, 0, POOL_BYTES);
    return 0;
}
void deinit_hook_functions(void)
{

    nf_unregister_net_hook(&init_net, &hook_input);
    nf_unregister_net_hook(&init_net, &hook_forward);
    nf_unregister_net_hook(&init_net, &hook_output);
    nf_unregister_net_hook(&init_net, &hook_nat_post_routing);
    nf_unregister_net_hook(&init_net, &hook_nat_pre_routing);
}
unsigned int hook_func_input(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{

    struct msg *rules = input_msg;
    if (!pass_rule(rules, skb))
    {

        return NF_DROP;
    }
    return NF_ACCEPT;
}
unsigned int hook_func_forward(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{

    struct msg *rules = forward_msg;
    if (!pass_rule(rules, skb))
    {

        return NF_DROP;
    }
    return NF_ACCEPT;
}
unsigned int hook_func_output(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{

    struct msg *rules = output_msg;
    if (!pass_rule(rules, skb))
    {

        return NF_DROP;
    }
    return NF_ACCEPT;
}
unsigned int hook_func_nat_post_routing(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{

      if (!open_nat)
       return NF_ACCEPT;
    struct iphdr *ip = (struct iphdr *)(skb_network_header(skb));
    struct tcphdr *tcp = NULL;
    struct udphdr *udp = NULL;
    struct nat_entry *entrys = NULL;
    int timeout;
    switch (ip->protocol)
    {
    case IPPROTO_TCP:
    {

        tcp = (struct tcphdr *)skb_transport_header(skb);
        //     printk("from %u.%u.%u.%u to %u.%u.%u.%u %d port:%d:%d out ", NIPQUAD(ip->saddr), NIPQUAD(ip->daddr),
        //           ntohs(ip->tot_len) - ip->ihl * 4, ntohs(tcp->source), ntohs(tcp->dest));
        timeout = TIMEOUT_TCP;
        entrys = tcp_entrys;
        break;
    }
    case IPPROTO_UDP:
    {
        return NF_ACCEPT;
        udp = (struct udphdr *)skb_transport_header(skb);
        timeout = TIMEOUT_UDP;
        entrys = udp_entrys;
        break;
    }
    default:
        return NF_ACCEPT;
    }
    struct nat_entry *entry = NULL;
    if (tcp)
        entry = search_entrys_output(entrys, ntohl(ip->daddr), ntohs(tcp->dest), ntohl(ip->saddr), ntohs(tcp->source));
    else
        entry = search_entrys_output(entrys, ntohl(ip->daddr), ntohs(udp->dest), ntohl(ip->saddr), ntohs(udp->source));
    if (!entry)
    {
        // printk("add entry\n");
        u_int32_t proxy_ip = mid_ip;
        u_int32_t proxy_port = get_free_port();
        if (proxy_port < 0)
        {
            printk("ports consumed over\n");
            return NF_DROP;
        }
        struct nat_entry new_entry;
        new_entry.d_ip = ntohl(ip->daddr);
        new_entry.s_ip = ntohl(ip->saddr);
        new_entry.mid_ip = (proxy_ip);
        new_entry.mid_port = proxy_port;
        if (tcp)
        {
            new_entry.s_port = ntohs(tcp->source);
            new_entry.d_port = ntohs(tcp->dest);
        }
        else
        {
            new_entry.s_port = ntohs(udp->source);
            new_entry.d_port = ntohs(udp->dest);
        }
        entry = add_entry(entrys, &new_entry);
        if (!entry)
        {
            //  printk("error :no free nat entry\n");
            free_port(proxy_port);
            return NF_DROP;
        }
    }
    // else  printk("find entry output\n");
    entry->timeout = get_time_after(timeout);
    if (ip->protocol == IPPROTO_TCP)
    {
        tcp->source = htons(entry->mid_port);
        ip->saddr = htonl(entry->mid_ip);
        ip->check = 0;
        ip->check = ip_fast_csum(ip, ip->ihl);

        tcp->check = 0;
        skb->csum = csum_partial((unsigned char *)tcp, ntohs(ip->tot_len) - ip->ihl * 4, 0);
        tcp->check = csum_tcpudp_magic(ip->saddr,
                                       ip->daddr,
                                       ntohs(ip->tot_len) - ip->ihl * 4, ip->protocol,
                                       skb->csum);

        // tcp->check = calculate_checksum_tcp((unsigned short *)tcp, ntohs(ip->tot_len) - ip->ihl * 4, ip, tcp);
        // ip->check = calculate_checksum_ip((unsigned short *)ip, ip->ihl * 4);
        //     printk("after from %u.%u.%u.%u to %u.%u.%u.%u %d port:%d:%d out ", NIPQUAD(ip->saddr), NIPQUAD(ip->daddr),
        //          ntohs(ip->tot_len) - ip->ihl * 4, ntohs(tcp->source), ntohs(tcp->dest));
    }
    else
    {
        ip->check = 0;
        udp->source = htons(entry->mid_port);
        ip->saddr = htonl(entry->mid_ip);
        ip->check = ip_fast_csum(ip, ip->ihl);
        udp->check = 0;
        skb->csum = csum_partial((unsigned char *)udp, ntohs(ip->tot_len) - ip->ihl * 4, 0);
        udp->check = csum_tcpudp_magic(ip->saddr,
                                       ip->daddr,
                                       ntohs(ip->tot_len) - ip->ihl * 4, ip->protocol,
                                       skb->csum);
        // udp->check = calculate_checksum_udp((unsigned short *)udp, ntohs(udp->len), ip, udp);
        //   ip->check = calculate_checksum_ip((unsigned short *)ip, ip->ihl * 4);
    }
    return NF_ACCEPT;
}
unsigned int hook_func_nat_pre_routing(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
      if (!open_nat)
         return NF_ACCEPT;
    struct iphdr *ip = (struct iphdr *)(skb_network_header(skb));
    struct tcphdr *tcp = NULL;
    struct udphdr *udp = NULL;
    struct nat_entry *entrys = NULL;
    int timeout;

    switch (ip->protocol)
    {
    case IPPROTO_TCP:
    {

        tcp = (struct tcphdr *)skb_transport_header(skb);
        timeout = TIMEOUT_TCP;
     //   printk("from %u.%u.%u.%u to %u.%u.%u.%u %d port:%d:%d in \n", NIPQUAD(ip->saddr), NIPQUAD(ip->daddr),
      //         ntohs(ip->tot_len) - ip->ihl * 4, ntohs(tcp->source), ntohs(tcp->dest));
        entrys = tcp_entrys;
        break;
    }
    case IPPROTO_UDP:
    {
        return NF_ACCEPT;
        udp = (struct udphdr *)skb_transport_header(skb);
        timeout = TIMEOUT_UDP;
        //      printk("udp data %d in\n",ntohs(udp->len) );
        entrys = udp_entrys;
        break;
    }
    default:

        return NF_ACCEPT;
    }
    struct nat_entry *entry = NULL;
    if (tcp)
        entry = search_entrys_input(entrys, ntohl(ip->saddr), ntohs(tcp->source), ntohl(ip->daddr), ntohs(tcp->dest));
    else
        entry = search_entrys_input(entrys, ntohl(ip->saddr), ntohs(udp->source), ntohl(ip->daddr), ntohs(udp->dest));
    if (!entry)
    {
        
        return NF_DROP;
    }
    entry->timeout = get_time_after(timeout);
    if (tcp)
    {

        ip->daddr = htonl(entry->s_ip);
        tcp->dest = htons(entry->s_port);
        ip->check = 0;
        ip->check = ip_fast_csum(ip, ip->ihl);
        tcp->check = 0;
        skb->csum = csum_partial((unsigned char *)tcp, ntohs(ip->tot_len) - ip->ihl * 4, 0);
        tcp->check = csum_tcpudp_magic(ip->saddr,
                                       ip->daddr,
                                       ntohs(ip->tot_len) - ip->ihl * 4, ip->protocol,
                                       skb->csum);
      //  printk("after from %u.%u.%u.%u to %u.%u.%u.%u %d port:%d:%d in \n", NIPQUAD(ip->saddr), NIPQUAD(ip->daddr),
       //        ntohs(ip->tot_len) - ip->ihl * 4, ntohs(tcp->source), ntohs(tcp->dest));
        // tcp->check = calculate_checksum_tcp((unsigned short *)tcp, ntohs(ip->tot_len) - ip->ihl * 4, ip, tcp);
        // ip->check = calculate_checksum_ip((unsigned short *)ip, ip->ihl * 4);
    }
    else
    {

        ip->check = 0;
        ip->daddr = htonl(entry->s_ip);
        udp->dest = htons(entry->s_port);
        ip->check = ip_fast_csum(ip, ip->ihl);
        udp->check = 0;

        skb->csum = csum_partial((unsigned char *)udp, ntohs(ip->tot_len) - ip->ihl * 4, 0);
        udp->check = csum_tcpudp_magic(ip->saddr,
                                       ip->daddr,
                                       ntohs(ip->tot_len) - ip->ihl * 4, ip->protocol,
                                       skb->csum);

        // udp->check = calculate_checksum_udp((unsigned short *)udp, ntohs(udp->len), ip, udp);
        // ip->check = calculate_checksum_ip((unsigned short *)ip, ip->ihl * 4);
    }
    return NF_ACCEPT;
}
void parse_msg(struct msg *msg)
{
    switch (msg->action)
    {
    case SIGN_UP:
        add_rule(msg);
        break;
    case REMOVE:
        remove_rule(msg);
        break;
    default:
        break;
    }
}
static void add_rule(struct msg *m)
{

    struct msg *rules;
    switch (m->hook_point)
    {
    case INPUT:
        rules = input_msg;
        break;
    case OUTPUT:
        rules = output_msg;
        break;
    case FORWARD:
        rules = forward_msg;
        break;
    default:
        printk("error hook_point\n");
        return;
    }
    int cnt = 0;
    for (; cnt < HOOK_MAX_RULES; cnt++)
    {
        if (rules->action != 0)
        {
            rules++;
        }
        else
            break;
    }
    if (cnt == HOOK_MAX_RULES)
    {
        return;
    }
    m->id = id++;
    memcpy(rules, m, sizeof(struct msg));
}
static void remove_rule(struct msg *m)
{

    struct msg *rules;
    for (int i = 0; i < 3; i++)
    {
        if (i == 0)
        {
            rules = input_msg;
        }
        else if (i == 1)
        {
            rules = forward_msg;
        }
        else
        {
            rules = output_msg;
        }
        for (int cnt = 0; cnt < HOOK_MAX_RULES; cnt++)
        {
            if (rules->id == m->id)
            {
                memset(rules, 0, sizeof(struct msg));
                return;
            }
            rules++;
        }
    }
}
static bool pass_rule(struct msg *rules, struct sk_buff *skb)
{
    struct ethhdr *ether = (struct ethhdr *)(skb_mac_header(skb));
    struct iphdr *ip = (struct iphdr *)(skb_network_header(skb));
    switch (ip->protocol)
    {
    case IPPROTO_TCP:
    {
        struct tcphdr *tcp = (struct tcphdr *)skb_transport_header(skb);
        for (int i = 0; i < HOOK_MAX_RULES; i++)
        {
            if (!rules->action)
                continue;
            __u32 d_ip = ntohl(ip->daddr);
            __u32 s_ip = ntohl(ip->saddr);
            __u16 d_port = ntohs(tcp->dest);
            __u16 s_port = ntohs(tcp->source);
            if (ip_judge(rules->d_ip, d_ip, rules->d_ip_mask) &&
                ip_judge(rules->s_ip, s_ip, rules->s_ip_mask))
            {
                if (port_judge(d_port, tcp->dest) && port_judge(s_port, tcp->source))
                    return false;
            }

            rules++;
        }
        return true;
    }
    case IPPROTO_UDP:
    {
        struct udphdr *udp = (struct udphdr *)skb_transport_header(skb);
        for (int i = 0; i < HOOK_MAX_RULES; i++)
        {
            if (!rules->action)
                continue;
            __u32 d_ip = ntohl(ip->daddr);
            __u32 s_ip = ntohl(ip->saddr);
            __u16 d_port = ntohs(udp->dest);
            __u16 s_port = ntohs(udp->source);
            if (ip_judge(rules->d_ip, d_ip, rules->d_ip_mask) &&
                ip_judge(rules->s_ip, s_ip, rules->s_ip_mask))
            {
                if (port_judge(d_port, udp->dest) && port_judge(s_port, udp->source))
                    return false;
            }
            rules++;
        }
        return true;
    }
    case IPPROTO_ICMP:
    {
        struct icmphdr *icmp = (struct icmphdr *)skb_transport_header(skb);
        for (int i = 0; i < HOOK_MAX_RULES; i++)
        {
            if (!rules->action)
                continue;
            __u32 d_ip = ntohl(ip->daddr);
            __u32 s_ip = ntohl(ip->saddr);
            if (ip_judge(rules->d_ip, d_ip, rules->d_ip_mask) &&
                ip_judge(rules->s_ip, s_ip, rules->s_ip_mask))
            {
                return false;
            }
            rules++;
        }
        return true;
    }
    }

    return false;
}
static inline bool ip_judge(__u32 ip, __u32 ip_, __u32 mask)
{
    return ((ip & mask) == (ip_ & mask));
}
static inline bool port_judge(__u16 port_rule, __u16 port_)
{
    if (!port_rule)
        return true;
    return port_rule == port_;
}
void send_all_rules()
{
    //  printk("send all\n");
    struct msg *rules = input_msg;
    for (int i = 0; i < HOOK_MAX_RULES; i++)
    {
        if (rules->action != 0)
        {

            send_msg(rules);
        }
        rules++;
    }
    rules = output_msg;
    for (int i = 0; i < HOOK_MAX_RULES; i++)
    {
        if (rules->action != 0)
        {

            send_msg(rules);
        }
        rules++;
    }
    rules = forward_msg;
    for (int i = 0; i < HOOK_MAX_RULES; i++)
    {
        if (rules->action != 0)
        {
            send_msg(rules);
        }
        rules++;
    }
    struct msg m;
    m.action = INFORMATION_NED;
    send_msg(&m);
}