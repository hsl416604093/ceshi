	export ARCH=arm
	export CROSS_COMPILE=arm-none-linux-gnueabi-
	obj-m:=gpios_rev1.o
	KERNEL_DIR ?= /home/imx6/my-imx6/02_source/imx_3.0.35_4.1.0/linux-2.6-imx-imx_3.0.35_4.1.0/
	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build 
        PWD:=$(shell pwd)
default:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	rm -rf *.o *.ko *.mod* *.order *.symvers
