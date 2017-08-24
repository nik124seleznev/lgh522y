#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <generated/autoconf.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/processor.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <mach/mt_reg_base.h>
#include <mach/mt_pm_ldo.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#include <mach/power_loss_test.h>
#include <mach/power_loss_emmc_test.h>
#include <mach/board.h>
#include <mt_sd.h>

extern struct msdc_host *mtk_msdc_host[];

#if defined(PWR_LOSS_SPOH) && defined(CONFIG_MTK_EMMC_SUPPORT)
#define TAG                 "[MVG_TEST]:"
/*
                                                         
                                                        
*/
//                                       

static struct hrtimer mvg_emmc_hrtimer;
static struct timespec t_hr_start,t_hr_end1;
static unsigned int hrtimer_delay_ns=0;
static unsigned int hrtimer_len=0;
static unsigned int hrtimer_addr=0;
static volatile unsigned int mvg_emmc_reset_planned=0;
static volatile unsigned int mvg_emmc_cancel_reset=0;
static volatile unsigned int mvg_emmc_reset_timeout=0;
static unsigned int mvg_reset_scheduled=0;


extern unsigned int upmu_get_qi_vemc_3v3_en(void); //                                                    
extern void wdt_arch_reset(char mode);

struct mvg_spoh_emmc_priv_type mvg_spoh_emmc_priv ={
    .reset_time_divisor = 1,
    .emmc_reset_mode = MVG_EMMC_RESET_WO_POWEROFF,
    .emmc_reset_time_mode = MVG_EMMC_RESET_LEN_DEPEND,
    .erase_set = 0
};

static unsigned int w_dealy_max_tbl[MVG_EMMC_PERFTABLE_SIZE];

void mvg_emmc_nanodelay(u32 delay_ns)
{
    //                               
    get_monotonic_boottime(&t_hr_start);

    do {
        get_monotonic_boottime(&t_hr_end1);
        if ( t_hr_end1.tv_nsec< t_hr_start.tv_nsec ) {
            t_hr_end1.tv_nsec=1000000000+t_hr_end1.tv_nsec-t_hr_start.tv_nsec;
        } else {
            t_hr_end1.tv_nsec=t_hr_end1.tv_nsec-t_hr_start.tv_nsec;
        }
        if ( t_hr_end1.tv_nsec>delay_ns ) break;
    } while(1);
}

//                              
void mvg_emmc_check_busy_and_reset(int delay_ms, int delay_us, u64 addr, u32 len)
{
    struct msdc_host *host=mtk_msdc_host[0];
    volatile unsigned int *reg1=(unsigned int *)(host->base+OFFSET_SDC_STS);
    //                                                                           
    static struct timespec t_start,t_end1;

    //                                                    
    get_monotonic_boottime(&t_start);
    if ( delay_ms ) mdelay(delay_ms);
    if ( delay_us ) udelay(delay_us);
    get_monotonic_boottime(&t_end1);
    if ( (*reg1&SDC_STS_SDCBUSY) || (len==0) ) { //               
        //                                                                                 
        if ( (mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_RESET_W_POWEROFF) ||
             (mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_RESET_WO_POWEROFF) ) {

            if ( t_end1.tv_nsec< t_start.tv_nsec ) {
                t_end1.tv_nsec=1000000000+t_end1.tv_nsec-t_start.tv_nsec;
            } else {
                t_end1.tv_nsec=t_end1.tv_nsec-t_start.tv_nsec;
            }

            if ( mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_RESET_W_POWEROFF ) {
                hwPowerDown(MT6325_POWER_LDO_VEMC33, "msdc");
                //                                                                                                   

                #if 0
                if ( mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_RESET_W_POWEROFF ) {
                    while (upmu_get_qi_vemc_3v3_en());
                }
                #else
                mdelay(12); //                                                                    
                #endif
            }

            //                                                              
            //                                                                            
            if ( mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_RESET_W_POWEROFF ) {
                //                                                                                                                                                            
            } else {
                //                                                                                                                                                               
            }

            mvg_wdt_reset();

        } else if ( mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_NO_RESET ) {
            //                                                                       
            if ( t_end1.tv_nsec< t_start.tv_nsec ) {
                t_end1.tv_nsec=1000000000+t_end1.tv_nsec-t_start.tv_nsec;
            } else {
                t_end1.tv_nsec=t_end1.tv_nsec-t_start.tv_nsec;
            }
            printk(KERN_ERR TAG " not reset after %d us, test len %d , addr %x, plan %u us\n", ((int)t_end1.tv_nsec)/1000, len, (unsigned int)addr, delay_ms*1000+delay_us);
            //                                           
            mvg_spoh_emmc_priv.reset_time_divisor=1;
            return;
        }

    } else {
        printk(KERN_ERR TAG " busy end before %d us, test len %u\n", delay_ms*1000+delay_us, mvg_spoh_emmc_priv.match_len);
        #if 0
        mvg_emmc_reset_planned=0;
        #else
        mvg_emmc_reset_timeout=1;
        mvg_emmc_reset_planned--;
        #endif
        mvg_spoh_emmc_priv.reset_delay_result=1;
        //                                                                                                       
        mvg_spoh_emmc_priv.reset_time_divisor=mvg_spoh_emmc_priv.reset_time_divisor<<1;
    }
}

