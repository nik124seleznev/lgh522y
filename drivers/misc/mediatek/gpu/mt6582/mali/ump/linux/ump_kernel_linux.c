/*
 * Copyright (C) 2010-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/module.h>            /*                           */
#include <linux/fs.h>                /*                        */
#include <linux/cdev.h>              /*                              */
#include <linux/ioport.h>            /*                    */
#include <linux/mm.h>                /*                                       */
#include <asm/uaccess.h>             /*                   */
#include <asm/atomic.h>
#include <linux/device.h>
#include <linux/debugfs.h>

#include "arch/config.h"             /*                                                                             */
#include "ump_ioctl.h"
#include "ump_kernel_common.h"
#include "ump_kernel_interface.h"
#include "ump_kernel_interface_ref_drv.h"
#include "ump_kernel_descriptor_mapping.h"
#include "ump_kernel_memory_backend.h"
#include "ump_kernel_memory_backend_os.h"
#include "ump_kernel_memory_backend_dedicated.h"
#include "ump_kernel_license.h"

#include "ump_osk.h"
#include "ump_ukk.h"
#include "ump_uk_types.h"
#include "ump_ukk_wrappers.h"
#include "ump_ukk_ref_wrappers.h"


/*                                       */
int ump_debug_level = 2;
module_param(ump_debug_level, int, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH); /*           */
MODULE_PARM_DESC(ump_debug_level, "Higher number, more dmesg output");

/*                                                                                                               */
int ump_major = 0;
module_param(ump_major, int, S_IRUGO); /*           */
MODULE_PARM_DESC(ump_major, "Device major number");

/*                               */
static char ump_dev_name[] = "ump"; /*                                                              */


#if UMP_LICENSE_IS_GPL
static struct dentry *ump_debugfs_dir = NULL;
#endif

/*
                                                                            
                                                                 
                                                                      
 */
typedef struct ump_vma_usage_tracker {
	int references;
	ump_dd_handle handle;
} ump_vma_usage_tracker;

struct ump_device {
	struct cdev cdev;
#if UMP_LICENSE_IS_GPL
	struct class * ump_class;
#endif
};

/*                                                       */
static struct ump_device ump_device;


/*                                  */
static int ump_file_open(struct inode *inode, struct file *filp);
static int ump_file_release(struct inode *inode, struct file *filp);
#ifdef HAVE_UNLOCKED_IOCTL
static long ump_file_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int ump_file_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif
static int ump_file_mmap(struct file * filp, struct vm_area_struct * vma);


/*                                                                        */
static struct file_operations ump_fops = {
	.owner   = THIS_MODULE,
	.open    = ump_file_open,
	.release = ump_file_release,
#ifdef HAVE_UNLOCKED_IOCTL
	.unlocked_ioctl   = ump_file_ioctl,
#else
	.ioctl   = ump_file_ioctl,
#endif
	.mmap    = ump_file_mmap
};


/*                                                            
                                                 
 */
static int ump_initialize_module(void)
{
	_mali_osk_errcode_t err;

	DBG_MSG(2, ("Inserting UMP device driver. Compiled: %s, time: %s\n", __DATE__, __TIME__));

	err = ump_kernel_constructor();
	if (_MALI_OSK_ERR_OK != err) {
		MSG_ERR(("UMP device driver init failed\n"));
		return map_errcode(err);
	}

	MSG(("UMP device driver %s loaded\n", SVN_REV_STRING));
	return 0;
}



/*
                                                                                 
                                                
 */
static void ump_cleanup_module(void)
{
	DBG_MSG(2, ("Unloading UMP device driver\n"));
	ump_kernel_destructor();
	DBG_MSG(2, ("Module unloaded\n"));
}



static ssize_t ump_memory_used_read(struct file *filp, char __user *ubuf, size_t cnt, loff_t *ppos)
{
	char buf[64];
	size_t r;
	u32 mem = _ump_ukk_report_memory_usage();

	r = snprintf(buf, 64, "%u\n", mem);
	return simple_read_from_buffer(ubuf, cnt, ppos, buf, r);
}

static const struct file_operations ump_memory_usage_fops = {
	.owner = THIS_MODULE,
	.read = ump_memory_used_read,
};

/*
                                    
 */
