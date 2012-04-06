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
