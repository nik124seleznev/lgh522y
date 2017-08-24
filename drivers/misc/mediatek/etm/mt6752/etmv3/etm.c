#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <linux/aee.h>
#include <linux/dma-mapping.h>
#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <asm/sizes.h>
#include <mach/mt_reg_base.h>
#include <mach/sync_write.h>
#include <mach/etm.h>
#include "etm_register.h"
#include "etb_register.h"

#define TRACE_RANGE_START 0xb0000000	/*                     */
#define TRACE_RANGE_END	0xd0000000	/*                     */
#define TIMEOUT 1000000
#define ETB_TIMESTAMP 1 
#define ETB_CYCLE_ACCURATE 0
#define CS_TP_PORTSIZE 16
/*                                         */
#define CS_FORMATMODE 0x2001	/*                                         */

enum {
	TRACE_STATE_STOP = 0,		/*               */
	TRACE_STATE_TRACING ,		/*         */
	TRACE_STATE_UNFORMATTING,	/*                    */
	TRACE_STATE_UNFORMATTED,	/*                   */
	TRACE_STATE_SYNCING,		/*                       */
	TRACE_STATE_PARSING,		/*                 */
};

struct etm_info
{
	int enable;
	int is_ptm;
	const int *pwr_down;
	u32 etmcr;
	u32 etmccer;
};

struct etm_trace_context_t 
{
	int nr_etm_regs;
	void __iomem **etm_regs;
	void __iomem *etb_regs;
	void __iomem *funnel_regs;
	void __iomem *tpiu_regs;
	void __iomem *dem_regs;
	u32 etr_virt;
	u32 etr_phys;
	u32 etr_len;
	int use_etr;
	int etb_total_buf_size;
	int enable_data_trace;
	u32 trace_range_start, trace_range_end;
	struct etm_info *etm_info;
	int etm_idx;
	int state;
	struct mutex mutex;
};

static struct etm_trace_context_t tracer;

#define DBGRST_ALL (tracer.dem_regs + 0x028)
#define DBGBUSCLK_EN (tracer.dem_regs + 0x02C)
#define DBGSYSCLK_EN (tracer.dem_regs + 0x030)
#define AHBAP_EN (tracer.dem_regs + 0x040)
#define DEM_UNLOCK (tracer.dem_regs + 0xFB0)
#define DEM_UNLOCK_MAGIC 0xC5ACCE55
#define AHB_EN (1 << 0)
#define POWER_ON_RESET (0 << 0)
#define SYSCLK_EN (1 << 0)
#define BUSCLK_EN (1 << 0)

/* 
                         
                           
                           
                                       
 */
unsigned int etb_readl(const struct etm_trace_context_t *ctx, int x)
{
	return __raw_readl(ctx->etb_regs + x);
}

/* 
                        
                           
                                               
                           
                                        
 */
void etb_writel(const struct etm_trace_context_t *ctx, unsigned int v, int x)
{
	mt65xx_reg_sync_writel(v, ctx->etb_regs + x);
}

/* 
                                  
                           
                                     
 */
int etb_supports_lock(const struct etm_trace_context_t *ctx)
{
	return etb_readl(ctx, ETBLS) & 0x1;
}

/* 
                                         
                           
                             
 */
int etb_is_locked(const struct etm_trace_context_t *ctx)
{
	return etb_readl(ctx, ETBLS) & 0x2;
}

/* 
                                                
                           
 */
void etb_lock(const struct etm_trace_context_t *ctx)
{
	if (etb_supports_lock(ctx)) {
		do {
			etb_writel(ctx, 0, ETBLA);
		} while (unlikely(!etb_is_locked(ctx)));
	} else {
		pr_warning("ETB does not support lock\n");
	}
}

/* 
                                               
                           
 */
void etb_unlock(const struct etm_trace_context_t *ctx)
{
	if (etb_supports_lock(ctx)) {
		do {
			etb_writel(ctx, ETBLA_UNLOCK_MAGIC, ETBLA);
		} while (unlikely(etb_is_locked(ctx)));
	} else {
		pr_warning("ETB does not support lock\n");
	}
}

int etb_get_data_length(const struct etm_trace_context_t *t)
{
	unsigned int v;
	int rp, wp;

	v = etb_readl(t, ETBSTS);
	rp = etb_readl(t, ETBRRP);
	wp = etb_readl(t, ETBRWP);
	
	pr_info("ETB status = 0x%x, rp = 0x%x, wp = 0x%x\n", v, rp, wp);

	if (v & 1) { 
		/*      */
		return t->etb_total_buf_size;
	} else {
		if (t->use_etr) {
			if (wp == 0) {
				/*                                          */
				return 0;
			} else {
				return (wp - tracer.etr_phys) / 4;
			}
		} else {
			return wp;
		}
	}
}