int mvg_emmc_match(void *hostv, u64 addr, u32 opcode, u32 len)
{
    struct msdc_host *host=(struct msdc_host *)hostv;
    u32 time_max=0;
    u32 timer_inc=0, erase_address_matched=0, timer_inc2=0;

    if ( host->hw->host_function!= MSDC_EMMC ) return 0;

    if ( mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_EXTERNAL_RANDOM_POWEROFF ) return 0;

    if ( mvg_emmc_reset_planned!=0 ) {
        mvg_emmc_reset_planned++;
    }
    if ( mvg_emmc_reset_planned==2 ) {
        //                                                                                                                           
        #if 0
        //                      
        mvg_emmc_cancel_reset=1;
        mvg_emmc_reset_planned=0;
        //                                                                                                                                          
        #else
        //                                                    
        while( mvg_emmc_reset_planned==2 && mvg_emmc_reset_timeout==0 ) {
            //                                                                                                                
        }
        #endif
        
    }

    if ( mvg_reset_scheduled==1 )  return 0;

    //                                                                                                                   
    if (opcode==35) {
        mvg_spoh_emmc_priv.match_len=0;
        mvg_spoh_emmc_priv.erase_start=addr;
        mvg_spoh_emmc_priv.erase_set=1;
        //                                                                                                                      
        return 0;

    } else if ( (opcode==36) && (mvg_spoh_emmc_priv.erase_set==1) ) {
        mvg_spoh_emmc_priv.erase_end=addr;
        mvg_spoh_emmc_priv.erase_set=2;
        mvg_spoh_emmc_priv.match_len=((unsigned int)(mvg_spoh_emmc_priv.erase_end-mvg_spoh_emmc_priv.erase_start)+1)<<9;
        //                                                                                                                   
        return 0;

    } else if ( (opcode==38 ) && (mvg_spoh_emmc_priv.erase_set==2) ) {
        if ( mvg_addr_range_check(mvg_spoh_emmc_priv.erase_start) && mvg_addr_range_check(mvg_spoh_emmc_priv.erase_end) ) {
            erase_address_matched=1;
            mvg_spoh_emmc_priv.erase_set=0;
            len=mvg_spoh_emmc_priv.match_len;
            //                                                  
        }
    }

    if ( (erase_address_matched) || (mvg_addr_range_check(addr)) ) {
        if ( mvg_trigger() && mvg_get_wdt() ) {
            mvg_spoh_emmc_priv.reset_delay_result=0;

            mvg_spoh_emmc_priv.match_len=len;
            get_random_bytes(&timer_inc, sizeof(u32));
            //                                                                                    
            //                                                                           
            if ( (mvg_spoh_emmc_priv.emmc_reset_time_mode==MVG_EMMC_RESET_LEN_DEPEND) || (erase_address_matched) ) {
                if ( erase_address_matched ) {
                    //                                                                                    
                    time_max=100000000;
                } else if ( len<= 16*512 ) {
                    //                                                                                    
                    time_max=w_dealy_max_tbl[4]; //      
                } else {
                    int i=5;
                    do {
                        if ( (((1<<i)*512)>=len) && (((1<<(i-1))*512)<len) ) {
                            time_max=w_dealy_max_tbl[i];
                            break;
                        }
                        i++;
                    } while (i<MVG_EMMC_PERFTABLE_SIZE);
                }
                if ( time_max==0 ) time_max=100000000;

                #if 0
                while ( mvg_spoh_emmc_priv.reset_time_divisor*50<time_max ) {
                    if ( mvg_spoh_emmc_priv.reset_time_divisor==1 ) break;
                    mvg_spoh_emmc_priv.reset_time_divisor=mvg_spoh_emmc_priv.reset_time_divisor>>1;
                }
                #else
                if ( mvg_spoh_emmc_priv.reset_time_divisor>time_max ) {
                    mvg_spoh_emmc_priv.reset_time_divisor=mvg_spoh_emmc_priv.reset_time_divisor>>2;
                }
                #endif
                time_max=time_max/mvg_spoh_emmc_priv.reset_time_divisor;
                if ( mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_RESET_W_POWEROFF )
                    time_max+=TIME_POWER_DROPING;

                timer_inc=(timer_inc%time_max);
                
                if ( timer_inc==0 ) timer_inc=1;
                //                                                                                                             

            } else  {
                //                             
                timer_inc=mvg_spoh_emmc_priv.emmc_reset_time_mode;
            }

            printk(KERN_ERR TAG " may reset in %d ns, test len %d , addr %x\n", timer_inc, len, (unsigned int)addr);

            #if defined(GPIO_TRIGGER_HW_POWER_LOSS)
            if ( mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_RESET_W_POWEROFF ) {
                //                                               
                if ( timer_inc<=TIME_TRIGGER_POWER_DROP_BEFORE_CMD ) {

                    if ( TIME_POWER_DROPING>timer_inc )
                        printk(KERN_ERR TAG " -%u ns plan poweroff  before CMD\n", timer_inc);
                    else
                        printk(KERN_ERR TAG " %u ns plan poweroff  after CMD\n", timer_inc-TIME_POWER_DROPING);

                    //            
                    sdr_write32(0xF0014088,0x1); //                       
                    sdr_write32(0xF0005134,0x400000); //                       
                    sdr_write32(0xF0005034,0x400000); //                          

                    if ( TIME_POWER_DROPING>timer_inc )
                        mvg_emmc_nanodelay(TIME_FROM_GPIO_TRIGGER_TO_POWER_DROP+timer_inc);
                    else
                        mvg_emmc_nanodelay(TIME_TRIGGER_POWER_DROP_BEFORE_CMD-timer_inc);

                    //                                                                                                    

                } else {
                    mvg_emmc_hrtimer_start(timer_inc-TIME_TRIGGER_POWER_DROP_BEFORE_CMD, addr, len);
                    timer_inc-=TIME_POWER_DROPING;
                }
            }
            #endif
            
            #if defined(GPIO_TRIGGER_HW_POWER_LOSS) || defined(SW_RESET_CONTROLLED_BY_TIMER)
            if ( mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_RESET_WO_POWEROFF ) {
                //                                                                                             
                mvg_emmc_hrtimer_start(timer_inc, addr, (opcode!=38)?len:0);
            }
            #endif

            mvg_emmc_reset_planned=1;
            return timer_inc;
        }
    }

    return 0;

}

