#ifndef KERNEL_NAT_H
#define KERNEL_NAT_H
#include<linux/types.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#define MAX_NAT_ENTRYS 200
#define TIMEOUT_TCP 60*60
#define TIMEOUT_UDP 60
#define POOL_BYTES 30000/8
#define BASE_PORT 10000

int get_empty_bitmap_idx(void);
void set_bitmap_idx(int idx,bool flag);
int get_free_port(void);
void free_port(int);
struct nat_entry
{
    __u32 s_ip;
    __u16 s_port;
    __u32 d_ip;
    __u16 d_port;
    __u32 mid_ip;
    __u16 mid_port;
    __u32 timeout;
};
struct psd_header
{
unsigned long saddr;  
unsigned long daddr; 
char mbz; 
char ptcl; 
unsigned short len;
};
__u32 get_time_after(int seconds);
struct nat_entry* search_entrys_output(struct nat_entry* entrys,__u32 s_ip,__u16 s_port,__u32 d_ip,__u16 d_port);
struct nat_entry* search_entrys_input(struct nat_entry* entrys,__u32 s_ip,__u16 s_port,__u32 d_ip,__u16 d_port);
struct nat_entry* add_entry(struct nat_entry* entrys,struct nat_entry* entry);
unsigned short  calculate_checksum_tcp(unsigned short * addr, int count,struct iphdr* ip,struct tcphdr* tcp);
unsigned short  calculate_checksum_udp(unsigned short * addr, int count,struct iphdr* ip,struct udphdr* udp);
unsigned short  calculate_checksum_ip(unsigned short * addr, int count);
__u32 sum_every_16bits(void *addr, int count);
__u16 checksum(void *addr, int count, int start_sum);
unsigned short  calculate_checksum_ip(unsigned short *ip_head_buffer, int ip_hdr_len);
#endif