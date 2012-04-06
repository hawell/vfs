obj-m := vfs301.o
vfs301-objs := validity_vfs.o usb_device.o protocol.o

all: validity_vfs.c defs.h usb_device.c usb_device.h protocol.c protocol.h
	./build.sh

clean:
	rm -f *.o *.ko modules.order Module.symvers *.mod.c