int mvg_emmc_get_reset_time_divisor(void *power_loss_info)
{
    wdt_reboot_info *l_power_loss_info=(wdt_reboot_info *)power_loss_info;
    struct mvg_spoh_emmc_priv_type *priv=(struct mvg_spoh_emmc_priv_type *)l_power_loss_info->drv_priv;
    int ret;

    down_write(&(l_power_loss_info->rwsem));
    ret=priv->reset_time_divisor;
    up_write(&(l_power_loss_info->rwsem));
    return ret;
}

void mvg_emmc_set_reset_time_divisor(void *power_loss_info, int divisor)
{
    wdt_reboot_info *l_power_loss_info=(wdt_reboot_info *)power_loss_info;
    struct mvg_spoh_emmc_priv_type *priv=(struct mvg_spoh_emmc_priv_type *)l_power_loss_info->drv_priv;

    down_write(&(l_power_loss_info->rwsem));
    priv->reset_time_divisor=divisor;
    up_write(&(l_power_loss_info->rwsem));
}

void mvg_emmc_reset_mode_set(void *power_loss_info, int mode)
{
    wdt_reboot_info *l_power_loss_info=(wdt_reboot_info *)power_loss_info;
    struct mvg_spoh_emmc_priv_type *priv=(struct mvg_spoh_emmc_priv_type *)l_power_loss_info->drv_priv;

    down_write(&(l_power_loss_info->rwsem));
    if ( mode==MVG_EMMC_NO_RESET )
        priv->emmc_reset_mode=MVG_EMMC_NO_RESET;
    else if ( mode==MVG_EMMC_EXTERNAL_RANDOM_POWEROFF )
        priv->emmc_reset_mode=MVG_EMMC_EXTERNAL_RANDOM_POWEROFF;
    else if ( mode!=MVG_EMMC_RESET_WO_POWEROFF )
        priv->emmc_reset_mode=MVG_EMMC_RESET_W_POWEROFF;
    else
        priv->emmc_reset_mode=MVG_EMMC_RESET_WO_POWEROFF;
    up_write(&(l_power_loss_info->rwsem));
}

