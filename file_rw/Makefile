KERNELDIR ?= /lib/modules/$(shell uname -r)/build
 
MODULE_NAME = mycdev
 
obj-m += $(MODULE_NAME).o
 
all:
	make -C $(KERNELDIR) M=$(shell pwd) modules
 
install:
	make -C $(KERNELDIR) M=$(shell pwd) modules_install INSTALL_MOD_PATH=$$PWD
 
clean:
	make -C $(KERNELDIR) M=$(shell pwd) clean