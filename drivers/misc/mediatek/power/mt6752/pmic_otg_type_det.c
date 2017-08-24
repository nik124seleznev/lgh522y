#include <generated/autoconf.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/wakelock.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/aee.h>
#include <linux/xlog.h>
#include <linux/proc_fs.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/writeback.h>
#include <linux/earlysuspend.h>
#include <linux/seq_file.h>

#include <asm/uaccess.h>

#include <mach/upmu_common.h>
#include <mach/upmu_sw.h>
#include <mach/upmu_hw.h>
#include <mach/mt_pm_ldo.h>
#include <mach/mt_pmic_wrap.h>
#include <mach/mt_gpio.h>
#include <mach/mtk_rtc.h>
#include <mach/mt_spm_mtcmos.h>

#include <mach/battery_common.h>
#include <linux/time.h>


typedef enum {
    ACA_UNKNOWN = 0,
    ACA_DOCK, //                         
    ACA_A,     //                          
    ACA_B,     //                 
    ACA_C,    //                           
} ACA_TYPE;
//                                                                
//               
//                                                                
extern kal_uint32 upmu_get_reg_value(kal_uint32 reg);
extern void Charger_Detect_Init(void);
extern void Charger_Detect_Release(void);

#if defined(CONFIG_MTK_FPGA)

int hw_otg_get_aca_type(void)
{
    return ACA_UNKNOWN;
}

#else

static void hw_bc11_aca_dump_register(void)
{
    battery_xlog_printk(BAT_LOG_FULL, "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_CHR_CON20, upmu_get_reg_value(MT6325_CHR_CON20),
        MT6325_CHR_CON21, upmu_get_reg_value(MT6325_CHR_CON21)
        );
}

static void hw_bc11_aca_init(void)
{
    //            
    Charger_Detect_Init(); //            
        
    //                     
    mt6325_upmu_set_rg_bc11_bias_en(0x1);
    //                       
    mt6325_upmu_set_rg_bc11_vsrc_en(0x0);
    //                           
    mt6325_upmu_set_rg_bc11_vref_vth(0x0);
    //                        
    mt6325_upmu_set_rg_bc11_cmp_en(0x0);
    //                        
    mt6325_upmu_set_rg_bc11_ipu_en(0x0);
    //                        
    mt6325_upmu_set_rg_bc11_ipd_en(0x0);
    //          
    mt6325_upmu_set_rg_bc11_rst(0x1);
    //                
    mt6325_upmu_set_rg_bc11_aca_en(0x1);
    //              
    mt6325_upmu_set_rg_bc11_bb_ctrl(0x1);

    //           
    udelay(100); //          
    
    if(Enable_BATDRV_LOG == BAT_LOG_FULL)
    {
        battery_xlog_printk(BAT_LOG_FULL, "hw_bc11_aca_init() \r\n");
        hw_bc11_aca_dump_register();
    }    

}
 
 
static U32 hw_bc11_ACA(void)
{
    U32 check_id_a = 0;

    check_id_a = mt6325_upmu_get_rgs_bc11_id_a();
    
    udelay(200); //          

    return check_id_a;
}
 
 
static U32 hw_bc11_ACA_stepA1(void)
{
   U32 adapterAvail = 0;
     
   //                                              
   mt6325_upmu_set_rg_bc11_vref_vth(0x0);   
   //                                               
   mt6325_upmu_set_rg_bc11_cmp_en(0x1);

   udelay(200);

   adapterAvail = mt6325_upmu_get_rgs_bc11_cmp_out();

   if(Enable_BATDRV_LOG == BAT_LOG_FULL)
   {
       battery_xlog_printk(BAT_LOG_FULL, "hw_bc11_aca_stepA1() \r\n");
       hw_bc11_aca_dump_register();
   }

   return  adapterAvail;
}
 
