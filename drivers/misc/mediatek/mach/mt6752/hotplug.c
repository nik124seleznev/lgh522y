#include <linux/errno.h>    //     
#include <asm/cacheflush.h> //               
#include <mach/hotplug.h>
#ifdef CONFIG_HOTPLUG_WITH_POWER_CTRL
#include <mach/mt_spm_mtcmos.h>
#endif
#include <mach/mt_spm_idle.h>
#include <mach/wd_api.h>
#include <mach/mt_secure_api.h>


/* 
                   
 */
extern void __disable_dcache(void);         //                           
extern void __enable_dcache(void);          //                           
extern void __inner_clean_dcache_L2(void);  //                           
extern void inner_dcache_flush_L1(void);    //                           
extern void inner_dcache_flush_L2(void);    //                           
extern void __switch_to_smp(void);          //                          
extern void __switch_to_amp(void);          //                          
extern void __disable_dcache__inner_flush_dcache_L1(void);                          //                           
extern void __disable_dcache__inner_flush_dcache_L1__inner_clean_dcache_L2(void);   //                           
extern void __disable_dcache__inner_flush_dcache_L1__inner_flush_dcache_L2(void);   //                           



/* 
                   
 */
atomic_t hotplug_cpu_count = ATOMIC_INIT(1);



/*
                  
 */
static inline void cpu_enter_lowpower(unsigned int cpu)
{
    //                                     

    if (((cpu == 4) && (cpu_online(5) == 0) && (cpu_online(6) == 0) && (cpu_online(7) == 0)) ||
        ((cpu == 5) && (cpu_online(4) == 0) && (cpu_online(6) == 0) && (cpu_online(7) == 0)) ||
        ((cpu == 6) && (cpu_online(4) == 0) && (cpu_online(5) == 0) && (cpu_online(7) == 0)) ||
        ((cpu == 7) && (cpu_online(4) == 0) && (cpu_online(5) == 0) && (cpu_online(6) == 0)))
    {
    #if 0
        /*                                                                */
        __disable_dcache();

        /*                                                         */
        inner_dcache_flush_L1();
        //                  

        /*                             */
        __asm__ __volatile__("clrex");

        /*                                       */
        inner_dcache_flush_L2();
    #else
        __disable_dcache__inner_flush_dcache_L1__inner_flush_dcache_L2();
    #endif

        /*                                                                              */
        __switch_to_amp();

        /*                                                                                                                        */
        isb();

        /*                                                                                                                                                                                                        */
        dsb();

        /*                                                 */
        REG_WRITE(CCI400_SI3_SNOOP_CONTROL, REG_READ(CCI400_SI3_SNOOP_CONTROL) & ~(SNOOP_REQ | DVM_MSG_REQ));
        while (REG_READ(CCI400_STATUS) & CHANGE_PENDING);

        /*                              */
    #if defined(CONFIG_ARM_PSCI) || defined(CONFIG_MTK_PSCI)
        mcusys_smc_write_phy(virt_to_phys(MP1_AXI_CONFIG), REG_READ(MP1_AXI_CONFIG) | ACINACTM);
    #else //                                                        
        mcusys_smc_write(MP1_AXI_CONFIG, REG_READ(MP1_AXI_CONFIG) | ACINACTM);
    #endif //                                                        
    }
    else
    {
    #if 0
        /*                                                                */
        __disable_dcache();

        /*                                                      */
        inner_dcache_flush_L1();
        //                     
        //                  

        /*                                       */
        //                          
    #else
        //                                                                       
        //                                          
        __disable_dcache__inner_flush_dcache_L1__inner_clean_dcache_L2();
    #endif

        /*                             */
        __asm__ __volatile__("clrex");

        /*                                                                              */
        __switch_to_amp();
    }
}

