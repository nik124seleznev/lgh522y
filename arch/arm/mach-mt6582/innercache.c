#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/cpu.h>
#include <linux/smp.h>
#include "mach/mt_reg_base.h"
#include "mach/sync_write.h"

/*                       */
extern void __inner_flush_dcache_all(void);
extern void __inner_flush_dcache_L1(void);
extern void __inner_flush_dcache_L2(void);

/*
                                                                               
  
                                            
                                                      
                                       
  
                                                                        
                                                                     
                       
                                            
                                
                         
                                         
                                                                                                    
                                                           
 */

void inner_dcache_flush_all(void)
{
	__inner_flush_dcache_all();
}

void inner_dcache_flush_L1(void)
{
	__inner_flush_dcache_L1();
}

void inner_dcache_flush_L2(void)
{
	__inner_flush_dcache_L2();
}

/*
                                                                                   
  
                                            
                                                      
                                       
  
                                                       
                                           
                                                                 
                                                            
 */
void smp_inner_dcache_flush_all(void)
{
	if (in_interrupt()) {
		printk(KERN_ERR
		       "Cannot invoke smp_inner_dcache_flush_all() in interrupt/softirq context\n");
		return;
	}
	get_online_cpus();

	on_each_cpu((smp_call_func_t)inner_dcache_flush_L1, NULL, true);
	inner_dcache_flush_L2();

	put_online_cpus();

}

EXPORT_SYMBOL(inner_dcache_flush_all);
EXPORT_SYMBOL(smp_inner_dcache_flush_all);
