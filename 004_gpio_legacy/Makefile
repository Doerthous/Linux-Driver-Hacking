#
# e.g:
# 	device_node {
#		.compatible="dts,gpiol";
#		gpios = <0x0f 0x0c 0x00>;
# 	}
#

obj-m := gpiol.o

ifeq ($(M),)
include $(shell pwd)/../Makefile.platform
endif
