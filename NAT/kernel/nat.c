#include "nat.h"
#define NIPQUAD(addr)                \
    ((unsigned char *)&addr)[0],     \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

// static struct NAT_ENTRY nat_entrys[MAX_NAT_ENTRY];
static u_int32_t proxy_ip_;
static struct bitmap bp_ports;
static struct timer_list timer;
static rwlock_t rwlock = __RW_LOCK_UNLOCKED(rwlock);
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
    //  printk("search out\n");
    struct iphdr *ip = (struct iphdr *)(skb_network_header(skb));
    if (ip->saddr == htonl(proxy_ip_))
        return 1;
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
    //  printk("search in\n");
    //   print_ip(sip, sport, dip, dport);
    struct NAT_ENTRY *entry = search_entry_out(sip, dip, sport, dport);
    if (!entry)
    {
        //   printk("create new entry\n");
        int temp = alloc_value(&bp_ports);
        if (temp < 0)
        {
            read_unlock(&rwlock);
            return 0;
        }
        u_int16_t proxy_port = (u_int16_t)temp;
        read_unlock(&rwlock);
        write_lock(&rwlock);
        entry = alloc_nat_entry(sip, dip, proxy_ip_, sport, dport, proxy_port);
        write_unlock(&rwlock);
        read_lock(&rwlock);
    }
    //  printk("sport: %d\n",sport);
    // change saddr
    updata_entry(entry);
    ip->saddr = htonl(entry->proxy_ip);
    ip->check = 0;
    ip->check = ip_fast_csum((unsigned char *)ip, ip->ihl);
    switch (ip->protocol)
    {
    case IPPROTO_TCP:
    {
        tcp->source = htons(entry->proxy_port);
        tcp->check = 0;
        skb->csum = csum_partial(tcp, skb->len - ip->ihl * 4, 0);
        tcp->check = csum_tcpudp_magic(ip->saddr, ip->daddr, skb->len - ip->ihl * 4,
                                       IPPROTO_TCP, skb->csum);

        break;
    }
    case IPPROTO_UDP:
    {
        udp->source = htons(entry->proxy_port);
        udp->check = 0;
        skb->csum = csum_partial(udp, skb->len - ip->ihl * 4, 0);
        udp->check = csum_tcpudp_magic(ip->saddr, ip->daddr, skb->len - ip->ihl * 4,
                                       IPPROTO_UDP, skb->csum);
        break;
    }
    }
    read_unlock(&rwlock);
    //  skb->ip_summed = CHECKSUM_UNNECESSARY;
    //  printk("change after\n");
    //  print_ip(entry->proxy_ip,entry->proxy_port , entry->dip, entry->dport);
    return 1;
}
bool search_nat_in(struct sk_buff *skb)
{
    struct iphdr *ip = (struct iphdr *)(skb_network_header(skb));
    if (ip->daddr != htonl(proxy_ip_))
    {
        return 1;
    }
    u_int32_t sip, dip;
    u_int16_t sport, dport;
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
    // printk("search in\n");
    //   print_ip(sip, sport, dip, dport);
    struct NAT_ENTRY *entry = search_entry_in(sip, dip, sport, dport);
    if (!entry)
    {
        printk("drop a in pkt\n");
        read_unlock(&rwlock);
        return 0;
    }
    else
    {
        printk("find\n");
    }
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
        skb->csum = csum_partial(tcp, skb->len - ip->ihl * 4, 0);
        tcp->check = csum_tcpudp_magic(ip->saddr, ip->daddr, skb->len - ip->ihl * 4,
                                       IPPROTO_TCP, skb->csum);
        break;
    }
    case IPPROTO_UDP:
    {
        udp->dest = htons(entry->sport);
        udp->check = 0;
        skb->csum = csum_partial(udp, skb->len - ip->ihl * 4, 0);
        udp->check = csum_tcpudp_magic(ip->saddr, ip->daddr, skb->len - ip->ihl * 4,
                                       IPPROTO_UDP, skb->csum);
        break;
    }
    }
    read_unlock(&rwlock);
    // skb->ip_summed = CHECKSUM_UNNECESSARY;
    //   struct in_addr t={.s_addr=ip->daddr};
    //  printk("daddr \n");
    //  print_ip_address(&t);
    //   skb->ip_summed = CHECKSUM_NONE;
    print_ip(ntohl(ip->saddr), 0, ntohl(ip->daddr), 0);
    return 1;
}
void init_nat()
{
    //  memset(nat_entrys, 0, sizeof(struct NAT_ENTRY) * MAX_NAT_ENTRY);
    proxy_ip_ = ntohl(in_aton("192.168.72.141"));
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
    struct NAT_ENTRY *new_entry = (struct NAT_ENTRY *)kzalloc(sizeof(struct NAT_ENTRY), GFP_KERNEL);
    new_entry->dip = dip;
    new_entry->dport = dport;
    new_entry->proxy_ip = proxy_ip;
    new_entry->proxy_port = proxy_port;
    new_entry->sip = sip;
    new_entry->sport = sport;
    u_int64_t key_in = get_key(proxy_ip, proxy_port);
    u_int64_t key_out = get_key(sip, sport);
    new_entry->key_in = key_in;
    new_entry->key_out = key_out;
    new_entry->timeout = get_jiffies_64() + ENTRY_TIMEOUT;
    list_add_tail(&new_entry->list_node, &entry_list);

    //    printk("key out %lld\n",key_out);
    printk("add entry \n");
    //   struct in_addr a = {.s_addr = proxy_ip};
    //   print_ip_address(&a);
    hash_add(entry_hash_in, &new_entry->hash_node_in, key_in);
    hash_add(entry_hash_out, &new_entry->hash_node_out, key_out);
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
    //   printk("tick\n");
    mod_timer(timer, jiffies + msecs_to_jiffies(CLEAR_TIME));
}
struct NAT_ENTRY *search_entry_in(u_int32_t sip, u_int32_t dip, u_int16_t sport, u_int16_t dport)
{
    struct NAT_ENTRY *entry;
    u_int64_t key_in = get_key(dip, dport);
    //  printk("search in dport %d key_in %llu\n", dport, key_in);
    //  struct in_addr a = {.s_addr = dip};
    //  print_ip_address(&a);
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
    u_int64_t key_out = get_key(sip, sport);
    //   printk("search out key %d key=%lld\n",sport,key_out);
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
void print_ip_address(struct in_addr *in)
{
    printk("%d.%d.%d.%d", NIPQUAD(in->s_addr));
}
void print_ip(u_int32_t sip, u_int16_t sport, u_int32_t dip, u_int16_t dport)
{
    struct in_addr a = {.s_addr = htonl(sip)}, b = {.s_addr = htonl(dip)};
    print_ip_address(&a);
    printk("%d", sport);
    print_ip_address(&b);
    printk("%d\n", dport);
}
u_int64_t get_key(u_int32_t ip, u_int16_t port)
{
    return (u_int64_t)ip | ((u_int64_t)port << 32);
}