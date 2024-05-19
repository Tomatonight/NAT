#include "hook_func.h"
struct nf_hook_ops hook_input, hook_forward, hook_output;
struct nf_hook_ops hook_post_routing, hook_pre_routing;
struct nf_hook_ops hook_nat_in, hook_nat_out;
int init_hookfunc(void)
{
    init_rule_entrys();
    init_nat();
    hook_input.hook = hook_func_filter;
    hook_forward.hook = hook_func_filter;
    hook_output.hook = hook_func_filter;
    hook_post_routing.hook = hook_func_filter;
    hook_pre_routing.hook = hook_func_filter;
    hook_nat_in.hook = hook_func_nat;
    hook_nat_out.hook = hook_func_nat;

    //////////////////
    hook_input.hooknum = NF_INET_LOCAL_IN;
    hook_forward.hooknum = NF_INET_FORWARD;
    hook_output.hooknum = NF_INET_LOCAL_OUT;
    hook_post_routing.hooknum = NF_INET_POST_ROUTING;
    hook_pre_routing.hooknum = NF_INET_PRE_ROUTING;
    hook_nat_in.hooknum = NF_INET_LOCAL_IN;
    hook_nat_out.hooknum = NF_INET_LOCAL_OUT;
    ////////////////
    hook_input.pf = PF_INET;
    hook_forward.pf = PF_INET;
    hook_output.pf = PF_INET;
    hook_post_routing.pf = PF_INET;
    hook_pre_routing.pf = PF_INET;
    hook_nat_in.pf = PF_INET;
    hook_nat_out.pf = PF_INET;
    //////////////
    hook_input.priority = NF_IP_PRI_FIRST;
    hook_forward.priority = NF_IP_PRI_FIRST;
    hook_output.priority = NF_IP_PRI_FIRST;
    hook_post_routing.priority = NF_IP_PRI_FIRST;
    hook_pre_routing.priority = NF_IP_PRI_FIRST;

    hook_nat_in.priority = NF_IP_PRI_LAST;
    hook_nat_out.priority = NF_IP_PRI_LAST;

    nf_register_net_hook(&init_net, &hook_input);
    nf_register_net_hook(&init_net, &hook_forward);
    nf_register_net_hook(&init_net, &hook_output);
    nf_register_net_hook(&init_net, &hook_post_routing);
    nf_register_net_hook(&init_net, &hook_pre_routing);
    nf_register_net_hook(&init_net, &hook_nat_in);
    nf_register_net_hook(&init_net, &hook_nat_out);
    return 1;
}
void deinit_hookfunc(void)
{
    deinit_rule_entrys();
    deinit_nat();
    nf_unregister_net_hook(&init_net, &hook_input);
    nf_unregister_net_hook(&init_net, &hook_forward);
    nf_unregister_net_hook(&init_net, &hook_output);
    nf_unregister_net_hook(&init_net, &hook_post_routing);
    nf_unregister_net_hook(&init_net, &hook_pre_routing);
    nf_unregister_net_hook(&init_net, &hook_nat_in);
    nf_unregister_net_hook(&init_net, &hook_nat_out);
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
unsigned int hook_func_nat(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    printk("test\n");
    unsigned int hook_point = state->hook;
    int re = 0;
    switch (hook_point)
    {
    case NF_INET_LOCAL_IN:
        if (search_nat_in(skb))
            return NF_ACCEPT;
        else
            return NF_DROP;
    case NF_INET_LOCAL_OUT:
        if (search_nat_out(skb))
            return NF_ACCEPT;
     //   else
            return NF_DROP;
        break;
    default:
        return NF_ACCEPT;
    }
}
