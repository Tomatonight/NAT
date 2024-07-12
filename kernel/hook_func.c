#include "hook_func.h"
struct nf_hook_ops hook_input, hook_forward, hook_output;
struct nf_hook_ops hook_post_routing, hook_pre_routing;
struct nf_hook_ops hook_snat_in, hook_snat_out;
struct nf_hook_ops hook_dnat_in, hook_dnat_out;
int init_hookfunc(void)
{
    init_rule_entrys();
    init_snat();
    dnat_init();
    hook_input.hook = hook_func_filter;
    hook_forward.hook = hook_func_filter;
    hook_output.hook = hook_func_filter;
    hook_post_routing.hook = hook_func_filter;
    hook_pre_routing.hook = hook_func_filter;
    hook_snat_in.hook = hook_func_snat;
    hook_snat_out.hook = hook_func_snat;
    hook_dnat_in.hook = hook_func_dnat;
    hook_dnat_out.hook = hook_func_dnat;
    //////////////////
    hook_input.hooknum = NF_INET_LOCAL_IN;
    hook_forward.hooknum = NF_INET_FORWARD;
    hook_output.hooknum = NF_INET_LOCAL_OUT;
    hook_post_routing.hooknum = NF_INET_POST_ROUTING;
    hook_pre_routing.hooknum = NF_INET_PRE_ROUTING;
    hook_snat_in.hooknum = NF_INET_PRE_ROUTING;
    hook_snat_out.hooknum = NF_INET_POST_ROUTING;
    hook_dnat_in.hooknum = NF_INET_PRE_ROUTING;
    hook_dnat_out.hooknum = NF_INET_POST_ROUTING;
    ////////////////
    hook_input.pf = PF_INET;
    hook_forward.pf = PF_INET;
    hook_output.pf = PF_INET;
    hook_post_routing.pf = PF_INET;
    hook_pre_routing.pf = PF_INET;
    hook_snat_in.pf = PF_INET;
    hook_snat_out.pf = PF_INET;
    hook_dnat_in.pf = PF_INET;
    hook_dnat_out.pf = PF_INET;
    //////////////
    hook_input.priority = NF_IP_PRI_FILTER;
    hook_forward.priority = NF_IP_PRI_FILTER;
    hook_output.priority = NF_IP_PRI_FILTER;
    hook_post_routing.priority = NF_IP_PRI_FILTER;
    hook_pre_routing.priority = NF_IP_PRI_FILTER;
    hook_snat_in.priority = NF_IP_PRI_FIRST;
    hook_snat_out.priority = NF_IP_PRI_FIRST;
    hook_dnat_in.priority = NF_IP_PRI_FIRST+1;
    hook_dnat_out.priority = NF_IP_PRI_FIRST+1;
    nf_register_net_hook(&init_net, &hook_input);
    nf_register_net_hook(&init_net, &hook_forward);
    nf_register_net_hook(&init_net, &hook_output);
    nf_register_net_hook(&init_net, &hook_post_routing);
    nf_register_net_hook(&init_net, &hook_pre_routing);
    nf_register_net_hook(&init_net, &hook_snat_in);
    nf_register_net_hook(&init_net, &hook_snat_out);
    nf_register_net_hook(&init_net, &hook_dnat_in);
    nf_register_net_hook(&init_net, &hook_dnat_out);
    return 1;
}
void deinit_hookfunc(void)
{
    deinit_rule_entrys();
    deinit_snat();
    dnat_deinit();
    nf_unregister_net_hook(&init_net, &hook_input);
    nf_unregister_net_hook(&init_net, &hook_forward);
    nf_unregister_net_hook(&init_net, &hook_output);
    nf_unregister_net_hook(&init_net, &hook_post_routing);
    nf_unregister_net_hook(&init_net, &hook_pre_routing);
    nf_unregister_net_hook(&init_net, &hook_snat_in);
    nf_unregister_net_hook(&init_net, &hook_snat_out);
    nf_unregister_net_hook(&init_net, &hook_dnat_in);
    nf_unregister_net_hook(&init_net, &hook_dnat_out);
}
unsigned int hook_func_filter(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    unsigned int hook_point = state->hook;
    int re = 0;
    switch (hook_point)
    {
    case NF_INET_LOCAL_OUT:
        re = search_rule_entrys(skb, MSG_HOOK_OUTPUT);
        break;
    case NF_INET_LOCAL_IN:
        re = search_rule_entrys(skb, MSG_HOOK_INPUT);
        break;
    case NF_INET_FORWARD:
        re = search_rule_entrys(skb, MSG_HOOK_FORWORLD);
        break;
    case NF_INET_PRE_ROUTING:
        re = search_rule_entrys(skb, MSG_HOOK_PRE_ROUTING);
        break;
    case NF_INET_POST_ROUTING:
        re = search_rule_entrys(skb, MSG_HOOK_POST_ROUTING);
        break;
    }
    return re == 0 ? NF_ACCEPT : NF_DROP;
}
unsigned int hook_func_snat(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{

    unsigned int hook_point = state->hook;
    int re = 0;
    switch (hook_point)
    {
    case NF_INET_PRE_ROUTING:
        if (search_snat_in(skb))
            return NF_ACCEPT;
        else
            return NF_DROP;
    case NF_INET_POST_ROUTING:
        if (search_snat_out(skb))
            return NF_ACCEPT;
        else
            return NF_DROP;
        break;
    default:
        return NF_ACCEPT;
    }
}
unsigned int hook_func_dnat(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{

    unsigned int hook_point = state->hook;
    int re = 0;
    switch (hook_point)
    {
    case NF_INET_PRE_ROUTING:
        search_dnat_in(skb);
        break;
    case NF_INET_POST_ROUTING:
        search_dnat_out(skb);
        break;
    }
    return NF_ACCEPT;
}