static int etb_open(struct inode *inode, struct file *file)
{
	if (!tracer.etb_regs)
		return -ENODEV;

	file->private_data = &tracer;

	return nonseekable_open(inode, file);
}

static ssize_t etb_read(struct file *file, char __user *data,
					size_t len, loff_t *ppos)
{
	int total, i;
	long length;
	struct etm_trace_context_t *t = file->private_data;
	u32 first = 0, buffer_end = 0;
	u32 *buf;
	int wpos;
	int skip;
	long wlength;
	loff_t pos = *ppos;

	mutex_lock(&t->mutex);

	if (t->state == TRACE_STATE_TRACING) {
		length = 0;
		pr_err("Need to stop trace\n");
		goto out;
	}

	etb_unlock(t);

	total = etb_get_data_length(t);
	if (total == t->etb_total_buf_size) {
		first = etb_readl(t, ETBRWP);
		if (t->use_etr) {
			first = (first - t->etr_phys) / 4;
		}
	}

	if (pos > total * 4) {
		skip = 0;
		wpos = total;
	} else {
		skip = (int)pos % 4;
		wpos = (int)pos / 4;
	}
	total -= wpos;
	first = (first + wpos) % t->etb_total_buf_size;

	etb_writel(t, first, ETBRRP);

	wlength = min(total, DIV_ROUND_UP(skip + (int)len, 4));
	length = min(total * 4 - skip, (int)len);
	if (wlength == 0) {
		goto out;
	}

	buf = vmalloc(wlength * 4);

	pr_info("ETB read %ld bytes to %lld from %ld words at %d\n",
		length, pos, wlength, first);
	pr_info("ETB buffer length: %d\n", total + wpos);
	pr_info("ETB status reg: 0x%x\n", etb_readl(t, ETBSTS));

	if (t->use_etr) {
		/* 
                                                      
                                                                 
   */

		pr_info("ETR virt = 0x%x, phys = 0x%x\n", t->etr_virt, t->etr_phys);

		/*                                                  */
		first *= 4;
		first += t->etr_virt;
		buffer_end = t->etr_virt + (t->etr_len * 4);
		pr_info("first(virt) = 0x%x\n\n", first);

		for (i = 0; i < wlength; i++) {
			buf[i] = *((unsigned int*)(first));
			first += 4;
			if (first >= buffer_end) {
				first = t->etr_virt;  
			}
		}
	} else {
		for (i = 0; i < wlength; i++) {
			buf[i] = etb_readl(t, ETBRRD);
		}
	}

	etb_lock(t);

	length -= copy_to_user(data, (u8 *)buf + skip, length);
	vfree(buf);
	*ppos = pos + length;

out:
	mutex_unlock(&t->mutex);

	return length;
}

static struct file_operations etb_file_ops = {
	.owner = THIS_MODULE,
	.read = etb_read,
	.open = etb_open,
};

static struct miscdevice etb_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "etb",
	.fops = &etb_file_ops
};

static struct miscdevice etm_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "etm",
};

/* 
                         
                           
                     
                           
                                       
 */
unsigned int etm_readl(const struct etm_trace_context_t *ctx, int n, int x)
{
	return __raw_readl(ctx->etm_regs[n] + x);
}

#if 0
/* 
                        
                           
                     
                                               
                           
                                        
 */
unsigned int etm_writel(const struct etm_trace_context_t *ctx, int n, unsigned int v,
							int x)
{
	return __raw_writel(v, ctx->etm_regs[n] + x);
}
#endif 

void cs_cpu_write(void __iomem *addr_base, u32 offset, u32 wdata)
{
	/*                                                              */
	mt65xx_reg_sync_writel(wdata, addr_base + offset);
}

u32 cs_cpu_read(const void __iomem *addr_base, u32 offset)
{
	u32 actual;
 
	/*                                                              */
	actual = __raw_readl(addr_base + offset);

	return actual;
}

void cs_cpu_lock(void __iomem *addr_base)
{
	u32 result;

	cs_cpu_write(addr_base, 0xFB0, 0);

	result = cs_cpu_read(addr_base, 0xFB4);

	if (result != 0x03) {
		//                                             
	} 

	/*         */
	cs_cpu_write(addr_base, 0x300, 0xC5ACCE55);
}

