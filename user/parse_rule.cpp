#include "parse_rule.h"
#include "msg.h"
#include <arpa/inet.h>
#include <string>
static uint16_t mask_string_to_16(std::string s)
{
    // 255.255
    std::regex re("([0-9]+).([0-9]+)");
    std::cmatch match;
    if (std::regex_match(s.data(), match, re))
    {
        std::string l = match[1];
        std::string r = match[2];
        uint8_t left = atoi(l.data()) & 0xff;
        uint8_t right = atoi(r.data()) & 0xff;
        return left << 8 | right;
    }
    else
    {
        return 0xffff;
    }
}
static std::string mask_16_to_string(uint16_t mask)
{
    uint8_t left = (mask & 0xff00) >> 8;
    uint8_t right = (mask & 0xff);
    return std::to_string(left) + "." + std::to_string(right);
}
static uint32_t uint_to_mask(uint32_t sum)
{
    uint32_t re = 0;
    while (sum-- > 0)
    {
        re = (re >> 1) | 0x80000000;
    }
    return re;
}
void parse_str(std::string str, netlink_connect *connect)
{
    if (str.find("show") != std::string::npos)
    {
        MSG_RULE msg;
        msg.action = MSG_ACTION_SHOW_RULES;
        connect->write((char *)&msg, sizeof(MSG_RULE));
        return;
    }

    if (str.find("delete") != std::string::npos)
    {
        std::cmatch match;
        int index = 0;
        std::regex reg("[ ]*delete[ ]*([0-9]+)[ \n]*");
        if (std::regex_match(str.data(), match, reg))
        {
            MSG_RULE msg;
            msg.action = MSG_ACTION_DELETE_ACL_RULE;
            std::string s = match[1];
            index = atoi(s.data());
            msg.id = index;
            connect->write((char *)&msg, sizeof(MSG_RULE));
            return;
        }
        else
        {
            printf("fault delete\n");
            return;
        }
    }

    std::cmatch match;
    std::string action;     // 1
    std::string hook_point; // 2
    std::string protocol;   // 3
    std::string sip;        // 4
    std::string sip_mask;   // 5
    std::string dip;        // 6
    std::string dip_mask;   // 7
    std::string sport;      // 8
    std::string sport_mask; // 9
    std::string dport;      // 10
    std::string dport_mask; // 11
    std::regex reg("^[ ]*(\\S+)[ ]+(\\S+)[ ]+(\\S+)[ ]+(\\S+)[ ]+(\\S+)[ ]+(\\S+)[ ]+(\\S+)[ ]+(\\S+)[ ]+(\\S+)[ ]+(\\S+)[ ]+(\\S+)[ \n]*$");
    if (std::regex_match(str.data(), match, reg))
    {
        //  printf("%s\n",str.data());
        MSG_RULE msg;
        action = match[1];
        hook_point = match[2];
        protocol = match[3];
        sip = match[4];
        sip_mask = match[5];
        dip = match[6];
        dip_mask = match[7];
        sport = match[8];
        sport_mask = match[9];
        dport = match[10];
        dport_mask = match[11];
        if (action == "add")
        {
            msg.action = MSG_ACTION_ADD_ACL_RULE;
        }
        else
        {
            printf("action fault\n");
            return;
        }
        if (hook_point == "input")
        {
            msg.hook_point = MSG_HOOK_INPUT;
        }
        else if (hook_point == "output")
        {
            msg.hook_point = MSG_HOOK_OUTPUT;
        }
        else if (hook_point == "forward")
        {
            msg.hook_point = MSG_HOOK_FORWORLD;
        }
        else if (hook_point == "prerouting")
        {
            msg.hook_point = MSG_HOOK_PRE_ROUTING;
        }
        else if (hook_point == "postrouting")
        {
            msg.hook_point = MSG_HOOK_POST_ROUTING;
        }
        else
        {
            printf("hook point fault\n");
            return;
        }
        if (protocol == "TCP")
        {
            msg.protocol = MSG_PROTOCOL_TCP;
        }
        else if (protocol == "UDP")
        {
            msg.protocol = MSG_PROTOCOL_UDP;
        }
        else if (protocol == "ICMP")
        {
            msg.protocol = MSG_PROTOCOL_ICMP;
        }
        else if (protocol == "ALL")
        {
            msg.protocol = MSG_PROTOCOL_ALL;
        }
        else
        {
            printf("protocol fault\n");
            return;
        }
        msg.sip = ntohl(inet_addr(sip.data()));
        msg.dip = ntohl(inet_addr(dip.data()));
        msg.sip_mask = ntohl(inet_addr(sip_mask.data()));
        msg.dip_mask = ntohl(inet_addr(dip_mask.data()));
        //*****************************
        msg.dport = (uint16_t)atoi(dport.data());
        msg.sport = (uint16_t)atoi(sport.data());
        uint16_t sum1 = mask_string_to_16(dport_mask);
        uint16_t sum2 = mask_string_to_16(sport_mask);
        msg.dport_mask = sum1;
        msg.sport_mask = sum2;
        // print_msg(&msg);
        connect->write((char *)&msg, sizeof(MSG_RULE));
    }
    else
        printf("fault rule\n");
}
void print_msg(MSG_RULE *msg)
{

    std::string hook, protocol;
    switch (msg->hook_point)
    {
    case MSG_HOOK_FORWORLD:
        hook = "forward";
        break;
    case MSG_HOOK_OUTPUT:
        hook = "output";
        break;
    case MSG_HOOK_INPUT:
        hook = "input";
        break;
    case MSG_HOOK_POST_ROUTING:
        hook = "post_routing";
        break;
    case MSG_HOOK_PRE_ROUTING:
        hook = "pre_routing";
        break;
    default:
        hook = "err";
        break;
    }
    switch (msg->protocol)
    {
    case MSG_PROTOCOL_TCP:
        protocol = "TCP";
        break;
    case MSG_PROTOCOL_UDP:
        protocol = "UDP";
        break;
    case MSG_PROTOCOL_ICMP:
        protocol = "ICMP";
        break;
    case MSG_PROTOCOL_ALL:
        protocol = "ALL";
        break;
    default:
        protocol = "err";
        break;
    }
    in_addr addr;
    addr.s_addr = htonl(msg->sip);
    std::string sip(inet_ntoa(addr));
    addr.s_addr = htonl(msg->dip);
    std::string dip(inet_ntoa(addr));
    addr.s_addr = htonl(msg->sip_mask);
    std::string sip_mask(inet_ntoa(addr));
    addr.s_addr = htonl(msg->sip_mask);
    std::string dip_mask(inet_ntoa(addr));
    std::string sport_mask = mask_16_to_string(msg->sport_mask);
    std::string dport_mask = mask_16_to_string(msg->dport_mask);

    printf("id: %d hook: %s protocol: %s\n\
sip: %s sip_mask: %s dip: %s dip_mask: %s\nsport: %d sport_mask: %s dport: %d dport_mask %s\n\n",
           msg->id, hook.data(), protocol.data(), sip.data(), sip_mask.data(), dip.data(), dip_mask.data(), msg->sport, sport_mask.data(), msg->dport, dport_mask.data());
}