int ump_kernel_device_initialize(void)
{
	int err;
	dev_t dev = 0;
#if UMP_LICENSE_IS_GPL
	ump_debugfs_dir = debugfs_create_dir(ump_dev_name, NULL);
	if (ERR_PTR(-ENODEV) == ump_debugfs_dir) {
		ump_debugfs_dir = NULL;
	} else {
		debugfs_create_file("memory_usage", 0400, ump_debugfs_dir, NULL, &ump_memory_usage_fops);
	}
#endif

	if (0 == ump_major) {
		/*                     */
		err = alloc_chrdev_region(&dev, 0, 1, ump_dev_name);
		ump_major = MAJOR(dev);
	} else {
		/*                                    */
		dev = MKDEV(ump_major, 0);
		err = register_chrdev_region(dev, 1, ump_dev_name);
	}

	if (0 == err) {
		memset(&ump_device, 0, sizeof(ump_device));

		/*                              */
		cdev_init(&ump_device.cdev, &ump_fops);
		ump_device.cdev.owner = THIS_MODULE;
		ump_device.cdev.ops = &ump_fops;

		/*                                   */
		err = cdev_add(&ump_device.cdev, dev, 1/*     */);
		if (0 == err) {

#if UMP_LICENSE_IS_GPL
			ump_device.ump_class = class_create(THIS_MODULE, ump_dev_name);
			if (IS_ERR(ump_device.ump_class)) {
				err = PTR_ERR(ump_device.ump_class);
			} else {
				struct device * mdev;
				mdev = device_create(ump_device.ump_class, NULL, dev, NULL, ump_dev_name);
				if (!IS_ERR(mdev)) {
					return 0;
				}

				err = PTR_ERR(mdev);
			}
			cdev_del(&ump_device.cdev);
#else
			return 0;
#endif
		}

		unregister_chrdev_region(dev, 1);
	}

	return err;
}



/*
                                  
 */
void ump_kernel_device_terminate(void)
{
	dev_t dev = MKDEV(ump_major, 0);

#if UMP_LICENSE_IS_GPL
	device_destroy(ump_device.ump_class, dev);
	class_destroy(ump_device.ump_class);
#endif

	/*                        */
	cdev_del(&ump_device.cdev);

	/*            */
	unregister_chrdev_region(dev, 1);

#if UMP_LICENSE_IS_GPL
	if(ump_debugfs_dir)
		debugfs_remove_recursive(ump_debugfs_dir);
#endif
}

/*
                                                          
 */
static int ump_file_open(struct inode *inode, struct file *filp)
{
	struct ump_session_data * session_data;
	_mali_osk_errcode_t err;

	/*                  */
	if (0 != MINOR(inode->i_rdev)) {
		MSG_ERR(("Minor not zero in ump_file_open()\n"));
		return -ENODEV;
	}

	/*                                           */
	err = _ump_ukk_open((void**) &session_data );
	if( _MALI_OSK_ERR_OK != err ) {
		MSG_ERR(("Ump failed to open a new session\n"));
		return map_errcode( err );
	}

	filp->private_data = (void*)session_data;
	filp->f_pos = 0;

	return 0; /*         */
}



/*
                                                                        
 */
static int ump_file_release(struct inode *inode, struct file *filp)
{
	_mali_osk_errcode_t err;

	err = _ump_ukk_close((void**) &filp->private_data );
	if( _MALI_OSK_ERR_OK != err ) {
		return map_errcode( err );
	}

	return 0;  /*         */
}



/*
                         
 */
#ifdef HAVE_UNLOCKED_IOCTL
static long ump_file_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int ump_file_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
	int err = -ENOTTY;
	void __user * argument;
	struct ump_session_data * session_data;

#ifndef HAVE_UNLOCKED_IOCTL
	(void)inode; /*                */
