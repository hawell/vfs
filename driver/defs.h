/*
 * defs.h
 *
 *  Created on: Sep 9, 2011
 *      Author: arash
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

#include "log.h"

#define VFS301_VENDOR_ID	0x138a
#define VFS301_DEVICE_ID	0x0005

#define SEND_DATA_ENDPOINT	0x01
#define RECV_CTRL_ENDPOINT	0x81
#define RECV_DATA_ENDPOINT	0x82

#define DEFAULT_TIMEOUT		(300 * HZ / 1000)
#define POLING_TIMEOUT		200

#define VFSDEV_MINOR_BASE	0xab

#define MAX_FINGERPRINT_SIZE	200000

#endif /* DEFS_H_ */
