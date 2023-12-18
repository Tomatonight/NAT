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
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xf56593f1, "__alloc_skb" },
	{ 0x31bb8db1, "__nlmsg_put" },
	{ 0x69acdf38, "memcpy" },
	{ 0x3aea62f5, "netlink_unicast" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x4db74291, "kfree_skb_reason" },
	{ 0xa916b694, "strnlen" },
	{ 0x122c3a7e, "_printk" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xa19b956, "__stack_chk_fail" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0xa8adec82, "init_net" },
	{ 0x39d3b564, "nf_register_net_hook" },
	{ 0xd4d89b8e, "__netlink_kernel_create" },
	{ 0x5dcfdcd2, "nf_unregister_net_hook" },
	{ 0xb4a1b970, "netlink_kernel_release" },
	{ 0x453e7dc, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "C58D6431BA08A08B136D465");
