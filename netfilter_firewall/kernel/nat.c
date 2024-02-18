#include <linux/ktime.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netlink.h>
#include <linux/in.h>
#include<linux/net.h>
#include "nat.h"
#include "msg.h"
struct nat_entry tcp_entrys[MAX_NAT_ENTRYS];
struct nat_entry udp_entrys[MAX_NAT_ENTRYS];
char ports_bitmap[POOL_BYTES];
__u32 inline get_time_after(int seconds)
{
    return ktime_get_seconds() + seconds;
}
struct nat_entry *search_entrys_output(struct nat_entry *entrys, __u32 d_ip, __u16 d_port,__u32 s_ip, __u16 s_port)
{
    for (int i = 0; i < MAX_NAT_ENTRYS; i++)
    {
        if (entrys->d_ip == d_ip && entrys->d_port == d_port&&entrys->s_ip==s_ip&&entrys->s_port==s_port)
        {
            return entrys;
        }

        entrys++;
    }
    return NULL;
}
struct nat_entry *search_entrys_input(struct nat_entry *entrys, __u32 s_ip, __u16 s_port,__u32 d_ip, __u16 d_port)
{
    for (int i = 0; i < MAX_NAT_ENTRYS; i++)
    {
        if (entrys->d_ip == s_ip && entrys->d_port == s_port&&entrys->mid_ip==d_ip&&entrys->mid_port==d_port)
        {
            return entrys;
        }
        entrys++;
    }
    return NULL;
}
struct nat_entry *add_entry(struct nat_entry *entrys, struct nat_entry *entry)
{
    struct nat_entry* last_entry=entrys;
    for (int i = 0; i < MAX_NAT_ENTRYS; i++)
    {
        //|| entrys->timeout < get_time_after(0)
        if (entrys->d_ip == 0 )
        {
            memcpy(entrys, entry, sizeof(struct nat_entry));
            return entrys;
        }
        if(entrys->timeout<last_entry->timeout)
        {
            last_entry=entrys;
        }
        entrys++;
    }
    printk("clear a entry\n");
    memcpy(last_entry, entry, sizeof(struct nat_entry));
    return last_entry;
}
int get_empty_bitmap_idx(void)
{
    char *tmp = ports_bitmap;
    for (int i = 0; i < POOL_BYTES; i++)
    {
        if (*tmp == 0xff)
        {
            tmp++;
            continue;
        }
        __u8 value = *tmp;
        for (int j = 0; j < 8; j++)
        {
            if (!(value & (0x80 >> j)))
            {
           
                return i * 8 + j;
            }
        }
        printk("bitmap alloc error\n");
        return -1;
    }
    return -1;
}
void set_bitmap_idx(int idx, bool flag)
{
    char *tmp = ports_bitmap + (idx) / 8;
    int index = idx % 8;
    if (flag)
        *tmp = (*tmp) | (0x80 >> index);
    else
        *tmp = (*tmp) & (~(0x80 >> index));
}
int get_free_port()
{
    int idx = get_empty_bitmap_idx();
    if (idx < 0)
        return -1;
    set_bitmap_idx(idx, 1);
    return idx + BASE_PORT;
}
void free_port(int port)
{
    int idx = port - BASE_PORT;
    set_bitmap_idx(idx, 0);
}
unsigned short calculate_checksum_tcp(unsigned short *addr, int count, struct iphdr *ip, struct tcphdr *tcp)
{
    struct psd_header header;
    header.daddr = ip->daddr;
    header.mbz = 0;
    header.ptcl = IPPROTO_TCP;
    header.saddr = ip->saddr;
    header.len =htons(count);
    tcp->check = 0;
    __u32 sum = sum_every_16bits(&header, sizeof(struct psd_header));
    return checksum(addr, count, sum);
}
unsigned short calculate_checksum_udp(unsigned short *addr, int count, struct iphdr *ip, struct udphdr *udp)
{
    struct psd_header header;
    header.daddr = ip->daddr;
    header.mbz = 0;
    header.ptcl = IPPROTO_UDP;
    header.saddr = ip->saddr;
    header.len = udp->len;
    udp->check = 0;
    __u32 sum = sum_every_16bits(&header, sizeof(struct psd_header));
    return checksum(addr, count, sum);
}
unsigned short calculate_checksum_ip(unsigned short *ip_head_buffer, int ip_hdr_len)
{
    ((struct iphdr *)ip_head_buffer)->check = 0;
    return checksum(ip_head_buffer, ip_hdr_len, 0);
}
uint32_t sum_every_16bits(void *addr, int count)
{
    register uint32_t sum = 0;
    __u16 *ptr = addr;
    __u16 answer = 0;
    while (count > 1)
    {
        sum += *ptr++;
        count -= 2;
    }

    if (count == 1)
    {
        *(char *)(&answer) = *(char *)ptr;
        sum += answer;
    }

    return sum;
}

__u16 checksum(void *addr, int count, int start_sum)
{
    uint32_t sum = start_sum;
    sum += sum_every_16bits(addr, count);

    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);
    return ~sum;
}
