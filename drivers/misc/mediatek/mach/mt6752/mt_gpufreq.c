/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/earlysuspend.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/xlog.h>
#include <linux/jiffies.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/input.h>
#include <linux/sched.h>
#include <linux/sched/rt.h>
#include <linux/kthread.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_address.h>
#endif

//                       
#include <asm/uaccess.h>

#include "mach/mt_typedefs.h"
#include "mach/mt_clkmgr.h"
#include "mach/mt_cpufreq.h"
#include "mach/mt_gpufreq.h"
#include "mach/upmu_common.h"
#include "mach/sync_write.h"
#include "mach/mt_pmic_wrap.h"

#include "mach/mt_freqhopping.h"
//                  
//                                 
#include "mach/mt_thermal.h"

#include "mach/upmu_sw.h"


/*
         
 */
/*                                                 
                                     
                                                   */
#define MT_GPUFREQ_LOW_BATT_VOLT_PROTECT

/*                                                 
                                    
                                                   */
#define MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT

/*                                                 
                    
                                                   */
//                             

/*                                                 
                               
                                                   */
#define MT_GPUFREQ_INPUT_BOOST

/*                          
                                        
                            */
#define MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE

/*                          
                         
                            */
//                               

/*                          
                            
                            */
#ifdef CONFIG_MTK_RAM_CONSOLE
#define MT_GPUFREQ_AEE_RR_REC
#endif

/*                                                 
                                 
                                                   */
#define mt_gpufreq_reg_write(val, addr)        mt_reg_sync_writel((val), ((void *)addr))

#define OPPS_ARRAY_AND_SIZE(x)	(x), ARRAY_SIZE(x)

/*                          
                           
                            */
#define GPUOP(khz, volt, idx)       \
{                           \
    .gpufreq_khz = khz,     \
    .gpufreq_volt = volt,   \
    .gpufreq_idx = idx,   \
}

/*                         
                             
                           */

#ifdef MTK_TABLET_TURBO
#define GPU_DVFS_FREQT (819000)
#endif

#define GPU_DVFS_FREQ0     (728000)   //    
#define GPU_DVFS_FREQ1     (650000)   //    
#define GPU_DVFS_FREQ2     (598000)   //    
#define GPU_DVFS_FREQ3     (520000)   //    
#define GPU_DVFS_FREQ4     (494000)   //    
#define GPU_DVFS_FREQ5     (416000)   //    
#define GPU_DVFS_FREQ6     (312000)   //    
#define GPUFREQ_LAST_FREQ_LEVEL    (GPU_DVFS_FREQ6)

#define GPU_DVFS_VOLT0     (112500)  //         
#define GPU_DVFS_VOLT1     (100000)  //         
#define GPU_DVFS_VOLT2     ( 93125)  //         

#define GPU_DVFS_CTRL_VOLT     (2)  

#define GPU_DVFS_PTPOD_DISABLE_VOLT    GPU_DVFS_VOLT1 

/*                                        
                                               
                                          */
#define PMIC_CMD_DELAY_TIME     5
#define MIN_PMIC_SETTLE_TIME    25
#define PMIC_VOLT_UP_SETTLE_TIME(old_volt, new_volt)    (((((new_volt) - (old_volt)) + 1250 - 1) / 1250) + PMIC_CMD_DELAY_TIME)
#define PMIC_VOLT_DOWN_SETTLE_TIME(old_volt, new_volt)    (((((old_volt) - (new_volt)) * 2)  / 625) + PMIC_CMD_DELAY_TIME)
//                                            

#define PMIC_ADDR_VGPU_VOSEL_ON       0x0618  /*         */
#define PMIC_ADDR_VGPU_VOSEL_CTRL     0x060E  /*         */
#define PMIC_ADDR_VGPU_EN             0x0612  /*         */
#define PMIC_ADDR_VGPU_EN_CTRL        0x060E  /*         */

/*       */
#define GPUFREQ_EFUSE_INDEX     (4)

/*
      
 */
//                   

#define HEX_FMT "0x%08x"
#undef TAG

#ifdef USING_XLOG
#include <linux/xlog.h>

#define TAG     "Power/gpufreq"