void cs_cpu_unlock(void __iomem *addr_base) 
{
	u32 result;

	cs_cpu_write(addr_base, 0xFB0, 0xC5ACCE55);

	result = cs_cpu_read(addr_base, 0xFB4);

	if (result != 0x01) {
		//                                               
	}

	/*              */
	cs_cpu_write(addr_base, 0x300, 0x0);
}

void cs_cpu_ptm_powerup(void __iomem *ptm_addr_base)
{
	u32 result;

	result = cs_cpu_read(ptm_addr_base, 0x000);
	result = result ^ 0x001;
	cs_cpu_write(ptm_addr_base, 0x000, result);
}

void cs_cpu_ptm_progbit(void __iomem *ptm_addr_base)
{
	u32 result;
	u32 counter = 0;

	/*                                      */
	result = cs_cpu_read(ptm_addr_base, 0x000);
	cs_cpu_write(ptm_addr_base, 0x000, result | (1 << 10));

	/*                                                   */
	result = 0;
	while (!(result & ((1 << 1))) ) {
		result = cs_cpu_read(ptm_addr_base, 0x010);
		counter++;
		if (counter >= TIMEOUT) {
			//                                                     
			break;
		}
	}
}

void cs_cpu_ptm_clear_progbit(void __iomem *ptm_addr_base)
{
	u32 result;
	u32 counter = 0;

	/*                                        */
	result = cs_cpu_read(ptm_addr_base, 0x000);
	cs_cpu_write(ptm_addr_base, 0x000, result ^ (1 << 10));

	/*                                                     */
	do{
		result = cs_cpu_read(ptm_addr_base, 0x010);
		counter++;
		if (counter >= TIMEOUT) {
			//                                                         
			break;
		}
	} while ((result & 0x2));
}

void cs_cpu_tpiu_setup(void)
{
	u32 result;

	/*                                  */
	cs_cpu_write(tracer.tpiu_regs, 0x004, 1 << (CS_TP_PORTSIZE - 1));

	result = cs_cpu_read(tracer.tpiu_regs, 0x000);
	if (!(result & ((CS_TP_PORTSIZE - 1))) ) {
		/*                                                     */
	}

	/*                                                                                */
	cs_cpu_write(tracer.tpiu_regs, 0x304, CS_FORMATMODE);
}

void cs_cpu_funnel_setup(void)
{
	u32 funnel_ports, i;

	funnel_ports = 0;
  
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (tracer.etm_info[i].enable) {
			funnel_ports = funnel_ports | (1 << i); 
		}
	}

	cs_cpu_write(tracer.funnel_regs, 0x000, funnel_ports);

#if 0
	/*                                           */
	cs_cpu_write (tracer.funnel_regs, 0x004, 0x00FAC0D1);
#endif
}

void cs_cpu_flushandstop(void __iomem *device_addr_base)
{
	u32 result;
	u32 counter = 0;

	/*                                                  */
	cs_cpu_write(device_addr_base, 0x304, CS_FORMATMODE | (1 << 12));
	/*                      */
	cs_cpu_write(device_addr_base, 0x304, CS_FORMATMODE | (1 << 6));

	/*                                 */
	result = 0x02;
	while (result & 0x02) {
		result = cs_cpu_read(device_addr_base, 0x300);
		counter++;
		if (counter >= TIMEOUT) {
			//                                                    
			break;
		}
	}
}

void cs_cpu_etb_setup(void)
{
	/*                                                                                */
	cs_cpu_write(tracer.etb_regs, 0x304, CS_FORMATMODE);
	/*                                          */
	cs_cpu_write(tracer.etb_regs, 0x020, 0x01);
}

