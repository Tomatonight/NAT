#include "filter.h"
static rwlock_t rwlock;
static struct MSG_RULE *rules_entry;
static u_int32_t max_rule_number;
void init_rule_entrys(void)
{
    max_rule_number=0;
    rules_entry = kmalloc(sizeof(struct MSG_RULE) * MAX_RULES_ENTRY, GFP_KERNEL);
    memset(rules_entry,0,sizeof(struct MSG_RULE)*MAX_RULES_ENTRY);
}
void deinit_rule_entrys(void)
{
    kfree(rules_entry);
}
int search_rule_entrys(struct sk_buff *skb, int hook_point)
{
    read_lock(&rwlock);
    struct iphdr *ip = (struct iphdr *)(skb_network_header(skb));
    u_int32_t sip, dip;
    u_int16_t sport, dport;
    int protocol;
    sip = ntohl(ip->saddr);
    dip = ntohl(ip->daddr);
    switch (ip->protocol)
    {
    case IPPROTO_TCP:
    {
        protocol = MSG_PROTOCOL_TCP;
        struct tcphdr *tcp = (struct tcphdr *)skb_transport_header(skb);
        sport = ntohs(tcp->source);
        dport = ntohs(tcp->dest);
        break;
    }
    case IPPROTO_UDP:
    {
        struct udphdr *udp = (struct udphdr *)skb_transport_header(skb);
        sport = ntohs(udp->source);
        dport = ntohs(udp->dest);
        protocol = MSG_PROTOCOL_UDP;
        break;
    }
    case IPPROTO_ICMP:
    {
        protocol = MSG_PROTOCOL_ICMP;
        break;
    }
    default:
        read_unlock(&rwlock);
        return 0;
    }

    for (int i = 0; i <= max_rule_number; i++)
    {

        if (hook_point != rules_entry[i].hook_point ||
            rules_entry[i].protocol != MSG_PROTOCOL_ALL &&
                protocol != rules_entry[i].protocol)
            continue;
        switch (protocol)
        {
        case MSG_PROTOCOL_ICMP:
            if ((rules_entry[i].sip & rules_entry[i].sip_mask) == (sip & rules_entry[i].sip_mask) && (rules_entry[i].dip & rules_entry[i].dip_mask) == (dip & rules_entry[i].dip_mask))
            {
              //  printk("drop \n");
                read_unlock(&rwlock);
                return 1;
            }
            break;
        case MSG_PROTOCOL_UDP:
        case MSG_PROTOCOL_TCP:
        {
          //  printk("test\n");
            if ((rules_entry[i].sip & rules_entry[i].sip_mask) == (sip & rules_entry[i].sip_mask) && (rules_entry[i].dip & rules_entry[i].dip_mask) == (dip & rules_entry[i].dip_mask) &&
                (rules_entry[i].sport & rules_entry[i].sport_mask) == (sport & rules_entry[i].sport_mask )&&
                (rules_entry[i].dport & rules_entry[i].dport_mask) == (dport & rules_entry[i].dport_mask))
            {
              //   printk("drop\n");
                read_unlock(&rwlock);
                return 1;
            }
        }
        break;
        default:
            break;
        }
    }
    read_unlock(&rwlock);
    return 0;
}
int add_rule_entry(struct MSG_RULE *msg)
{
    write_lock(&rwlock);
    int index = 0;
    while (index < MAX_RULES_ENTRY)
    {
        if (rules_entry[index].protocol == 0)
            break;
        index++;
    }
    if (index == MAX_RULES_ENTRY)
    {
        write_unlock(&rwlock);
        return 0;
    }
    memcpy(&rules_entry[index], msg, sizeof(struct MSG_RULE));
    if (index > max_rule_number)
        max_rule_number = index;
    write_unlock(&rwlock);
    return 1;
}
int delete_rule_entry(struct MSG_RULE *msg)
{
    write_lock(&rwlock);
    int delete_index = msg->id;
    if (delete_index < 0 || delete_index >= MAX_RULES_ENTRY)
    {
        write_unlock(&rwlock);
        return 0;
    }
    memset(&rules_entry[delete_index], 0, sizeof(struct MSG_RULE));
    if (delete_index == max_rule_number)
    {
        while (max_rule_number > 0 && rules_entry[max_rule_number].protocol == 0)
        {
            max_rule_number--;
        }
    }
    write_unlock(&rwlock);
    return 1;
}
void send_all_rules(void)
{
    read_lock(&rwlock);
    for (int i = 0; i <= max_rule_number; i++)
    {
        if (rules_entry[i].protocol != 0)
        {
            rules_entry[i].id = i;
            send_msg(&rules_entry[i]);
        }
    }
    read_unlock(&rwlock);
}