#define gpufreq_err(fmt, args...)       \
    xlog_printk(ANDROID_LOG_ERROR, TAG, "[ERROR]"fmt, ##args)
#define gpufreq_warn(fmt, args...)      \
    xlog_printk(ANDROID_LOG_WARN, TAG, "[WARNING]"fmt, ##args)
#define gpufreq_info(fmt, args...)      \
    xlog_printk(ANDROID_LOG_INFO, TAG, fmt, ##args)
#define gpufreq_dbg(fmt, args...)       \
    do {                                \
        if (mt_gpufreq_debug)           \
            xlog_printk(ANDROID_LOG_DEBUG, TAG, fmt, ##args);    \
    } while (0)
#define gpufreq_ver(fmt, args...)       \
    do {                                \
        if (mt_gpufreq_debug)           \
            xlog_printk(ANDROID_LOG_VERBOSE, TAG, fmt, ##args);  \
    } while (0)   
    
#else   /*            */

#define TAG     "[Power/gpufreq] "

#define gpufreq_err(fmt, args...)       \
    printk(KERN_ERR TAG KERN_CONT "[ERROR]"fmt, ##args)
#define gpufreq_warn(fmt, args...)      \
    printk(KERN_WARNING TAG KERN_CONT "[WARNING]"fmt, ##args)
#define gpufreq_info(fmt, args...)      \
    printk(KERN_NOTICE TAG KERN_CONT fmt, ##args)
#define gpufreq_dbg(fmt, args...)       \
    do {                                \
        if (mt_gpufreq_debug)           \
            printk(KERN_INFO TAG KERN_CONT fmt, ##args);     \
    } while (0)  
#define gpufreq_ver(fmt, args...)       \
    do {                                \
        if (mt_gpufreq_debug)           \
            printk(KERN_DEBUG TAG KERN_CONT fmt, ##args);    \
    } while (0)     

#endif  /*            */


#ifdef CONFIG_HAS_EARLYSUSPEND
static struct early_suspend mt_gpufreq_early_suspend_handler =
{
    .level = EARLY_SUSPEND_LEVEL_DISABLE_FB + 200,
    .suspend = NULL,
    .resume  = NULL,
};
#endif

static sampler_func g_pFreqSampler = NULL;
static sampler_func g_pVoltSampler = NULL;

static gpufreq_power_limit_notify g_pGpufreq_power_limit_notify = NULL;
#ifdef MT_GPUFREQ_INPUT_BOOST
static gpufreq_input_boost_notify g_pGpufreq_input_boost_notify = NULL;
#endif


/*                          
                        
                            */
//     
static struct mt_gpufreq_table_info mt_gpufreq_opp_tbl_e1_0[] = {
    GPUOP(GPU_DVFS_FREQ0, GPU_DVFS_VOLT0, 0),
    GPUOP(GPU_DVFS_FREQ1, GPU_DVFS_VOLT0, 1),
    GPUOP(GPU_DVFS_FREQ2, GPU_DVFS_VOLT0, 2),
    GPUOP(GPU_DVFS_FREQ3, GPU_DVFS_VOLT1, 3),
    GPUOP(GPU_DVFS_FREQ5, GPU_DVFS_VOLT1, 4),
    GPUOP(GPU_DVFS_FREQ6, GPU_DVFS_VOLT2, 5),
};

//     
static struct mt_gpufreq_table_info mt_gpufreq_opp_tbl_e1_1[] = {
    GPUOP(GPU_DVFS_FREQ2, GPU_DVFS_VOLT0, 0),
    GPUOP(GPU_DVFS_FREQ3, GPU_DVFS_VOLT1, 1),
    GPUOP(GPU_DVFS_FREQ5, GPU_DVFS_VOLT1, 2),
    GPUOP(GPU_DVFS_FREQ6, GPU_DVFS_VOLT2, 3),
};

//     
static struct mt_gpufreq_table_info mt_gpufreq_opp_tbl_e1_2[] = {
    GPUOP(GPU_DVFS_FREQ4, GPU_DVFS_VOLT1, 0),
    GPUOP(GPU_DVFS_FREQ5, GPU_DVFS_VOLT1, 1),
    GPUOP(GPU_DVFS_FREQ6, GPU_DVFS_VOLT2, 2),
};

#ifdef MTK_TABLET_TURBO
//           
static struct mt_gpufreq_table_info mt_gpufreq_opp_tbl_e1_t[] = {
    GPUOP(GPU_DVFS_FREQT, GPU_DVFS_VOLT0, 0),
    GPUOP(GPU_DVFS_FREQ0, GPU_DVFS_VOLT0, 1),
    GPUOP(GPU_DVFS_FREQ2, GPU_DVFS_VOLT0, 2),
    GPUOP(GPU_DVFS_FREQ3, GPU_DVFS_VOLT1, 3),
    GPUOP(GPU_DVFS_FREQ5, GPU_DVFS_VOLT1, 4),
    GPUOP(GPU_DVFS_FREQ6, GPU_DVFS_VOLT2, 5),
};
#endif

/*
                   
 */
#ifdef MT_GPUFREQ_AEE_RR_REC
enum gpu_dvfs_state
{
    GPU_DVFS_IS_DOING_DVFS = 0,
    GPU_DVFS_IS_VGPU_ENABLED,       
};

extern void aee_rr_rec_gpu_dvfs_vgpu(u8 val);
extern void aee_rr_rec_gpu_dvfs_oppidx(u8 val);
extern void aee_rr_rec_gpu_dvfs_status(u8 val);
extern u8 aee_rr_curr_gpu_dvfs_status(void);

static void _mt_gpufreq_aee_init(void)
{
    aee_rr_rec_gpu_dvfs_vgpu(0xFF);
    aee_rr_rec_gpu_dvfs_oppidx(0xFF);
    aee_rr_rec_gpu_dvfs_status(0xFF);
}
#endif

/*                         
                        
                           */
static int g_gpufreq_dvfs_disable_count = 0;

static unsigned int g_cur_gpu_freq = 455000;    //              
static unsigned int g_cur_gpu_volt = 100000;    //              
static unsigned int g_cur_gpu_idx = 0xFF;
static unsigned int g_cur_gpu_OPPidx = 0xFF;

static unsigned int g_cur_freq_init_keep = 0;

static bool mt_gpufreq_ready = false;

/*                                                                                         */
static unsigned int g_gpufreq_max_id = 0;

/*                                                                   */
static unsigned int g_limited_max_id = 0;
static unsigned int g_limited_min_id;

static bool mt_gpufreq_debug = false;
static bool mt_gpufreq_pause = false;
static bool mt_gpufreq_keep_max_frequency_state = false;
static bool mt_gpufreq_keep_opp_frequency_state = false;
static unsigned int mt_gpufreq_keep_opp_frequency = 0;
static unsigned int mt_gpufreq_keep_opp_index = 0;
static bool mt_gpufreq_fixed_freq_volt_state = false;
static unsigned int mt_gpufreq_fixed_frequency = 0;
static unsigned int mt_gpufreq_fixed_voltage = 0;

static unsigned int mt_gpufreq_volt_enable = 0;
static unsigned int mt_gpufreq_volt_enable_state = 0;
#ifdef MT_GPUFREQ_INPUT_BOOST
static unsigned int mt_gpufreq_input_boost_state = 1;
#endif
//                                                 
static bool g_limited_thermal_ignore_state = false;
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
static bool g_limited_low_batt_volt_ignore_state = false;
#endif
#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
static bool g_limited_low_batt_volume_ignore_state = false;
#endif
#ifdef MT_GPUFREQ_OC_PROTECT
static bool g_limited_oc_ignore_state = false;
#endif

static bool mt_gpufreq_opp_max_frequency_state = false;
static unsigned int mt_gpufreq_opp_max_frequency = 0;
static unsigned int mt_gpufreq_opp_max_index = 0;

static unsigned int mt_gpufreq_dvfs_table_type = 0;

//                                        
static DEFINE_MUTEX(mt_gpufreq_lock);
static DEFINE_MUTEX(mt_gpufreq_power_lock);

static unsigned int mt_gpufreqs_num = 0;
static struct mt_gpufreq_table_info *mt_gpufreqs;
static struct mt_gpufreq_table_info *mt_gpufreqs_default;
static struct mt_gpufreq_power_table_info *mt_gpufreqs_power;
//                                                                     

static bool mt_gpufreq_ptpod_disable = false;

static int mt_gpufreq_ptpod_disable_idx = 0;

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_POLLING_TIMER
static int mt_gpufreq_low_batt_volume_timer_flag = 0;
static DECLARE_WAIT_QUEUE_HEAD(mt_gpufreq_low_batt_volume_timer_waiter);
static struct hrtimer mt_gpufreq_low_batt_volume_timer;
static int mt_gpufreq_low_batt_volume_period_s = 1;
static int mt_gpufreq_low_batt_volume_period_ns = 0;
struct task_struct *mt_gpufreq_low_batt_volume_thread = NULL;
#endif

static void mt_gpufreq_clock_switch(unsigned int freq_new);
static void mt_gpufreq_volt_switch(unsigned int volt_old, unsigned int volt_new);
static unsigned int mt_gpufreq_dvfs_get_gpu_freq(void);

/*                             
                              
                               */
extern int mtk_gpufreq_register(struct mt_gpufreq_power_table_info *freqs, int num);
extern unsigned int mt_get_mfgclk_freq(void);  //               
extern u32 get_devinfo_with_index(u32 index);


#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_POLLING_TIMER
enum hrtimer_restart mt_gpufreq_low_batt_volume_timer_func(struct hrtimer *timer)
{
    mt_gpufreq_low_batt_volume_timer_flag = 1; wake_up_interruptible(&mt_gpufreq_low_batt_volume_timer_waiter);
    return HRTIMER_NORESTART;
}

int mt_gpufreq_low_batt_volume_thread_handler(void *unused)
{
    do {
        ktime_t ktime = ktime_set(mt_gpufreq_low_batt_volume_period_s, mt_gpufreq_low_batt_volume_period_ns);

        wait_event_interruptible(mt_gpufreq_low_batt_volume_timer_waiter, mt_gpufreq_low_batt_volume_timer_flag != 0);
        mt_gpufreq_low_batt_volume_timer_flag = 0;

        gpufreq_dbg("@%s: begin\n", __func__);
        mt_gpufreq_low_batt_volume_check();

        hrtimer_start(&mt_gpufreq_low_batt_volume_timer, ktime, HRTIMER_MODE_REL);

    } while (!kthread_should_stop());

    return 0;
}

void mt_gpufreq_cancel_low_batt_volume_timer(void)
{
    hrtimer_cancel(&mt_gpufreq_low_batt_volume_timer);
}
EXPORT_SYMBOL(mt_gpufreq_cancel_low_batt_volume_timer);

void mt_gpufreq_start_low_batt_volume_timer(void)
{
    ktime_t ktime = ktime_set(mt_gpufreq_low_batt_volume_period_s, mt_gpufreq_low_batt_volume_period_ns);
    hrtimer_start(&mt_gpufreq_low_batt_volume_timer, ktime ,HRTIMER_MODE_REL);
}
EXPORT_SYMBOL(mt_gpufreq_start_low_batt_volume_timer);
#endif


/*                                                                                    
                       
                                                                                      */
static unsigned int mt_gpufreq_get_dvfs_table_type(void)
{
    unsigned int gpu_speed_bounding = 0;
    unsigned int type = 0;
#ifdef MTK_TABLET_TURBO
    unsigned int gpu_speed_turbo = 0;
    gpu_speed_turbo = get_devinfo_with_index(3) >> 31;
    gpufreq_info("GPU freq can be turbo %x?\n", gpu_speed_turbo);
    if(gpu_speed_turbo == 1) {
        return 3;
    }
#endif

    gpu_speed_bounding = (get_devinfo_with_index(GPUFREQ_EFUSE_INDEX) >> 20) & 0xF;
    gpufreq_info("GPU frequency bounding from efuse = %x\n", gpu_speed_bounding);

    //                                                                                        
    if (gpu_speed_bounding == 0) {
#ifdef CONFIG_OF
        static const struct of_device_id gpu_ids[] = {
        	{ .compatible = "arm,malit6xx" },
        	{ .compatible = "arm,mali-midgard" },
        	{ /*          */ }
        };
        struct device_node *node;
        unsigned int gpu_speed = 0;
        
        node = of_find_matching_node(NULL, gpu_ids);
        if (!node) {
            gpufreq_err("@%s: find GPU node failed\n", __func__);
            gpu_speed = 700;    //              
        } else {
            if (!of_property_read_u32(node, "clock-frequency", &gpu_speed)) {
                gpu_speed = gpu_speed / 1000 / 1000;    //    
            }
            else {
                gpufreq_err("@%s: missing clock-frequency property, use default GPU level\n", __func__);
                gpu_speed = 700;    //              
            }
        }
        gpufreq_info("GPU clock-frequency from DT = %d MHz\n", gpu_speed);

        if (gpu_speed >= 700)
            type = 0;   //     
        else if (gpu_speed >= 600)
            type = 1;   //     
        else
            type = 2;   //     
#else
        gpufreq_err("@%s: Cannot get GPU speed from DT!\n", __func__);
        type = 0;
#endif
        return type;
    }

    switch (gpu_speed_bounding) {
        case 1:
        case 2:
            type = 0;   //     
            break;
        case 3:
        case 4:
            type = 1;   //     
            break;            
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        default:            
            type = 2;   //     
            break;                         
    }
        
    return type;
}

#ifdef MT_GPUFREQ_INPUT_BOOST
static struct task_struct *mt_gpufreq_up_task;

static int mt_gpufreq_input_boost_task(void *data)
{
    while (1) {
        gpufreq_dbg("@%s: begin\n", __func__);

        if(NULL != g_pGpufreq_input_boost_notify) {
            gpufreq_dbg("@%s: g_pGpufreq_input_boost_notify\n", __func__);
            g_pGpufreq_input_boost_notify(g_gpufreq_max_id);
        }

        gpufreq_dbg("@%s: end\n", __func__);

        set_current_state(TASK_INTERRUPTIBLE);
        schedule();

        if (kthread_should_stop())
            break;
    }

    return 0;
}

static void mt_gpufreq_input_event(struct input_handle *handle, unsigned int type,
        unsigned int code, int value)
{
    if(mt_gpufreq_ready == false) {
        gpufreq_warn("@%s: GPU DVFS not ready!\n", __func__);
        return;
    }

    if ((type == EV_KEY) && (code == BTN_TOUCH) && (value == 1) && (mt_gpufreq_input_boost_state == 1)) {
        gpufreq_dbg("@%s: accept.\n", __func__);

        //                                                                                                                                 
        // 
        wake_up_process(mt_gpufreq_up_task);
        // 
    }
}

static int mt_gpufreq_input_connect(struct input_handler *handler,
        struct input_dev *dev, const struct input_device_id *id)
{
    struct input_handle *handle;
    int error;

    handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "gpufreq_ib";

    error = input_register_handle(handle);
    if (error)
        goto err2;

    error = input_open_device(handle);
    if (error)
        goto err1;

    return 0;
err1:
    input_unregister_handle(handle);
err2:
    kfree(handle);
    return error;
}

static void mt_gpufreq_input_disconnect(struct input_handle *handle)
{
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static const struct input_device_id mt_gpufreq_ids[] = {
    { .driver_info = 1 },
    { },
};

static struct input_handler mt_gpufreq_input_handler = {
    .event		= mt_gpufreq_input_event,
    .connect	= mt_gpufreq_input_connect,
    .disconnect	= mt_gpufreq_input_disconnect,
    .name		= "gpufreq_ib",
    .id_table	= mt_gpufreq_ids,
};
#endif

static void mt_gpufreq_power_calculation(unsigned int oppidx, unsigned int temp)
{
#define GPU_ACT_REF_POWER	    1596	/*     */
#define GPU_ACT_REF_FREQ	    728000 /*     */
#define GPU_ACT_REF_VOLT	    100000	/*          */

    unsigned int p_total = 0, p_dynamic = 0, p_leakage = 0, ref_freq = 0, ref_volt = 0;

    p_dynamic = GPU_ACT_REF_POWER;
    ref_freq  = GPU_ACT_REF_FREQ;
    ref_volt  = GPU_ACT_REF_VOLT;

    p_dynamic = p_dynamic *
        ((mt_gpufreqs[oppidx].gpufreq_khz * 100) / ref_freq) *
        ((mt_gpufreqs[oppidx].gpufreq_volt * 100) / ref_volt) *
        ((mt_gpufreqs[oppidx].gpufreq_volt * 100) / ref_volt) /
        (100 * 100 * 100);

    //                                                        
#if 0
    p_leakage = mt_spower_get_leakage(MT_SPOWER_GPU, (mt_gpufreqs[oppidx].gpufreq_volt / 100), temp);
#else
    p_leakage = 130;
#endif

    p_total = p_dynamic + p_leakage;

    mt_gpufreqs_power[oppidx].gpufreq_khz = mt_gpufreqs[oppidx].gpufreq_khz;
    mt_gpufreqs_power[oppidx].gpufreq_power = p_total;

}

/*                                     
                                 
                                       */
#ifdef MT_GPU_DVFS_RANDOM_TEST
static int mt_gpufreq_idx_get(int num)
{
    int random = 0, mult = 0, idx;
    random = jiffies & 0xF;

    while (1) {
        if ((mult * num) >= random) {
            idx = (mult * num) - random;
            break;
        }
        mult++;
    }
    return idx;
}
#endif

/*                                     
                                        
                                       */
static unsigned int mt_gpufreq_pmic_wrap_to_volt(unsigned int pmic_wrap_value)
{
    unsigned int volt = 0;

    volt = (pmic_wrap_value * 625) + 60000;

    gpufreq_dbg("@%s: volt = %d\n", __func__, volt);

    //         
    if (volt > 139375) {
        gpufreq_err("@%s: volt > 1.39375v!\n", __func__);
        return 139375;
    }

    return volt;
}

/*                                     
                                        
                                       */
static unsigned int mt_gpufreq_volt_to_pmic_wrap(unsigned int volt)
{
    unsigned int reg_val = 0;

    reg_val = (volt - 60000) / 625;

    gpufreq_dbg("@%s: reg_val = %d\n", __func__, reg_val);

    if (reg_val > 0x7F) {
        gpufreq_err("@%s: reg_val > 0x7F!\n", __func__);
        return 0x7F;
    }

    return reg_val;
}

/*                                                    */
static void mt_gpufreq_set_initial(unsigned int index)
{
    //                    

    mutex_lock(&mt_gpufreq_lock);

#ifdef MT_GPUFREQ_AEE_RR_REC
    aee_rr_rec_gpu_dvfs_status(aee_rr_curr_gpu_dvfs_status() | (1 << GPU_DVFS_IS_DOING_DVFS));
    aee_rr_rec_gpu_dvfs_oppidx(index);
#endif

    //                             
    mt_gpufreq_volt_switch(93125, mt_gpufreqs[index].gpufreq_volt);

    mt_gpufreq_clock_switch(mt_gpufreqs[index].gpufreq_khz);

    g_cur_gpu_freq = mt_gpufreqs[index].gpufreq_khz;
    g_cur_gpu_volt = mt_gpufreqs[index].gpufreq_volt;
    g_cur_gpu_idx = mt_gpufreqs[index].gpufreq_idx;
    g_cur_gpu_OPPidx = index;

#ifdef MT_GPUFREQ_AEE_RR_REC
    aee_rr_rec_gpu_dvfs_status(aee_rr_curr_gpu_dvfs_status() & ~(1 << GPU_DVFS_IS_DOING_DVFS));
#endif

    mutex_unlock(&mt_gpufreq_lock);
}

static unsigned int mt_gpufreq_calc_pmic_settle_time(unsigned int volt_old, unsigned int volt_new)
{
    unsigned int delay = 100;

    if (volt_new == volt_old)
        return 0;
    else if (volt_new > volt_old)
        delay = PMIC_VOLT_UP_SETTLE_TIME(volt_old, volt_new);
    else
        delay = PMIC_VOLT_DOWN_SETTLE_TIME(volt_old, volt_new);

    if (delay < MIN_PMIC_SETTLE_TIME)
        delay = MIN_PMIC_SETTLE_TIME;

    return delay;
}

/*                                                           */
unsigned int mt_gpufreq_voltage_enable_set(unsigned int enable)
{
    unsigned int reg_val = 0;
    unsigned int delay = 0;

    mutex_lock(&mt_gpufreq_lock);

    if(mt_gpufreq_ready == false) {
        gpufreq_warn("@%s: GPU DVFS not ready!\n", __func__);
        mutex_unlock(&mt_gpufreq_lock);
        return -ENOSYS;
    }

    if(mt_gpufreq_ptpod_disable == true)
    {
        if(enable == 0)
        {
            gpufreq_info("mt_gpufreq_ptpod_disable == true\n");
            mutex_unlock(&mt_gpufreq_lock);
            return -ENOSYS;	
        }
    }

    if(enable == 1)
        pmic_config_interface(PMIC_ADDR_VGPU_EN, 0x1, 0x1, 0x0); //                    
    else
        pmic_config_interface(PMIC_ADDR_VGPU_EN, 0x0, 0x1, 0x0); //                    

    mt_gpufreq_volt_enable_state = enable;

    delay = mt_gpufreq_calc_pmic_settle_time(0, g_cur_gpu_volt);//                             

    gpufreq_dbg("@%s: enable = %x, delay = %d\n", __func__, enable, delay);

    udelay(delay);

#ifdef MT_GPUFREQ_AEE_RR_REC
    if (mt_gpufreq_volt_enable_state)
        aee_rr_rec_gpu_dvfs_status(aee_rr_curr_gpu_dvfs_status() | (1 << GPU_DVFS_IS_VGPU_ENABLED));
    else
        aee_rr_rec_gpu_dvfs_status(aee_rr_curr_gpu_dvfs_status() & ~(1 << GPU_DVFS_IS_VGPU_ENABLED));
#endif 

    pmic_read_interface(PMIC_ADDR_VGPU_EN, &reg_val, 0x1, 0x0);

    //               
    if (enable == 1 && reg_val == 0) {
        //                                              
        int i = 0;

        gpufreq_err("@%s: enable = %x, delay = %d\n", __func__, enable, delay);
            
        //                                   
        for (i=0; i<10; i++) {
            pwrap_read(0x200, &reg_val);
            gpufreq_err("@%s: PMIC CID via pwap = 0x%x\n", __func__, reg_val);
        }

        BUG();
    }

    mutex_unlock(&mt_gpufreq_lock);

    return 0;
}
EXPORT_SYMBOL(mt_gpufreq_voltage_enable_set);

/*                                               
                            
                                                 */
void mt_gpufreq_enable_by_ptpod(void)
{
    mt_gpufreq_ptpod_disable = false;
    gpufreq_info("mt_gpufreq enabled by ptpod\n");

    return;
}
EXPORT_SYMBOL(mt_gpufreq_enable_by_ptpod);

/*                                               
                             
                                                 */
void mt_gpufreq_disable_by_ptpod(void)
{
    int i = 0, volt_level_reached = 0, target_idx = 0;

    if(mt_gpufreq_ready == false) {
        gpufreq_warn("@%s: GPU DVFS not ready!\n", __func__);
        return;
    }

    mt_gpufreq_ptpod_disable = true;
    gpufreq_info("mt_gpufreq disabled by ptpod\n");

    for (i = 0; i < mt_gpufreqs_num; i++) {
        //                     
        if(mt_gpufreqs_default[i].gpufreq_volt == GPU_DVFS_PTPOD_DISABLE_VOLT) {
            volt_level_reached = 1;
            if(i == (mt_gpufreqs_num - 1)) {
                target_idx = i;
                break;
            }
        }
        else {
            if(volt_level_reached == 1) {
                target_idx = i - 1;
                break;
            }
        }
    }

    mt_gpufreq_ptpod_disable_idx = target_idx;

    mt_gpufreq_voltage_enable_set(1);
    mt_gpufreq_target(target_idx);

    return;
}
EXPORT_SYMBOL(mt_gpufreq_disable_by_ptpod);

/*                                               
                                                                    
                                                 */
void mt_gpufreq_restore_default_volt(void)
{
    int i;

    if(mt_gpufreq_ready == false) {
        gpufreq_warn("@%s: GPU DVFS not ready!\n", __func__);
        return;
    }

    mutex_lock(&mt_gpufreq_lock);

    for (i = 0; i < mt_gpufreqs_num; i++) {
        mt_gpufreqs[i].gpufreq_volt = mt_gpufreqs_default[i].gpufreq_volt;
        gpufreq_dbg("@%s: mt_gpufreqs[%d].gpufreq_volt = %x\n", __func__, i, mt_gpufreqs[i].gpufreq_volt);
    }

    mt_gpufreq_volt_switch(g_cur_gpu_volt, mt_gpufreqs[g_cur_gpu_OPPidx].gpufreq_volt);

    g_cur_gpu_volt = mt_gpufreqs[g_cur_gpu_OPPidx].gpufreq_volt;

    mutex_unlock(&mt_gpufreq_lock);

    return;
}
EXPORT_SYMBOL(mt_gpufreq_restore_default_volt);

/*                                                                   */
unsigned int mt_gpufreq_update_volt(unsigned int pmic_volt[], unsigned int array_size)
{
    int i;//      
    //                    
    unsigned volt = 0;

    if(mt_gpufreq_ready == false) {
        gpufreq_warn("@%s: GPU DVFS not ready!\n", __func__);
        return -ENOSYS;
    }

    mutex_lock(&mt_gpufreq_lock);

    for (i = 0; i < array_size; i++) {
        volt = mt_gpufreq_pmic_wrap_to_volt(pmic_volt[i]);
        mt_gpufreqs[i].gpufreq_volt = volt;
        gpufreq_dbg("@%s: mt_gpufreqs[%d].gpufreq_volt = %x\n", __func__, i, mt_gpufreqs[i].gpufreq_volt);
    }

    mt_gpufreq_volt_switch(g_cur_gpu_volt, mt_gpufreqs[g_cur_gpu_OPPidx].gpufreq_volt);

    g_cur_gpu_volt = mt_gpufreqs[g_cur_gpu_OPPidx].gpufreq_volt;

    mutex_unlock(&mt_gpufreq_lock);

    return 0;
}
EXPORT_SYMBOL(mt_gpufreq_update_volt);

unsigned int mt_gpufreq_get_dvfs_table_num(void)
{
    return mt_gpufreqs_num;
}
EXPORT_SYMBOL(mt_gpufreq_get_dvfs_table_num);

unsigned int mt_gpufreq_get_freq_by_idx(unsigned int idx)
{
    if(mt_gpufreq_ready == false) {
        gpufreq_warn("@%s: GPU DVFS not ready!\n", __func__);
        return -ENOSYS;
    }

    if(idx < mt_gpufreqs_num) {
        gpufreq_dbg("@%s: idx = %d, frequency= %d\n", __func__, idx, mt_gpufreqs[idx].gpufreq_khz);
        return mt_gpufreqs[idx].gpufreq_khz;
    }

    gpufreq_dbg("@%s: idx = %d, NOT found! return 0!\n", __func__, idx);
    return 0;
}
EXPORT_SYMBOL(mt_gpufreq_get_freq_by_idx);


#ifdef MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE
static void mt_update_gpufreqs_power_table(void)
{
    int i = 0, temp = 0;

    if (mt_gpufreq_ready == false) {
        gpufreq_warn("@%s: GPU DVFS not ready\n", __func__);
        return;
    }

    temp = get_immediate_gpu_wrap() / 1000;

    gpufreq_dbg("@%s, temp = %d\n", __func__, temp);

    mutex_lock(&mt_gpufreq_lock);

    if((temp > 0) && (temp < 125)) {
        for (i = 0; i < mt_gpufreqs_num; i++) {
            mt_gpufreq_power_calculation(i, temp);

            gpufreq_dbg("update mt_gpufreqs_power[%d].gpufreq_khz = %d\n", i, mt_gpufreqs_power[i].gpufreq_khz);
            gpufreq_dbg("update mt_gpufreqs_power[%d].gpufreq_power = %d\n", i, mt_gpufreqs_power[i].gpufreq_power);
        }
    }
    else
        gpufreq_err("@%s: temp < 0 or temp > 125, NOT update power table!\n", __func__);

    mutex_unlock(&mt_gpufreq_lock);
}
#endif

static void mt_setup_gpufreqs_power_table(int num)
{
    int i = 0, temp = 0;

    mt_gpufreqs_power = kzalloc((num) * sizeof(struct mt_gpufreq_power_table_info), GFP_KERNEL);
    if (mt_gpufreqs_power == NULL) {
        gpufreq_err("GPU power table memory allocation fail\n");
        return;
    }

    temp = get_immediate_gpu_wrap() / 1000;

    gpufreq_dbg("@%s: temp = %d \n", __func__, temp);

    if((temp < 0) || (temp > 125)) {
        gpufreq_dbg("@%s: temp < 0 or temp > 125!\n", __func__);
        temp = 65;
    }

    for (i = 0; i < num; i++) {
        mt_gpufreq_power_calculation(i, temp);

        gpufreq_info("mt_gpufreqs_power[%d].gpufreq_khz = %u\n", i, mt_gpufreqs_power[i].gpufreq_khz);
        gpufreq_info("mt_gpufreqs_power[%d].gpufreq_power = %u\n", i, mt_gpufreqs_power[i].gpufreq_power);
    }

#ifdef CONFIG_THERMAL
    mtk_gpufreq_register(mt_gpufreqs_power, num);
#endif
}

/*                                              
                                                
                                                */
static int mt_setup_gpufreqs_table(struct mt_gpufreq_table_info *freqs, int num)
{
    int i = 0;

    mt_gpufreqs = kzalloc((num) * sizeof(*freqs), GFP_KERNEL);
    mt_gpufreqs_default = kzalloc((num) * sizeof(*freqs), GFP_KERNEL);
    if (mt_gpufreqs == NULL)
        return -ENOMEM;

    for (i = 0; i < num; i++) {
        mt_gpufreqs[i].gpufreq_khz = freqs[i].gpufreq_khz;
        mt_gpufreqs[i].gpufreq_volt = freqs[i].gpufreq_volt;
        mt_gpufreqs[i].gpufreq_idx = freqs[i].gpufreq_idx;

        mt_gpufreqs_default[i].gpufreq_khz = freqs[i].gpufreq_khz;
        mt_gpufreqs_default[i].gpufreq_volt = freqs[i].gpufreq_volt;
        mt_gpufreqs_default[i].gpufreq_idx = freqs[i].gpufreq_idx;

        gpufreq_dbg("freqs[%d].gpufreq_khz = %u\n", i, freqs[i].gpufreq_khz);
        gpufreq_dbg("freqs[%d].gpufreq_volt = %u\n", i, freqs[i].gpufreq_volt);
        gpufreq_dbg("freqs[%d].gpufreq_idx = %u\n", i, freqs[i].gpufreq_idx);
    }

    mt_gpufreqs_num = num;

    g_limited_max_id = 0;
    g_limited_min_id = mt_gpufreqs_num - 1;

    gpufreq_info("@%s: g_cur_gpu_freq = %d, g_cur_gpu_volt = %d\n", __func__, g_cur_gpu_freq, g_cur_gpu_volt);

    mt_setup_gpufreqs_power_table(num);

    return 0;
}



/*                                     
                                       
                                       */
static int mt_gpufreq_keep_max_freq(unsigned int freq_old, unsigned int freq_new)
{
    if (mt_gpufreq_keep_max_frequency_state == true)
        return 1;

    return 0;
}

#if 1

/*                            
                      
                              */
int mt_gpufreq_state_set(int enabled)
{
    if (enabled) {
        if (!mt_gpufreq_pause) {
            gpufreq_dbg("gpufreq already enabled\n");
            return 0;
        }

        /*                
                          
                          */
        g_gpufreq_dvfs_disable_count--;
        gpufreq_dbg("enable GPU DVFS: g_gpufreq_dvfs_disable_count = %d\n", g_gpufreq_dvfs_disable_count);

        /*                                              
                                                        
                                                        */
        if (g_gpufreq_dvfs_disable_count <= 0)
            mt_gpufreq_pause = false;
        else
            gpufreq_warn("someone still disable gpufreq, cannot enable it\n");
    }
    else {
        /*                 
                           
                           */
        g_gpufreq_dvfs_disable_count++;
        gpufreq_dbg("disable GPU DVFS: g_gpufreq_dvfs_disable_count = %d\n", g_gpufreq_dvfs_disable_count);

        if (mt_gpufreq_pause) {
            gpufreq_dbg("gpufreq already disabled\n");
            return 0;
        }

        mt_gpufreq_pause = true;
    }

    return 0;
}
EXPORT_SYMBOL(mt_gpufreq_state_set);
#endif

static unsigned int mt_gpufreq_dds_calc(unsigned int khz)
{
    unsigned int dds;
#ifdef MTK_TABLET_TURBO
    dds = ((khz * 4/1000) * 8192)/13;
    return dds;
#endif
    if ((khz >= 250250) && (khz <= 747500))
        dds = 0x0209A000 + ((khz - 250250) * 4 / 13000) * 0x2000;
    else if ((khz > 747500) && (khz <= 793000))
        dds = 0x010E6000 + ((khz - 747500) * 2 / 13000) * 0x2000;
    else {
        gpufreq_err("@%s: target khz(%d) out of range!\n", __func__, khz);
        BUG();
    }

    return dds;
}

static void mt_gpufreq_clock_switch(unsigned int freq_new)
{
#if 1
    unsigned int dds = mt_gpufreq_dds_calc(freq_new);

    mt_dfs_mmpll(dds);

    gpufreq_dbg("@%s: freq_new = %d, dds = 0x%x\n", __func__, freq_new, dds);
#else
    switch (freq_new) {
        case GPU_DVFS_FREQ0: //           
            mt_dfs_mmpll(2912000);
            break;
        case GPU_DVFS_FREQ1: //           
            mt_dfs_mmpll(2600000);
            break;
        case GPU_DVFS_FREQ2: //           
            mt_dfs_mmpll(2392000);
            break;
        case GPU_DVFS_FREQ3: //           
            mt_dfs_mmpll(2080000);
            break;
        case GPU_DVFS_FREQ4: //           
            mt_dfs_mmpll(1976000);
            break;
        case GPU_DVFS_FREQ5: //           
            mt_dfs_mmpll(1664000);
            break;
        case GPU_DVFS_FREQ6: //           
            mt_dfs_mmpll(1248000);
            break;
        default:
            if(mt_gpufreq_fixed_freq_volt_state == true)
                mt_dfs_mmpll(freq_new * 4);
            break;
    }

    gpufreq_dbg("@%s: freq_new = %d KHz\n", __func__, freq_new);
#endif

    if(NULL != g_pFreqSampler)
        g_pFreqSampler(freq_new);

}

static void mt_gpufreq_volt_switch(unsigned int volt_old, unsigned int volt_new)
{
    unsigned int reg_val = 0;
    unsigned int delay = 0;
    //                           

    gpufreq_dbg("@%s: volt_new = %d \n", __func__, volt_new);

    //                                                  

    reg_val = mt_gpufreq_volt_to_pmic_wrap(volt_new);

#ifdef MT_GPUFREQ_AEE_RR_REC
    aee_rr_rec_gpu_dvfs_vgpu(reg_val);
#endif

#if 1
    mt_cpufreq_set_pmic_cmd(PMIC_WRAP_PHASE_NORMAL, IDX_NM_VGPU, reg_val);
    mt_cpufreq_apply_pmic_cmd(IDX_NM_VGPU);

    //                                                                         
    //                                        
#else
    mt_gpufreq_reg_write(reg_val, PMIC_WRAP_DVFS_WDATA2);

    spm_dvfs_ctrl_volt(GPU_DVFS_CTRL_VOLT); 
#endif

    if(volt_new > volt_old) {
        delay = mt_gpufreq_calc_pmic_settle_time(volt_old, volt_new);
        gpufreq_dbg("@%s: delay = %d \n", __func__, delay);
        udelay(delay);
    }
    
    if(NULL != g_pVoltSampler)
        g_pVoltSampler(volt_new);
}


/*                                        
                                          
                                          */
/*                                                          
         
                                                   
                                                            
                                                            */
static void mt_gpufreq_set(unsigned int freq_old, unsigned int freq_new, unsigned int volt_old, unsigned int volt_new)
{
    if(freq_new > freq_old) {
        //                               
        // 
        mt_gpufreq_volt_switch(volt_old, volt_new);
        // 

        mt_gpufreq_clock_switch(freq_new);
    }
    else {
        mt_gpufreq_clock_switch(freq_new);

        //                        
        // 
        mt_gpufreq_volt_switch(volt_old, volt_new);
        // 
    }

    g_cur_gpu_freq = freq_new;
    g_cur_gpu_volt = volt_new;
}


/*                                 
                                   
                                   */
/*                                                
         
                                     
                                                 
                                                  */
unsigned int mt_gpufreq_target(unsigned int idx)
{
    //                    
    unsigned int target_freq, target_volt, target_idx, target_OPPidx;

    mutex_lock(&mt_gpufreq_lock);

    if(mt_gpufreq_ready == false) {
        gpufreq_warn("GPU DVFS not ready!\n");
        mutex_unlock(&mt_gpufreq_lock);
        return -ENOSYS;
    }

    if(mt_gpufreq_volt_enable_state == 0) {
        gpufreq_info("mt_gpufreq_volt_enable_state == 0! return\n");
        mutex_unlock(&mt_gpufreq_lock);
        return -ENOSYS;	
    }

#ifdef MT_GPU_DVFS_RANDOM_TEST
    idx = mt_gpufreq_idx_get(5);
    gpufreq_dbg("@%s: random test index is %d !\n", __func__, idx);
#endif

    if (idx > (mt_gpufreqs_num - 1)) {
        mutex_unlock(&mt_gpufreq_lock);
        gpufreq_err("@%s: idx out of range! idx = %d\n", __func__, idx);
        return -1;
    }

#ifdef MT_GPUFREQ_AEE_RR_REC
    aee_rr_rec_gpu_dvfs_status(aee_rr_curr_gpu_dvfs_status() | (1 << GPU_DVFS_IS_DOING_DVFS));
#endif

    /*                                 
                                     
                                       */
    target_freq = mt_gpufreqs[idx].gpufreq_khz;
    target_volt = mt_gpufreqs[idx].gpufreq_volt;
    target_idx = mt_gpufreqs[idx].gpufreq_idx;
    target_OPPidx = idx;

    gpufreq_dbg("@%s: begin, receive freq: %d, OPPidx: %d\n", __func__, target_freq, target_OPPidx);

    /*                                 
                                          
                                       */
    if (mt_gpufreq_keep_max_freq(g_cur_gpu_freq, target_freq)) {
        target_freq = mt_gpufreqs[g_gpufreq_max_id].gpufreq_khz;
        target_volt = mt_gpufreqs[g_gpufreq_max_id].gpufreq_volt;
        target_idx = mt_gpufreqs[g_gpufreq_max_id].gpufreq_idx;
        target_OPPidx = g_gpufreq_max_id;
        gpufreq_dbg("Keep MAX frequency %d !\n", target_freq);
    }    

#if 0
    /*                                                   
                                                         
                                                         */
    if(target_freq > g_cur_freq)
    {
        target_freq = mt_gpufreqs[g_gpufreq_max_id].gpufreq_khz;
        target_volt = mt_gpufreqs[g_gpufreq_max_id].gpufreq_volt;
        gpufreq_dbg("Need to raise frequency, raise to MAX frequency %d !\n", target_freq);
    }
#endif	

    /*                                               
                                           
                                                     */
    if(mt_gpufreq_keep_opp_frequency_state == true) {
        target_freq = mt_gpufreqs[mt_gpufreq_keep_opp_index].gpufreq_khz;
        target_volt = mt_gpufreqs[mt_gpufreq_keep_opp_index].gpufreq_volt;
        target_idx = mt_gpufreqs[mt_gpufreq_keep_opp_index].gpufreq_idx;
        target_OPPidx = mt_gpufreq_keep_opp_index;
        gpufreq_dbg("Keep opp! opp frequency %d, opp voltage %d, opp idx %d\n", target_freq, target_volt, target_OPPidx);
    }

    /*                                               
                                          
                                                     */
    if(mt_gpufreq_fixed_freq_volt_state == true) {
        target_freq = mt_gpufreq_fixed_frequency;
        target_volt = mt_gpufreq_fixed_voltage;
        target_idx = 0;
        target_OPPidx = 0;
        gpufreq_dbg("Fixed! fixed frequency %d, fixed voltage %d\n", target_freq, target_volt);
    }

    /*                                               
                                               
                                                     */
    if(mt_gpufreq_opp_max_frequency_state == true) {
        if (target_freq > mt_gpufreq_opp_max_frequency) {
            target_freq = mt_gpufreqs[mt_gpufreq_opp_max_index].gpufreq_khz;
            target_volt = mt_gpufreqs[mt_gpufreq_opp_max_index].gpufreq_volt;
            target_idx = mt_gpufreqs[mt_gpufreq_opp_max_index].gpufreq_idx;
            target_OPPidx = mt_gpufreq_opp_max_index;

            gpufreq_dbg("opp max freq! opp max frequency %d, opp max voltage %d, opp max idx %d\n", target_freq, target_volt, target_OPPidx);
        }
    }

    /*                                               
                          
                                                     */	
    if(g_limited_max_id != 0) {
        if (target_freq > mt_gpufreqs[g_limited_max_id].gpufreq_khz) {
            /*                                            
                                                         
                                                          */
            target_freq = mt_gpufreqs[g_limited_max_id].gpufreq_khz;
            target_volt = mt_gpufreqs[g_limited_max_id].gpufreq_volt;
            target_idx = mt_gpufreqs[g_limited_max_id].gpufreq_idx;
            target_OPPidx = g_limited_max_id;
            gpufreq_info("Limit! Thermal/Power limit gpu frequency %d\n", mt_gpufreqs[g_limited_max_id].gpufreq_khz);
        }
    }

    /*                                               
                                                
                                                     */
    if (mt_gpufreq_ptpod_disable == true) {
#if 1
        target_freq = mt_gpufreqs[mt_gpufreq_ptpod_disable_idx].gpufreq_khz;
        target_volt = mt_gpufreqs[mt_gpufreq_ptpod_disable_idx].gpufreq_volt;
        target_idx = mt_gpufreqs[mt_gpufreq_ptpod_disable_idx].gpufreq_idx;
        target_OPPidx = mt_gpufreq_ptpod_disable_idx;
        gpufreq_dbg("PTPOD disable dvfs, mt_gpufreq_ptpod_disable_idx = %d\n", mt_gpufreq_ptpod_disable_idx);
#else
        mutex_unlock(&mt_gpufreq_lock);
        gpufreq_dbg("PTPOD disable dvfs, return\n");
        return 0;
#endif
    }

    /*                                               
                                                     
                                                     */
    if (g_cur_gpu_freq == target_freq) {
        mutex_unlock(&mt_gpufreq_lock);
        gpufreq_dbg("GPU frequency from %d KHz to %d KHz (skipped) due to same frequency\n", g_cur_gpu_freq, target_freq);
        return 0;
    }

    gpufreq_dbg("GPU current frequency %d KHz, target frequency %d KHz\n", g_cur_gpu_freq, target_freq);

#ifdef MT_GPUFREQ_AEE_RR_REC
    aee_rr_rec_gpu_dvfs_oppidx(target_OPPidx);
#endif

    /*                             
                                  
                                   */
    mt_gpufreq_set(g_cur_gpu_freq, target_freq, g_cur_gpu_volt, target_volt);

    g_cur_gpu_idx = target_idx;
    g_cur_gpu_OPPidx = target_OPPidx;

#ifdef MT_GPUFREQ_AEE_RR_REC
    aee_rr_rec_gpu_dvfs_status(aee_rr_curr_gpu_dvfs_status() & ~(1 << GPU_DVFS_IS_DOING_DVFS));
#endif

    mutex_unlock(&mt_gpufreq_lock);

    return 0;
}
EXPORT_SYMBOL(mt_gpufreq_target);


/*                                           
                            
                                            */
enum {
    IDX_THERMAL_LIMITED,
    IDX_LOW_BATT_VOLT_LIMITED,
    IDX_LOW_BATT_VOLUME_LIMITED,
    IDX_OC_LIMITED,
    
    NR_IDX_POWER_LIMITED,
};

//                                   
#ifdef MT_GPUFREQ_OC_PROTECT
static unsigned int mt_gpufreq_oc_level = 0;

#define MT_GPUFREQ_OC_LIMIT_FREQ_1     GPU_DVFS_FREQ3
static unsigned int mt_gpufreq_oc_limited_index_0 = 0; //                              
static unsigned int mt_gpufreq_oc_limited_index_1 = 0;
static unsigned int mt_gpufreq_oc_limited_index = 0; //                               
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
static unsigned int mt_gpufreq_low_battery_volume = 0;

#define MT_GPUFREQ_LOW_BATT_VOLUME_LIMIT_FREQ_1     GPU_DVFS_FREQ6
static unsigned int mt_gpufreq_low_bat_volume_limited_index_0 = 0; //                              
static unsigned int mt_gpufreq_low_bat_volume_limited_index_1 = 0;
static unsigned int mt_gpufreq_low_batt_volume_limited_index = 0; //                                               
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT	
static unsigned int mt_gpufreq_low_battery_level = 0;

#define MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ_1     GPU_DVFS_FREQ5
#define MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ_2     GPU_DVFS_FREQ6
static unsigned int mt_gpufreq_low_bat_volt_limited_index_0 = 0; //                              
static unsigned int mt_gpufreq_low_bat_volt_limited_index_1 = 0;
static unsigned int mt_gpufreq_low_bat_volt_limited_index_2 = 0;
static unsigned int mt_gpufreq_low_batt_volt_limited_index = 0; //                                                
#endif

static unsigned int mt_gpufreq_thermal_limited_gpu_power = 0; //                    
static unsigned int mt_gpufreq_power_limited_index_array[NR_IDX_POWER_LIMITED] = {0}; //                            

/*                                               
                                                 
                                                 */
/*                                                     
                          
                                                       */
static int mt_gpufreq_power_throttle_protect(void)
{
    int ret = 0;
    int i = 0;
    unsigned int limited_index = 0;

    //                                         
    for (i = 0; i < NR_IDX_POWER_LIMITED; i++) {
        if (mt_gpufreq_power_limited_index_array[i] != 0 && limited_index == 0)
            limited_index = mt_gpufreq_power_limited_index_array[i];
        else if (mt_gpufreq_power_limited_index_array[i] != 0 && limited_index != 0) {
            if (mt_gpufreq_power_limited_index_array[i] > limited_index)
                limited_index = mt_gpufreq_power_limited_index_array[i];
        }

        gpufreq_dbg("mt_gpufreq_power_limited_index_array[%d] = %d\n", i, mt_gpufreq_power_limited_index_array[i]);
    }

    g_limited_max_id = limited_index;
    gpufreq_info("Final limit frequency upper bound to id = %d, frequency = %d\n", g_limited_max_id, mt_gpufreqs[g_limited_max_id].gpufreq_khz);

    if(NULL != g_pGpufreq_power_limit_notify)
        g_pGpufreq_power_limit_notify(g_limited_max_id);

    return ret;
}

#ifdef MT_GPUFREQ_OC_PROTECT
/*                                               
                                                
                                                 */
static void mt_gpufreq_oc_protect(unsigned int limited_index)
{
    mutex_lock(&mt_gpufreq_power_lock);

    gpufreq_dbg("@%s: limited_index = %d\n", __func__, limited_index);

    mt_gpufreq_power_limited_index_array[IDX_OC_LIMITED] = limited_index;	
    mt_gpufreq_power_throttle_protect();

    mutex_unlock(&mt_gpufreq_power_lock);

    return;
}

void mt_gpufreq_oc_callback(BATTERY_OC_LEVEL oc_level)
{
    gpufreq_dbg("@%s: oc_level = %d\n", __func__, oc_level);

    if(mt_gpufreq_ready == false) {
        gpufreq_warn("@%s: GPU DVFS not ready!\n", __func__);
        return;
    }

    if(g_limited_oc_ignore_state == true) {
        gpufreq_info("@%s: g_limited_oc_ignore_state == true!\n", __func__);
        return;
    }

    mt_gpufreq_oc_level = oc_level;

    //                                                   
    if (oc_level == BATTERY_OC_LEVEL_1) {
        if (mt_gpufreq_oc_limited_index != mt_gpufreq_oc_limited_index_1) {
            mt_gpufreq_oc_limited_index = mt_gpufreq_oc_limited_index_1;
            mt_gpufreq_oc_protect(mt_gpufreq_oc_limited_index_1); //                   
        }
    }
    //           
    else {
        if (mt_gpufreq_oc_limited_index != mt_gpufreq_oc_limited_index_0) {
            mt_gpufreq_oc_limited_index = mt_gpufreq_oc_limited_index_0;
            mt_gpufreq_oc_protect(mt_gpufreq_oc_limited_index_0); //        
        }
    }
}
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
/*                                               
                                                            
                                                 */
static void mt_gpufreq_low_batt_volume_protect(unsigned int limited_index)
{
    mutex_lock(&mt_gpufreq_power_lock);

    gpufreq_dbg("@%s: limited_index = %d\n", __func__, limited_index);

    mt_gpufreq_power_limited_index_array[IDX_LOW_BATT_VOLUME_LIMITED] = limited_index;	
    mt_gpufreq_power_throttle_protect();

    mutex_unlock(&mt_gpufreq_power_lock);

    return;
}

void mt_gpufreq_low_batt_volume_callback(BATTERY_PERCENT_LEVEL low_battery_volume)
{
    gpufreq_dbg("@%s: low_battery_volume = %d\n", __func__, low_battery_volume);

    if(mt_gpufreq_ready == false) {
        gpufreq_warn("@%s: GPU DVFS not ready!\n", __func__);
        return;
    }

    if(g_limited_low_batt_volume_ignore_state == true) {
        gpufreq_info("@%s: g_limited_low_batt_volume_ignore_state == true!\n", __func__);
        return;
    }

    mt_gpufreq_low_battery_volume = low_battery_volume;

    //                                                        
    if (low_battery_volume == BATTERY_PERCENT_LEVEL_1) {
        if (mt_gpufreq_low_batt_volume_limited_index != mt_gpufreq_low_bat_volume_limited_index_1) {
            mt_gpufreq_low_batt_volume_limited_index = mt_gpufreq_low_bat_volume_limited_index_1;
            mt_gpufreq_low_batt_volume_protect(mt_gpufreq_low_bat_volume_limited_index_1); //                 
        }
    }
    //           
    else {
        if (mt_gpufreq_low_batt_volume_limited_index != mt_gpufreq_low_bat_volume_limited_index_0) {
            mt_gpufreq_low_batt_volume_limited_index = mt_gpufreq_low_bat_volume_limited_index_0;
            mt_gpufreq_low_batt_volume_protect(mt_gpufreq_low_bat_volume_limited_index_0); //        
        }
    }
}
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT	
/*                                               
                                                          
                                                 */
static void mt_gpufreq_low_batt_volt_protect(unsigned int limited_index)
{
    mutex_lock(&mt_gpufreq_power_lock);

    gpufreq_dbg("@%s: limited_index = %d\n", __func__, limited_index);
    mt_gpufreq_power_limited_index_array[IDX_LOW_BATT_VOLT_LIMITED] = limited_index;	
    mt_gpufreq_power_throttle_protect();

    mutex_unlock(&mt_gpufreq_power_lock);

    return;
}

/*                                                     
                               
                                                       */
void mt_gpufreq_low_batt_volt_callback(LOW_BATTERY_LEVEL low_battery_level)
{
    gpufreq_dbg("@%s: low_battery_level = %d\n", __func__, low_battery_level);

    if(mt_gpufreq_ready == false) {
        gpufreq_warn("@%s: GPU DVFS not ready!\n", __func__);
        return;
    }

    if(g_limited_low_batt_volt_ignore_state == true) {
        gpufreq_info("@%s: g_limited_low_batt_volt_ignore_state == true!\n", __func__);
        return;
    }

    mt_gpufreq_low_battery_level = low_battery_level;

    //                                                         
    //                                                                                                                       
    if (low_battery_level == LOW_BATTERY_LEVEL_1) {
        if (mt_gpufreq_low_batt_volt_limited_index != mt_gpufreq_low_bat_volt_limited_index_1) {
            mt_gpufreq_low_batt_volt_limited_index = mt_gpufreq_low_bat_volt_limited_index_1;
            mt_gpufreq_low_batt_volt_protect(mt_gpufreq_low_bat_volt_limited_index_1); //                 
        }
    }
    else if(low_battery_level == LOW_BATTERY_LEVEL_2) {
        if (mt_gpufreq_low_batt_volt_limited_index != mt_gpufreq_low_bat_volt_limited_index_2) {
            mt_gpufreq_low_batt_volt_limited_index = mt_gpufreq_low_bat_volt_limited_index_2;
            mt_gpufreq_low_batt_volt_protect(mt_gpufreq_low_bat_volt_limited_index_2); //                 
        }
    }
    //           
    else {
        if (mt_gpufreq_low_batt_volt_limited_index != mt_gpufreq_low_bat_volt_limited_index_0) {
            mt_gpufreq_low_batt_volt_limited_index = mt_gpufreq_low_bat_volt_limited_index_0;
            mt_gpufreq_low_batt_volt_protect(mt_gpufreq_low_bat_volt_limited_index_0); //        
        }
    }
}
#endif

/*                                               
                                                 
                                                 */
/*                                                     
                          
                                                       */
void mt_gpufreq_thermal_protect(unsigned int limited_power)
{
    int i = 0;
    unsigned int limited_freq = 0;
    unsigned int found = 0;

    mutex_lock(&mt_gpufreq_power_lock);

    if(mt_gpufreq_ready == false) {
        gpufreq_warn("@%s: GPU DVFS not ready!\n", __func__);
        mutex_unlock(&mt_gpufreq_power_lock);
        return;
    }

    if (mt_gpufreqs_num == 0) {
        gpufreq_warn("@%s: mt_gpufreqs_num == 0!\n", __func__);
        mutex_unlock(&mt_gpufreq_power_lock);
        return;
    }

    if(g_limited_thermal_ignore_state == true) {
        gpufreq_info("@%s: g_limited_thermal_ignore_state == true!\n", __func__);
        mutex_unlock(&mt_gpufreq_power_lock);
        return;
    }

    mt_gpufreq_thermal_limited_gpu_power = limited_power;

    gpufreq_info("@%s: limited_power = %d\n", __func__, limited_power);

#ifdef MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE
    mt_update_gpufreqs_power_table();
#endif

    if (limited_power == 0)
        mt_gpufreq_power_limited_index_array[IDX_THERMAL_LIMITED] = 0;
    else {
        //                                       

        for (i = 0; i < mt_gpufreqs_num; i++) {
            if (mt_gpufreqs_power[i].gpufreq_power <= limited_power) {
                limited_freq = mt_gpufreqs_power[i].gpufreq_khz;
                found = 1;
                break;
            }
        }

        gpufreq_dbg("@%s: found = %d\n", __func__, found);

        if(found == 0)
            limited_freq = mt_gpufreqs[mt_gpufreqs_num - 1].gpufreq_khz;

        for (i = 0; i < mt_gpufreqs_num; i++) {
            if (mt_gpufreqs[i].gpufreq_khz <= limited_freq) {
                mt_gpufreq_power_limited_index_array[IDX_THERMAL_LIMITED] = i;
                break;
            }
        }
    }

    gpufreq_info("Thermal limit frequency upper bound to id = %d\n", mt_gpufreq_power_limited_index_array[IDX_THERMAL_LIMITED]);

    mt_gpufreq_power_throttle_protect();

    mutex_unlock(&mt_gpufreq_power_lock);

    return;
}
EXPORT_SYMBOL(mt_gpufreq_thermal_protect);


/*                                               
                                         
                                                 */
unsigned int mt_gpufreq_get_thermal_limit_index(void)
{
    gpufreq_dbg("current GPU thermal limit index is %d\n", g_limited_max_id);
    return g_limited_max_id;
}
EXPORT_SYMBOL(mt_gpufreq_get_thermal_limit_index);

/*                                               
                                             
                                                 */
unsigned int mt_gpufreq_get_thermal_limit_freq(void)
{
    gpufreq_dbg("current GPU thermal limit freq is %d MHz\n", mt_gpufreqs[g_limited_max_id].gpufreq_khz / 1000);
    return mt_gpufreqs[g_limited_max_id].gpufreq_khz;
}
EXPORT_SYMBOL(mt_gpufreq_get_thermal_limit_freq);

/*                                               
                                     
                                                 */
unsigned int mt_gpufreq_get_cur_freq_index(void)
{
    gpufreq_dbg("current GPU frequency OPP index is %d\n", g_cur_gpu_OPPidx);
    return g_cur_gpu_OPPidx;
}
EXPORT_SYMBOL(mt_gpufreq_get_cur_freq_index);

/*                                               
                               
                                                 */
unsigned int mt_gpufreq_get_cur_freq(void)
{
    gpufreq_dbg("current GPU frequency is %d MHz\n", g_cur_gpu_freq / 1000);
    return g_cur_gpu_freq;
}
EXPORT_SYMBOL(mt_gpufreq_get_cur_freq);

/*                                               
                             
                                                 */
unsigned int mt_gpufreq_get_cur_volt(void)
{
    return g_cur_gpu_volt;
}
EXPORT_SYMBOL(mt_gpufreq_get_cur_volt);

/*                                               
                                                       
                                                 */
void mt_gpufreq_input_boost_notify_registerCB(gpufreq_input_boost_notify pCB)
{
#ifdef MT_GPUFREQ_INPUT_BOOST
    g_pGpufreq_input_boost_notify = pCB;
#endif
}
EXPORT_SYMBOL(mt_gpufreq_input_boost_notify_registerCB);

/*                                               
                                                       
                                                 */
void mt_gpufreq_power_limit_notify_registerCB(gpufreq_power_limit_notify pCB)
{
    g_pGpufreq_power_limit_notify = pCB;
}
EXPORT_SYMBOL(mt_gpufreq_power_limit_notify_registerCB);

/*                                               
                                        
                                                 */
void mt_gpufreq_setfreq_registerCB(sampler_func pCB)
{
    g_pFreqSampler = pCB;
}
EXPORT_SYMBOL(mt_gpufreq_setfreq_registerCB);

/*                                               
                                        
                                                 */
void mt_gpufreq_setvolt_registerCB(sampler_func pCB)
{
    g_pVoltSampler = pCB;
}
EXPORT_SYMBOL(mt_gpufreq_setvolt_registerCB);


/*                                
                                  
                                  */
void mt_gpufreq_early_suspend(struct early_suspend *h)
{
    //                        

}

/*                              
                                
                                */
void mt_gpufreq_late_resume(struct early_suspend *h)
{
    //                                    

    //                        
}

static unsigned int mt_gpufreq_dvfs_get_gpu_freq(void)
{
#if 1
    unsigned int mmpll = 0;
    unsigned int freq = 0;

    freq = DRV_Reg32(MMPLL_CON1) & ~0x80000000;

#ifdef MTK_TABLET_TURBO
    unsigned int div = 1 << ((freq & (0x7 << 24)) >> 24);
    unsigned int n_info = (freq & 0x1fffff) >> 14;
    mmpll = n_info * 26000 / div;    
    return mmpll;
#endif

    if ((freq >= 0x0209A000) && (freq <= 0x021CC000)) {
        mmpll = 0x0209A000;
        mmpll = 250250 + (((freq - mmpll) / 0x2000) * 3250);
    }
    else if ((freq >= 0x010E6000) && (freq <= 0x010F4000)) {
        mmpll = 0x010E6000;
        mmpll = 747500 + (((freq - mmpll) / 0x2000) * 6500);
    }
    else
        BUG();
    
    return mmpll; //   
#else
    unsigned int gpu_freq = mt_get_mfgclk_freq();
    //                 
    gpufreq_dbg("g_cur_gpu_freq = %d KHz, Meter = %d KHz\n", g_cur_gpu_freq, gpu_freq);
    return gpu_freq;
#endif    
}

static int mt_gpufreq_pm_restore_early(struct device *dev)
{
    int i = 0;
    int found = 0;

    g_cur_gpu_freq = mt_gpufreq_dvfs_get_gpu_freq();

    for (i = 0; i < mt_gpufreqs_num; i++) {
        if (g_cur_gpu_freq == mt_gpufreqs[i].gpufreq_khz) {
            g_cur_gpu_idx = mt_gpufreqs[i].gpufreq_idx;
            g_cur_gpu_volt = mt_gpufreqs[i].gpufreq_volt;
            g_cur_gpu_OPPidx = i;
            found = 1;
            gpufreq_dbg("match g_cur_gpu_OPPidx: %d\n", g_cur_gpu_OPPidx);
            break;
        }
    }

    if(found == 0) {
        g_cur_gpu_idx = mt_gpufreqs[0].gpufreq_idx;
        g_cur_gpu_volt = mt_gpufreqs[0].gpufreq_volt;
        g_cur_gpu_OPPidx = 0;
        gpufreq_err("gpu freq not found, set parameter to max freq\n");
    }

    gpufreq_dbg("GPU freq SW/HW: %d/%d\n", g_cur_gpu_freq, mt_gpufreq_dvfs_get_gpu_freq());
    gpufreq_dbg("g_cur_gpu_OPPidx: %d\n", g_cur_gpu_OPPidx);

    return 0;
}

static int mt_gpufreq_pdrv_probe(struct platform_device *pdev)
{
    unsigned int reg_val = 0;
    int i = 0, init_idx = 0;
#ifdef MT_GPUFREQ_INPUT_BOOST
    int rc;
    struct sched_param param = { .sched_priority = MAX_RT_PRIO-1 };
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_POLLING_TIMER
    ktime_t ktime = ktime_set(mt_gpufreq_low_batt_volume_period_s, mt_gpufreq_low_batt_volume_period_ns);
    hrtimer_init(&mt_gpufreq_low_batt_volume_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    mt_gpufreq_low_batt_volume_timer.function = mt_gpufreq_low_batt_volume_timer_func;
#endif

    mt_gpufreq_dvfs_table_type = mt_gpufreq_get_dvfs_table_type();

#ifdef CONFIG_HAS_EARLYSUSPEND
    mt_gpufreq_early_suspend_handler.suspend = mt_gpufreq_early_suspend;
    mt_gpufreq_early_suspend_handler.resume = mt_gpufreq_late_resume;
    register_early_suspend(&mt_gpufreq_early_suspend_handler);
#endif


    /*                     
                              
                           */
    mt_cpufreq_set_pmic_phase(PMIC_WRAP_PHASE_NORMAL);

    /*                     
                                   
                           */
    //                  
    //                 

    /*                     
                                 
                           */
#ifdef MT_GPUFREQ_AEE_RR_REC     
    _mt_gpufreq_aee_init();
#endif

    /*                     
                          
                           */
    gpufreq_info("setup gpufreqs table\n");

    if (mt_gpufreq_dvfs_table_type == 0)    //     
        mt_setup_gpufreqs_table(OPPS_ARRAY_AND_SIZE(mt_gpufreq_opp_tbl_e1_0));
    else if (mt_gpufreq_dvfs_table_type == 1)   //     
        mt_setup_gpufreqs_table(OPPS_ARRAY_AND_SIZE(mt_gpufreq_opp_tbl_e1_1));
    else if (mt_gpufreq_dvfs_table_type == 2)   //     
        mt_setup_gpufreqs_table(OPPS_ARRAY_AND_SIZE(mt_gpufreq_opp_tbl_e1_2));
#ifdef MTK_TABLET_TURBO
    else if (mt_gpufreq_dvfs_table_type == 3)   //    
        mt_setup_gpufreqs_table(OPPS_ARRAY_AND_SIZE(mt_gpufreq_opp_tbl_e1_t));
#endif
    else
        mt_setup_gpufreqs_table(OPPS_ARRAY_AND_SIZE(mt_gpufreq_opp_tbl_e1_0));

    /*                     
                            
                           */
    pmic_config_interface(PMIC_ADDR_VGPU_VOSEL_CTRL, 0x1, 0x1, 0x1); //                                      
    pmic_config_interface(PMIC_ADDR_VGPU_EN_CTRL, 0x0, 0x1, 0x0); //                                    
    pmic_config_interface(PMIC_ADDR_VGPU_EN, 0x1, 0x1, 0x0); //                    

#ifdef MT_GPUFREQ_AEE_RR_REC
    aee_rr_rec_gpu_dvfs_status(aee_rr_curr_gpu_dvfs_status() | (1 << GPU_DVFS_IS_VGPU_ENABLED));
#endif

    mt_gpufreq_volt_enable_state = 1;

    pmic_read_interface(PMIC_ADDR_VGPU_VOSEL_CTRL, &reg_val, 0x1, 0x1); //                       
    gpufreq_dbg("VGPU_VOSEL_CTRL[1] = %d\n", reg_val);
    pmic_read_interface(PMIC_ADDR_VGPU_EN_CTRL, &reg_val, 0x1, 0x0); //                    
    gpufreq_dbg("VGPU_EN_CTRL[0] = %d\n", reg_val);
    pmic_read_interface(PMIC_ADDR_VGPU_EN, &reg_val, 0x1, 0x0); //                
    gpufreq_dbg("VGPU_EN[0] = %d\n", reg_val);

    g_cur_freq_init_keep = g_cur_gpu_freq;

#if 1
    /*                     
                                                   
                           */
    reg_val = mt_gpufreq_volt_to_pmic_wrap(mt_gpufreqs[0].gpufreq_volt);
    mt_cpufreq_set_pmic_cmd(PMIC_WRAP_PHASE_NORMAL, IDX_NM_VGPU, reg_val);
#else
    mt_gpufreq_reg_write(0x02B0, PMIC_WRAP_DVFS_ADR2);

    reg_val = mt_gpufreq_volt_to_pmic_wrap(mt_gpufreqs[0].gpufreq_volt);
    mt_gpufreq_reg_write(reg_val, PMIC_WRAP_DVFS_WDATA2); //       
#endif

    /*                     
                              
                           */
    for (i = 0; i < mt_gpufreqs_num; i++) {
        //                                 
        if(mt_gpufreqs[i].gpufreq_khz == GPU_DVFS_FREQ3 || 
            mt_gpufreqs[i].gpufreq_khz == GPU_DVFS_FREQ4) {
            init_idx = i;
            break;
        }
    }

    gpufreq_dbg("init_idx = %d\n", init_idx);

    mt_gpufreq_set_initial(mt_gpufreqs[init_idx].gpufreq_idx);

    gpufreq_info("g_cur_gpu_freq = %d, g_cur_gpu_volt = %d\n", g_cur_gpu_freq, g_cur_gpu_volt);
    gpufreq_info("g_cur_gpu_idx = %d, g_cur_gpu_OPPidx = %d\n", g_cur_gpu_idx, g_cur_gpu_OPPidx);

    mt_gpufreq_ready = true;

#ifdef MT_GPUFREQ_INPUT_BOOST	
    mt_gpufreq_up_task = kthread_create(mt_gpufreq_input_boost_task, NULL, "mt_gpufreq_input_boost_task");
    if (IS_ERR(mt_gpufreq_up_task))
        return PTR_ERR(mt_gpufreq_up_task);

    sched_setscheduler_nocheck(mt_gpufreq_up_task, SCHED_FIFO, &param);
    get_task_struct(mt_gpufreq_up_task);

    rc = input_register_handler(&mt_gpufreq_input_handler);
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
    for (i = 0; i < mt_gpufreqs_num; i++) {
        if (mt_gpufreqs[i].gpufreq_khz == MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ_1) {
            mt_gpufreq_low_bat_volt_limited_index_1 = i;
            break;
        }
    }

    for (i = 0; i < mt_gpufreqs_num; i++) {
        if (mt_gpufreqs[i].gpufreq_khz == MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ_2) {
            mt_gpufreq_low_bat_volt_limited_index_2 = i;
            break;
        }
    }

    register_low_battery_notify(&mt_gpufreq_low_batt_volt_callback, LOW_BATTERY_PRIO_GPU);
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT	
    for (i = 0; i < mt_gpufreqs_num; i++) {
        if (mt_gpufreqs[i].gpufreq_khz == MT_GPUFREQ_LOW_BATT_VOLUME_LIMIT_FREQ_1) {
            mt_gpufreq_low_bat_volume_limited_index_1 = i;
            break;
        }
    }

    register_battery_percent_notify(&mt_gpufreq_low_batt_volume_callback, BATTERY_PERCENT_PRIO_GPU);
#endif

#ifdef MT_GPUFREQ_OC_PROTECT	
    for (i = 0; i < mt_gpufreqs_num; i++) {
        if (mt_gpufreqs[i].gpufreq_khz == MT_GPUFREQ_OC_LIMIT_FREQ_1) {
            mt_gpufreq_oc_limited_index_1 = i;
            break;
        }
    }

    register_battery_oc_notify(&mt_gpufreq_oc_callback, BATTERY_OC_PRIO_GPU);	
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_POLLING_TIMER
    mt_gpufreq_low_batt_volume_thread = kthread_run(mt_gpufreq_low_batt_volume_thread_handler, 0, "gpufreq low batt volume");
    if (IS_ERR(mt_gpufreq_low_batt_volume_thread))
        gpufreq_err("failed to create gpufreq_low_batt_volume thread\n");

    hrtimer_start(&mt_gpufreq_low_batt_volume_timer, ktime, HRTIMER_MODE_REL);
#endif

    return 0;
}

/*                                      
                                       
                                        */
static int mt_gpufreq_pdrv_remove(struct platform_device *pdev)
{
#ifdef MT_GPUFREQ_INPUT_BOOST
    input_unregister_handler(&mt_gpufreq_input_handler);

    kthread_stop(mt_gpufreq_up_task);
    put_task_struct(mt_gpufreq_up_task);
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_POLLING_TIMER
    kthread_stop(mt_gpufreq_low_batt_volume_thread);
    hrtimer_cancel(&mt_gpufreq_low_batt_volume_timer);
#endif

    return 0;
}


struct dev_pm_ops mt_gpufreq_pm_ops = {
    .suspend    = NULL,
    .resume     = NULL,
    .restore_early = mt_gpufreq_pm_restore_early,
};

struct platform_device mt_gpufreq_pdev = {
    .name   = "mt-gpufreq",
    .id     = -1,
};

static struct platform_driver mt_gpufreq_pdrv = {
    .probe      = mt_gpufreq_pdrv_probe,
    .remove     = mt_gpufreq_pdrv_remove,
    .driver     = {
        .name   = "mt-gpufreq",
        .pm     = &mt_gpufreq_pm_ops,
        .owner  = THIS_MODULE,
    },
};


#ifdef CONFIG_PROC_FS
/*
       
 */

/*                          
                            
                            */
static int mt_gpufreq_debug_proc_show(struct seq_file *m, void *v)
{    
    if (mt_gpufreq_debug)
        seq_printf(m, "gpufreq debug enabled\n");
    else
        seq_printf(m, "gpufreq debug disabled\n");

    return 0;
}

/*                      
                       
                        */
static ssize_t mt_gpufreq_debug_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{    
    char desc[32];
    int len = 0;

    int debug = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%d", &debug) == 1) {
        if (debug == 0) 
            mt_gpufreq_debug = 0;
        else if (debug == 1)
            mt_gpufreq_debug = 1;
        else
            gpufreq_warn("bad argument!! should be 0 or 1 [0: disable, 1: enable]\n");
    }
    else
        gpufreq_warn("bad argument!! should be 0 or 1 [0: disable, 1: enable]\n");

    return count; 
}

#ifdef MT_GPUFREQ_OC_PROTECT
/*                           
                                          
                             */
static int mt_gpufreq_limited_oc_ignore_proc_show(struct seq_file *m, void *v)
{    
    seq_printf(m, "g_limited_max_id = %d, g_limited_oc_ignore_state = %d\n", g_limited_max_id, g_limited_oc_ignore_state);

    return 0;
}

/*                                 
                                      
                                   */
static ssize_t mt_gpufreq_limited_oc_ignore_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{    
    char desc[32];
    int len = 0;

    unsigned int ignore = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%u", &ignore) == 1) {
        if(ignore == 1)
            g_limited_oc_ignore_state = true;
        else if(ignore == 0)
            g_limited_oc_ignore_state = false;
        else
            gpufreq_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");
    }
    else
        gpufreq_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");

    return count; 
}
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
/*                           
                                          
                             */
static int mt_gpufreq_limited_low_batt_volume_ignore_proc_show(struct seq_file *m, void *v)
{    
    seq_printf(m, "g_limited_max_id = %d, g_limited_low_batt_volume_ignore_state = %d\n", g_limited_max_id, g_limited_low_batt_volume_ignore_state);

    return 0;
}

/*                                 
                                      
                                   */
static ssize_t mt_gpufreq_limited_low_batt_volume_ignore_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{    
    char desc[32];
    int len = 0;

    unsigned int ignore = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%u", &ignore) == 1) {
        if(ignore == 1)
            g_limited_low_batt_volume_ignore_state = true;
        else if(ignore == 0)
            g_limited_low_batt_volume_ignore_state = false;
        else
            gpufreq_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");
    }
    else
        gpufreq_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");

    return count; 
}
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
/*                           
                                        
                             */
static int mt_gpufreq_limited_low_batt_volt_ignore_proc_show(struct seq_file *m, void *v)
{    
    seq_printf(m, "g_limited_max_id = %d, g_limited_low_batt_volt_ignore_state = %d\n", g_limited_max_id, g_limited_low_batt_volt_ignore_state);

    return 0;
}

/*                                 
                                    
                                   */
static ssize_t mt_gpufreq_limited_low_batt_volt_ignore_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{    
    char desc[32];
    int len = 0;

    unsigned int ignore = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%u", &ignore) == 1) {
        if(ignore == 1)
            g_limited_low_batt_volt_ignore_state = true;
        else if(ignore == 0)
            g_limited_low_batt_volt_ignore_state = false;
        else
            gpufreq_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");
    }
    else
        gpufreq_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");

    return count; 
}
#endif

/*                           
                                  
                             */
static int mt_gpufreq_limited_thermal_ignore_proc_show(struct seq_file *m, void *v)
{    
    seq_printf(m, "g_limited_max_id = %d, g_limited_thermal_ignore_state = %d\n", g_limited_max_id, g_limited_thermal_ignore_state);

    return 0;
}

/*                                 
                              
                                   */
static ssize_t mt_gpufreq_limited_thermal_ignore_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{    
    char desc[32];
    int len = 0;

    unsigned int ignore = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%u", &ignore) == 1) {
        if(ignore == 1)
            g_limited_thermal_ignore_state = true;
        else if(ignore == 0)
            g_limited_thermal_ignore_state = false;
        else
            gpufreq_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");
    }
    else
        gpufreq_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");

    return count; 
}

/*                           
                             
                             */
static int mt_gpufreq_limited_power_proc_show(struct seq_file *m, void *v)
{    

    seq_printf(m, "g_limited_max_id = %d, limit frequency = %d\n", g_limited_max_id, mt_gpufreqs[g_limited_max_id].gpufreq_khz);

    return 0;
}

/*                                 
                                    
                                   */
static ssize_t mt_gpufreq_limited_power_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{    
    char desc[32];
    int len = 0;

    unsigned int power = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%u", &power) == 1)
        mt_gpufreq_thermal_protect(power);
    else
        gpufreq_warn("bad argument!! please provide the maximum limited power\n");

    return count; 
}

/*                             
                               
                               */
static int mt_gpufreq_state_proc_show(struct seq_file *m, void *v)
{    
    if (!mt_gpufreq_pause)
        seq_printf(m, "GPU DVFS enabled\n");
    else
        seq_printf(m, "GPU DVFS disabled\n");

    return 0;
}

/*                                       
                                         
                                         */
static ssize_t mt_gpufreq_state_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{    
    char desc[32];
    int len = 0;

    int enabled = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%d", &enabled) == 1) {
        if (enabled == 1) {
            mt_gpufreq_keep_max_frequency_state = false;
            mt_gpufreq_state_set(1);
        }
        else if (enabled == 0) {
            /*                                            */
            mt_gpufreq_keep_max_frequency_state = true;
            mt_gpufreq_voltage_enable_set(1);
            mt_gpufreq_target(g_gpufreq_max_id);
            mt_gpufreq_state_set(0);
        }
        else
            gpufreq_warn("bad argument!! argument should be \"1\" or \"0\"\n");
    }
    else
        gpufreq_warn("bad argument!! argument should be \"1\" or \"0\"\n");

    return count; 
}

/*                   
                     
                     */
static int mt_gpufreq_opp_dump_proc_show(struct seq_file *m, void *v)
{
    int i = 0;

    for (i = 0; i < mt_gpufreqs_num; i++) {
        seq_printf(m, "[%d] ", i);
        seq_printf(m, "freq = %d, ", mt_gpufreqs[i].gpufreq_khz);
        seq_printf(m, "volt = %d, ", mt_gpufreqs[i].gpufreq_volt);
        seq_printf(m, "idx = %d\n", mt_gpufreqs[i].gpufreq_idx);

#if 0
        for (j = 0; j < ARRAY_SIZE(mt_gpufreqs_golden_power); j++)
        {
            if (mt_gpufreqs_golden_power[j].gpufreq_khz == mt_gpufreqs[i].gpufreq_khz)
            {
                p += sprintf(p, "power = %d\n", mt_gpufreqs_golden_power[j].gpufreq_power);
                break;
            }
        }
#endif
    }

    return 0;
}

/*                   
                       
                     */
static int mt_gpufreq_power_dump_proc_show(struct seq_file *m, void *v)
{
    int i = 0;

    for (i = 0; i < mt_gpufreqs_num; i++) {
        seq_printf(m, "mt_gpufreqs_power[%d].gpufreq_khz = %d \n", i, mt_gpufreqs_power[i].gpufreq_khz);
        seq_printf(m, "mt_gpufreqs_power[%d].gpufreq_power = %d \n", i, mt_gpufreqs_power[i].gpufreq_power);
    }

    return 0;
}

/*                          
                                         
                            */
static int mt_gpufreq_opp_freq_proc_show(struct seq_file *m, void *v)
{    
    if (mt_gpufreq_keep_opp_frequency_state) {
        seq_printf(m, "gpufreq keep opp frequency enabled\n");
        seq_printf(m, "freq = %d\n", mt_gpufreqs[mt_gpufreq_keep_opp_index].gpufreq_khz);
        seq_printf(m, "volt = %d\n", mt_gpufreqs[mt_gpufreq_keep_opp_index].gpufreq_volt);
    }
    else
        seq_printf(m, "gpufreq keep opp frequency disabled\n");

    return 0;
}

/*                      
                            
                        */
static ssize_t mt_gpufreq_opp_freq_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{    
    char desc[32];
    int len = 0;

    int i = 0;
    int fixed_freq = 0;
    int found = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%d", &fixed_freq) == 1)
    {
        if (fixed_freq == 0) {
            mt_gpufreq_keep_opp_frequency_state = false;
        }
        else {		
            for (i = 0; i < mt_gpufreqs_num; i++) {
                if(fixed_freq == mt_gpufreqs[i].gpufreq_khz) {
                    mt_gpufreq_keep_opp_index = i;
                    found = 1;
                    break;
                }
            }

            if(found == 1) {
                mt_gpufreq_keep_opp_frequency_state = true;
                mt_gpufreq_keep_opp_frequency = fixed_freq;

                mt_gpufreq_voltage_enable_set(1);
                mt_gpufreq_target(mt_gpufreq_keep_opp_index);
            }

        }
    }
    else
        gpufreq_warn("bad argument!! please provide the fixed frequency\n");

    return count; 
}

/*                          
                                         
                            */
static int mt_gpufreq_opp_max_freq_proc_show(struct seq_file *m, void *v)
{    
    if (mt_gpufreq_opp_max_frequency_state) {
        seq_printf(m, "gpufreq opp max frequency enabled\n");
        seq_printf(m, "freq = %d\n", mt_gpufreqs[mt_gpufreq_opp_max_index].gpufreq_khz);
        seq_printf(m, "volt = %d\n", mt_gpufreqs[mt_gpufreq_opp_max_index].gpufreq_volt);
    }
    else
        seq_printf(m, "gpufreq opp max frequency disabled\n");

    return 0;
}

/*                      
                            
                        */
static ssize_t mt_gpufreq_opp_max_freq_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{    
    char desc[32];
    int len = 0;

    int i = 0;
    int max_freq = 0;
    int found = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%d", &max_freq) == 1) {
        if (max_freq == 0) {
            mt_gpufreq_opp_max_frequency_state = false;
        }
        else {		
            for (i = 0; i < mt_gpufreqs_num; i++) {
                if(mt_gpufreqs[i].gpufreq_khz <= max_freq) {
                    mt_gpufreq_opp_max_index = i;
                    found = 1;
                    break;
                }
            }

            if(found == 1) {
                mt_gpufreq_opp_max_frequency_state = true;
                mt_gpufreq_opp_max_frequency = mt_gpufreqs[mt_gpufreq_opp_max_index].gpufreq_khz;

                mt_gpufreq_voltage_enable_set(1);
                mt_gpufreq_target(mt_gpufreq_opp_max_index);
            }
        }
    }
    else
        gpufreq_warn("bad argument!! please provide the maximum limited frequency\n");

    return count;
}

/*                   
                     
                     */
static int mt_gpufreq_var_dump_proc_show(struct seq_file *m, void *v)
{   
    int i = 0;

    seq_printf(m, "g_cur_gpu_freq = %d, g_cur_gpu_volt = %d\n", mt_gpufreq_get_cur_freq(), mt_gpufreq_get_cur_volt());
    seq_printf(m, "g_cur_gpu_idx = %d, g_cur_gpu_OPPidx = %d\n", g_cur_gpu_idx, g_cur_gpu_OPPidx);
    seq_printf(m, "g_limited_max_id = %d\n", g_limited_max_id);

    for (i = 0; i < NR_IDX_POWER_LIMITED; i++)
        seq_printf(m, "mt_gpufreq_power_limited_index_array[%d] = %d\n", i, mt_gpufreq_power_limited_index_array[i]);

    seq_printf(m, "mt_gpufreq_dvfs_get_gpu_freq = %d\n", mt_gpufreq_dvfs_get_gpu_freq());
    seq_printf(m, "mt_gpufreq_volt_enable_state = %d\n", mt_gpufreq_volt_enable_state);
    seq_printf(m, "mt_gpufreq_dvfs_table_type = %d\n", mt_gpufreq_dvfs_table_type);
    seq_printf(m, "mt_gpufreq_ptpod_disable_idx = %d\n", mt_gpufreq_ptpod_disable_idx);

    return 0;
}

/*                          
                                     
                            */
static int mt_gpufreq_volt_enable_proc_show(struct seq_file *m, void *v)
{    
    if (mt_gpufreq_volt_enable)
        seq_printf(m, "gpufreq voltage enabled\n");
    else
        seq_printf(m, "gpufreq voltage disabled\n");

    return 0;
}

/*                      
                            
                        */
static ssize_t mt_gpufreq_volt_enable_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{    
    char desc[32];
    int len = 0;

    int enable = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%d", &enable) == 1) {
        if (enable == 0) {
            mt_gpufreq_voltage_enable_set(0);
            mt_gpufreq_volt_enable = false;
        }
        else if (enable == 1) {
            mt_gpufreq_voltage_enable_set(1);
            mt_gpufreq_volt_enable = true;
        }
        else
            gpufreq_warn("bad argument!! should be 0 or 1 [0: disable, 1: enable]\n");
    }
    else
        gpufreq_warn("bad argument!! should be 0 or 1 [0: disable, 1: enable]\n");

    return count;
}

/*                          
                                         
                            */
static int mt_gpufreq_fixed_freq_volt_proc_show(struct seq_file *m, void *v)
{    
    if (mt_gpufreq_fixed_freq_volt_state) {
        seq_printf(m, "gpufreq fixed frequency enabled\n");
        seq_printf(m, "fixed frequency = %d\n", mt_gpufreq_fixed_frequency);
        seq_printf(m, "fixed voltage = %d\n", mt_gpufreq_fixed_voltage);
    }
    else
        seq_printf(m, "gpufreq fixed frequency disabled\n");

    return 0;
}

/*                      
                            
                        */
static ssize_t mt_gpufreq_fixed_freq_volt_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{    
    char desc[32];
    int len = 0;

    int fixed_freq = 0;
    int fixed_volt = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%d %d", &fixed_freq, &fixed_volt) == 2) {
        if((fixed_freq == 0) && (fixed_volt == 0)) {
            mt_gpufreq_fixed_freq_volt_state = false;
            mt_gpufreq_fixed_frequency = 0;
            mt_gpufreq_fixed_voltage = 0;
        }
        else {        
            //           
#ifdef MTK_TABLET_TURBO
            if((fixed_freq >= GPUFREQ_LAST_FREQ_LEVEL) && (fixed_freq <= GPU_DVFS_FREQT)) {
#else
            if((fixed_freq >= GPUFREQ_LAST_FREQ_LEVEL) && (fixed_freq <= GPU_DVFS_FREQ0)) {
#endif                
                mt_gpufreq_fixed_freq_volt_state = true;
                mt_gpufreq_fixed_frequency = fixed_freq;
                mt_gpufreq_fixed_voltage = g_cur_gpu_volt;
                mt_gpufreq_voltage_enable_set(1);
                mt_gpufreq_clock_switch(mt_gpufreq_fixed_frequency);
                g_cur_gpu_freq = mt_gpufreq_fixed_frequency;
            }
            
            //          
            if (fixed_volt >= (mt_gpufreq_pmic_wrap_to_volt(0x0) / 100) && 
                fixed_volt <= (mt_gpufreq_pmic_wrap_to_volt(0x7F) / 100)) {
                mt_gpufreq_fixed_freq_volt_state = true;
                mt_gpufreq_fixed_frequency = g_cur_gpu_freq;
                mt_gpufreq_fixed_voltage = fixed_volt * 100;
                mt_gpufreq_voltage_enable_set(1);
                mt_gpufreq_volt_switch(g_cur_gpu_volt, mt_gpufreq_fixed_voltage);
                g_cur_gpu_volt = mt_gpufreq_fixed_voltage;
            }
        }
    }
    else
        gpufreq_warn("bad argument!! should be [enable fixed_freq fixed_volt]\n");

    return count;
}

#ifdef MT_GPUFREQ_INPUT_BOOST
/*                            
                                  
                              */
static int mt_gpufreq_input_boost_proc_show(struct seq_file *m, void *v)
{    
    if (mt_gpufreq_input_boost_state == 1)
        seq_printf(m, "gpufreq debug enabled\n");
    else
        seq_printf(m, "gpufreq debug disabled\n");

    return 0;
}

/*                          
                             
                            */
static ssize_t mt_gpufreq_input_boost_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{    
    char desc[32];
    int len = 0;

    int debug = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%d", &debug) == 1) {
        if (debug == 0)
            mt_gpufreq_input_boost_state = 0;
        else if (debug == 1)
            mt_gpufreq_input_boost_state = 1;
        else
            gpufreq_warn("bad argument!! should be 0 or 1 [0: disable, 1: enable]\n");
    }
    else
        gpufreq_warn("bad argument!! should be 0 or 1 [0: disable, 1: enable]\n");

    return count; 
}
#endif

#define PROC_FOPS_RW(name)							\
    static int mt_ ## name ## _proc_open(struct inode *inode, struct file *file)	\
{									\
    return single_open(file, mt_ ## name ## _proc_show, PDE_DATA(inode));	\
}									\
static const struct file_operations mt_ ## name ## _proc_fops = {		\
    .owner          = THIS_MODULE,				\
    .open           = mt_ ## name ## _proc_open,	\
    .read           = seq_read,					\
    .llseek         = seq_lseek,					\
    .release        = single_release,				\
    .write          = mt_ ## name ## _proc_write,				\
}

#define PROC_FOPS_RO(name)							\
    static int mt_ ## name ## _proc_open(struct inode *inode, struct file *file)	\
{									\
    return single_open(file, mt_ ## name ## _proc_show, PDE_DATA(inode));	\
}									\
static const struct file_operations mt_ ## name ## _proc_fops = {		\
    .owner          = THIS_MODULE,				\
    .open           = mt_ ## name ## _proc_open,	\
    .read           = seq_read,					\
    .llseek         = seq_lseek,					\
    .release        = single_release,				\
}

#define PROC_ENTRY(name)	{__stringify(name), &mt_ ## name ## _proc_fops}

PROC_FOPS_RW(gpufreq_debug);
PROC_FOPS_RW(gpufreq_limited_power);
#ifdef MT_GPUFREQ_OC_PROTECT
PROC_FOPS_RW(gpufreq_limited_oc_ignore);
#endif
#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
PROC_FOPS_RW(gpufreq_limited_low_batt_volume_ignore);
#endif
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
PROC_FOPS_RW(gpufreq_limited_low_batt_volt_ignore);
#endif
PROC_FOPS_RW(gpufreq_limited_thermal_ignore);
PROC_FOPS_RW(gpufreq_state);
PROC_FOPS_RO(gpufreq_opp_dump);
PROC_FOPS_RO(gpufreq_power_dump);
PROC_FOPS_RW(gpufreq_opp_freq);
PROC_FOPS_RW(gpufreq_opp_max_freq);
PROC_FOPS_RO(gpufreq_var_dump);
PROC_FOPS_RW(gpufreq_volt_enable);
PROC_FOPS_RW(gpufreq_fixed_freq_volt);
#ifdef MT_GPUFREQ_INPUT_BOOST
PROC_FOPS_RW(gpufreq_input_boost);
#endif

static int mt_gpufreq_create_procfs(void)
{
    struct proc_dir_entry *dir = NULL;
    int i;

    struct pentry {
        const char *name;
        const struct file_operations *fops;
    };

    const struct pentry entries[] = {
        PROC_ENTRY(gpufreq_debug),
        PROC_ENTRY(gpufreq_limited_power),
#ifdef MT_GPUFREQ_OC_PROTECT
        PROC_ENTRY(gpufreq_limited_oc_ignore),
#endif
#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
        PROC_ENTRY(gpufreq_limited_low_batt_volume_ignore),
#endif
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
        PROC_ENTRY(gpufreq_limited_low_batt_volt_ignore),
#endif
        PROC_ENTRY(gpufreq_limited_thermal_ignore),
        PROC_ENTRY(gpufreq_state),
        PROC_ENTRY(gpufreq_opp_dump),
        PROC_ENTRY(gpufreq_power_dump),
        PROC_ENTRY(gpufreq_opp_freq),
        PROC_ENTRY(gpufreq_opp_max_freq),
        PROC_ENTRY(gpufreq_var_dump),
        PROC_ENTRY(gpufreq_volt_enable),
        PROC_ENTRY(gpufreq_fixed_freq_volt),
#ifdef MT_GPUFREQ_INPUT_BOOST
        PROC_ENTRY(gpufreq_input_boost),
#endif
    };


    dir = proc_mkdir("gpufreq", NULL);

    if (!dir) {
        gpufreq_err("fail to create /proc/gpufreq @ %s()\n", __func__);
        return -ENOMEM;
    }

    for (i = 0; i < ARRAY_SIZE(entries); i++) {
        if (!proc_create(entries[i].name, S_IRUGO | S_IWUSR | S_IWGRP, dir, entries[i].fops))
            gpufreq_err("@%s: create /proc/gpufreq/%s failed\n", __func__, entries[i].name);
    }

    return 0;
}
#endif /*                */

/*                                 
                                  
                                   */
static int __init mt_gpufreq_init(void)
{
    int ret = 0;

    gpufreq_info("@%s\n", __func__);
    
#ifdef CONFIG_PROC_FS

    /*           */
    if (mt_gpufreq_create_procfs())
        goto out;

#endif /*                */

#if 0	
    clk_cfg_0 = DRV_Reg32(CLK_CFG_0);
    clk_cfg_0 = (clk_cfg_0 & 0x00070000) >> 16;

    gpufreq_dbg("clk_cfg_0 = %d\n", clk_cfg_0);

    switch (clk_cfg_0)
    {
        case 0x5: //       
            g_cur_freq = GPU_MMPLL_D3;
            break;
        case 0x2: //       
            g_cur_freq = GPU_SYSPLL_D2;
            break;
        case 0x6: //       
            g_cur_freq = GPU_MMPLL_D4;
            break;
        case 0x4: //       
            g_cur_freq = GPU_UNIVPLL1_D2;
            break;
        case 0x7: //       
            g_cur_freq = GPU_MMPLL_D5;
            break;
        case 0x3: //       
            g_cur_freq = GPU_SYSPLL_D3;
            break;
        case 0x1: //       
            g_cur_freq = GPU_MMPLL_D6;
            break;
        case 0x0: //       
            g_cur_freq = GPU_UNIVPLL1_D4;
            break;
        default:
            break;
    }


    g_cur_freq_init_keep = g_cur_gpu_freq;
    gpufreq_dbg("g_cur_freq_init_keep = %d\n", g_cur_freq_init_keep);
#endif

    /*                                 */
    ret = platform_device_register(&mt_gpufreq_pdev);
    if (ret) {
        gpufreq_err("fail to register gpufreq device @ %s()\n", __func__);
        goto out;
    }
    
    ret = platform_driver_register(&mt_gpufreq_pdrv);
    if (ret) {
        gpufreq_err("fail to register gpufreq driver @ %s()\n", __func__);
        platform_device_unregister(&mt_gpufreq_pdev);
    }

out:
    return ret;

}

static void __exit mt_gpufreq_exit(void)
{
    platform_driver_unregister(&mt_gpufreq_pdrv);
    platform_device_unregister(&mt_gpufreq_pdev);
}

module_init(mt_gpufreq_init);
module_exit(mt_gpufreq_exit);

MODULE_DESCRIPTION("MediaTek GPU Frequency Scaling driver");
MODULE_LICENSE("GPL");

