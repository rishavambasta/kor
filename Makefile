ONFIG_MODULE_SIG=n

obj-m += hello.o

all:	
	make -C /lib/modules/`uname -r`/build M=`pwd` modules

clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
