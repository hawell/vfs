/*
 * protocol.h
 *
 *  Created on: Mar 24, 2012
 *      Author: arash
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "defs.h"
#include "usb_device.h"

int protocol_configure_device(struct vfs_device_t *dev);
int protocol_initialize(struct vfs_device_t *dev);
void protocol_request_fingerprint(struct vfs_device_t *dev);
int protocol_pol_device(struct vfs_device_t *dev);
int protocol_read_fingerprint(struct vfs_device_t *dev);
void protocol_process_image_data(struct vfs_device_t *dev, int size);

#endif /* PROTOCOL_H_ */