void cs_cpu_test_common_ptm_setup(void __iomem *ptm_addr_base, int core)
{
	u32 result;

	/*
                                                
                 
  */
	cs_cpu_write(ptm_addr_base, 0x040, tracer.trace_range_start); 
	cs_cpu_write(ptm_addr_base, 0x044, tracer.trace_range_end); 
	/*         */
	cs_cpu_write(ptm_addr_base, 0x080, 0x01);
	cs_cpu_write(ptm_addr_base, 0x084, 0x01);
	/*                   */
	cs_cpu_write(ptm_addr_base, 0x020, 0x10);
	/*                     */
	cs_cpu_write(ptm_addr_base, 0x024, 0x01);	
  
	/*                                     */
	cs_cpu_write(ptm_addr_base, 0x008, 0x0000406F);
#if 0
	cs_cpu_write(ptm_addr_base, 0x008, 0x00000010);
#endif

	/*                                       */
	cs_cpu_write(ptm_addr_base, 0x1F8, 0x0000406F);
  
#if ETB_TIMESTAMP
	/*                  */
	result = cs_cpu_read (ptm_addr_base, 0x000);
	cs_cpu_write(ptm_addr_base, 0x000, result | (1 << 28));
#endif

#if ETB_CYCLE_ACCURATE
	/*                       */
	result = cs_cpu_read (ptm_addr_base, 0x000);
	cs_cpu_write (ptm_addr_base, 0x000, result | (1 << 12));
#endif

	/*                               */
	result = cs_cpu_read (ptm_addr_base, 0x000);
	cs_cpu_write(ptm_addr_base, 0x000, result | (3 << 14));

	if (!(tracer.etm_info[core].is_ptm)) {
		if (tracer.enable_data_trace == 0x1) {
			/*                            */
			/*                                                     */
			result = cs_cpu_read(ptm_addr_base, 0x000);
			cs_cpu_write(ptm_addr_base, 0x000, result | (3 << 2));
	
			/*                                */
			cs_cpu_write(ptm_addr_base, 0x30, 0x37EF);
			/*              */
			cs_cpu_write(ptm_addr_base, 0x3C, 0x10000);  
		} else {
			/*                     */
			result = cs_cpu_read(ptm_addr_base, 0x000);
			cs_cpu_write (ptm_addr_base, 0x000, result & 0xFFFFFFF9);
		}
	}

	/*                                  */
#if 0
	cs_cpu_write(ptm_addr_base, 0x1E0, 0x00000400);  /*      */
#endif
	cs_cpu_write(ptm_addr_base, 0x1E0, 0x00000080);  /*     */

	/*          */
	result = cs_cpu_read(ptm_addr_base, 0x000);
	cs_cpu_write(ptm_addr_base, 0x000, result | (1 << 11));
}

static void trace_start(void)
{
	int i;
	int pwr_down;

	if (tracer.state == TRACE_STATE_TRACING) {
		pr_info("ETM trace is already running\n");
		return;
	}

	get_online_cpus();

	mutex_lock(&tracer.mutex);
  
	/*                                                                       */
	__raw_writel(DEM_UNLOCK_MAGIC, DEM_UNLOCK);
	mt65xx_reg_sync_writel(AHB_EN, AHBAP_EN);
  
	etb_unlock(&tracer);

	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (tracer.etm_info[i].pwr_down == NULL) {
			pwr_down = 0;
		} else {
			pwr_down = *(tracer.etm_info[i].pwr_down);
		}
		if (!pwr_down) {
			cs_cpu_unlock(tracer.etm_regs[i]);
		}
	}

	cs_cpu_unlock(tracer.tpiu_regs);
	cs_cpu_unlock(tracer.funnel_regs);
	cs_cpu_unlock(tracer.etb_regs);
  
	cs_cpu_funnel_setup();
	cs_cpu_etb_setup();  
  
	/*              */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (tracer.etm_info[i].pwr_down == NULL) {
			pwr_down = 0;
		} else {
			pwr_down = *(tracer.etm_info[i].pwr_down);
		}
		if (!pwr_down) {
			cs_cpu_ptm_powerup(tracer.etm_regs[i]);
		}
	}

	/*                                               */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (tracer.etm_info[i].pwr_down == NULL) {
			pwr_down = 0;
		} else {
			pwr_down = *(tracer.etm_info[i].pwr_down);
		}
		if (!pwr_down) {
			cs_cpu_ptm_progbit(tracer.etm_regs[i]);
		}
	}

	/*            */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (tracer.etm_info[i].pwr_down == NULL) {
			pwr_down = 0;
		} else {
			pwr_down = *(tracer.etm_info[i].pwr_down);
		}
		if (!pwr_down) {
			cs_cpu_test_common_ptm_setup(tracer.etm_regs[i], i);
		}
	}
  
	/*                         */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (tracer.etm_info[i].pwr_down == NULL) {
			pwr_down = 0;
		} else {
			pwr_down = *(tracer.etm_info[i].pwr_down);
		}
		if (!pwr_down) {
			cs_cpu_write(tracer.etm_regs[i], 0x200, i + 1);  
		}
	}

	/*                              */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (tracer.etm_info[i].pwr_down == NULL) {
			pwr_down = 0;
		} else {
			pwr_down = *(tracer.etm_info[i].pwr_down);
		}
		if (!pwr_down) {
			tracer.etm_info[i].etmcr = etm_readl(&tracer, i, ETMCR);
			tracer.etm_info[i].etmccer = etm_readl(&tracer, i, ETMCCER);
		}
	}
  
	/*                                           */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (tracer.etm_info[i].pwr_down == NULL) {
			pwr_down = 0;
		} else {
			pwr_down = *(tracer.etm_info[i].pwr_down);
		}
		if (!pwr_down) {
			cs_cpu_ptm_clear_progbit(tracer.etm_regs[i]);
		}
	}

	/*                           */
	__raw_writel(DEM_UNLOCK_MAGIC, DEM_UNLOCK);
	__raw_writel(POWER_ON_RESET, DBGRST_ALL);
	__raw_writel(BUSCLK_EN, DBGBUSCLK_EN);
	mt65xx_reg_sync_writel(SYSCLK_EN, DBGSYSCLK_EN);
  
	tracer.state = TRACE_STATE_TRACING;
  
	etb_lock(&tracer);

	mutex_unlock(&tracer.mutex);

	put_online_cpus();
}
  
