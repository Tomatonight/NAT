obj-m += Firewall.o  
Firewall-objs :=firewall.o  hookfuncs.o netlink.o nat.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
