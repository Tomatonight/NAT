#pragma once
#include<sys/types.h>
#define MSG_ACTION_ADD_ACL_RULE 0x1
#define MSG_ACTION_DELETE_ACL_RULE 0x2
#define MSG_ACTION_START_SNAT 0x3
#define MSG_ACTION_STOP_SNAT 0x4
#define MSG_ACTION_SHOW_RULES 0x5
#define MSG_ACTION_ADD_DNAT 0x6
#define MSG_ACTION_DELETE_DNAT 0x7
//****************
#define MSG_HOOK_PRE_ROUTING 0x1
#define MSG_HOOK_POST_ROUTING 0x2
#define MSG_HOOK_INPUT 0x3
#define MSG_HOOK_OUTPUT 0x4
#define MSG_HOOK_FORWORLD 0x5
//*****************
#define MSG_PROTOCOL_TCP 0x1
#define MSG_PROTOCOL_UDP 0x2
#define MSG_PROTOCOL_ICMP 0x3
#define MSG_PROTOCOL_ALL 0x4
//*****************
#define MAX_MSG_SIZE sizeof(MSG_RULE)

struct MSG_RULE //主机序
{
    int id;
    int action;
    int hook_point;
    int protocol;
    u_int32_t sip;
    u_int32_t sip_mask;
    u_int32_t dip;
    u_int32_t dip_mask;
    u_int16_t sport;
    u_int16_t sport_mask;
    u_int16_t dport;
    u_int16_t dport_mask;
};