static void trace_stop(void)
{ 
	int i;	 
	int pwr_down;

	if (tracer.state == TRACE_STATE_STOP) {
		pr_info("ETM trace is already stop!\n");
		return;
	}
  
	get_online_cpus();

	mutex_lock(&tracer.mutex);
  
	etb_unlock(&tracer);
  
	/*                      */
	/*                            */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (tracer.etm_info[i].pwr_down == NULL) {
			pwr_down = 0;
		} else {
			pwr_down = *(tracer.etm_info[i].pwr_down);
		}
		if (!pwr_down) {
			cs_cpu_ptm_progbit(tracer.etm_regs[i]);
		}
	}

#if 0
	cs_cpu_flushandstop(tracer.tpiu_regs);
#endif
  
	/*                                          */
	cs_cpu_write(tracer.etb_regs, 0x20, 0x0);
	/*                                                 */
	/*                      */
#if 0
	cs_cpu_write (tracer.etb_regs, 0x14, 0x0);
#endif

	/*            */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (tracer.etm_info[i].pwr_down == NULL) {
			pwr_down = 0;
		} else {
			pwr_down = *(tracer.etm_info[i].pwr_down);
		}
		if (!pwr_down) {
			cs_cpu_write(tracer.etm_regs[i], 0x0, 0x1);
		}
	}

	dsb();

	tracer.state = TRACE_STATE_STOP;

	etb_lock(&tracer);

	mutex_unlock(&tracer.mutex);

	put_online_cpus();
}

/* 
                                                         
                  
                                                          
 */
void trace_start_by_cpus(const struct cpumask *mask, int init_etb)
{
	int i;

	if (!mask) {
		return ;
	}

	if (init_etb) {
		/*                                                               */
		mt65xx_reg_sync_writel(AHB_EN, (volatile u32 *)AHBAP_EN);

		etb_unlock(&tracer);

		if (tracer.use_etr) {
			/*                                  */  
			etb_writel(&tracer, tracer.etr_phys, 0x118);
			/*                               */
			etb_writel(&tracer, tracer.etr_len, 0x4);
		}

		/*                                          */
		/*               */
		cs_cpu_write (tracer.etb_regs, 0x20, 0x0);
	}

	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (cpumask_test_cpu(i, mask)) {
			cs_cpu_unlock(tracer.etm_regs[i]);
		}
	}

	if (init_etb) {
		cs_cpu_unlock(tracer.tpiu_regs);
		cs_cpu_unlock(tracer.funnel_regs);
		cs_cpu_unlock(tracer.etb_regs);
  
		cs_cpu_funnel_setup();
		cs_cpu_etb_setup();
	}

	/*               */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (cpumask_test_cpu(i, mask)) {
			cs_cpu_ptm_powerup(tracer.etm_regs[i]);
		}
	}

	/*                                                */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (cpumask_test_cpu(i, mask)) {
			cs_cpu_ptm_progbit(tracer.etm_regs[i]);
		}
	}

	/*             */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (cpumask_test_cpu(i, mask)) {
			cs_cpu_test_common_ptm_setup(tracer.etm_regs[i], i);
		}
	}
  
	/*                         */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (cpumask_test_cpu(i, mask)) {
			cs_cpu_write(tracer.etm_regs[i], 0x200, i + 1);
		}
	}
 
	/*                                            */
	for (i = 0; i < tracer.nr_etm_regs; i++) {
		if (cpumask_test_cpu(i, mask)) {
			cs_cpu_ptm_clear_progbit(tracer.etm_regs[i]);
		}
	}

	if (init_etb) { 
		/*                           */
		mt65xx_reg_sync_writel(DEM_UNLOCK_MAGIC, (volatile u32 *)DEM_UNLOCK);
		mt65xx_reg_sync_writel(POWER_ON_RESET, (volatile u32 *)DBGRST_ALL);
		mt65xx_reg_sync_writel(BUSCLK_EN, (volatile u32 *)DBGBUSCLK_EN);
		mt65xx_reg_sync_writel(SYSCLK_EN, (volatile u32 *)DBGSYSCLK_EN);

		etb_lock(&tracer);
	}
}

