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

#include "defs.h"
#include "log.h"
#include "protocol.h"

#define REQUEST_TYPE_STANDARD 0x00
#define REQUEST_SET_FEATURE	0x03

#include "protocol_data.inc"

void print_data(unsigned char* data, int len)
{
	int i;

	for (i=0; i<len; i++)
		printk(KERN_DEBUG"%02x", data[i]);
	printk(KERN_DEBUG"\n");
}

int bulkin_ctrl(struct vfs_device_t *dev, int len)
{
	unsigned int pipe = usb_rcvbulkpipe(dev->udev, RECV_CTRL_ENDPOINT);
	int count = len;
	int r;

	r = usb_bulk_msg(dev->udev, pipe, dev->bulkin_ctrl_buffer, count, &count, DEFAULT_TIMEOUT);

	INF("bulk_in_ctrl : pipe=%u, transfer=%d, result=%d\n", pipe, count, r);

	print_data(dev->bulkin_ctrl_buffer, count);

	if (!r)
		return count;

	return r;
}

int bulkin_data(struct vfs_device_t *dev, int len)
{
	unsigned int pipe = usb_rcvbulkpipe(dev->udev, RECV_DATA_ENDPOINT);
	int count = len;
	int r;

	r = usb_bulk_msg(dev->udev, pipe, dev->bulkin_data_buffer, count, &count, DEFAULT_TIMEOUT);

	INF("bulk_in_data : pipe=%u, transfer=%d, result=%d\n", pipe, count, r);

	/*
	print_data(dev->bulkin_ctrl_buffer, count);
	*/
	if (!r)
		return count;

	return r;
}

int bulkout(struct vfs_device_t *dev, unsigned char* data, int len)
{
	unsigned int pipe = usb_sndbulkpipe(dev->udev, SEND_DATA_ENDPOINT);
	int count = len;
	int r,i;

	for (i=0; i<len; i++)
		dev->bulkout_buffer[i] = data[i];

	r = usb_bulk_msg(dev->udev, pipe, dev->bulkout_buffer, count, &count, DEFAULT_TIMEOUT);

	INF("bulk_out : pipe=%u, transfer=%d, result=%d\n", pipe, count, r);

	/*
	print_data(dev->bulkout_buffer, count);
	*/
	if (!r)
		return count;

	return r;
}

int protocol_configure_device(struct vfs_device_t *dev)
{
	unsigned int pipe = usb_sndctrlpipe(dev->udev, 0x00);
	unsigned int pipe2 = usb_rcvctrlpipe(dev->udev, 0x80);
	unsigned char data[20];

	int r;

	r = usb_reset_configuration(dev->udev);
	DBG("reset config : %d\n", r);
	usb_reset_endpoint(dev->udev, dev->bulkin_data_endpointAddr);
	usb_reset_endpoint(dev->udev, dev->bulkin_ctrl_endpointAddr);
	usb_reset_endpoint(dev->udev, dev->bulkout_endpointAddr);

	r = usb_control_msg(dev->udev, pipe2, 0, 0x80, 0, 0, data, 2, 300 * HZ / 1000);
	DBG("ctrl_io : pipe=%u, result=%d\n", pipe2, r);
	print_data(data, 2);

	r = usb_control_msg(dev->udev, pipe, 3, 0, 1, 0, NULL, 0, 300 * HZ / 1000);
	DBG("ctrl_io : pipe=%u, result=%d\n", pipe, r);

	r = usb_control_msg(dev->udev, pipe, 3, 0, 1, 1, NULL, 0, 300 * HZ / 1000);
	DBG("ctrl_io : pipe=%u, result=%d\n", pipe, r);

	r = usb_control_msg(dev->udev, pipe2, 0, 0x80, 0, 0, data, 2, 300 * HZ / 1000);
	DBG("ctrl_io : pipe=%u, result=%d\n", pipe2, r);
	print_data(data, 2);

	r = usb_control_msg(dev->udev, pipe2, 0, 0x82, 0, 0x81, data, 2, 300 * HZ / 1000);
	DBG("ctrl_io : pipe=%u, result=%d\n", pipe2, r);
	print_data(data, 2);

	r = usb_control_msg(dev->udev, pipe2, 0, 0x82, 0, 0x82, data, 2, 300 * HZ / 1000);
	DBG("ctrl_io : pipe=%u, result=%d\n", pipe2, r);
	print_data(data, 2);

	usb_set_device_state(dev->udev, USB_STATE_CONFIGURED);

	return r;
}

