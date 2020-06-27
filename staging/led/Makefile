obj-m := led.o

export CC=/home/chenmz/ttgj/rv1108_linux_sdk_v2.3.0_200508/buildroot/output/rockchip_rv1108/host/bin/arm-buildroot-linux-gnueabihf-gcc
export CCFLAG=--sysroot=/home/chenmz/ttgj/sysroot

LINUX_PATH:=/home/chenmz/ttgj/rv1108_linux_sdk_v2.3.0_200508/kernel
CURRENT_PATH:=$(shell pwd)

all:
	make -C $(LINUX_PATH) M=$(CURRENT_PATH) modules
clean:
	make -C $(LINUX_PATH) M=$(CURRENT_PATH) clean