static inline ssize_t run_show(struct device *kobj,
			struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%x\n", tracer.state);
}

static ssize_t run_store(struct device *kobj, struct device_attribute *attr,
			const char *buf, size_t n)
{
	unsigned int value;

	if (unlikely(sscanf(buf, "%u", &value) != 1))
		return -EINVAL;

	if (value == 1) {
		trace_start();
	} else if(value == 0) {
		trace_stop();
	} else {
		return -EINVAL;
	}

	return n;
}

DEVICE_ATTR(run, 0644, run_show, run_store);

static inline ssize_t etb_length_show(struct device *kobj, 
			struct device_attribute *attr, char *buf)
{
	int etb_legnth;

	etb_legnth = etb_get_data_length(&tracer);

	return sprintf(buf, "%08x\n", etb_legnth);
}

static ssize_t etb_length_store(struct device *kobj, struct device_attribute *attr, 
			const char *buf, size_t n)
{
	/*            */
	return n;
}

DEVICE_ATTR(etb_length, 0644, etb_length_show, etb_length_store);

static inline ssize_t trace_data_show(struct device *kobj, 
			struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%08x\n", tracer.enable_data_trace);
}

static ssize_t trace_data_store(struct device *kobj, struct device_attribute *attr, 
			const char *buf, size_t n)
{
	unsigned int value;

	if (unlikely(sscanf(buf, "%u", &value) != 1))
		return -EINVAL;  

	if (tracer.state == TRACE_STATE_TRACING) {
		pr_err("ETM trace is running. Stop first before changing setting\n");
		return n;
	}

	mutex_lock(&tracer.mutex);

	if(value == 1) {	
		tracer.enable_data_trace = 1;
	} else {
		tracer.enable_data_trace = 0;
	}

	mutex_unlock(&tracer.mutex);
  
	return n;
}

DEVICE_ATTR(trace_data, 0644, trace_data_show, trace_data_store);

static ssize_t trace_range_show(struct device *kobj, 
			struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%08x %08x\n", tracer.trace_range_start, tracer.trace_range_end);
}

static ssize_t trace_range_store(struct device *kobj, struct device_attribute *attr, 
			const char *buf, size_t n)
{
	unsigned int range_start, range_end;
  
	if (sscanf(buf, "%x %x", &range_start, &range_end) != 2)
		return -EINVAL;

	if (tracer.state == TRACE_STATE_TRACING) {
		pr_err("ETM trace is running. Stop first before changing setting\n");
		return n;
	}

	mutex_lock(&tracer.mutex);

	tracer.trace_range_start = range_start;
	tracer.trace_range_end = range_end;

	mutex_unlock(&tracer.mutex);

	return n;
}

DEVICE_ATTR(trace_range, 0644, trace_range_show, trace_range_store);

static ssize_t etm_online_show(struct device *kobj, 
			struct device_attribute *attr, char *buf)
{
	unsigned int i;
  
	for (i = 0; i < tracer.nr_etm_regs; i++ ) {
		sprintf(buf, "%sETM_%d is %s\n", buf, i, 
			(tracer.etm_info[i].enable)? "Enabled": "Disabled");
	}

	return strlen(buf);
}

static ssize_t etm_online_store(struct device *kobj, struct device_attribute *attr, 
			const char *buf, size_t n)
{
	unsigned int ret;
	unsigned int num = 0;
	unsigned char str[10];

	ret = sscanf(buf, "%9s %d",str ,&num) ;
        
        if (num >= tracer.nr_etm_regs)
        {
            printk("cpu input number is larger then %d\n",tracer.nr_etm_regs);	
            return n;
        }

	if (tracer.state == TRACE_STATE_TRACING) {
		pr_err("ETM trace is running. Stop first before changing setting\n");
		return n;
	}

	mutex_lock(&tracer.mutex);

	if (!strncmp(str, "ENABLE", strlen("ENABLE"))) {
		tracer.etm_info[num].enable = 1;
	} else if(!strncmp(str, "DISABLE", strlen("DISABLE"))) {
		tracer.etm_info[num].enable = 0;
	} else {
		pr_err("Input is not correct\n");
	}

	mutex_unlock(&tracer.mutex);

	return n;
}

DEVICE_ATTR(etm_online, 0644, etm_online_show, etm_online_store);

