#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#include <linux/xlog.h>
#include <linux/device.h>
#ifdef CONFIG_OF
#include <linux/of_fdt.h>
#endif
#include <asm/setup.h>
#include "devinfo.h"

u32 g_devinfo_data[DEVINFO_MAX_SIZE];
static struct cdev devinfo_cdev;
static struct class *devinfo_class;
static dev_t devinfo_dev;
/*                                                                            
                    
                                                                            */
static int devinfo_open(struct inode *inode, struct file *filp);
static int devinfo_release(struct inode *inode, struct file *filp);
static long devinfo_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/*                                                                         
                
                                                                         */
u32 devinfo_get_size(void)
{
    return (sizeof(g_devinfo_data)/sizeof(g_devinfo_data[0]));
}

u32 get_devinfo_with_index(u32 index)
{
    int size = devinfo_get_size();
    if ((index >= 0) && (index < size)) {
        return g_devinfo_data[index];
    } else {
        pr_warn("devinfo data index out of range:%d\n", index);
        pr_warn("devinfo data size:%d\n", size);
        return 0xFFFFFFFF;
    }
}
/*                                                                         
                
                                                                         */

static struct file_operations devinfo_fops = {
    .open = devinfo_open,
    .release = devinfo_release,
    .unlocked_ioctl   = devinfo_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = devinfo_ioctl,
#endif
    .owner = THIS_MODULE,
};


static int devinfo_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int devinfo_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/*                                                                         
                   
                                                                         */
static long devinfo_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    u32 index = 0;
    int err   = 0;
    int ret   = 0;
    u32 data_size = sizeof(g_devinfo_data)/sizeof(g_devinfo_data[0]);
    u32 data_read = 0;

    /*                                    */
    /*                                    */
    /*                                    */
    if (_IOC_TYPE(cmd) != DEV_IOC_MAGIC)
        return -ENOTTY;
    if (_IOC_NR(cmd) > DEV_IOC_MAXNR)
        return -ENOTTY;
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    if (_IOC_DIR(cmd) & _IOC_WRITE)
        err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    if (err) return -EFAULT;

    switch (cmd) {
    /*                                    */
    /*                                    */
    /*                                    */
    case READ_DEV_DATA:
        if (copy_from_user((void *)&index, (void __user *)arg, sizeof(u32))) {
            return -1;
        }
        if (index < data_size) {
            data_read = get_devinfo_with_index(index);
            ret = copy_to_user((void __user *)arg, (void *)&(data_read), sizeof(u32));
        } else {
            pr_warn("%s Error! Data index larger than data size. index:%d, size:%d\n",MODULE_NAME,
                index, data_size);
            return -2;
        }
    break;
    }
    return 0;
}

/*                                                                             
               
  
               
                             
  
              
         
  
           
                  
  
         
         
  
                                                                              */
static int __init devinfo_init(void)
{
    int ret = 0;
    struct device *device;

    devinfo_dev = MKDEV(MAJOR_DEV_NUM, 0);
    pr_info("[%s]init\n",MODULE_NAME);
    ret = register_chrdev_region(devinfo_dev, 1, DEV_NAME );
    if (ret) {
        pr_info("[%s] register device failed, ret:%d\n", MODULE_NAME, ret);
        return ret;
    }
    /*            */
    devinfo_class = class_create(THIS_MODULE, DEV_NAME);
    if (IS_ERR(devinfo_class)) {
        ret = PTR_ERR(devinfo_class);
        pr_warn("[%s] register class failed, ret:%d\n", MODULE_NAME, ret);
        unregister_chrdev_region(devinfo_dev, 1);
        return ret;
    }
    /*                                                          */
    cdev_init(&devinfo_cdev, &devinfo_fops);
    devinfo_cdev.owner = THIS_MODULE;
    if ( 0 > (ret = cdev_add(&devinfo_cdev,devinfo_dev,1))) {
        pr_warn("[%s] could not allocate chrdev for the device, ret:%d\n", MODULE_NAME, ret);
        class_destroy(devinfo_class);
        unregister_chrdev_region(devinfo_dev, 1);
        return ret;
    }
    /*             */
    device = device_create(devinfo_class,NULL,devinfo_dev,NULL,"devmap");
    if (IS_ERR(device)) {
        ret = PTR_ERR(device);
        pr_warn("[%s]device create fail\n", MODULE_NAME);
        cdev_del(&devinfo_cdev);
        class_destroy(devinfo_class);
        unregister_chrdev_region(devinfo_dev, 1);
        return ret;
    }
    return 0;
}

#ifdef CONFIG_OF
static int __init devinfo_parse_dt(unsigned long node, const char *uname, int depth, void *data)
{
    struct devinfo_tag *tags;
    int i;
    u32 size = 0;

    if (depth != 1 || (strcmp(uname, "chosen") != 0 && strcmp(uname, "chosen@0") != 0))
        return 0;

    tags = (struct devinfo_tag *) of_get_flat_dt_prop(node, "atag,devinfo", NULL);
    if (tags) {
        size = tags->data_size;
        for (i = 0; i < size; i++) {
            g_devinfo_data[i] = tags->data[i];
        }
        /*                                     */
        pr_info("tag_devinfo_data size:%d\n", size);
    }
    return 1;
}

static int __init devinfo_of_init(void)
{
    of_scan_flat_dt(devinfo_parse_dt, NULL);
    return 0;
}
#endif
/*                                                                             
               
  
               
                             
  
              
         
  
           
         
  
         
         
  
                                                                              */
static void __exit devinfo_exit(void)
{
    cdev_del(&devinfo_cdev);
    class_destroy(devinfo_class);
    unregister_chrdev_region(devinfo_dev, 1);
}
#ifdef CONFIG_OF
early_initcall(devinfo_of_init);
#endif
module_init(devinfo_init);
module_exit(devinfo_exit);
MODULE_LICENSE("GPL");