#endif

	session_data = (struct ump_session_data *)filp->private_data;
	if (NULL == session_data) {
		MSG_ERR(("No session data attached to file object\n"));
		return -ENOTTY;
	}

	/*                                                       */
	argument = (void __user *)arg;

	switch (cmd) {
	case UMP_IOC_QUERY_API_VERSION:
		err = ump_get_api_version_wrapper((u32 __user *)argument, session_data);
		break;

	case UMP_IOC_ALLOCATE :
		err = ump_allocate_wrapper((u32 __user *)argument, session_data);
		break;

	case UMP_IOC_RELEASE:
		err = ump_release_wrapper((u32 __user *)argument, session_data);
		break;

	case UMP_IOC_SIZE_GET:
		err = ump_size_get_wrapper((u32 __user *)argument, session_data);
		break;

	case UMP_IOC_MSYNC:
		err = ump_msync_wrapper((u32 __user *)argument, session_data);
		break;

	case UMP_IOC_CACHE_OPERATIONS_CONTROL:
		err = ump_cache_operations_control_wrapper((u32 __user *)argument, session_data);
		break;

	case UMP_IOC_SWITCH_HW_USAGE:
		err = ump_switch_hw_usage_wrapper((u32 __user *)argument, session_data);
		break;

	case UMP_IOC_LOCK:
		err = ump_lock_wrapper((u32 __user *)argument, session_data);
		break;

	case UMP_IOC_UNLOCK:
		err = ump_unlock_wrapper((u32 __user *)argument, session_data);
		break;

	default:
		DBG_MSG(1, ("No handler for IOCTL. cmd: 0x%08x, arg: 0x%08lx\n", cmd, arg));
		err = -EFAULT;
		break;
	}

	return err;
}

int map_errcode( _mali_osk_errcode_t err )
{
	switch(err) {
	case _MALI_OSK_ERR_OK :
		return 0;
	case _MALI_OSK_ERR_FAULT:
		return -EFAULT;
	case _MALI_OSK_ERR_INVALID_FUNC:
		return -ENOTTY;
	case _MALI_OSK_ERR_INVALID_ARGS:
		return -EINVAL;
	case _MALI_OSK_ERR_NOMEM:
		return -ENOMEM;
	case _MALI_OSK_ERR_TIMEOUT:
		return -ETIMEDOUT;
	case _MALI_OSK_ERR_RESTARTSYSCALL:
		return -ERESTARTSYS;
	case _MALI_OSK_ERR_ITEM_NOT_FOUND:
		return -ENOENT;
	default:
		return -EFAULT;
	}
}

/*
                                                                          
 */
static int ump_file_mmap(struct file * filp, struct vm_area_struct * vma)
{
	_ump_uk_map_mem_s args;
	_mali_osk_errcode_t err;
	struct ump_session_data * session_data;

	/*                           */
	session_data = (struct ump_session_data *)filp->private_data;
	if (NULL == session_data) {
		MSG_ERR(("mmap() called without any session data available\n"));
		return -EFAULT;
	}

	/*                                                 */
	args.ctx = session_data;
	args.phys_addr = 0;
	args.size = vma->vm_end - vma->vm_start;
	args._ukk_private = vma;
	args.secure_id = vma->vm_pgoff;
	args.is_cached = 0;

	if (!(vma->vm_flags & VM_SHARED)) {
		args.is_cached = 1;
		vma->vm_flags = vma->vm_flags | VM_SHARED | VM_MAYSHARE  ;
		DBG_MSG(3, ("UMP Map function: Forcing the CPU to use cache\n"));
	}
	/*                                                                                                      */
	vma->vm_flags |= VM_DONTCOPY;

	DBG_MSG(4, ("UMP vma->flags: %x\n", vma->vm_flags ));

	/*                              */
	err = _ump_ukk_map_mem( &args );
	if ( _MALI_OSK_ERR_OK != err) {
		MSG_ERR(("_ump_ukk_map_mem() failed in function ump_file_mmap()"));
		return map_errcode( err );
	}

	return 0; /*         */
}

/*                                       */
EXPORT_SYMBOL(ump_dd_secure_id_get);
EXPORT_SYMBOL(ump_dd_handle_create_from_secure_id);
EXPORT_SYMBOL(ump_dd_phys_block_count_get);
EXPORT_SYMBOL(ump_dd_phys_block_get);
EXPORT_SYMBOL(ump_dd_phys_blocks_get);
EXPORT_SYMBOL(ump_dd_size_get);
EXPORT_SYMBOL(ump_dd_reference_add);
EXPORT_SYMBOL(ump_dd_reference_release);

/*                                                */
EXPORT_SYMBOL(ump_dd_handle_create_from_phys_blocks);

/*                                               */
module_init(ump_initialize_module);
module_exit(ump_cleanup_module);

/*                            */
MODULE_LICENSE(UMP_KERNEL_LINUX_LICENSE);
MODULE_AUTHOR("ARM Ltd.");
MODULE_VERSION(SVN_REV_STRING);
