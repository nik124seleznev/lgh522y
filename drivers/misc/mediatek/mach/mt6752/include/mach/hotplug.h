#ifndef _HOTPLUG
#define _HOTPLUG


#include <linux/xlog.h>
#include <linux/kernel.h>   //      
#include <linux/io.h>       //     
#include <asm/atomic.h>
#include <mach/mt_reg_base.h>
#include <mach/sync_write.h>


/*     */
#define HOTPLUG_LOG_NONE                                0
#define HOTPLUG_LOG_WITH_XLOG                           1
#define HOTPLUG_LOG_WITH_PRINTK                         2

#define HOTPLUG_LOG_PRINT                               HOTPLUG_LOG_WITH_PRINTK

#if (HOTPLUG_LOG_PRINT == HOTPLUG_LOG_NONE)
#define HOTPLUG_INFO(fmt, args...)                    
#elif (HOTPLUG_LOG_PRINT == HOTPLUG_LOG_WITH_XLOG)
#define HOTPLUG_INFO(fmt, args...)                      xlog_printk(ANDROID_LOG_INFO, "Power/hotplug", fmt, ##args)
#elif (HOTPLUG_LOG_PRINT == HOTPLUG_LOG_WITH_PRINTK)
#define HOTPLUG_INFO(fmt, args...)                      pr_warn("[Power/hotplug] "fmt, ##args)
//                                                                                               
#endif

#define STEP_BY_STEP_DEBUG                              pr_emerg("@@@### file:%s, func:%s, line:%d ###@@@\n", __FILE__, __func__, __LINE__)


/*            */
//                                                        
#define CONFIG_HOTPLUG_PROFILING_COUNT                  100


/*                                 */
#define BOOTROM_BOOT_ADDR                               (INFRACFG_AO_BASE + 0x800)
#define BOOTROM_SEC_CTRL                                (INFRACFG_AO_BASE + 0x804)
#define SW_ROM_PD                                       (1U << 31)


/*                           */
#define CCI400_STATUS                                   (CCI400_BASE + 0x000C)
#define CHANGE_PENDING                                  (1U << 0)
//         
#define CCI400_SI4_BASE                                 (CCI400_BASE + 0x5000)
#define CCI400_SI4_SNOOP_CONTROL                        (CCI400_SI4_BASE)
//         
#define CCI400_SI3_BASE                                 (CCI400_BASE + 0x4000)
#define CCI400_SI3_SNOOP_CONTROL                        (CCI400_SI3_BASE)
#define DVM_MSG_REQ                                     (1U << 1)
#define SNOOP_REQ                                       (1U << 0)


/*                             */
//         
#define MP0_AXI_CONFIG                                  (MCUCFG_BASE + 0x02C)
//         
#define MP1_AXI_CONFIG                                  (MCUCFG_BASE + 0x22C)
#define ACINACTM                                        (1U << 4)


/*                              */
//         
#define MP0_MISC_CONFIG3                                (MCUCFG_BASE + 0x03C)
//         
#define MP1_MISC_CONFIG3                                (MCUCFG_BASE + 0x23C)


/*                                  */
#if 0
//         
#define DBG_MON_CTL                                     (CA7MCUCFG_BASE + 0x40)
#define DBG_MON_DATA                                    (CA7MCUCFG_BASE + 0x44)
//         
#define CA15L_MON_SEL                                   (CA15L_CONFIG_BASE + 0x1C)
#define CA15L_MON                                       (CA15L_CONFIG_BASE + 0x5C)
#endif


/*                     */
//                                                                                 
#define REG_READ(addr)                                  __raw_readl(IOMEM(addr))
#define REG_WRITE(addr, value)                          mt_reg_sync_writel(value, addr)


/*                 */
//                    
#define CONFIG_HOTPLUG_WITH_POWER_CTRL


/*                 */
extern atomic_t hotplug_cpu_count;


/*                                            */
extern int mt_cpu_kill(unsigned int cpu);
extern void mt_cpu_die(unsigned int cpu);
extern int mt_cpu_disable(unsigned int cpu);


#endif //                       
