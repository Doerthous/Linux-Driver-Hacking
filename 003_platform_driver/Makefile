#
# add some device node to dts with compatible equal to "dts,test"
# e.g:
# 	pd {
#		.compatible="dts,pd";
# 	}
#

obj-m := pd.o

ifeq ($(M),)
include $(shell pwd)/../Makefile.platform
endif
