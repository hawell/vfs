/*
 *  Copyright (c) 2011-2012 Arash Kordi <arash.cordi@gmail.com>
 *
 *  Copyright (c) 2017-2018 Gaurav Mishra <gmishx@gmail.com>
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

#include "usb_device.h"
#include "protocol.h"

extern struct usb_class_driver vfsdev_class;


int poling_thread(void* args)
{
	struct vfs_device_t* dev = (struct vfs_device_t*)args;
	int res;

	while (!atomic_read(&dev->terminate_poling))
	{
		res = protocol_pol_device(dev);
		if (res == 0)
		{
			protocol_read_fingerprint(dev);
			protocol_request_fingerprint(dev);
		}

		msleep(POLING_TIMEOUT);
	}

	complete(&dev->poling_completion);

	return 0;
}

void vfsdev_delete(struct kref *kref)
{
	struct vfs_device_t *dev = to_vfs_dev(kref);

	DBG("delete");

	atomic_inc(&dev->terminate_poling);
	wait_for_completion(&dev->poling_completion);

	usb_put_dev(dev->udev);
	kfree(dev->bulkin_ctrl_buffer);
	kfree(dev->bulkin_data_buffer);
	kfree(dev->bulkout_buffer);
	kfree(dev->fingerprint_buffer);
	kfree(dev);
}

int vfsdev_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct vfs_device_t *dev;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	size_t buffer_size;
	int i;
	int retval = -ENOMEM;

	DBG("probe");

	/* allocate memory for our device state & initialize it */
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
	{
		ERR("Out of memory");
		goto error;
	}
	kref_init(&dev->kref);
	mutex_init(&dev->io_mutex);
	atomic_set(&dev->terminate_poling, 0);
	init_completion(&dev->poling_completion);
	init_waitqueue_head(&dev->io_block);

	dev->udev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;
	dev->opened = false;

	dev->fingerprint_buffer = kmalloc(MAX_FINGERPRINT_SIZE, GFP_KERNEL);
	dev->fingerprint_length = 0;

	/* set up the endpoint information */
	iface_desc = interface->cur_altsetting;
	INF("number of endpoints = %d", iface_desc->desc.bNumEndpoints);
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i)
	{
		endpoint = &iface_desc->endpoint[i].desc;
		INF("endpoint address=%x type=%d dir=%d buffer_size=%d", endpoint->bEndpointAddress, usb_endpoint_type(endpoint), endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK, endpoint->wMaxPacketSize);

		switch (endpoint->bEndpointAddress)
		{
			case SEND_DATA_ENDPOINT:
				/* we found a bulk out end point */
				INF("bulk out found");

				dev->bulkout_endpointAddr = endpoint->bEndpointAddress;
				buffer_size = endpoint->wMaxPacketSize;
				dev->bulkout_buffer = kmalloc(4000, GFP_KERNEL);
			break;

			case RECV_CTRL_ENDPOINT:
				/* we found a bulk in endpoint */
				INF("bulk in found");

				buffer_size = endpoint->wMaxPacketSize;
				dev->bulkin_ctrl_size = buffer_size;
				dev->bulkin_ctrl_endpointAddr = endpoint->bEndpointAddress;
				dev->bulkin_ctrl_buffer = kmalloc(4000, GFP_KERNEL);
				if (!dev->bulkin_ctrl_buffer)
				{
					ERR("could not allocate bulk in buffer");
					goto error;
				}
			break;

			case RECV_DATA_ENDPOINT:
				/* we found a bulk in endpoint */
				INF("bulk in found");

				buffer_size = endpoint->wMaxPacketSize;
				dev->bulkin_data_size = buffer_size;
				dev->bulkin_data_endpointAddr = endpoint->bEndpointAddress;
				dev->bulkin_data_buffer = kmalloc(100000, GFP_KERNEL);
				if (!dev->bulkin_data_buffer)
				{
					ERR("could not allocate bulk in buffer");
					goto error;
				}
			break;

		}
	}

	/* save our data pointer in this interface device */
	usb_set_intfdata(interface, dev);

	/* we can register the device now, as it is ready */
	retval = usb_register_dev(interface, &vfsdev_class);
	if (retval)
	{
		/* something prevented us from registering this driver */
		ERR("Not able to get a minor for this device.");
		usb_set_intfdata(interface, NULL);
		goto error;
	}

	usb_reset_device(dev->udev);

	DBG("configuring device");
	protocol_configure_device(dev);
	DBG("configuring device done");

	DBG("initializing device");
	protocol_initialize(dev);
	DBG("initializing completed");

	DBG("requesting finger print");
	protocol_request_fingerprint(dev);
	DBG("requesting finger print done");

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
	kernel_thread(poling_thread, dev, 0);
#else
	kthread_run(poling_thread, dev, "vfsthread");
#endif


	/* let the user know what node this device is now attached to */
	 INF("USB vfs device now attached to USBSVFS-%d", interface->minor);

	return 0;

error:
	if (dev)
		kref_put(&dev->kref, vfsdev_delete);

	return retval;
}

void vfsdev_disconnect(struct usb_interface *interface)
{
	struct vfs_device_t *dev;
	int minor = interface->minor;

	DBG("disconnect");
	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	/* give back our minor */
	usb_deregister_dev(interface, &vfsdev_class);

	mutex_lock(&dev->io_mutex);
	dev->interface = NULL;
	mutex_unlock(&dev->io_mutex);

	/* decrement our usage count */
	kref_put(&dev->kref, vfsdev_delete);

	INF("USB device #%d now disconnected", minor);
}
