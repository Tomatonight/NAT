#include "dnat.h"
#define NIPQUAD(addr)                \
    ((unsigned char *)&addr)[0],     \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

static struct entry *search_entry_in(u_int32_t dip, u_int16_t dport);
static struct entry *search_entry_out(u_int32_t sip, u_int16_t sport);
DEFINE_HASHTABLE(entry_hash,6);
static rwlock_t rwlock = __RW_LOCK_UNLOCKED(rwlock);
void dnat_init()
{
    hash_init(entry_hash);
    return;
}
void dnat_deinit()
{
    return;
}
static u_int64_t get_key(u_int32_t ip, u_int16_t port)
{
    return (u_int64_t)(((((u_int64_t)ip) << 32) & 0xffff0000) | port);
}
void search_dnat_in(struct sk_buff *skb)
{
   
    struct iphdr *ip = (struct iphdr *)(skb_network_header(skb));
    u_int32_t sip, dip;
    u_int16_t sport, dport;
    struct udphdr *udp;
    struct tcphdr *tcp;
    sip = (ip->saddr);
    dip = (ip->daddr);
    switch (ip->protocol)
    {
    case IPPROTO_TCP:
        tcp = (struct tcphdr *)skb_transport_header(skb);
        sport = tcp->source;
        dport = tcp->dest;
        break;
    case IPPROTO_UDP:
        udp = (struct udphdr *)skb_transport_header(skb);
        sport = udp->source;
        dport = udp->dest;
        break;
    default:
        return;
    }
    read_lock(&rwlock);
    struct entry *found_entry = search_entry_in(dip, dport);
    if (!found_entry)
    {
        read_unlock(&rwlock);
        return;
    }
    printk("find\n");
    ip->daddr = found_entry->private_ip;
    ip->check = 0;
    ip->check = ip_fast_csum((unsigned char *)ip, ip->ihl);
    switch (ip->protocol)
    {
    case IPPROTO_TCP:
        tcp->dest = found_entry->private_port;
        tcp->check = 0;
        skb->csum = csum_partial(tcp, ntohs(ip->tot_len) - ip->ihl * 4, 0);
        tcp->check = csum_tcpudp_magic(ip->saddr, ip->daddr, ntohs(ip->tot_len) - ip->ihl * 4,
                                       IPPROTO_TCP, skb->csum);

        break;
    case IPPROTO_UDP:
        udp->dest = found_entry->private_port;
        udp->check = 0;
        skb->csum = csum_partial(udp, ntohs(ip->tot_len) - ip->ihl * 4, 0);
        udp->check = csum_tcpudp_magic(ip->saddr, ip->daddr, ntohs(ip->tot_len) - ip->ihl * 4,
                                       IPPROTO_UDP, skb->csum);
        break;
    }
    skb->ip_summed = CHECKSUM_UNNECESSARY;
    read_unlock(&rwlock);
    return;
}
void search_dnat_out(struct sk_buff *skb)
{
    if (skb_is_nonlinear(skb))
        skb_linearize(skb);
    struct iphdr *ip = (struct iphdr *)(skb_network_header(skb));
    u_int32_t sip, dip;
    u_int16_t sport, dport;
    struct udphdr *udp;
    struct tcphdr *tcp;
    sip = ntohl(ip->saddr);
    dip = ntohl(ip->daddr);
    switch (ip->protocol)
    {
    case IPPROTO_TCP:
        tcp = (struct tcphdr *)skb_transport_header(skb);
        sport = tcp->source;
        dport = tcp->dest;
        break;
    case IPPROTO_UDP:
        udp = (struct udphdr *)skb_transport_header(skb);
        sport = udp->source;
        dport = udp->dest;
        break;
    default:
        return;
    }
    read_lock(&rwlock);
    struct entry *found_entry = search_entry_out(sip, sport);
    if (!found_entry)
    {
        read_unlock(&rwlock);
        return;
    }
    printk("find_\n");
    ip->saddr = found_entry->public_ip;
    ip->check = 0;
    ip->check = ip_fast_csum((unsigned char *)ip, ip->ihl);
    switch (ip->protocol)
    {
    case IPPROTO_TCP:
        tcp->source = found_entry->public_port;
        tcp->check = 0;
        skb->csum = csum_partial(tcp, ntohs(ip->tot_len) - ip->ihl * 4, 0);
        tcp->check = csum_tcpudp_magic(ip->saddr, ip->daddr, ntohs(ip->tot_len) - ip->ihl * 4,
                                       IPPROTO_TCP, skb->csum);

        break;
    case IPPROTO_UDP:
        udp->source = found_entry->public_port;
        udp->check = 0;
        skb->csum = csum_partial(udp, ntohs(ip->tot_len) - ip->ihl * 4, 0);
        udp->check = csum_tcpudp_magic(ip->saddr, ip->daddr, ntohs(ip->tot_len) - ip->ihl * 4,
                                       IPPROTO_UDP, skb->csum);
        break;
    }
    read_unlock(&rwlock);
    skb->ip_summed = CHECKSUM_NONE;
    return;
}
void add_entry(u_int32_t private_ip, u_int16_t private_port, u_int32_t public_ip, u_int16_t public_port)
{
    u_int64_t flag;
    write_lock_irqsave(&rwlock, flag);
    struct entry *found_entry = search_entry_in(public_ip, public_port);
    struct entry *found_entry_ = search_entry_out(private_ip, private_port);
    if (found_entry || found_entry_)
    {
        write_unlock_irqrestore(&rwlock, flag);
        printk("add dnat entry err\n");
        return;
    }

    struct entry *new_entry = (struct entry *)kzalloc(sizeof(struct entry), GFP_ATOMIC);
    new_entry->private_ip = private_ip;
    new_entry->private_port = private_port;
    new_entry->public_ip = public_ip;
    new_entry->public_port = public_port;
    new_entry->key_in=get_key(public_ip, public_port);
    new_entry->key_out=get_key(private_ip, private_port);
  //  printk("add %d.%d.%d.%d %d %d\n", NIPQUAD(private_ip), ntohs(private_port), get_key(private_ip, private_port));
    //printk("add %d.%d.%d.%d %d %d\n", NIPQUAD(public_ip), ntohs(public_port), get_key(public_ip, public_port));
    hash_add(entry_hash, &new_entry->hash_node_out, new_entry->key_out);
    hash_add(entry_hash, &new_entry->hash_node_in, new_entry->key_in);
    write_unlock_irqrestore(&rwlock, flag);
}
void delete_entry(u_int32_t private_ip, u_int16_t private_port, u_int32_t public_ip, u_int16_t public_port)
{
    u_int64_t flag;
    write_lock_irqsave(&rwlock, flag);
    struct entry *found_entry = search_entry_in(public_ip, public_port);
    struct entry *found_entry_ = search_entry_out(private_ip, private_port);
    if(!found_entry||found_entry_||found_entry!=found_entry_)
    {
        write_unlock_irqrestore(&rwlock, flag);
        return;
    }
    hash_del(&found_entry->hash_node_out);
    hash_del(&found_entry->hash_node_in);
    kfree(entry_hash);
    write_unlock_irqrestore(&rwlock, flag);
}
struct entry *search_entry_in(u_int32_t dip, u_int16_t dport)
{
    struct entry *entry_ = NULL;
    u_int64_t key = get_key(dip, dport);
 //   printk("search %d.%d.%d.%d %d\n", NIPQUAD(dip), ntohs(key));
    hash_for_each_possible(entry_hash, entry_, hash_node_in, key)
    {

        if ( entry_->key_in== key)
        {
            return entry_;
        }
    }
    return NULL;
}
struct entry *search_entry_out(u_int32_t sip, u_int16_t sport)
{
    struct entry *entry_;
    u_int64_t key = get_key(sip, sport);
    hash_for_each_possible(entry_hash, entry_, hash_node_out, key)
    {
  
        if (entry_->key_out == key)
        {
            return entry_;
        }
    }
    return NULL;
}