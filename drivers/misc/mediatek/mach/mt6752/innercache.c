#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/cpu.h>
#include <linux/smp.h>
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

#ifdef CONFIG_ARM64
int get_cluster_core_count(void)
{
    unsigned int cores;

    asm volatile(
    "mrs %0, S3_1_C11_C0_2\n"
    : "=r" (cores)
    :
    : "cc"
    );

    return ((cores >> 24)& 0x3) + 1;
}
#else
int get_cluster_core_count(void)
{
    unsigned int cores;

    asm volatile(
    "MRC p15, 1, %0, c9, c0, 2\n"
    : "=r" (cores)
    :
    : "cc"
    );

    return (cores >> 24) + 1;
}
#endif

/*
                                                                                   
  
                                            
                                                      
                                       
  
                                                       
                                           
                                                                 
                                                            
 */
void smp_inner_dcache_flush_all(void)
{
    int i, j, num_core, total_core, online_cpu;
    struct cpumask mask;

    if (in_interrupt()) {
        printk(KERN_ERR
        "Cannot invoke smp_inner_dcache_flush_all() in interrupt/softirq context\n");
        return;
    }

    get_online_cpus();    
    preempt_disable();

    on_each_cpu((smp_call_func_t)inner_dcache_flush_L1, NULL, true);

    num_core = get_cluster_core_count();
    total_core = num_possible_cpus();

    //                                                                                               

    for(i = 0; i < total_core; i+=num_core){
        cpumask_clear(&mask);
        for(j = i; j < (i + num_core); j++)
        {
            //                                      
            if (cpu_online(j))
                cpumask_set_cpu(j, &mask);
        }
        online_cpu = cpumask_first_and(cpu_online_mask, &mask);
        //                                                                                                                                     
        smp_call_function_single(online_cpu, (smp_call_func_t)inner_dcache_flush_L2, NULL, true);

    }

    preempt_enable();        
    put_online_cpus();
}

EXPORT_SYMBOL(inner_dcache_flush_all);
EXPORT_SYMBOL(smp_inner_dcache_flush_all);
#if 0
extern void __inner_flush_dcache_all(void);
extern void __inner_flush_dcache_L1(void);
extern void __inner_flush_dcache_L2(void);
extern void __inner_clean_dcache_all(void);
extern void __inner_clean_dcache_L1(void);
extern void __inner_clean_dcache_L2(void);
extern void __inner_inv_dcache_all(void);
extern void __inner_inv_dcache_L1(void);
extern void __inner_inv_dcache_L2(void)
extern void __enable_dcache(void);
extern void __enable_icache(void);
extern void __enable_cache(void);
extern void __disable_dcache(void);
extern void __disable_icache(void);
extern void __disable_cache(void);
extern void __disable_dcache__inner_flush_dcache_L1(void);
extern void __disable_dcache__inner_flush_dcache_L1__inner_flush_dcache_L2(void);
extern void __disable_dcache__inner_flush_dcache_L1__inner_clean_dcache_L2(void);

static ssize_t cache_test_show(struct device_driver *driver, char *buf)
{
	__disable_icache();
	__enable_icache();
	__disable_dcache();
	__enable_dcache();
	__disable_cache();
	__enable_cache();
	__inner_inv_dcache_L1();
	__inner_inv_dcache_L2();
	__inner_inv_dcache_all();
	__inner_clean_dcache_L1();
	__inner_clean_dcache_L2();
	__inner_clean_dcache_all();
	__inner_flush_dcache_L1();
	__inner_flush_dcache_L2();
	__inner_flush_dcache_all();
	__disable_dcache__inner_flush_dcache_L1();
	__disable_dcache__inner_flush_dcache_L1__inner_clean_dcache_L2();
	__disable_dcache__inner_flush_dcache_L1__inner_flush_dcache_L2();

	return strlen(buf);;
}

static ssize_t cache_test_store(struct device_driver * driver, const char *buf,
                           size_t count)
{
	return count;
}

DRIVER_ATTR(lastpc_dump, 0664, lastpc_dump_show, lastpc_dump_store);
#endif