void mvg_emmc_reset_time_mode_set(void *power_loss_info, int mode)
{
    wdt_reboot_info *l_power_loss_info=(wdt_reboot_info *)power_loss_info;
    struct mvg_spoh_emmc_priv_type *priv=(struct mvg_spoh_emmc_priv_type *)l_power_loss_info->drv_priv;

    down_write(&(l_power_loss_info->rwsem));
    if ( mode>=1 ) //                          
        priv->emmc_reset_time_mode=mode*1000;
    else
        priv->emmc_reset_time_mode=mode;
    up_write(&(l_power_loss_info->rwsem));
}

void mvg_emmc_set_erase_group_sector(void *power_loss_info, int sector)
{
    wdt_reboot_info *l_power_loss_info=(wdt_reboot_info *)power_loss_info;
    struct mvg_spoh_emmc_priv_type *priv=(struct mvg_spoh_emmc_priv_type *)l_power_loss_info->drv_priv;
    int i=0;

    down_write(&(l_power_loss_info->rwsem));
    if ( sector>0 )
        priv->emmc_erase_group_sector=sector; //                       
    else
        priv->emmc_erase_group_sector=(1<<10); //                              

    do {
        if ( (1<<i)==priv->emmc_erase_group_sector ) {
            priv->emmc_erase_group_sector_perf_table_idx=i;
            break;
        }
        i++;
    } while(i<MVG_EMMC_PERFTABLE_SIZE);

    up_write(&(l_power_loss_info->rwsem));
}

int mvg_emmc_get_delay_result(void *power_loss_info)
{
    wdt_reboot_info *l_power_loss_info=(wdt_reboot_info *)power_loss_info;
    struct mvg_spoh_emmc_priv_type *priv=(struct mvg_spoh_emmc_priv_type *)l_power_loss_info->drv_priv;
    int ret;

    down_write(&(l_power_loss_info->rwsem));
    ret=priv->reset_delay_result;
    //                                                 
    up_write(&(l_power_loss_info->rwsem));
    return ret;
}

int mvg_emmc_get_set_delay_table(void *power_loss_info, unsigned char *tbl)
{
    wdt_reboot_info *l_power_loss_info=(wdt_reboot_info *)power_loss_info;
    int ret;
    int i;

    down_write(&(l_power_loss_info->rwsem));
    ret  = copy_from_user(w_dealy_max_tbl, tbl, sizeof(w_dealy_max_tbl));

    for(i=0;i<MVG_EMMC_PERFTABLE_SIZE ;i++) {
        w_dealy_max_tbl[i]*=1000;
    }

    up_write(&(l_power_loss_info->rwsem));

    return ret;
}

enum hrtimer_restart mvg_emmc_hrtimer_check_busy_and_reset(struct hrtimer *timer)
{
    struct msdc_host *host=mtk_msdc_host[0];
    volatile unsigned int *reg1=(unsigned int *)(host->base+OFFSET_SDC_STS);
    //                                                                           
    static struct timespec t_start,t_end1;

    #if 0
    if ( mvg_emmc_cancel_reset ) {
        if ( mvg_reset_scheduled ) mvg_reset_scheduled=0;
        mvg_emmc_cancel_reset=0;
        goto out;
    }
    #endif

    if ( mvg_reset_scheduled ) {
        if ( *reg1&SDC_STS_SDCBUSY ) {
            printk(KERN_ERR TAG " busy\n");
            //                      
            hrtimer_start(&mvg_emmc_hrtimer, ktime_set(0, 1000000), HRTIMER_MODE_REL);
        } else{
            //                                   
            mvg_reset_scheduled=0;
        }

        goto out;
    }

