#CONFIG_MODULE_SIG=n
# If KERNELRELEASE is undefined, we've been invoked from the 
# kernel build system and can use its language

ifneq ($(KERNELRELEASE),)
	obj-m := scull.o

# Otherwise we were called direclty from the command line
# invike the kernel build system

else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
