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
#include "usb_device.h"
#include "protocol.h"

#define VFS_IOC_MAGIC	'w'
#define IOC_FINGERPRINT_READY	_IO(VFS_IOC_MAGIC, 0)

/* table of devices that work with this driver */
static struct usb_device_id device_table[] = {
		{ USB_DEVICE(VFS301_VENDOR_ID, VFS301_DEVICE_ID) },
		{} /* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, device_table);

static struct usb_driver vfs_driver;

static ssize_t vfsdev_read(struct file* file, char __user *buffer, size_t count, loff_t *ppos)
{
	struct vfs_device_t *dev;
	int rv = 0;

	DBG("reading...");

	dev = file->private_data;

	mutex_lock(&dev->io_mutex);
	if (*ppos == 0)
		dev->fingerprint_length = 0;
	mutex_unlock(&dev->io_mutex);

	if (wait_event_interruptible(dev->io_block, dev->fingerprint_length > 0))
		return -ERESTART;

	mutex_lock(&dev->io_mutex);

	if (!dev->fingerprint_length)
		goto end;

	if (*ppos >= dev->fingerprint_length)
	{
		dev->fingerprint_length = 0;
		*ppos = 0;
		goto end;
	}

	if (*ppos+count > dev->fingerprint_length)
		count = dev->fingerprint_length - *ppos;

	if (copy_to_user(buffer, &dev->fingerprint_buffer[*ppos], count))
	{
		rv = -EFAULT;
		goto end;
	}
	*ppos += count;
	rv = count;

end:
	mutex_unlock(&dev->io_mutex);

	DBG("read finished");

	return rv;
}

static ssize_t vfsdev_write(struct file* file, const char __user *user_buffer, size_t count, loff_t *ppos)
{
	return 0;
}

static int vfsdev_ioctl(struct inode* inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct vfs_device_t *dev;
	struct usb_interface *interface;
	int subminor;
	int retval;

	DBG("ioctl request %x", cmd);

	subminor = iminor(inode);

	interface = usb_find_interface(&vfs_driver, subminor);
	if(!interface)
	{
		ERR("can't find device for minor %d", subminor);
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface);
	if (!dev)
	{
		retval = -ENODEV;
		goto exit;
	}

	retval = -EIO;
	goto exit;

exit:
	DBG("ioctl finished : %d", retval);
	return retval;
}

static int vfsdev_open(struct inode *inode, struct file *file)
{
	struct vfs_device_t *dev;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	DBG("opening file");

	subminor = iminor(inode);

	interface = usb_find_interface(&vfs_driver, subminor);
	if(!interface)
	{
		ERR("can't find device for minor %d", subminor);
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface);
	if (!dev)
	{
		retval = -ENODEV;
		goto exit;
	}

	/* increment our usage count for the device */
	kref_get(&dev->kref);

	/* lock the device to allow correctly handling errors in resumption */
	mutex_lock(&dev->io_mutex);

	if (dev->opened)
	{
		retval = -EBUSY;
		mutex_unlock(&dev->io_mutex);
		kref_put(&dev->kref, vfsdev_delete);
		goto exit;
	}
	else
		dev->opened = true;


	/* save our object in the file's private structure */
	file->private_data = dev;

	mutex_unlock(&dev->io_mutex);


exit:
	DBG("open finished");
	return retval;
}

static int vfsdev_release(struct inode* inode, struct file *file)
{
	struct vfs_device_t *dev;

	dev = file->private_data;
	if (dev == NULL)
		return -ENODEV;

	mutex_lock(&dev->io_mutex);
	dev->opened = false;
	mutex_unlock(&dev->io_mutex);

	/* decrement the count on our device */
	kref_put(&dev->kref, vfsdev_delete);
	return 0;
}

static int vfsdev_flush(struct file *file, fl_owner_t id)
{
	return 0;
}

static struct file_operations vfsdev_fops = {
	.owner = THIS_MODULE,
	.read = vfsdev_read,
	.write = vfsdev_write,
	.ioctl = vfsdev_ioctl,
	.open = vfsdev_open,
	.release = vfsdev_release,
	.flush = vfsdev_flush,
	.llseek = noop_llseek,
};

struct usb_class_driver vfsdev_class = {
	.name = "vfs301",
	.fops = &vfsdev_fops,
	.minor_base = VFSDEV_MINOR_BASE,
};



static struct usb_driver vfs_driver = {
	.name = "validity_VFS",
	.id_table = device_table,
	.probe = vfsdev_probe,
	.disconnect = vfsdev_disconnect,
	/*.supports_autosuspend = 1,*/
};

static int __init usb_vfs_init(void)
{
	int result;
	/* register this driver with the USB subsystem */
	result = usb_register(&vfs_driver);
	if (result)
		ERR("usb_register failed. Error number %d", result);
	return result;
}

static void __exit usb_vfs_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&vfs_driver);
}

module_init(usb_vfs_init);
module_exit(usb_vfs_exit);

MODULE_LICENSE("GPL");
