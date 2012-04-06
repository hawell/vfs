/*
 *  Copyright (c) 2011-2012 Arash Kordi <arash.cordi@gmail.com>
 *
 *  This file is part of vfs.
 *
 *  vfs is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  vfs is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with vfs.  If not, see <http://www.gnu.org/licenses/>.
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
