/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */





#include <linux/ump-ioctl.h>
#include <linux/ump.h>

#include <asm/uaccess.h>	         /*             */
#include <linux/compat.h>
#include <linux/module.h>            /*                           */
#include <linux/fs.h>                /*                        */
#include <linux/cdev.h>              /*                              */
#include <linux/ioport.h>            /*                    */
#include <linux/device.h>            /*                            */

#include <common/ump_kernel_core.h>

#include "ump_kernel_linux_mem.h"
#include <ump_arch.h>


struct ump_linux_device
{
	struct cdev cdev;
	struct class * ump_class;
};

/*                               */
static char ump_dev_name[] = "ump"; /*                                                              */

/*                                       */
int ump_debug_level = 2;
module_param(ump_debug_level, int, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH); /*           */
MODULE_PARM_DESC(ump_debug_level, "Higher number, more dmesg output");

/*                                                                                                               */
int ump_major = 0;
module_param(ump_major, int, S_IRUGO); /*           */
MODULE_PARM_DESC(ump_major, "Device major number");

#define UMP_REV_STRING "1.0"

char * ump_revision = UMP_REV_STRING;
module_param(ump_revision, charp, S_IRUGO); /*           */
MODULE_PARM_DESC(ump_revision, "Revision info");

static int umpp_linux_open(struct inode *inode, struct file *filp);
static int umpp_linux_release(struct inode *inode, struct file *filp);
#ifdef HAVE_UNLOCKED_IOCTL
static long umpp_linux_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int umpp_linux_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif

/*                                                                         */
static struct file_operations ump_fops =
{
	.owner   = THIS_MODULE,
	.open    = umpp_linux_open,
	.release = umpp_linux_release,
#ifdef HAVE_UNLOCKED_IOCTL
	.unlocked_ioctl   = umpp_linux_ioctl,
#else
	.ioctl   = umpp_linux_ioctl,
#endif
	.compat_ioctl = umpp_linux_ioctl,
	.mmap = umpp_linux_mmap
};

/*                        */
DEFINE_MUTEX(import_list_lock);
struct ump_import_handler *  import_handlers[UMPP_EXTERNAL_MEM_COUNT];

/*                                                       */
static struct ump_linux_device ump_linux_device;

#define DBG_MSG(level, ...) do { \
if ((level) <=  ump_debug_level)\
{\
printk(KERN_DEBUG "UMP<" #level ">:\n" __VA_ARGS__);\
} \
} while (0)

#define MSG_ERR(...) do{ \
printk(KERN_ERR "UMP: ERR: %s\n           %s()%4d\n", __FILE__, __func__  , __LINE__) ; \
printk(KERN_ERR __VA_ARGS__); \
printk(KERN_ERR "\n"); \
} while(0)

#define MSG(...) do{ \
printk(KERN_INFO "UMP: " __VA_ARGS__);\
} while (0)

/*
                                                              
                                                 
 */
static int __init umpp_linux_initialize_module(void)
{
	ump_result err;

	err = umpp_core_constructor();
	if (UMP_OK != err)
	{
		MSG_ERR("UMP device driver init failed\n");
		return -ENOTTY;
	}

	MSG("UMP device driver %s loaded\n", UMP_REV_STRING);
	return 0;
}



/*
                                                                                 
                                                
 */
static void __exit umpp_linux_cleanup_module(void)
{
	DBG_MSG(2, "Unloading UMP device driver\n");
	umpp_core_destructor();
	DBG_MSG(2, "Module unloaded\n");
}



/*
                                    
 */
