#include <linux/errno.h>    //     
#include <asm/cacheflush.h> //               
#include <mach/hotplug.h>
#ifdef CONFIG_HOTPLUG_WITH_POWER_CTRL
#include <mach/mt_spm_mtcmos.h>
#endif
#include <mach/mt_spm_idle.h>
#include <mach/wd_api.h>


/* 
                   
 */
extern void __disable_dcache(void);         //                           
extern void __enable_dcache(void);          //                           
extern void __inner_clean_dcache_L2(void);  //                           
extern void inner_dcache_flush_L1(void);    //                           
extern void __switch_to_smp(void);          //                          
extern void __switch_to_amp(void);          //                          



/* 
                   
 */
atomic_t hotplug_cpu_count = ATOMIC_INIT(1);



/*
                  
 */
static inline void cpu_enter_lowpower(unsigned int cpu)
{
    //                                     

#ifdef SPM_MCDI_FUNC
    spm_hot_plug_out_after(cpu);
#endif

    /*                                                                */
    __disable_dcache();

    /*                                                      */
    inner_dcache_flush_L1();
    //                     
    //                  

    /*                                       */
    __inner_clean_dcache_L2();

    /*                             */
    __asm__ __volatile__("clrex");

    /*                                                                              */
    __switch_to_amp();
}

static inline void cpu_leave_lowpower(unsigned int cpu)
{
    //                                     

    /*                                         */
    __switch_to_smp();

    /*               */
    __enable_dcache();
}

static inline void platform_do_lowpower(unsigned int cpu, int *spurious)
{
    /*                                                          */
    for (;;) {

        /*                                                                                                                        */
        isb();

        /*                                                                                                                                                                                                        */
        dsb();

        /*
                         
         */
        __asm__ __volatile__("wfi");

        if (pen_release == cpu) {
            /*
                                            
             */
            break;
        }

        /*
                                                                   
                                                       
          
                                                                   
                          
         */
        (*spurious)++;
    }
}



/*
               
        
              
 */
int mt_cpu_kill(unsigned int cpu)
{
    HOTPLUG_INFO("mt_cpu_kill, cpu: %d\n", cpu);

#ifdef CONFIG_HOTPLUG_WITH_POWER_CTRL
    switch(cpu)
    {
        case 1:
            spm_mtcmos_ctrl_cpu1(STA_POWER_DOWN, 1);
            break;
        case 2:
            spm_mtcmos_ctrl_cpu2(STA_POWER_DOWN, 1);
            break;
        case 3:
            spm_mtcmos_ctrl_cpu3(STA_POWER_DOWN, 1);
            break;
        default:
            break;
    }
#endif    
    atomic_dec(&hotplug_cpu_count);

    return 1;
}

/*
                             
        
 */
void mt_cpu_die(unsigned int cpu)
{
    int spurious = 0;
    struct wd_api *wd_api = NULL;

    HOTPLUG_INFO("mt_cpu_die, cpu: %d\n", cpu);

    get_wd_api(&wd_api);
    if (wd_api)
        wd_api->wd_cpu_hot_plug_off_notify(cpu);

    /*
                                             
     */
    cpu_enter_lowpower(cpu);
    platform_do_lowpower(cpu, &spurious);

    /*
                                                  
                                             
     */
    cpu_leave_lowpower(cpu);

    if (spurious)
        HOTPLUG_INFO("platform_do_lowpower, spurious wakeup call, cpu: %d, spurious: %d\n", cpu, spurious);
}

/*
                  
        
                     
 */
int mt_cpu_disable(unsigned int cpu)
{
    /*
                                                                  
                                 
    */
    HOTPLUG_INFO("mt_cpu_disable, cpu: %d\n", cpu);
    return cpu == 0 ? -EPERM : 0;
}