static U32 hw_bc11_ACA_stepA2(void)
{
   U32 adapterAvail = 0;
     
   //                                            
   mt6325_upmu_set_rg_bc11_vref_vth(0x01);   

   //                                                   
   //                                               
   //                                    

   udelay(200);

   adapterAvail = mt6325_upmu_get_rgs_bc11_cmp_out();

   if(Enable_BATDRV_LOG == BAT_LOG_FULL)
   {
       battery_xlog_printk(BAT_LOG_FULL, "hw_bc11_aca_stepA2() \r\n");
       hw_bc11_aca_dump_register();
   }

   return  adapterAvail;
} 
static U32 hw_bc11_ACA_stepB(void)
{
   U32 adapterAvail = 0;

   adapterAvail = mt6325_upmu_get_rgs_bc11_id_b();

   if(Enable_BATDRV_LOG == BAT_LOG_FULL)
   {
       battery_xlog_printk(BAT_LOG_FULL, "hw_bc11_acs_stepB() \r\n");
       hw_bc11_aca_dump_register();
   }

   return  adapterAvail;
}

static U32 hw_bc11_ACA_stepC(void)
{
   U32 adapterAvail = 0;

   adapterAvail = mt6325_upmu_get_rgs_bc11_id_c();

   if(Enable_BATDRV_LOG == BAT_LOG_FULL)
   {
       battery_xlog_printk(BAT_LOG_FULL, "hw_bc11_aca_stepC() \r\n");
       hw_bc11_aca_dump_register();
   }

   return  adapterAvail;
} 
static void hw_bc11_aca_done(void)
{
   //                       
   mt6325_upmu_set_rg_bc11_vsrc_en(0x0);
   //                                             
   mt6325_upmu_set_rg_bc11_vref_vth(0x0);
   //                        
   mt6325_upmu_set_rg_bc11_cmp_en(0x0);
   //                        
   mt6325_upmu_set_rg_bc11_ipu_en(0x0);
   //                        
   mt6325_upmu_set_rg_bc11_ipd_en(0x0);
    //                
    mt6325_upmu_set_rg_bc11_aca_en(0x0);
   //                 
   mt6325_upmu_set_rg_bc11_bias_en(0x0); 

   Charger_Detect_Release();

   if(Enable_BATDRV_LOG == BAT_LOG_FULL)
   {
       battery_xlog_printk(BAT_LOG_FULL, "hw_bc11_aca_done() \r\n");
       hw_bc11_aca_dump_register();
   }
   
}

int hw_otg_get_aca_type(void)
{
#if 0
    return STANDARD_HOST;
    //                                  
#else
    ACA_TYPE ACA_Type_num = ACA_UNKNOWN;
    
    /*                                     */         
    hw_bc11_aca_init();
 
    /*                                */  
    if(1 == hw_bc11_ACA())
    {
         /*                                  */
         if(1 == hw_bc11_ACA_stepA1())
         {             
             ACA_Type_num = ACA_A;
             battery_xlog_printk(BAT_LOG_CRTI, "step A1 : ACA_A, B device on accessory port!\r\n");
         }
         else
         {
             if(1 == hw_bc11_ACA_stepA2())
             {             
                ACA_Type_num = ACA_A;
                battery_xlog_printk(BAT_LOG_CRTI, "step A2 : ACA_A, B device on accessory port!\r\n");
             }
             else
             {
			 	ACA_Type_num = ACA_DOCK;
                battery_xlog_printk(BAT_LOG_CRTI, "step A2 : ACA_DOCK, has external power source!\r\n");
             }
		 }
    }
    else
    {
         /*                                 */
         if(1 == hw_bc11_ACA_stepB())
         {
            ACA_Type_num = ACA_B;
            battery_xlog_printk(BAT_LOG_CRTI, "step B : ACA_B, only for charging!\r\n");

         }
         else
         {
             
            /*                                 */
            if(1 == hw_bc11_ACA_stepC())
            {
               ACA_Type_num = ACA_C;
               battery_xlog_printk(BAT_LOG_CRTI, "step C : ACA_C, A device on accessory port!\r\n");

            }
            else
            {
               ACA_Type_num = ACA_UNKNOWN;
               battery_xlog_printk(BAT_LOG_CRTI, "step C : ACA_UNKNOWN !\r\n");
            }
         }
 
    }
 
    /*                                                       */
    hw_bc11_aca_done();

    return ACA_Type_num;
#endif    
}
#endif
