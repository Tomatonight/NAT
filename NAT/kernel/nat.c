#include "nat.h"
// static struct NAT_ENTRY nat_entrys[MAX_NAT_ENTRY];
static u_int32_t proxy_ip_;
static struct bitmap bp_ports;
static struct timer_list timer;
static rwlock_t rwlock;
LIST_HEAD(entry_list);
DEFINE_HASHTABLE(entry_hash_in, 10);
DEFINE_HASHTABLE(entry_hash_out, 10);
static void timer_callback(struct timer_list *timer);
static void delete_entry(struct NAT_ENTRY *entry);
static void updata_entry(struct NAT_ENTRY *entry);
static struct NAT_ENTRY *alloc_nat_entry(u_int32_t sip, u_int32_t dip, u_int32_t proxy_ip,
                                         u_int16_t sport, u_int16_t dport, u_int16_t proxy_port);
bool search_nat_out(struct sk_buff *skb)
{
    printk("search out\n");
    struct iphdr *ip = (struct iphdr *)(skb_network_header(skb));
    u_int32_t sip, dip;
    u_int16_t sport, dport;
    int protocol;
    struct udphdr *udp;
    struct tcphdr *tcp;
    sip = ntohl(ip->saddr);
    dip = ntohl(ip->daddr);
    switch (ip->protocol)
    {
    case IPPROTO_UDP:
    {
        udp = (struct udphdr *)skb_transport_header(skb);
        sport = ntohs(udp->source);
        dport = ntohs(udp->dest);
        break;
    }
    case IPPROTO_TCP:
    {
        tcp = (struct tcphdr *)skb_transport_header(skb);
        sport = ntohs(tcp->source);
        dport = ntohs(tcp->dest);
        break;
    }
    default:
        return 1;
    }
    read_lock(&rwlock);
    struct NAT_ENTRY *entry = search_entry_out(sip, dip, dport, dport);
    if (!entry)
    {
        int temp = alloc_value(&bp_ports);
        if (temp == -1)
            return 0;
        u_int16_t proxy_port = temp;
        read_unlock(&rwlock);
        write_lock(&rwlock);
        entry = alloc_nat_entry(sip, dip, proxy_ip_, sport, dport, proxy_port);
        write_unlock(&rwlock);
        read_lock(&rwlock);
    }
    // change saddr
    updata_entry(entry);
    ip->saddr = htonl(entry->proxy_ip);
    ip->check = 0;
    ip->check = ip_fast_csum((unsigned char *)ip, ip->ihl);
    switch (ip->protocol)
    {
    case IPPROTO_TCP:
    {
        tcp->source = htons(entry->sport);
        tcp->check = 0;
        tcp->check = csum_tcpudp_magic(ip->saddr, ip->daddr, skb->len - ip->ihl * 4,
                                       IPPROTO_TCP, csum_partial(tcp, skb->len - ip->ihl * 4, 0));
        break;
    }
    case IPPROTO_UDP:
    {
        udp->source = htons(entry->sport);
        udp->check = 0;
        udp->check = csum_tcpudp_magic(ip->saddr, ip->daddr, skb->len - ip->ihl * 4,
                                       IPPROTO_UDP, csum_partial(udp, skb->len - ip->ihl * 4, 0));
        break;
    }
    }
    read_unlock(&rwlock);
    // skb->ip_summed = CHECKSUM_NONE
    return 1;
}
bool search_nat_in(struct sk_buff *skb)
{
    printk("search in\n");
    struct iphdr *ip = (struct iphdr *)(skb_network_header(skb));
    u_int32_t sip, dip;
    u_int16_t sport, dport;
    int protocol;
    struct udphdr *udp;
    struct tcphdr *tcp;
    sip = ntohl(ip->saddr);
    dip = ntohl(ip->daddr);
    switch (ip->protocol)
    {
    case IPPROTO_UDP:
    {
        udp = (struct udphdr *)skb_transport_header(skb);
        sport = ntohs(udp->source);
        dport = ntohs(udp->dest);
        break;
    }
    case IPPROTO_TCP:
    {
        tcp = (struct tcphdr *)skb_transport_header(skb);
        sport = ntohs(tcp->source);
        dport = ntohs(tcp->dest);
        break;
    }
    default:
        return 1;
    }
    read_lock(&rwlock);
    struct NAT_ENTRY *entry = search_entry_in(sip, dip, dport, dport);
    if (!entry)
        return 0;
    // change daddr
    updata_entry(entry);
    ip->daddr = htonl(entry->sip);
    ip->check = 0;
    ip->check = ip_fast_csum((unsigned char *)ip, ip->ihl);
    switch (ip->protocol)
    {
    case IPPROTO_TCP:
    {
        tcp->dest = htons(entry->sport);
        tcp->check = 0;
        tcp->check = csum_tcpudp_magic(ip->saddr, ip->daddr, skb->len - ip->ihl * 4,
                                       IPPROTO_TCP, csum_partial(tcp, skb->len - ip->ihl * 4, 0));
        break;
    }
    case IPPROTO_UDP:
    {
        udp->dest = htons(entry->dport);
        udp->check = 0;
        udp->check = csum_tcpudp_magic(ip->saddr, ip->daddr, skb->len - ip->ihl * 4,
                                       IPPROTO_UDP, csum_partial(udp, skb->len - ip->ihl * 4, 0));
        break;
    }
    }
    read_unlock(&rwlock);
    // skb->ip_summed = CHECKSUM_NONE
    return 1;
}
void init_nat()
{
    //  memset(nat_entrys, 0, sizeof(struct NAT_ENTRY) * MAX_NAT_ENTRY);
    proxy_ip_ = ntohl(in_aton("192.168.72.131"));
    timer_setup(&timer, timer_callback, 0);
    // 启动定时器，设置超时时间
    mod_timer(&timer, jiffies + msecs_to_jiffies(CLEAR_TIME));
    // alloc_bitmap(&bp_entrys, 0, MAX_NAT_ENTRY);
    alloc_bitmap(&bp_ports, NAT_PORT_START, NAT_PORT_NUMBER);
}
void deinit_nat()
{
    //  free_bitmap(bp_entrys);
    free_bitmap(&bp_ports);
    del_timer(&timer);
}
static struct NAT_ENTRY *alloc_nat_entry(u_int32_t sip, u_int32_t dip, u_int32_t proxy_ip,
                                         u_int16_t sport, u_int16_t dport, u_int16_t proxy_port)
{
    struct in_addr a = {.s_addr = sip}, b = {.s_addr = dip};
    printk("create entry from %lld to %lld\n", sip, dip);
    struct NAT_ENTRY *new_entry = (struct NAT_ENTRY *)kzalloc(sizeof(struct NAT_ENTRY), GFP_KERNEL);
    new_entry->dip = dip;
    new_entry->dport = dport;
    new_entry->proxy_ip = proxy_ip;
    new_entry->proxy_port = proxy_port;
    new_entry->sip = sip;
    new_entry->sport = sport;
    new_entry->timeout = get_jiffies_64() + ENTRY_TIMEOUT;
    list_add_tail(&new_entry->list_node, &entry_list);
    hash_add(entry_hash_in, &new_entry->hash_node_in, new_entry->key_in);
    hash_add(entry_hash_out, &new_entry->hash_node_out, new_entry->key_out);
    return new_entry;
}
static void clear_entry(void)
{
    u_int64_t j = get_jiffies_64();
    write_lock(&rwlock);
    struct NAT_ENTRY *entry, *temp;
    list_for_each_entry_safe(entry, temp, &entry_list, list_node)
    {

        if (time_after(j, entry->timeout))
        {
            delete_entry(entry);
        }
        else
        {
            break;
        }
    };
    write_unlock(&rwlock);
}
static void updata_entry(struct NAT_ENTRY *entry)
{
    entry->timeout = get_jiffies_64() + ENTRY_TIMEOUT;
    list_del(&entry->list_node);
    list_add_tail(&entry->list_node, &entry_list);
}
static void timer_callback(struct timer_list *timer)
{
    clear_entry();
    mod_timer(timer, jiffies + msecs_to_jiffies(CLEAR_TIME));
}
struct NAT_ENTRY *search_entry_in(u_int32_t sip, u_int32_t dip, u_int16_t sport, u_int16_t dport)
{
    u_int64_t key_in = (u_int64_t)(dip) | (((u_int64_t)dport << 32) & 0x00ff0000);
    struct NAT_ENTRY *entry;
    hash_for_each_possible(entry_hash_in, entry, hash_node_in, key_in)
    {
        if (entry->key_in == key_in)
        {
            return entry;
        }
    }
    return NULL;
}
struct NAT_ENTRY *search_entry_out(u_int32_t sip, u_int32_t dip, u_int16_t sport, u_int16_t dport)
{
    u_int64_t key_out = (u_int64_t)(sip) | (((u_int64_t)sport << 32) & 0x00ff0000);
    struct NAT_ENTRY *entry;
    hash_for_each_possible(entry_hash_out, entry, hash_node_out, key_out)
    {

        if (entry->key_out == key_out)
        {
            return entry;
        }
    };
    return NULL;
}
static void delete_entry(struct NAT_ENTRY *entry)
{
    u_int16_t proxy_port = entry->proxy_port;
    free_value(&bp_ports, proxy_port);
    hash_del(&entry->hash_node_out);
    hash_del(&entry->hash_node_in);
    list_del(&entry->list_node);
    kfree(entry);
}