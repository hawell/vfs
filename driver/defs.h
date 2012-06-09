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

#ifndef DEFS_H_
#define DEFS_H_

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/uaccess.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include <linux/usb/ch9.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/wait.h>

#include "log.h"

#define VFS301_VENDOR_ID	0x138a
#define VFS301_DEVICE_ID	0x0005

#define SEND_DATA_ENDPOINT	0x01
#define RECV_CTRL_ENDPOINT	0x81
#define RECV_DATA_ENDPOINT	0x82

#define DEFAULT_TIMEOUT		(300 * HZ / 1000)
#define POLING_TIMEOUT		200

#define VFSDEV_MINOR_BASE	0xab

#define MAX_FINGERPRINT_SIZE	400000
#define SCANLINE_DIFF_TRESHOLD	14

#define VFS_SEGMENT_LENGTH	288

#endif /* DEFS_H_ */