int protocol_initialize(struct vfs_device_t *dev)
{
	int r;

	bulkout(dev, init_1, 1);
	bulkin_ctrl(dev, 38);
	bulkout(dev, init_2, 39);
	bulkin_ctrl(dev, 6);
	bulkout(dev, init_2_1, 39);
	bulkin_ctrl(dev, 7);
	bulkout(dev, init_3, 1);
	bulkin_ctrl(dev, 64);
	bulkin_ctrl(dev, 4);
	bulkout(dev, init_4, 2401);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_5, 1);
	bulkin_ctrl(dev, 38);
	bulkout(dev, init_6, 1);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_7, 1905);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_8, 2247);
	bulkin_ctrl(dev, 2);
	bulkin_data(dev, 256);
	bulkin_data(dev, 32);
	bulkout(dev, init_9, 1);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_10, 785);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_11, 1);
	bulkin_ctrl(dev, 38);
	bulkout(dev, init_12, 2737);
	bulkin_ctrl(dev, 2);

	bulkin_data(dev, 11648);
	bulkout(dev, init_13, 2529);
	bulkin_ctrl(dev, 2);
	bulkin_data(dev, 53248);
	bulkout(dev, init_14, 2542);
	bulkin_ctrl(dev, 2);
	bulkin_data(dev, 19968);
	bulkout(dev, init_15, 2768);
	bulkin_ctrl(dev, 2);
	bulkin_data(dev, 5824);
	bulkout(dev, init_16, 2638);
	bulkin_ctrl(dev, 2);
	bulkin_data(dev, 6656);
	bulkout(dev, init_17, 2651);
	bulkin_ctrl(dev, 2);
	bulkin_data(dev, 6656);
	bulkout(dev, init_18, 570);
	bulkin_ctrl(dev, 2);
	bulkin_data(dev, 832);
	bulkout(dev, init_19, 57);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_20, 1);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_21, 1905);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_22, 2587);
	r = bulkin_ctrl(dev, 2);
	bulkin_data(dev, 5760);
	if (r < 0)
	{
		bulkin_ctrl(dev, 2);
	}

	bulkout(dev, init_23, 1);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_4, 2401);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_23, 1);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_24, 3057);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_25, 119);
	bulkin_ctrl(dev, 2);
	bulkout(dev, init_11, 1);
	bulkin_ctrl(dev, 38);
	bulkout(dev, init_26, 2591);
	bulkin_ctrl(dev, 2368);
	bulkin_ctrl(dev, 36);
	bulkin_data(dev, 5760);
	return 0;
}

void protocol_request_fingerprint(struct vfs_device_t *dev)
{
	bulkout(dev, request_1, 2615);
	bulkin_ctrl(dev, 2);
}

int protocol_read_fingerprint(struct vfs_device_t *dev)
{
	int val;

	mutex_lock(&dev->io_mutex);

	dev->fingerprint_length = 0;

	bulkin_data(dev, 64);

	val = bulkin_data(dev, 84032);
	protocol_process_image_data(dev, val);

	if (val < 84032)
		goto end;

	do
	{
		val = bulkin_data(dev, 84096);
		protocol_process_image_data(dev, val);
	}
	while (val == 84096);

end:
	mutex_unlock(&dev->io_mutex);

	bulkout(dev, pol_4, 1);
	val = bulkin_ctrl(dev, 2);
	bulkin_data(dev, 16384);
	if (val < 0)
		bulkin_ctrl(dev, 2);
	bulkout(dev, init_22, 2587);
	val = bulkin_ctrl(dev, 2);
	bulkin_data(dev, 5760);
	if (val < 0)
		bulkin_ctrl(dev, 2);

	return 0;
}

int protocol_pol_device(struct vfs_device_t *dev)
{
	bulkout(dev, pol_1, 1);
	bulkin_ctrl(dev, 7);

	if (memcmp(dev->bulkin_ctrl_buffer, pol_3, 7) == 0)
		return 0;
	else
		return -1;
}

void protocol_process_image_data(struct vfs_device_t *dev, int size)
{
	int i;

	for (i=0; i<size; i++)
	{
		if (dev->fingerprint_length >= MAX_FINGERPRINT_SIZE)
			break;
		dev->fingerprint_buffer[dev->fingerprint_length++] = dev->bulkin_data_buffer[i];
	}
}
