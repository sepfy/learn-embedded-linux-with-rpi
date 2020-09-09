PWD := $(shell pwd) 
KVERSION := $(shell uname -r)
KERNEL_DIR = /usr/src/linux-headers-$(KVERSION)/

MODULE_NAME = misc_dev
obj-m := $(MODULE_NAME).o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
	gcc sample.c -o sample
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf sample