static ssize_t nr_etm_show(struct device *kobj, 
			struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", tracer.nr_etm_regs);
}

static ssize_t nr_etm_store(struct device *kobj, struct device_attribute *attr, 
			const char *buf, size_t n)
{
	return n;
}

DEVICE_ATTR(nr_etm, 0644, nr_etm_show, nr_etm_store);

static ssize_t etm_cr_show(struct device *kobj, 
			struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%08x\n", tracer.etm_info[tracer.etm_idx].etmcr);
}

static ssize_t etm_cr_store(struct device *kobj, struct device_attribute *attr, 
			const char *buf, size_t n)
{
	return n;
}

DEVICE_ATTR(etm_cr, 0644, etm_cr_show, etm_cr_store);

static ssize_t etm_ccer_show(struct device *kobj, 
			struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%08x\n", tracer.etm_info[tracer.etm_idx].etmccer);
}

static ssize_t etm_ccer_store(struct device *kobj, struct device_attribute *attr, 
			const char *buf, size_t n)
{
	return n;
}

DEVICE_ATTR(etm_ccer, 0644, etm_ccer_show, etm_ccer_store);

static ssize_t is_ptm_show(struct device *kobj, 
			struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", 
			(tracer.etm_info[tracer.etm_idx].is_ptm)? 1: 0);
}

static ssize_t is_ptm_store(struct device *kobj, struct device_attribute *attr, 
			const char *buf, size_t n)
{
	return n;
}

DEVICE_ATTR(is_ptm, 0644, is_ptm_show, is_ptm_store);

static ssize_t index_show(struct device *kobj, 
			struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", tracer.etm_idx);
}

static ssize_t index_store(struct device *kobj, struct device_attribute *attr, 
			const char *buf, size_t n)
{
	unsigned int value;
	int ret;

	if (unlikely(sscanf(buf, "%u", &value) != 1))
		return -EINVAL;  

	mutex_lock(&tracer.mutex);

	if (value >= tracer.nr_etm_regs) {
		ret = -EINVAL;
	} else {
		tracer.etm_idx = value;
		ret = n;
	}

	mutex_unlock(&tracer.mutex);

	return ret;
}

DEVICE_ATTR(index, 0644, index_show, index_store);

static int create_files(void)
{
	int ret = device_create_file(etm_device.this_device, &dev_attr_run);
	if (unlikely(ret != 0))
		return ret;	
		
	ret = device_create_file(etm_device.this_device, &dev_attr_etb_length);
	if (unlikely(ret != 0))
		return ret;

	ret = device_create_file(etm_device.this_device, &dev_attr_trace_data);
	if (unlikely(ret != 0))
		return ret;

	ret = device_create_file(etm_device.this_device, &dev_attr_trace_range);
	if (unlikely(ret != 0))
		return ret;
 
	ret = device_create_file(etm_device.this_device, &dev_attr_etm_online);
	if (unlikely(ret != 0))
		return ret;
 
	ret = device_create_file(etm_device.this_device, &dev_attr_nr_etm);
	if (unlikely(ret != 0))
		return ret;
 
	ret = device_create_file(etm_device.this_device, &dev_attr_etm_cr);
	if (unlikely(ret != 0))
		return ret;
 
	ret = device_create_file(etm_device.this_device, &dev_attr_etm_ccer);
	if (unlikely(ret != 0))
		return ret;

	ret = device_create_file(etm_device.this_device, &dev_attr_is_ptm);
	if (unlikely(ret != 0))
		return ret;
 
	ret = device_create_file(etm_device.this_device, &dev_attr_index);
	if (unlikely(ret != 0))
		return ret;

	return 0;
}

static void remove_files(void)
{
	device_remove_file(etm_device.this_device, &dev_attr_run);

	device_remove_file(etm_device.this_device, &dev_attr_etb_length);

	device_remove_file(etm_device.this_device, &dev_attr_trace_data);

	device_remove_file(etm_device.this_device, &dev_attr_trace_range);

	device_remove_file(etm_device.this_device, &dev_attr_etm_online);
}

