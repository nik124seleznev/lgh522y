/*
 * Spin Table SMP initialisation
 *
 * Copyright (C) 2013 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/smp.h>

#include <asm/cacheflush.h>
#include <asm/cpu_ops.h>
#include <asm/cputype.h>
#include <asm/smp_plat.h>

extern void secondary_holding_pen(void);
volatile unsigned long secondary_holding_pen_release = INVALID_HWID;

#include <linux/io.h>
#include <linux/of_address.h>


static phys_addr_t cpu_release_addr[NR_CPUS];
static DEFINE_RAW_SPINLOCK(boot_lock);

/*
                                                                        
                                                                        
                                                                       
            
 */
static void write_pen_release(u64 val)
{
	void *start = (void *)&secondary_holding_pen_release;
	unsigned long size = sizeof(secondary_holding_pen_release);

	secondary_holding_pen_release = val;
	__flush_dcache_area(start, size);
}


static int smp_spin_table_cpu_init(struct device_node *dn, unsigned int cpu)
{
	/*
                                                        
  */
	if (of_property_read_u64(dn, "cpu-release-addr",
				 &cpu_release_addr[cpu])) {
		pr_err("CPU %d: missing or invalid cpu-release-addr property\n",
		       cpu);

		return -1;
	}

	return 0;
}

/*        */
#define CCI400_SI4_BASE                                 0x5000
#define CCI400_SI4_SNOOP_CONTROL           CCI400_SI4_BASE
#define DVM_MSG_REQ                                     (1U << 1)
#define SNOOP_REQ                                       (1U << 0)
#define CCI400_STATUS                                   0x000C
#define CHANGE_PENDING                                  (1U << 0)

static int smp_spin_table_cpu_prepare(unsigned int cpu)
{
	void **release_addr;

    struct device_node *node;
    void __iomem *cci400_base;

	if (!cpu_release_addr[cpu])
		return -ENODEV;

    /*                                   */
    node = of_find_compatible_node(NULL, NULL, "mediatek,CCI400");
    if(node)
    {
        cci400_base = of_iomap(node, 0);
                
        printk(KERN_EMERG "1.CCI400_SI4_SNOOP_CONTROL:0x%p, 0x%08x\n", cci400_base + CCI400_SI4_SNOOP_CONTROL, readl(cci400_base + CCI400_SI4_SNOOP_CONTROL));
        /*                                               */
        writel(readl(cci400_base + CCI400_SI4_SNOOP_CONTROL) | (SNOOP_REQ | DVM_MSG_REQ), cci400_base + CCI400_SI4_SNOOP_CONTROL);
        while (readl(cci400_base + CCI400_STATUS) & CHANGE_PENDING);
        printk(KERN_EMERG "2.CCI400_SI4_SNOOP_CONTROL:0x%p, 0x%08x\n", cci400_base + CCI400_SI4_SNOOP_CONTROL,readl(cci400_base + CCI400_SI4_SNOOP_CONTROL));
    }

	release_addr = __va(cpu_release_addr[cpu]);
	release_addr[0] = (void *)__pa(secondary_holding_pen);
	__flush_dcache_area(release_addr, sizeof(release_addr[0]));

	/*
                                               
  */
	sev();

	return 0;
}

static int smp_spin_table_cpu_boot(unsigned int cpu)
{
	unsigned long timeout;

	/*
                                                         
                         
  */
	raw_spin_lock(&boot_lock);

	/*
                                
  */
	write_pen_release(cpu_logical_map(cpu));

	/*
                                                               
  */
	sev();

	timeout = jiffies + (1 * HZ);
	while (time_before(jiffies, timeout)) {
		if (secondary_holding_pen_release == INVALID_HWID)
			break;
		udelay(10);
	}

	/*
                                                        
                                            
  */
	raw_spin_unlock(&boot_lock);

	return secondary_holding_pen_release != INVALID_HWID ? -ENOSYS : 0;
}

void smp_spin_table_cpu_postboot(void)
{
	/*
                                                        
  */
	write_pen_release(INVALID_HWID);

	/*
                                     
  */
	raw_spin_lock(&boot_lock);
	raw_spin_unlock(&boot_lock);
}

const struct cpu_operations smp_spin_table_ops = {
	.name		= "spin-table",
	.cpu_init	= smp_spin_table_cpu_init,
	.cpu_prepare	= smp_spin_table_cpu_prepare,
	.cpu_boot	= smp_spin_table_cpu_boot,
	.cpu_postboot	= smp_spin_table_cpu_postboot,
};
