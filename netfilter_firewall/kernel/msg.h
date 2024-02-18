#ifndef KERNEL_MSG_H
#define KERNEL_MSG_H
#include <linux/types.h>
enum msg_hook_point
{
    INPUT = 0,
    OUTPUT,
    FORWARD
};
enum msg_protocol
{
    TCP = 1,
    UDP,
    ICMP
};
enum msg_action
{
    SIGN_UP = 1,
    REMOVE,
    INFORMATION,
    INFORMATION_NED,
    NAT_START,
    NAT_STOP,
};
enum msg_handle
{
    ACCEPT = 1,
    REJECT
};
struct msg
{
    enum msg_action action;
    enum msg_hook_point hook_point;
    enum msg_protocol protocol;
    enum msg_handle handle;
    __u32 d_ip;
    __u32 d_ip_mask;
    __u32 s_ip;
    __u32 s_ip_mask;
    __u16 d_port;
    __u16 s_port;
    int id; // for delete
};
#endif