    //                                                                        
    if ( (*reg1&SDC_STS_SDCBUSY) ) { //                 
        //                                                                                 
        //                                                          
        get_monotonic_boottime(&t_hr_end1);
        if ( t_hr_end1.tv_nsec< t_hr_start.tv_nsec ) {
            t_hr_end1.tv_nsec=1000000000+t_hr_end1.tv_nsec-t_hr_start.tv_nsec;
        } else {
            t_hr_end1.tv_nsec=t_hr_end1.tv_nsec-t_hr_start.tv_nsec;
        }
        
        if ( mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_RESET_W_POWEROFF ) {
            t_hr_end1.tv_nsec+=TIME_FROM_GPIO_TRIGGER_TO_POWER_DROP;
            
            printk(KERN_ERR TAG " %d ns reset with poweroff, test len %d, addr %x, plan %u ns\n", (int)t_hr_end1.tv_nsec, hrtimer_len, (unsigned int)hrtimer_addr, hrtimer_delay_ns);
            //            
            sdr_write32(0xF0014088,0x1); //                       
            sdr_write32(0xF0005134,0x400000); //                       
            sdr_write32(0xF0005034,0x400000); //                          

            mvg_reset_scheduled=1;
            if ( hrtimer_len!=0 ) { //                                               
                //                      
                hrtimer_start(&mvg_emmc_hrtimer, ktime_set(0, 1000000), HRTIMER_MODE_REL);
            }

            goto out;

        } else if ( mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_RESET_WO_POWEROFF ) {
            //                                                              
            //                                                                            
            //                                                                                                                                                                             
            mvg_wdt_reset();

        } else if ( mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_NO_RESET ) {
            //                                                                       
            printk(KERN_ERR TAG " not reset after %d ns, test len %d , addr %x, plan %u ns\n", (int)t_hr_end1.tv_nsec, hrtimer_len, (unsigned int)hrtimer_addr, hrtimer_delay_ns);
            //                                           
            mvg_spoh_emmc_priv.reset_time_divisor=1;
            goto out;
        }

    } else {
        get_monotonic_boottime(&t_hr_end1);
        if ( t_hr_end1.tv_nsec< t_hr_start.tv_nsec ) {
            t_hr_end1.tv_nsec=1000000000+t_hr_end1.tv_nsec-t_hr_start.tv_nsec;
        } else {
            t_hr_end1.tv_nsec=t_hr_end1.tv_nsec-t_hr_start.tv_nsec;
        }
        printk(KERN_ERR TAG " busy end after %d ns, test len %d , addr %x, plan %u ns\n", (int)t_hr_end1.tv_nsec, hrtimer_len, (unsigned int)hrtimer_addr, hrtimer_delay_ns);
        #if 0
        mvg_emmc_reset_planned=0;
        #else
        mvg_emmc_reset_timeout=1;
        mvg_emmc_reset_planned--;
        #endif
        mvg_spoh_emmc_priv.reset_delay_result=1;
        //                                                                                                       
        mvg_spoh_emmc_priv.reset_time_divisor=mvg_spoh_emmc_priv.reset_time_divisor<<1;
    }

out:
    return HRTIMER_NORESTART;
}

void mvg_emmc_hrtimer_start(int delay_ns, u64 addr, u32 len)
{
    ktime_t ktime_delay;

    //                             
        mvg_emmc_cancel_reset=0;
        hrtimer_delay_ns=delay_ns;
        hrtimer_len=len;
        hrtimer_addr=addr;
        ktime_delay=ktime_set(0, hrtimer_delay_ns);
        if ( mvg_spoh_emmc_priv.emmc_reset_mode==MVG_EMMC_RESET_W_POWEROFF )
            hrtimer_delay_ns+=TIME_FROM_GPIO_TRIGGER_TO_POWER_DROP;
        get_monotonic_boottime(&t_hr_start);
        mvg_emmc_reset_timeout=0;
        hrtimer_start(&mvg_emmc_hrtimer, ktime_delay, HRTIMER_MODE_REL);
    // 
}

void mvg_emmc_hrtimer_init(unsigned long data)
{
    hrtimer_init(&mvg_emmc_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    mvg_emmc_hrtimer.function=mvg_emmc_hrtimer_check_busy_and_reset;
}

#endif


MODULE_AUTHOR("light.hsieh <light.hsieh@mediatek.com>");
MODULE_DESCRIPTION(" This module is for power loss test");