ump_result umpp_device_initialize(void)
{
	int err;
	dev_t dev = 0;

	if (0 == ump_major)
	{
		/*                     */
		err = alloc_chrdev_region(&dev, 0, 1, ump_dev_name);
		ump_major = MAJOR(dev);
	}
	else
	{
		/*                                    */
		dev = MKDEV(ump_major, 0);
		err = register_chrdev_region(dev, 1, ump_dev_name);
	}

	if (0 == err)
	{
		memset(&ump_linux_device, 0, sizeof(ump_linux_device));

		/*                              */
		cdev_init(&ump_linux_device.cdev, &ump_fops);
		ump_linux_device.cdev.owner = THIS_MODULE;
		ump_linux_device.cdev.ops = &ump_fops;

		/*                                   */
		err = cdev_add(&ump_linux_device.cdev, dev, 1/*     */);
		if (0 == err)
		{

			ump_linux_device.ump_class = class_create(THIS_MODULE, ump_dev_name);
			if (IS_ERR(ump_linux_device.ump_class))
			{
				err = PTR_ERR(ump_linux_device.ump_class);
			}
			else
			{
				struct device * mdev;
				mdev = device_create(ump_linux_device.ump_class, NULL, dev, NULL, ump_dev_name);
				if (!IS_ERR(mdev))
				{
					return UMP_OK;
				}

				err = PTR_ERR(mdev);
				class_destroy(ump_linux_device.ump_class);
			}
			cdev_del(&ump_linux_device.cdev);
		}

		unregister_chrdev_region(dev, 1);
	}

	return UMP_ERROR;
}



/*
                                  
 */
void umpp_device_terminate(void)
{
	dev_t dev = MKDEV(ump_major, 0);

	device_destroy(ump_linux_device.ump_class, dev);
	class_destroy(ump_linux_device.ump_class);

	/*                        */
	cdev_del(&ump_linux_device.cdev);

	/*            */
	unregister_chrdev_region(dev, 1);
}


static int umpp_linux_open(struct inode *inode, struct file *filp)
{
	umpp_session *session;
	
	session = umpp_core_session_start();
	if (NULL == session)
	{
		return -EFAULT;
	}
	
	filp->private_data = session;

	return 0;
}

static int umpp_linux_release(struct inode *inode, struct file *filp)
{
	umpp_session *session;
	
	session = filp->private_data;

	umpp_core_session_end(session);

	filp->private_data = NULL;

	return 0;
}

/*                        */
/*                        */
/*                        */
static int do_ump_dd_allocate(umpp_session * session, ump_k_allocate * params)
{
	ump_dd_handle new_allocation;
	new_allocation = ump_dd_allocate_64(params->size, params->alloc_flags, NULL, NULL, NULL);

	if (UMP_DD_INVALID_MEMORY_HANDLE != new_allocation)
	{
		umpp_session_memory_usage * tracker;

		tracker = kmalloc(sizeof(*tracker), GFP_KERNEL | __GFP_HARDWALL);
		if (NULL != tracker)
		{
			/*                                          */
			params->secure_id = ump_dd_secure_id_get(new_allocation);

			tracker->mem = new_allocation;
			tracker->id = params->secure_id;
			atomic_set(&tracker->process_usage_count, 1);

			/*                                      */
			mutex_lock(&session->session_lock);
			list_add(&tracker->link, &session->memory_usage);
			mutex_unlock(&session->session_lock);

			return 0;
		}
		ump_dd_release(new_allocation);
	}

	printk(KERN_WARNING "UMP: Allocation FAILED\n");
	return -ENOMEM;
}

static int do_ump_dd_retain(umpp_session * session, ump_k_retain * params)
{
	umpp_session_memory_usage * it;

	mutex_lock(&session->session_lock);

	/*                                          */
	list_for_each_entry(it, &session->memory_usage, link)
	{
		if (it->id == params->secure_id)
		{
			/*                            */
			/*                    */
			while(1)
			{
				int refcnt = atomic_read(&it->process_usage_count);
				if (refcnt + 1 > 0)
				{
					/*                         */
					if(atomic_cmpxchg(&it->process_usage_count, refcnt, refcnt + 1) == refcnt)
					{
						mutex_unlock(&session->session_lock);
						return 0;
					}
				}
				else
				{
					/*                           */
					mutex_unlock(&session->session_lock);
					return -EBUSY;
				}
			}
		}
	}
	/*                            */

	it = kmalloc(sizeof(*it), GFP_KERNEL);

	if (NULL != it)
	{
		it->mem = ump_dd_from_secure_id(params->secure_id);
		if (UMP_DD_INVALID_MEMORY_HANDLE != it->mem)
		{
			/*                                         */
			it->id = params->secure_id;
			atomic_set(&it->process_usage_count, 1);
			list_add(&it->link, &session->memory_usage);
		}
		else
		{
			/*           */
			kfree(it);
			it = NULL;
		}
	}

	mutex_unlock(&session->session_lock);

	return (NULL != it) ? 0 : -ENODEV;
}


