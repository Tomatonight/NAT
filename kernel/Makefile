obj-m += Firewall.o  
Firewall-objs :=firewall.o hook_func.o connect.o  filter.o bitmap.o snat.o dnat.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
