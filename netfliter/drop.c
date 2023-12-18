#include <linux/kernel.h> // For KERN_INFO
#include <linux/module.h> // For all Kernal Modules
#include <linux/netfilter.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netlink.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <net/sock.h>
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linux Firewall Kernal Module");
MODULE_AUTHOR("Danis Fermi");
#define MAX_BAN 2048
#define PORT_1 100
#define PORT_2 101
static unsigned int ban_ips[MAX_BAN];
static unsigned int ip_index = 0;
bool defence_start = false;
int defence_port = 0;
typedef struct two_ports
{
    union
    {
        unsigned int ip;
        unsigned short ports[2];
    };
} two_ports;
static struct two_ports ban_ports[MAX_BAN];
static unsigned int port_index = 0;
/*
struct nf_hook_state {
    unsigned int hook;
    u_int8_t pf;
    struct net_device *in;
    struct net_device *out;
    struct sock *sk;
    struct net *net;
    int (*okfn)(struct net *, struct sock *, struct sk_buff *);
};
*/
typedef enum send_msg_type
{
    BAN_IP = 0,
    BAN_PORT,
    ADD_DDOS,
    ADD_SCAN,
} send_msg_type;
typedef struct send_test_msg
{
    enum send_msg_type type;
    unsigned int ip;
    unsigned short port;
} send_test_msg;
typedef struct send_msg
{
    enum send_msg_type type;
    unsigned int data;
    char buf[20]; 
} send_msg;
static struct nf_hook_ops nfhook;
struct sock *nlsk = NULL;
// Function called by Hook
static void reset_ips_and_ports(void)
{
    ip_index = 0;
    port_index = 0;
    memset(&ban_ips[0], 0, MAX_BAN * 4);
    memset(&ban_ports[0], 0, MAX_BAN * 4);
}
int send_to_usr(char *pbuf, uint16_t len, int port)
{
    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;
    int ret;

    // Create sk_buff using nlmsg_new().
    nl_skb = nlmsg_new(len, GFP_ATOMIC);
    if (!nl_skb)
    {
        return -1;
    }

    // Set up nlmsghdr.
    nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_FIREWALL, len, 0);
    if (nlh == NULL)
    {

        nlmsg_free(nl_skb); // If nlmsg_put() failed, nlmsg_free() will free sk_buff.
        return -1;
    }

    // Copy pbuf to nlmsghdr payload.
    memcpy(nlmsg_data(nlh), pbuf, len);
    ret = netlink_unicast(nlsk, nl_skb, port, MSG_DONTWAIT);
    return ret;
}
typedef struct revc_msg
{
    char ctl[2];
    struct two_ports ip_and_ports;
} revc_msg;
void ban_ip(uint32_t ip)
{
    if (ip_index >= MAX_BAN)
        return;
    ban_ips[ip_index++] = ip;
}
void ban_port(struct two_ports ports)
{
    if (port_index >= MAX_BAN)
        return;
    ban_ports[port_index].ip = ports.ip;
    port_index++;
}
void recover_ip(uint32_t ip)
{
    for (int i = 0; i < ip_index; i++)
    {
        if (ban_ips[i] == ip)
        {
            ban_ips[i] = 0;
            memcpy(&ban_ips[i], &ban_ips[i + 1], 4 * (ip_index - i - 1));
            ip_index--;
            return;
        }
    }
}
void recover_port(struct two_ports ports)
{
    for (int i = 0; i < port_index; i++)
    {
        if (ban_ports[i].ip == ports.ip)
        {
            ban_ports[i].ip = 0;
            memcpy(&ban_ports[i], &ban_ports[i + 1], (port_index - i - 1) * 4);
            port_index--;
            return;
        }
    }
}
bool test_ip(uint32_t ip)
{
    for (int i = 0; i < ip_index; i++)
    {
        if (ban_ips[i] == ip)
            return 1;
    }
    return 0;
}
bool test_port(unsigned short port)
{

    for (int i = 0; i < port_index; i++)
    {
        if (port >= ban_ports[i].ports[0] && port <= ban_ports[i].ports[1])
            return 1;
    }
    return 0;
}
void parse_msg(revc_msg *msg)
{

    switch (msg->ctl[0])
    {
    case 'b':
        if (msg->ctl[1] == 'i')
        {

            ban_ip(msg->ip_and_ports.ip);
        }
        else if (msg->ctl[1] == 'p')
        {
            ban_port(msg->ip_and_ports);
        }
        break;
    case 'r': //
        if (msg->ctl[1] == 'i')
        {

            recover_ip(msg->ip_and_ports.ip);
        }
        else if (msg->ctl[1] == 'p')
        {

            recover_port(msg->ip_and_ports);
        }
        else if (msg->ctl[1] == 'e')
        {
            reset_ips_and_ports();
        }
        break;
    case 'c':                   // change
        if (msg->ctl[1] == 'o') // open
        {
           
            defence_start = true;
        
        }
        else if (msg->ctl[1] == 'c') // close
        {
   
            defence_start = false;
        }
        else if (msg->ctl[1] == 'p')
        {
            defence_port=msg->ip_and_ports.ip;
        }
    }
}
void receive_from_usr(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = NULL;
    char *usr_msg = NULL;
    nlh = nlmsg_hdr(skb);
    usr_msg = NLMSG_DATA(nlh);
    parse_msg((revc_msg *)usr_msg);
}
void send_for_ddos(unsigned int ip, struct tcphdr *tcp)
{
    if (tcp->dest==defence_port)
    { 
         printk("send one");
        struct send_test_msg send;
        
        send.type = ADD_DDOS;
        send.ip = ip;
        send_to_usr((char *)&send, sizeof(send_test_msg), PORT_2);
    }
}
void send_for_scan(unsigned short port, struct tcphdr *tcp)
{
   
    if (tcp->syn == 1 && tcp->ack != 1)
    {
       
        struct send_test_msg send;
        send.type = ADD_SCAN;
        send.ip = port;
        send.port=tcp->source;
        send_to_usr((char *)&send, sizeof(send_test_msg), PORT_2);
    }
}
//------------------------------------------------------