static int do_ump_dd_release(umpp_session * session, ump_k_release * params)
{
	umpp_session_memory_usage * it;
	int result = -ENODEV;

	mutex_lock(&session->session_lock);

	/*                                                */
	list_for_each_entry(it, &session->memory_usage, link)
	{
		if (it->id == params->secure_id)
		{
			/*                     */
			result = 0;

			if (0 == atomic_sub_return(1, &it->process_usage_count))
			{
				/*                                                                                   */
				list_del(&it->link);
				ump_dd_release(it->mem);
				kfree(it);
			}

			break;
		}
	}
	mutex_unlock(&session->session_lock);

	return result;
}

static int do_ump_dd_sizequery(umpp_session * session, ump_k_sizequery * params)
{
	umpp_session_memory_usage * it;
	int result = -ENODEV;

	mutex_lock(&session->session_lock);

	/*                                         */
	list_for_each_entry(it, &session->memory_usage, link)
	{
		if (it->id == params->secure_id)
		{
			/*                     */
			params->size = ump_dd_size_get_64(it->mem);
			result = 0;
			break;
		}

	}
	mutex_unlock(&session->session_lock);

	return result;
}

static int do_ump_dd_allocation_flags_get(umpp_session * session, ump_k_allocation_flags * params)
{
	umpp_session_memory_usage * it;
	int result = -ENODEV;

	mutex_lock(&session->session_lock);

	/*                                         */
	list_for_each_entry(it, &session->memory_usage, link)
	{
		if (it->id == params->secure_id)
		{
			/*                     */
			params->alloc_flags = ump_dd_allocation_flags_get(it->mem);
			result = 0;
			break;
		}

	}
	mutex_unlock(&session->session_lock);

	return result;
}

static int do_ump_dd_msync_now(umpp_session * session, ump_k_msync * params)
{
	umpp_session_memory_usage * it;
	int result = -ENODEV;

	mutex_lock(&session->session_lock);

	/*                                         */
	list_for_each_entry(it, &session->memory_usage, link)
	{
		if (it->id == params->secure_id)
		{
			/*                        */
#ifdef CONFIG_COMPAT
			if (is_compat_task())
			{
				umpp_dd_cpu_msync_now(it->mem, params->cache_operation, compat_ptr(params->mapped_ptr.compat_value), params->size);
				result = 0;
			}
			else
			{
#endif
				umpp_dd_cpu_msync_now(it->mem, params->cache_operation, params->mapped_ptr.value, params->size);
				result = 0;
#ifdef CONFIG_COMPAT
			}
#endif
			break;
		}
	}
	mutex_unlock(&session->session_lock);

	return result;
}


void umpp_import_handlers_init(umpp_session * session)
{
	int i;
	mutex_lock(&import_list_lock);
	for ( i = 1; i < UMPP_EXTERNAL_MEM_COUNT; i++ )
	{
		if (import_handlers[i])
		{
			import_handlers[i]->session_begin(&session->import_handler_data[i]);
			/*                                             
                                         */
		}
	}
	mutex_unlock(&import_list_lock);
}

void umpp_import_handlers_term(umpp_session * session)
{
	int i;
	mutex_lock(&import_list_lock);
	for ( i = 1; i < UMPP_EXTERNAL_MEM_COUNT; i++ )
	{
		/*                                      */
		if (session->import_handler_data[i] != NULL)
		{
			/*                                        
                                           */
			BUG_ON(!import_handlers[i]);
			import_handlers[i]->session_end(session->import_handler_data[i]);
			session->import_handler_data[i] = NULL;
		}
	}
	mutex_unlock(&import_list_lock);
}

int ump_import_module_register(enum ump_external_memory_type type, struct ump_import_handler * handler)
{
	int res = -EEXIST;

	/*                */
	BUG_ON(type == 0 || type >= UMPP_EXTERNAL_MEM_COUNT);
	BUG_ON(!handler);
	BUG_ON(!handler->linux_module);
	BUG_ON(!handler->session_begin);
	BUG_ON(!handler->session_end);
	BUG_ON(!handler->import);

	mutex_lock(&import_list_lock);

	if (!import_handlers[type])
	{
		import_handlers[type] = handler;
		res = 0;
	}

	mutex_unlock(&import_list_lock);

	return res;
}

