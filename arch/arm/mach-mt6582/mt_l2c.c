#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/cpu.h>
#include <linux/smp.h>
#include "mach/mt_reg_base.h"
#include "mach/sync_write.h"
#include "asm/cacheflush.h"

/*                       */
extern int config_L2(int size);
extern void __inner_flush_dcache_all(void);
extern void __inner_flush_dcache_L1(void);
extern void __inner_flush_dcache_L2(void);
extern void __disable_cache(void);
extern void __enable_cache(void);
void inner_dcache_flush_all(void);
void inner_dcache_flush_L1(void);
void inner_dcache_flush_L2(void);
#define L2C_SIZE_CFG_OFF 5
#define L2C_DCM_CFG_OFF 8
#define INFRA_DCM_CFG_OFF 9

static DEFINE_SPINLOCK(cache_cfg_lock);
static DEFINE_SPINLOCK(cache_cfg1_lock);
/*                                      */
int config_L2_size(int size)
{
	volatile unsigned int cache_cfg, ret = 1;
	/*                       */
	spin_lock(&cache_cfg_lock);
	cache_cfg = readl(IOMEM(MCUSYS_CFGREG_BASE));
	if (size == SZ_256K) {
		cache_cfg &= (~0x7) << L2C_SIZE_CFG_OFF;
		cache_cfg |= 0x1 << L2C_SIZE_CFG_OFF;
		mt65xx_reg_sync_writel(cache_cfg, MCUSYS_CFGREG_BASE);
	} else if (size == SZ_512K) {
		cache_cfg &= (~0x7) << L2C_SIZE_CFG_OFF;
		cache_cfg |= 0x3 << L2C_SIZE_CFG_OFF;
		mt65xx_reg_sync_writel(cache_cfg, MCUSYS_CFGREG_BASE);
	} else {
		ret = -1;
	}
	spin_unlock(&cache_cfg_lock);
	return ret;
}

int get_l2c_size(void)
{
	volatile unsigned int cache_cfg;
	int size;
	cache_cfg = readl(IOMEM(MCUSYS_CFGREG_BASE));
	cache_cfg = cache_cfg >> L2C_SIZE_CFG_OFF;
	cache_cfg = cache_cfg & 0x7;
	if (cache_cfg == 0)
		size = SZ_128K;
	else if (cache_cfg == 1)
		size = SZ_256K;
	else if (cache_cfg == 3)
		size = SZ_512K;
	else if (cache_cfg == 7)
		size = SZ_1M;
	else
		size = -1;
	return size;
}

atomic_t L1_flush_done = ATOMIC_INIT(0);
extern int __is_dcache_enable(void);
extern int cr_alignment;

void atomic_flush(void)
{
	__disable_cache();
	inner_dcache_flush_L1();

    atomic_inc(&L1_flush_done);
    //                                                    
    cr_alignment = cr_alignment & ~(0x4); //       
}

int config_L2(int size)
{ 
	int cur_size = get_l2c_size();
	if (size != SZ_256K && size != SZ_512K) {
		printk("inlvalid input size %x\n", size);
		return -1;
	}
	if (in_interrupt()) {
		printk(KERN_ERR "Cannot use %s in interrupt/softirq context\n",
		       __func__);
		return -1;
	}
	if (size == cur_size) {
		printk("Config L2 size %x is equal to current L2 size %x\n",
		       size, cur_size);
		return 0;
	}

    atomic_set(&L1_flush_done, 0);
	get_online_cpus();
	//                                                                             
	
	/*                            */
	on_each_cpu((smp_call_func_t)atomic_flush, NULL, true);
	//                                                         
    //                                      
    
    /*          */	
	inner_dcache_flush_L2();
	//                                      
	
	/*                */	
	config_L2_size(size);
	//                                                                  
		
	/*              */
	atomic_set(&L1_flush_done, 0);
	on_each_cpu((smp_call_func_t)__enable_cache, NULL, true);
	
	//                                                    
	cr_alignment = cr_alignment | (0x4); //       
	put_online_cpus();
	printk("Config L2 size %x done\n", size);
	return 0;
}

#include <linux/device.h>
#include <linux/platform_device.h>
static struct device_driver mt_l2c_drv = {
	.name = "l2c",
	.bus = &platform_bus_type,
	.owner = THIS_MODULE,
};

int mt_l2c_get_status(void)
{
	unsigned int size, cache_cfg;
	cache_cfg = readl(IOMEM(MCUSYS_CFGREG_BASE));
	cache_cfg = cache_cfg >> L2C_SIZE_CFG_OFF;
	cache_cfg &= 0x7;
	if (cache_cfg == 1) {
		//               
		size = 256;
	} else if (cache_cfg == 3) {
		//               
		size = 512;
	} else {
		size = -1;
		printk("Wrong cache_cfg = %x, size = %d\n", cache_cfg, size);
	}
	return size;
}

/*
                                      
 */
static ssize_t cur_l2c_show(struct device_driver *driver, char *buf)
{
	unsigned int size;

	size = mt_l2c_get_status();
	return snprintf(buf, PAGE_SIZE, "%d\n", size);
}

/*
                                      
 */
static ssize_t cur_l2c_store(struct device_driver *driver, const char *buf,
			     size_t count)
{
	char *p = (char *)buf;
	int size, ret;

	size = simple_strtoul(p, &p, 10);
	if (size == 256) {
		size = SZ_256K;
	} else if (size == 512) {
		size = SZ_512K;
	} else {
		printk("invalid size value: %d\n", size);
		return count;
	}

	ret = config_L2(size);
	if (ret < 0)
		printk("Config L2 error ret:%d size value: %d\n", ret, size);
	return count;
}

DRIVER_ATTR(current_l2c, 0644, cur_l2c_show, cur_l2c_store);
/*
                                      
                   
 */
int mt_l2c_init(void)
{
	int ret;

	ret = driver_register(&mt_l2c_drv);
	if (ret) {
		printk("fail to register mt_l2c_drv\n");
	}

	ret = driver_create_file(&mt_l2c_drv, &driver_attr_current_l2c);
	if (ret) {
		printk("fail to create mt_l2c sysfs files\n");
	}
	return 0;
}

arch_initcall(mt_l2c_init);

/*                                             
                                                   
                                                   
                                  
*/
int auto_inv_cache(unsigned int disable)
{
	volatile unsigned int cache_cfg, cache_cfg_new;
	spin_lock(&cache_cfg1_lock);
	if(disable) {
		/*                        */
		cache_cfg = readl(IOMEM(MCUSYS_CFGREG_BASE));
		cache_cfg |= 0x1F;
		writel(cache_cfg, IOMEM(MCUSYS_CFGREG_BASE));
	} else if (disable == 0){
		/*                       */
		cache_cfg = readl(IOMEM(MCUSYS_CFGREG_BASE));
		cache_cfg &= ~0x1F;
		writel(cache_cfg, IOMEM(MCUSYS_CFGREG_BASE));
	} else {
		printk("Caller give a wrong arg:%d\n", disable);
		spin_unlock(&cache_cfg1_lock);
		return -1;
	}
	cache_cfg_new = readl(IOMEM(MCUSYS_CFGREG_BASE));
	spin_unlock(&cache_cfg1_lock);
	if((cache_cfg_new & 0x1F) != (cache_cfg & 0x1F))
		return 0;
	else
		return -1;
}

EXPORT_SYMBOL(config_L2);
