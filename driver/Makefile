obj-m := vfs301.o
vfs301-objs := validity_vfs.o usb_device.o protocol.o
KERNEL_PATH := /lib/modules/`uname -r`/build

all: validity_vfs.c defs.h usb_device.c usb_device.h protocol.c protocol.h
	$(MAKE) -C $(KERNEL_PATH) M=`pwd` modules

clean:
	$(MAKE) -C $(KERNEL_PATH) M=`pwd` clean
