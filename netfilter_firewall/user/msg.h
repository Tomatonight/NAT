#ifndef USER_MSG_H
#define USER_MSG_H
#include<sys/types.h>
enum msg_hook_point
{
    INPUT=0,
    OUTPUT,
    FORWARD
};
enum msg_protocol
{
    TCP=1,
    UDP,
    ICMP
};
enum msg_action
{
    SIGN_UP=1,
    REMOVE,
    INFORMATION,
    INFORMATION_NED,
    NAT_START,
    NAT_STOP,
};
enum msg_handle
{
    ACCEPT=1,
    REJECT
};
class msg
{
public:
enum msg_action action;
enum msg_hook_point hook_point;
enum msg_protocol protocol;
enum msg_handle handle;
u_int32_t d_ip;
u_int32_t d_ip_mask;
u_int32_t s_ip;
u_int32_t s_ip_mask;
u_int16_t d_port;
u_int16_t s_port;
int id;
msg(enum msg_action action_,enum msg_hook_point hook_point_,enum msg_protocol protocol_,
enum msg_handle handle_,u_int32_t d_ip_,u_int32_t s_ip_,u_int16_t d_port_,u_int16_t s_port_,u_int32_t d_mask,u_int32_t s_mask):
action(action_),hook_point(hook_point_),handle(handle_),d_ip(d_ip_),s_ip(s_ip_),d_port(d_port_),
    s_port(s_port_),protocol(protocol_),d_ip_mask(d_mask),s_ip_mask(s_mask)
{
}
msg(){};
};
#endif
