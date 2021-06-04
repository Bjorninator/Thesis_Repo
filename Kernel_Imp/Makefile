obj-m:=hrtimer.o
PWD:=$(shell pwd)
KERNELPATH:=/lib/modules/5.9.1-rt20
all:
	make -C $(KERNELPATH) M=$(PWD) modules
clean:
	make -C $(KERNELPATH) M=$(PWD) clean