static int etm_probe(struct platform_device *pdev)
{
	static int first_device = 0;
	struct etm_driver_data *data = dev_get_platdata(&pdev->dev);
	int ret = 0;
	void __iomem **new_regs;
	struct etm_info *new_info;
	int new_count;
	u32 id, config_code, config_code_extension, system_config;

	mutex_lock(&tracer.mutex);

	new_count = tracer.nr_etm_regs + 1;
	new_regs = krealloc(tracer.etm_regs,
			sizeof(tracer.etm_regs[0]) * new_count, GFP_KERNEL);
	if (!new_regs) {
		pr_err("Failed to allocate ETM register array\n");
		ret = -ENOMEM;
		goto out;
	}
	tracer.etm_regs = new_regs;
	new_info = krealloc(tracer.etm_info,
			sizeof(tracer.etm_info[0]) * new_count, GFP_KERNEL);
	if (!new_info) {
		pr_err("Failed to allocate ETM info array\n");
		ret = -ENOMEM;
		goto out;
	}
	tracer.etm_info = new_info;

	tracer.etm_regs[tracer.nr_etm_regs] = data->etm_regs;

	if (!first_device) {
		first_device = 1;

		if (unlikely((ret = misc_register(&etm_device)) != 0)) {
			pr_err("Fail to register etm device\n");
			goto out;
		}

		if (unlikely((ret = create_files()) != 0)) {
			pr_err("Fail to create device files\n");
			goto deregister;
		}

	} 

	memset(&(tracer.etm_info[tracer.nr_etm_regs]), 0, sizeof(struct etm_info));
	tracer.etm_info[tracer.nr_etm_regs].enable = 1;
	tracer.etm_info[tracer.nr_etm_regs].is_ptm = data->is_ptm;
	tracer.etm_info[tracer.nr_etm_regs].pwr_down = data->pwr_down;

	id = etm_readl(&tracer, tracer.nr_etm_regs, ETMIDR);
	config_code = etm_readl(&tracer, tracer.nr_etm_regs, ETMCCR);
	config_code_extension = etm_readl(&tracer, tracer.nr_etm_regs, ETMCCER);
	system_config = etm_readl(&tracer, tracer.nr_etm_regs, ETMSCR);
 
	tracer.nr_etm_regs = new_count;

out:
	mutex_unlock(&tracer.mutex);
	return ret;

deregister:
	misc_deregister(&etm_device);
	mutex_unlock(&tracer.mutex);
	return ret;
}

static struct platform_driver etm_driver =
{
	.probe = etm_probe,
	.driver = {
		.name = "etm",
	},
};

static int etb_probe(struct platform_device *pdev)
{
	struct etb_driver_data *data = dev_get_platdata(&pdev->dev);

	mutex_lock(&tracer.mutex);

	tracer.etb_regs = data->etb_regs;
	tracer.funnel_regs = data->funnel_regs;
	tracer.tpiu_regs = data->tpiu_regs;
	tracer.dem_regs = data->dem_regs;
	tracer.etr_virt = data->etr_virt;
	tracer.etr_phys = data->etr_phys;
	tracer.etr_len = data->etr_len;
	tracer.use_etr = data->use_etr;
  
	if (unlikely(misc_register(&etb_device) != 0)) {
		pr_err("Fail to register etb device\n");
	}

	/*                                                                       */
	__raw_writel(DEM_UNLOCK_MAGIC, DEM_UNLOCK);
	mt65xx_reg_sync_writel(AHB_EN, AHBAP_EN);

	etb_unlock(&tracer);

	if (tracer.use_etr) {
		pr_info("ETR virt = 0x%x, phys = 0x%x\n", tracer.etr_virt, tracer.etr_phys);
		/*                                  */  
		etb_writel(&tracer, tracer.etr_phys, 0x118);
		/*                               */
		etb_writel(&tracer, tracer.etr_len, 0x4);
	}

	/*                                          */
	/*               */
	cs_cpu_write(tracer.etb_regs, 0x20, 0x0);

	tracer.etb_total_buf_size = etb_readl(&tracer, ETBRDP);
	tracer.state = TRACE_STATE_STOP;

	mutex_unlock(&tracer.mutex);

	return 0;
}

static struct platform_driver etb_driver =
{
	.probe = etb_probe,
	.driver = {
		.name = "etb",
	},
};

/* 
                                    
 */
static int __init etm_init(void)
{
	int err;

	memset(&tracer, 0,sizeof(struct etm_trace_context_t));
	mutex_init(&tracer.mutex);
	tracer.trace_range_start = TRACE_RANGE_START;
	tracer.trace_range_end = TRACE_RANGE_END;

	err = platform_driver_register(&etm_driver);
	if (err) {
		return err;
	}

	err = platform_driver_register(&etb_driver);
	if (err) {
		return err;
	}

	return 0;
}

/* 
                    
 */
static void __exit etm_exit(void)
{
	kfree(tracer.etm_info);
	kfree(tracer.etm_regs);

	remove_files();

	if (misc_deregister(&etm_device)) {
		pr_err("Fail to deregister dervice\n");
	}
}

module_init(etm_init);
module_exit(etm_exit);
