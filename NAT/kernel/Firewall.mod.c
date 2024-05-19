#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x31bb8db1, "__nlmsg_put" },
	{ 0xe113bbbc, "csum_partial" },
	{ 0x37a0cba, "kfree" },
	{ 0xd4d89b8e, "__netlink_kernel_create" },
	{ 0xdc0e4855, "timer_delete" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0xf56593f1, "__alloc_skb" },
	{ 0x5dcfdcd2, "nf_unregister_net_hook" },
	{ 0x39d3b564, "nf_register_net_hook" },
	{ 0xa8adec82, "init_net" },
	{ 0x3aea62f5, "netlink_unicast" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x4db74291, "kfree_skb_reason" },
	{ 0xe68efe41, "_raw_write_lock" },
	{ 0xb4a1b970, "netlink_kernel_release" },
	{ 0xdd4d55b6, "_raw_read_unlock" },
	{ 0x40235c98, "_raw_write_unlock" },
	{ 0x9ed12e20, "kmalloc_large" },
	{ 0xfb578fc5, "memset" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x15ba50a6, "jiffies" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0xfe8c61f0, "_raw_read_lock" },
	{ 0x1b6314fd, "in_aton" },
	{ 0x850e6a88, "kmalloc_trace" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0xad6d045f, "kmalloc_caches" },
	{ 0x453e7dc, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "D7EA6F91E276D0CE54F8C5B");