static inline void cpu_leave_lowpower(unsigned int cpu)
{
    //                                     

    if (((cpu == 4) && (cpu_online(5) == 0) && (cpu_online(6) == 0) && (cpu_online(7) == 0)) ||
        ((cpu == 5) && (cpu_online(4) == 0) && (cpu_online(6) == 0) && (cpu_online(7) == 0)) ||
        ((cpu == 6) && (cpu_online(4) == 0) && (cpu_online(5) == 0) && (cpu_online(7) == 0)) ||
        ((cpu == 7) && (cpu_online(4) == 0) && (cpu_online(5) == 0) && (cpu_online(6) == 0)))
    {
        /*                             */
    #if defined(CONFIG_ARM_PSCI) || defined(CONFIG_MTK_PSCI)
        mcusys_smc_write_phy(virt_to_phys(MP1_AXI_CONFIG), REG_READ(MP1_AXI_CONFIG) & ~ACINACTM);
    #else //                                                        
        mcusys_smc_write(MP1_AXI_CONFIG, REG_READ(MP1_AXI_CONFIG) & ~ACINACTM);
    #endif //                                                        

        /*                                                */
        REG_WRITE(CCI400_SI3_SNOOP_CONTROL, REG_READ(CCI400_SI3_SNOOP_CONTROL) | (SNOOP_REQ | DVM_MSG_REQ));
        while (REG_READ(CCI400_STATUS) & CHANGE_PENDING);
    }

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
        case 4:
            spm_mtcmos_ctrl_cpu4(STA_POWER_DOWN, 1);
            break;
        case 5:
            spm_mtcmos_ctrl_cpu5(STA_POWER_DOWN, 1);
            break;
        case 6:
            spm_mtcmos_ctrl_cpu6(STA_POWER_DOWN, 1);
            break;
        case 7:
            spm_mtcmos_ctrl_cpu7(STA_POWER_DOWN, 1);
            break;
        default:
            break;
    }
#endif    
    atomic_dec(&hotplug_cpu_count);

    #if 0
        pr_emerg("SPM_CA7_CPU0_PWR_CON: 0x%08x\n", REG_READ(SPM_CA7_CPU0_PWR_CON));
        pr_emerg("SPM_CA7_CPU1_PWR_CON: 0x%08x\n", REG_READ(SPM_CA7_CPU1_PWR_CON));
        pr_emerg("SPM_CA7_CPU2_PWR_CON: 0x%08x\n", REG_READ(SPM_CA7_CPU2_PWR_CON));
        pr_emerg("SPM_CA7_CPU3_PWR_CON: 0x%08x\n", REG_READ(SPM_CA7_CPU3_PWR_CON));
        pr_emerg("SPM_CA7_DBG_PWR_CON: 0x%08x\n", REG_READ(SPM_CA7_DBG_PWR_CON));
        pr_emerg("SPM_CA7_CPUTOP_PWR_CON: 0x%08x\n", REG_READ(SPM_CA7_CPUTOP_PWR_CON));
        pr_emerg("SPM_CA15_CPU0_PWR_CON: 0x%08x\n", REG_READ(SPM_CA15_CPU0_PWR_CON));
        pr_emerg("SPM_CA15_CPU1_PWR_CON: 0x%08x\n", REG_READ(SPM_CA15_CPU1_PWR_CON));
        pr_emerg("SPM_CA15_CPU2_PWR_CON: 0x%08x\n", REG_READ(SPM_CA15_CPU2_PWR_CON));
        pr_emerg("SPM_CA15_CPU3_PWR_CON: 0x%08x\n", REG_READ(SPM_CA15_CPU3_PWR_CON));
        pr_emerg("SPM_CA15_CPUTOP_PWR_CON: 0x%08x\n", REG_READ(SPM_CA15_CPUTOP_PWR_CON));
    #endif

    return 1;
}

/*
                             
        
 */
void mt_cpu_die(unsigned int cpu)
{
    int spurious = 0;
#if 0
    //                   
    struct wd_api *wd_api = NULL;

    get_wd_api(&wd_api);
    if (wd_api)
        wd_api->wd_cpu_hot_plug_off_notify(cpu);
#endif

    HOTPLUG_INFO("mt_cpu_die, cpu: %d\n", cpu);

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