void ump_import_module_unregister(enum ump_external_memory_type type)
{
	BUG_ON(type == 0 || type >= UMPP_EXTERNAL_MEM_COUNT);

	mutex_lock(&import_list_lock);
	/*                                                                    */
	BUG_ON(!import_handlers[type]);
	import_handlers[type] = NULL;
	mutex_unlock(&import_list_lock);
}

static struct ump_import_handler * import_handler_get(unsigned int type_id)
{
	enum ump_external_memory_type type;
	struct ump_import_handler * handler;

	/*                            */
	/*                                       */
	if (type_id == 0 || type_id >= UMPP_EXTERNAL_MEM_COUNT)
		return NULL;

	type = (enum ump_external_memory_type)type_id;

	/*                  */
	mutex_lock(&import_list_lock);

	handler = import_handlers[type];

	if (handler)
	{
		if (!try_module_get(handler->linux_module))
		{
			handler = NULL;
		}
	}

	mutex_unlock(&import_list_lock);

	return handler;
}

static void import_handler_put(struct ump_import_handler * handler)
{
	module_put(handler->linux_module);
}

static int do_ump_dd_import(umpp_session * session, ump_k_import * params)
{
	ump_dd_handle new_allocation = UMP_DD_INVALID_MEMORY_HANDLE;
	struct ump_import_handler * handler;

	handler = import_handler_get(params->type);

	if (handler)
	{
		/*                                       */
		if (!session->import_handler_data[params->type])
		{
			handler->session_begin(&session->import_handler_data[params->type]);
		}

		/*                             */
		if (session->import_handler_data[params->type])
		{
			new_allocation = handler->import( session->import_handler_data[params->type],
		                                      params->phandle.value,
		                                      params->alloc_flags);
		}

		/*                       */
		import_handler_put(handler);
	}

	/*                         */
	if (UMP_DD_INVALID_MEMORY_HANDLE != new_allocation)
	{
		umpp_session_memory_usage * tracker;

		tracker = kmalloc(sizeof(*tracker), GFP_KERNEL | __GFP_HARDWALL);
		if (NULL != tracker)
		{
			/*                                          */
			params->secure_id = ump_dd_secure_id_get(new_allocation);

			tracker->mem = new_allocation;
			tracker->id = params->secure_id;
			atomic_set(&tracker->process_usage_count, 1);

			/*                                      */
			mutex_lock(&session->session_lock);
			list_add(&tracker->link, &session->memory_usage);
			mutex_unlock(&session->session_lock);

			return 0;
		}
		ump_dd_release(new_allocation);
	}

	return -ENOMEM;

}

#ifdef HAVE_UNLOCKED_IOCTL
static long umpp_linux_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int umpp_linux_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
	int ret;
	uint64_t msg[(UMP_CALL_MAX_SIZE+7)>>3]; /*                 */
	uint32_t size = _IOC_SIZE(cmd);
	struct umpp_session *session = filp->private_data;

#ifndef HAVE_UNLOCKED_IOCTL
	(void)inode; /*            */