static unsigned int hookfunc(void *a,
                             struct sk_buff *skb,
                             const struct nf_hook_state *state)
{
    struct ethhdr *ether = (struct ethhdr *)(skb_mac_header(skb));
    //  short ethernet_type = ntohs(ether->ether_type);
    struct iphdr *ip = (struct iphdr *)(skb_network_header(skb));

    if (test_ip(ip->saddr))
    {
        struct send_msg send;
        memset(&send, 0, sizeof(send));
        send.type = BAN_IP;
        send.data = ip->saddr;
        memcpy(send.buf, state->in->name, strlen(state->in->name));
        send_to_usr((char *)&send, sizeof(send), PORT_1);
        return NF_DROP;
    }

    switch (ip->protocol)
    {
    case IPPROTO_TCP:

        struct tcphdr *tcp = (struct tcphdr *)skb_transport_header(skb);
        if (test_port(tcp->source))
        {
            struct send_msg send;
            memset(&send, 0, sizeof(send));
            memcpy(send.buf, state->in->name, strlen(state->in->name));
            send.data = tcp->source;
            send.type = BAN_PORT;
            send_to_usr((char *)&send, sizeof(send), PORT_1);
            return NF_DROP;
        }
        if (defence_start)
        {
            send_for_ddos(ip->saddr, tcp);
            send_for_scan(ip->saddr, tcp);
        }
        break;
    case IPPROTO_UDP:
  
        struct udphdr *udp = (struct udphdr *)skb_transport_header(skb);
        if (test_port(udp->source))
        {
            struct send_msg send;
            memset(&send, 0, sizeof(send));
            memcpy(send.buf, state->in->name, strlen(state->in->name));
            send.data = udp->source;
            send.type = BAN_PORT;
            send_to_usr((char *)&send, sizeof(send), PORT_1);
            return NF_DROP;
        }
        break;
    default:
   
        break;
    }

    //    if (test_port())
    //  union temp t;
    // t.address=ip->saddr;
    //   struct net_device *dev_in=state->in;
    // struct net_device *dev_out=state->out;
    // char buffer[100]={0};

    //  sprintf(buffer,"%d.%d.%d.%d\n",t.addr[0],t.addr[1],t.addr[2],t.addr[3]);
    //  printk("%s",buffer);
    //	send_to_usr(buffer,strlen(buffer));

    return NF_ACCEPT; // accept  packets
}
struct netlink_kernel_cfg cfg =
    {
        .input = receive_from_usr,
};
int init_module()
{

    nfhook.hook = hookfunc;               // Call hookfunc if match condition
    nfhook.hooknum = NF_INET_PRE_ROUTING; // After sanity checks, before routing decisions
    nfhook.pf = PF_INET;                  // Protocol Family IPv4
    nfhook.priority = NF_IP_PRI_FIRST;    // Placement of hook function in the order of execution
    nf_register_net_hook(&init_net, &nfhook);
    nlsk = netlink_kernel_create(&init_net, NETLINK_FIREWALL, &cfg);

    if (nlsk == NULL)
    {
        printk("netlink_kernel_create error !\n");
        return -1;
    }
    printk(KERN_INFO "Loaded Firewall to Kernal Module\n");
    return 0;
}

// Called when module is Unloaded using 'rmmod'
void cleanup_module()
{

    printk(KERN_INFO "Unloading Firewall to Kernal Module\n");
    nf_unregister_net_hook(&init_net, &nfhook);
    if (nlsk)
    {
        netlink_kernel_release(nlsk);
        nlsk = NULL;
    }
}
