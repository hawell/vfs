/*
 * usb_device.h
 *
 *  Created on: Mar 24, 2012
 *      Author: arash
 */

#ifndef USB_DEVICE_H_
#define USB_DEVICE_H_

#include "defs.h"

struct vfs_device_t {
	struct usb_device       *udev;
	struct usb_interface    *interface;

	/* bulk in data endpoint */
	unsigned char*          bulkin_data_buffer;
	size_t                  bulkin_data_size;
	__u8                    bulkin_data_endpointAddr;

	/* bulk in ctrl endpoint */
	unsigned char*          bulkin_ctrl_buffer;
	size_t                  bulkin_ctrl_size;
	__u8                    bulkin_ctrl_endpointAddr;

	/* bulk out endpoint */
	unsigned char*			bulkout_buffer;
	__u8                    bulkout_endpointAddr;

	bool					opened;	/* Exclusive open */
	struct kref             kref; /* module reference */
	struct mutex            io_mutex;

	/* poling thread synchronization */
	atomic_t				terminate_poling;
	struct completion		poling_completion;

	/* fingerprint data */
	unsigned char*			fingerprint_buffer;
	size_t					fingerprint_length;
};

#define to_vfs_dev(d) container_of(d, struct vfs_device_t, kref)

void vfsdev_delete(struct kref *kref);
int vfsdev_probe(struct usb_interface *interface, const struct usb_device_id *id);
void vfsdev_disconnect(struct usb_interface *interface);

#endif /* USB_DEVICE_H_ */