#endif

	/*
                                                           
                                                                      
  */
	if (_IOC_TYPE(cmd) != UMP_IOC_MAGIC)
	{
		return -ENOTTY;

	}
	if (_IOC_NR(cmd) > UMP_IOC_MAXNR)
	{
		return -ENOTTY;
	}

	switch(cmd)
	{
		case UMP_FUNC_ALLOCATE:
			if (size != sizeof(ump_k_allocate))
			{
				return -ENOTTY;
			}
			if (copy_from_user(&msg, (void __user *)arg, size))
			{
				return -EFAULT;
			}
			ret = do_ump_dd_allocate(session, (ump_k_allocate *)&msg);
			if (ret)
			{
				return ret;
			}
			if (copy_to_user((void *)arg, &msg, size))
			{
				return -EFAULT;
			}
			return 0;
		case UMP_FUNC_SIZEQUERY:
			if (size != sizeof(ump_k_sizequery))
			{
				return -ENOTTY;
			}
			if (copy_from_user(&msg, (void __user *)arg, size))
			{
				return -EFAULT;
			}
			ret = do_ump_dd_sizequery(session,(ump_k_sizequery*) &msg);
			if (ret)
			{
				return ret;
			}
			if (copy_to_user((void *)arg, &msg, size))
			{
				return -EFAULT;
			}
			return 0;
		case UMP_FUNC_MSYNC:
			if (size != sizeof(ump_k_msync))
			{
				return -ENOTTY;
			}
			if (copy_from_user(&msg, (void __user *)arg, size))
			{
				return -EFAULT;
			}
			ret = do_ump_dd_msync_now(session,(ump_k_msync*) &msg);
			if (ret)
			{
				return ret;
			}
			if (copy_to_user((void *)arg, &msg, size))
			{
				return -EFAULT;
			}
			return 0;
		case UMP_FUNC_IMPORT:
			if (size != sizeof(ump_k_import))
			{
				return -ENOTTY;
			}
			if (copy_from_user(&msg, (void __user*)arg, size))
			{
				return -EFAULT;
			}
			ret = do_ump_dd_import(session, (ump_k_import*) &msg);
			if (ret)
			{
				return ret;
			}
			if (copy_to_user((void *)arg, &msg, size))
			{
				return -EFAULT;
			}
			return 0;
		/*                     */
		case UMP_FUNC_ALLOCATION_FLAGS_GET:
			if (size != sizeof(ump_k_allocation_flags))
			{
				return -ENOTTY;
			}
			if (copy_from_user(&msg, (void __user *)arg, size))
			{
				return -EFAULT;
			}
			ret = do_ump_dd_allocation_flags_get(session,(ump_k_allocation_flags*) &msg);
			if (ret)
			{
				return ret;
			}
			if (copy_to_user((void *)arg, &msg, size))
			{
				return -EFAULT;
			}
			return 0;
		case UMP_FUNC_RETAIN:
			if (size != sizeof(ump_k_retain))
			{
				return -ENOTTY;
			}
			if (copy_from_user(&msg, (void __user *)arg, size))
			{
				return -EFAULT;
			}
			ret = do_ump_dd_retain(session,(ump_k_retain*) &msg);
			if (ret)
			{
				return ret;
			}
			return 0;
		case UMP_FUNC_RELEASE:
			if (size != sizeof(ump_k_release))
			{
				return -ENOTTY;
			}
			if (copy_from_user(&msg, (void __user *)arg, size))
			{
				return -EFAULT;
			}
			ret = do_ump_dd_release(session,(ump_k_release*) &msg);
			if (ret)
			{
				return ret;
			}
			return 0;
		default:
			/*          */
			return -ENOTTY;
	}
	/*               */
	return -ENOTTY;
}


/*                                       */
EXPORT_SYMBOL(ump_dd_allocate_64);
EXPORT_SYMBOL(ump_dd_allocation_flags_get);
EXPORT_SYMBOL(ump_dd_secure_id_get);
EXPORT_SYMBOL(ump_dd_from_secure_id);
EXPORT_SYMBOL(ump_dd_phys_blocks_get_64);
EXPORT_SYMBOL(ump_dd_size_get_64);
EXPORT_SYMBOL(ump_dd_retain);
EXPORT_SYMBOL(ump_dd_release);
EXPORT_SYMBOL(ump_dd_create_from_phys_blocks_64);
#ifdef CONFIG_KDS
EXPORT_SYMBOL(ump_dd_kds_resource_get);
#endif

/*            */
EXPORT_SYMBOL(ump_import_module_register);
EXPORT_SYMBOL(ump_import_module_unregister);



/*        */
EXPORT_SYMBOL(ump_dd_handle_create_from_secure_id);
EXPORT_SYMBOL(ump_dd_phys_block_count_get);
EXPORT_SYMBOL(ump_dd_phys_block_get);
EXPORT_SYMBOL(ump_dd_phys_blocks_get);
EXPORT_SYMBOL(ump_dd_size_get);
EXPORT_SYMBOL(ump_dd_reference_add);
EXPORT_SYMBOL(ump_dd_reference_release);
EXPORT_SYMBOL(ump_dd_handle_create_from_phys_blocks);


/*                                               */
module_init(umpp_linux_initialize_module);
module_exit(umpp_linux_cleanup_module);

/*                            */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ARM Ltd.");
MODULE_VERSION(UMP_REV_STRING);
