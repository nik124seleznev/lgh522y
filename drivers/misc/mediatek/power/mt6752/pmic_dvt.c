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
#include <linux/time.h>

#include <asm/uaccess.h>

#include <mach/upmu_common.h>
#include <mach/upmu_sw.h>
#include <mach/upmu_hw.h>
#include <mach/mt_pm_ldo.h>
#include <mach/eint.h>
#include <mach/mt_pmic_wrap.h>
#include <mach/mt_gpio.h>
#include <mach/mtk_rtc.h>
#include <mach/mt_spm_mtcmos.h>

#include <mach/battery_common.h>
#include <mach/pmic_mt6325_sw.h>
#include <mach/mt6311.h>

#include <cust_pmic.h>
#include <cust_battery_meter.h>

#include <linux/rtc.h>
//                                        
//           
//                                        
//    
#define BUCK_ON_OFF_DVT_ENABLE
#define BUCK_VOSEL_DVT_ENABLE
#define BUCK_DLC_DVT_ENABLE
#define BUCK_BURST_DVT_ENABLE
//   
#define LDO_ON_OFF_DVT_ENABLE
#define LDO_VOSEL_DVT_ENABLE
#define LDO_CAL_DVT_ENABLE
#define LDO_MODE_DVT_ENABLE
//   
#define BIF_DVT_ENABLE
//   
#define TOP_DVT_ENABLE
//     
#define EFUSE_DVT_ENABLE
//      
#define AUXADC_DVT_ENABLE

//                                        
//        
//                                        
extern kal_uint32 upmu_get_reg_value(kal_uint32 reg);
extern void upmu_set_reg_value(kal_uint32 reg, kal_uint32 reg_val);
extern U32 pmic_config_interface (U32 RegNum, U32 val, U32 MASK, U32 SHIFT);
extern int PMIC_IMM_GetOneChannelValue(upmu_adc_chl_list_enum dwChannel, int deCount, int trimd);
extern void dump_ldo_status_read_debug(void);
extern void mt6311_set_reg_value(kal_uint32 reg, kal_uint32 reg_val);
extern kal_uint32 mt6311_get_reg_value(kal_uint32 reg);

//                                        
//     
//                                        

//           
#define VCORE1_INDEX  0
#define VCORE2_INDEX  1
#define VDRAM_INDEX   2
#define VGPU_INDEX    3
#define VRF18_0_INDEX 4
#define VIO18_INDEX   5
#define VDVFS11_INDEX 6
#define VDVFS12_INDEX 7
#define VPA_INDEX     8

//      
#define LDO_ONLY      99
#define ADC_ONLY      100

//           
#define MT6311_VDVFS11_INDEX 101
#define MT6311_VDVFS12_INDEX 102

void read_adc_value(int index)
{
    int ret=1234;

    mdelay(50);
    
    ret = PMIC_IMM_GetOneChannelValue(AUX_ISENSE_AP,1,0);    
    
    printk("[read_auxadc_value] ret = %d\n", ret);

#if 1
    //                                                            
    if(index == VCORE1_INDEX) {
        printk("mt6325_upmu_get_qi_vcore1_en=%d\n", mt6325_upmu_get_qi_vcore1_en());
        printk("mt6325_upmu_get_ni_vcore1_vosel=0x%x\n", mt6325_upmu_get_ni_vcore1_vosel());
    } 
    else if(index == VCORE2_INDEX) {
        printk("mt6325_upmu_get_qi_vcore2_en=%d\n", mt6325_upmu_get_qi_vcore2_en());
        printk("mt6325_upmu_get_ni_vcore2_vosel=0x%x\n", mt6325_upmu_get_ni_vcore2_vosel());
    }
    else if(index == VDRAM_INDEX) {
        printk("mt6325_upmu_get_qi_vdram_en=%d\n", mt6325_upmu_get_qi_vdram_en());
        printk("mt6325_upmu_get_ni_vdram_vosel=0x%x\n", mt6325_upmu_get_ni_vdram_vosel());
    }
    else if(index == VGPU_INDEX) {
        printk("mt6325_upmu_get_qi_vgpu_en=%d\n", mt6325_upmu_get_qi_vgpu_en());
        printk("mt6325_upmu_get_ni_vgpu_vosel=0x%x\n", mt6325_upmu_get_ni_vgpu_vosel());
    }
    else if(index == VRF18_0_INDEX) {
        printk("mt6325_upmu_get_qi_vrf18_0_en=%d\n", mt6325_upmu_get_qi_vrf18_0_en());
        printk("mt6325_upmu_get_ni_vrf18_0_vosel=0x%x\n", mt6325_upmu_get_ni_vrf18_0_vosel());
    }
    else if(index == VIO18_INDEX) {
        printk("mt6325_upmu_get_qi_vio18_en=%d\n", mt6325_upmu_get_qi_vio18_en());
        printk("mt6325_upmu_get_ni_vio18_vosel=0x%x\n", mt6325_upmu_get_ni_vio18_vosel());
    }
    else if(index == VDVFS11_INDEX) {
        printk("mt6325_upmu_get_qi_vdvfs11_en=%d\n", mt6325_upmu_get_qi_vdvfs11_en());
        printk("mt6325_upmu_get_ni_vdvfs11_vosel=0x%x\n", mt6325_upmu_get_ni_vdvfs11_vosel());
    }
    else if(index == VDVFS12_INDEX) {
        printk("mt6325_upmu_get_qi_vdvfs12_en=%d\n", mt6325_upmu_get_qi_vdvfs12_en());
        printk("mt6325_upmu_get_ni_vdvfs12_vosel=0x%x\n", mt6325_upmu_get_ni_vdvfs12_vosel());
    }
    else if(index == VPA_INDEX) {
        printk("mt6325_upmu_get_qi_vpa_en=%d\n", mt6325_upmu_get_qi_vpa_en());
        printk("mt6325_upmu_get_ni_vpa_vosel=0x%x\n", mt6325_upmu_get_ni_vpa_vosel());
    }
    else if(index == MT6311_VDVFS11_INDEX) {
        printk("mt6311_get_qi_vdvfs11_en=%d\n", mt6311_get_qi_vdvfs11_en());
        printk("mt6311_get_ni_vdvfs11_vosel=0x%x\n", mt6311_get_ni_vdvfs11_vosel());
    }
    else if(index == MT6311_VDVFS12_INDEX) {
        printk("mt6311_get_qi_vdvfs12_en=%d\n", mt6311_get_qi_vdvfs12_en());
        printk("mt6311_get_ni_vdvfs12_vosel=0x%x\n", mt6311_get_ni_vdvfs12_vosel());
    }
    else if(index == LDO_ONLY) {
        dump_ldo_status_read_debug();
        printk("mt6311_get_qi_vbiasn_en=%d\n", mt6311_get_qi_vbiasn_en());
    }
    else
    {        
    }
#endif    

    printk("\n");
}

void set_srclken_sw_mode(void)
{
    //               
    //         
    //         

#if 1        
    mt6325_upmu_set_rg_srclken_in0_hw_mode(0);
    mt6325_upmu_set_rg_srclken_in1_hw_mode(0);
    printk("[25 SRCLKEN] Reg[0x%x]=0x%x, [5:4]\n", MT6325_TOP_CON, upmu_get_reg_value(MT6325_TOP_CON));    
#endif

}

void set_srclken_0_val(int val)
{
    //            
    //             
    
#if 1      
    mt6325_upmu_set_rg_srclken_in0_en(val);
    printk("[25 SRCLKEN=%d] Reg[0x%x]=0x%x, [bit 0]\n", val, MT6325_TOP_CON, upmu_get_reg_value(MT6325_TOP_CON));    
#endif    
}

void set_srclken_1_val(int val)
{
    //            
    //             
    
#if 1        
    mt6325_upmu_set_rg_srclken_in1_en(val);
    printk("[25 SRCLKEN=%d] Reg[0x%x]=0x%x, [bit 1]\n", val, MT6325_TOP_CON, upmu_get_reg_value(MT6325_TOP_CON));    
#endif    
}

void set_mt6311_srclken_sw_mode(void)
{
    //               
    //         
    //         

#if 1        
     mt6311_set_rg_srclken_in_hw_mode(0);
     printk("[6311 SRCLKEN] Reg[0x%x]=0x%x, [bit 0]\n", MT6311_TOP_CON, mt6311_get_reg_value(MT6311_TOP_CON));    
#endif

}

void set_mt6311_srclken_val(int val)
{
    //            
    //             
    
#if 1      
    mt6311_set_rg_srclken_in_en(val);
    printk("[6311 SRCLKEN=%d] Reg[0x%x]=0x%x, [bit 1]\n", val, MT6311_TOP_CON, mt6311_get_reg_value(MT6311_TOP_CON));    
#endif    
}

void exec_scrxxx_map(int buck_index)
{
    if(buck_index < 100)
    {
        set_srclken_sw_mode();

        #if 0 //                            
        printk("[exec_scrxxx_map] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                  set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(buck_index);      
                                  
        printk("[exec_scrxxx_map] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                  set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(buck_index);  

        printk("[exec_scrxxx_map] restore to set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                  set_srclken_1_val(1); set_srclken_0_val(1);                          
        #else
        printk("[exec_scrxxx_map] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                                  set_srclken_0_val(0); set_srclken_1_val(0); read_adc_value(buck_index);

        printk("[exec_scrxxx_map] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                                  set_srclken_0_val(0); set_srclken_1_val(1); read_adc_value(buck_index);

        printk("[exec_scrxxx_map] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                                  set_srclken_0_val(1); set_srclken_1_val(0); read_adc_value(buck_index);

        printk("[exec_scrxxx_map] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                                  set_srclken_0_val(1); set_srclken_1_val(1); read_adc_value(buck_index);    
        #endif                          
    }
    else
    {
        set_mt6311_srclken_sw_mode();

        printk("[exec_scrxxx_map] set_mt6311_srclken_val(0);\n");
                                  set_mt6311_srclken_val(0);read_adc_value(buck_index);

        printk("[exec_scrxxx_map] set_mt6311_srclken_val(1);\n");
                                  set_mt6311_srclken_val(1);read_adc_value(buck_index);        
    }
}

#ifdef BUCK_ON_OFF_DVT_ENABLE
void exec_vcore1_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vcore1_en_ctrl] %d\n", i);
        mt6325_upmu_set_vcore1_en_ctrl(i);

        switch(i){
            case 0:
                printk("[mt6325_upmu_set_vcore1_en(0)]\n");
                mt6325_upmu_set_vcore1_en(0);
                read_adc_value(VCORE1_INDEX);

                printk("[mt6325_upmu_set_vcore1_en(1)]\n");
                mt6325_upmu_set_vcore1_en(1);
                read_adc_value(VCORE1_INDEX);
                break;

            case 1:
                printk("[mt6325_upmu_set_vcore1_en_sel(0)]\n");
                mt6325_upmu_set_vcore1_en_sel(0);
                exec_scrxxx_map(VCORE1_INDEX);                       

                printk("[mt6325_upmu_set_vcore1_en_sel(1)]\n");
                mt6325_upmu_set_vcore1_en_sel(1);
                exec_scrxxx_map(VCORE1_INDEX);

                printk("[mt6325_upmu_set_vcore1_en_sel(2)]\n");
                mt6325_upmu_set_vcore1_en_sel(2);
                exec_scrxxx_map(VCORE1_INDEX);

                printk("[mt6325_upmu_set_vcore1_en_sel(3)]\n");
                mt6325_upmu_set_vcore1_en_sel(3);
                exec_scrxxx_map(VCORE1_INDEX);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vcore2_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vcore2_en_ctrl] %d\n", i);
        mt6325_upmu_set_vcore2_en_ctrl(i);

        switch(i){
            case 0:
                printk("[mt6325_upmu_set_vcore2_en(0)]\n");
                mt6325_upmu_set_vcore2_en(0);
                read_adc_value(VCORE2_INDEX);

                printk("[mt6325_upmu_set_vcore2_en(1)]\n");
                mt6325_upmu_set_vcore2_en(1);
                read_adc_value(VCORE2_INDEX);
                break;

            case 1:
                printk("[mt6325_upmu_set_vcore2_en_sel(0)]\n");
                mt6325_upmu_set_vcore2_en_sel(0);
                exec_scrxxx_map(VCORE2_INDEX);                       

                printk("[mt6325_upmu_set_vcore2_en_sel(1)]\n");
                mt6325_upmu_set_vcore2_en_sel(1);
                exec_scrxxx_map(VCORE2_INDEX);

                printk("[mt6325_upmu_set_vcore2_en_sel(2)]\n");
                mt6325_upmu_set_vcore2_en_sel(2);
                exec_scrxxx_map(VCORE2_INDEX);

                printk("[mt6325_upmu_set_vcore2_en_sel(3)]\n");
                mt6325_upmu_set_vcore2_en_sel(3);
                exec_scrxxx_map(VCORE2_INDEX);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vdram_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vdram_en_ctrl] %d\n", i);
        mt6325_upmu_set_vdram_en_ctrl(i);

        switch(i){
            case 0:
                printk("[mt6325_upmu_set_vdram_en(0)]\n");
                mt6325_upmu_set_vdram_en(0);
                read_adc_value(VDRAM_INDEX);

                printk("[mt6325_upmu_set_vdram_en(1)]\n");
                mt6325_upmu_set_vdram_en(1);
                read_adc_value(VDRAM_INDEX);
                break;

            case 1:
                printk("[mt6325_upmu_set_vdram_en_sel(0)]\n");
                mt6325_upmu_set_vdram_en_sel(0);
                exec_scrxxx_map(VDRAM_INDEX);                       

                printk("[mt6325_upmu_set_vdram_en_sel(1)]\n");
                mt6325_upmu_set_vdram_en_sel(1);
                exec_scrxxx_map(VDRAM_INDEX);

                printk("[mt6325_upmu_set_vdram_en_sel(2)]\n");
                mt6325_upmu_set_vdram_en_sel(2);
                exec_scrxxx_map(VDRAM_INDEX);

                printk("[mt6325_upmu_set_vdram_en_sel(3)]\n");
                mt6325_upmu_set_vdram_en_sel(3);
                exec_scrxxx_map(VDRAM_INDEX);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vgpu_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vgpu_en_ctrl] %d\n", i);
        mt6325_upmu_set_vgpu_en_ctrl(i);

        switch(i){
            case 0:
                printk("[mt6325_upmu_set_vgpu_en(0)]\n");
                mt6325_upmu_set_vgpu_en(0);
                read_adc_value(VGPU_INDEX);

                printk("[mt6325_upmu_set_vgpu_en(1)]\n");
                mt6325_upmu_set_vgpu_en(1);
                read_adc_value(VGPU_INDEX);
                break;

            case 1:
                printk("[mt6325_upmu_set_vgpu_en_sel(0)]\n");
                mt6325_upmu_set_vgpu_en_sel(0);
                exec_scrxxx_map(VGPU_INDEX);                       

                printk("[mt6325_upmu_set_vgpu_en_sel(1)]\n");
                mt6325_upmu_set_vgpu_en_sel(1);
                exec_scrxxx_map(VGPU_INDEX);

                printk("[mt6325_upmu_set_vgpu_en_sel(2)]\n");
                mt6325_upmu_set_vgpu_en_sel(2);
                exec_scrxxx_map(VGPU_INDEX);

                printk("[mt6325_upmu_set_vgpu_en_sel(3)]\n");
                mt6325_upmu_set_vgpu_en_sel(3);
                exec_scrxxx_map(VGPU_INDEX);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vrf18_0_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vrf18_0_en_ctrl] %d\n", i);
        mt6325_upmu_set_vrf18_0_en_ctrl(i);

        switch(i){
            case 0:
                printk("[mt6325_upmu_set_vrf18_0_en(0)]\n");
                mt6325_upmu_set_vrf18_0_en(0);
                read_adc_value(VRF18_0_INDEX);

                printk("[mt6325_upmu_set_vrf18_0_en(1)]\n");
                mt6325_upmu_set_vrf18_0_en(1);
                read_adc_value(VRF18_0_INDEX);
                break;

            case 1:
                printk("[mt6325_upmu_set_vrf18_0_en_sel(0)]\n");
                mt6325_upmu_set_vrf18_0_en_sel(0);
                exec_scrxxx_map(VRF18_0_INDEX);                       

                printk("[mt6325_upmu_set_vrf18_0_en_sel(1)]\n");
                mt6325_upmu_set_vrf18_0_en_sel(1);
                exec_scrxxx_map(VRF18_0_INDEX);

                printk("[mt6325_upmu_set_vrf18_0_en_sel(2)]\n");
                mt6325_upmu_set_vrf18_0_en_sel(2);
                exec_scrxxx_map(VRF18_0_INDEX);

                printk("[mt6325_upmu_set_vrf18_0_en_sel(3)]\n");
                mt6325_upmu_set_vrf18_0_en_sel(3);
                exec_scrxxx_map(VRF18_0_INDEX);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vio18_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vio18_en_ctrl] %d\n", i);
        mt6325_upmu_set_vio18_en_ctrl(i);

        switch(i){
            case 0:
                printk("[mt6325_upmu_set_vio18_en(0)]\n");
                mt6325_upmu_set_vio18_en(0);
                read_adc_value(VIO18_INDEX);

                printk("[mt6325_upmu_set_vio18_en(1)]\n");
                mt6325_upmu_set_vio18_en(1);
                read_adc_value(VIO18_INDEX);
                break;

            case 1:
                printk("[mt6325_upmu_set_vio18_en_sel(0)]\n");
                mt6325_upmu_set_vio18_en_sel(0);
                exec_scrxxx_map(VIO18_INDEX);                       

                printk("[mt6325_upmu_set_vio18_en_sel(1)]\n");
                mt6325_upmu_set_vio18_en_sel(1);
                exec_scrxxx_map(VIO18_INDEX);

                printk("[mt6325_upmu_set_vio18_en_sel(2)]\n");
                mt6325_upmu_set_vio18_en_sel(2);
                exec_scrxxx_map(VIO18_INDEX);

                printk("[mt6325_upmu_set_vio18_en_sel(3)]\n");
                mt6325_upmu_set_vio18_en_sel(3);
                exec_scrxxx_map(VIO18_INDEX);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vdvfs11_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vdvfs11_en_ctrl] %d\n", i);
        mt6325_upmu_set_vdvfs11_en_ctrl(i);

        switch(i){
            case 0:
                printk("[mt6325_upmu_set_vdvfs11_en(0)]\n");
                mt6325_upmu_set_vdvfs11_en(0);
                read_adc_value(VDVFS11_INDEX);

                printk("[mt6325_upmu_set_vdvfs11_en(1)]\n");
                mt6325_upmu_set_vdvfs11_en(1);
                read_adc_value(VDVFS11_INDEX);
                break;

            case 1:
                printk("[mt6325_upmu_set_vdvfs11_en_sel(0)]\n");
                mt6325_upmu_set_vdvfs11_en_sel(0);
                exec_scrxxx_map(VDVFS11_INDEX);                       

                printk("[mt6325_upmu_set_vdvfs11_en_sel(1)]\n");
                mt6325_upmu_set_vdvfs11_en_sel(1);
                exec_scrxxx_map(VDVFS11_INDEX);

                printk("[mt6325_upmu_set_vdvfs11_en_sel(2)]\n");
                mt6325_upmu_set_vdvfs11_en_sel(2);
                exec_scrxxx_map(VDVFS11_INDEX);

                printk("[mt6325_upmu_set_vdvfs11_en_sel(3)]\n");
                mt6325_upmu_set_vdvfs11_en_sel(3);
                exec_scrxxx_map(VDVFS11_INDEX);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vdvfs12_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vdvfs12_en_ctrl] %d\n", i);
        mt6325_upmu_set_vdvfs12_en_ctrl(i);

        switch(i){
            case 0:
                printk("[mt6325_upmu_set_vdvfs12_en(0)]\n");
                mt6325_upmu_set_vdvfs12_en(0);
                read_adc_value(VDVFS12_INDEX);

                printk("[mt6325_upmu_set_vdvfs12_en(1)]\n");
                mt6325_upmu_set_vdvfs12_en(1);
                read_adc_value(VDVFS12_INDEX);
                break;

            case 1:
                printk("[mt6325_upmu_set_vdvfs12_en_sel(0)]\n");
                mt6325_upmu_set_vdvfs12_en_sel(0);
                exec_scrxxx_map(VDVFS12_INDEX);                       

                printk("[mt6325_upmu_set_vdvfs12_en_sel(1)]\n");
                mt6325_upmu_set_vdvfs12_en_sel(1);
                exec_scrxxx_map(VDVFS12_INDEX);

                printk("[mt6325_upmu_set_vdvfs12_en_sel(2)]\n");
                mt6325_upmu_set_vdvfs12_en_sel(2);
                exec_scrxxx_map(VDVFS12_INDEX);

                printk("[mt6325_upmu_set_vdvfs12_en_sel(3)]\n");
                mt6325_upmu_set_vdvfs12_en_sel(3);
                exec_scrxxx_map(VDVFS12_INDEX);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vpa_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vpa_en_ctrl] %d\n", i);
        mt6325_upmu_set_vpa_en_ctrl(i);

        switch(i){
            case 0:
                printk("[mt6325_upmu_set_vpa_en(0)]\n");
                mt6325_upmu_set_vpa_en(0);
                read_adc_value(VPA_INDEX);

                printk("[mt6325_upmu_set_vpa_en(1)]\n");
                mt6325_upmu_set_vpa_en(1);
                read_adc_value(VPA_INDEX);
                break;

            case 1:
                printk("[mt6325_upmu_set_vpa_en_sel(0)]\n");
                mt6325_upmu_set_vpa_en_sel(0);
                exec_scrxxx_map(VPA_INDEX);                       

                printk("[mt6325_upmu_set_vpa_en_sel(1)]\n");
                mt6325_upmu_set_vpa_en_sel(1);
                exec_scrxxx_map(VPA_INDEX);

                printk("[mt6325_upmu_set_vpa_en_sel(2)]\n");
                mt6325_upmu_set_vpa_en_sel(2);
                exec_scrxxx_map(VPA_INDEX);

                printk("[mt6325_upmu_set_vpa_en_sel(3)]\n");
                mt6325_upmu_set_vpa_en_sel(3);
                exec_scrxxx_map(VPA_INDEX);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_mt6311_vdvfs11_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6311_set_vdvfs11_en_ctrl] %d\n", i);
        mt6311_set_vdvfs11_en_ctrl(i);

        switch(i){
            case 0:
                printk("[mt6311_set_vdvfs11_en(0)]\n");
                mt6311_set_vdvfs11_en(0);
                read_adc_value(MT6311_VDVFS11_INDEX);

                printk("[mt6311_set_vdvfs11_en(1)]\n");
                mt6311_set_vdvfs11_en(1);
                read_adc_value(MT6311_VDVFS11_INDEX);
                break;

            case 1:
                #if 1
                exec_scrxxx_map(MT6311_VDVFS11_INDEX);                
                #else
                printk("[mt6311_set_vdvfs11_en_sel(0)]\n");
                mt6311_set_vdvfs11_en_sel(0);
                exec_scrxxx_map(MT6311_VDVFS11_INDEX);                       

                printk("[mt6311_set_vdvfs11_en_sel(1)]\n");
                mt6311_set_vdvfs11_en_sel(1);
                exec_scrxxx_map(MT6311_VDVFS11_INDEX);

                printk("[mt6311_set_vdvfs11_en_sel(2)]\n");
                mt6311_set_vdvfs11_en_sel(2);
                exec_scrxxx_map(MT6311_VDVFS11_INDEX);

                printk("[mt6311_set_vdvfs11_en_sel(3)]\n");
                mt6311_set_vdvfs11_en_sel(3);
                exec_scrxxx_map(MT6311_VDVFS11_INDEX);                
                #endif
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_mt6311_vdvfs12_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6311_set_vdvfs12_en_ctrl] %d\n", i);
        mt6311_set_vdvfs12_en_ctrl(i);

        switch(i){
            case 0:
                printk("[mt6311_set_vdvfs12_en(0)]\n");
                mt6311_set_vdvfs12_en(0);
                read_adc_value(MT6311_VDVFS12_INDEX);

                printk("[mt6311_set_vdvfs12_en(1)]\n");
                mt6311_set_vdvfs12_en(1);
                read_adc_value(MT6311_VDVFS12_INDEX);
                break;

            case 1:
                #if 1
                exec_scrxxx_map(MT6311_VDVFS12_INDEX);
                #else
                printk("[mt6311_set_vdvfs12_en_sel(0)]\n");
                mt6311_set_vdvfs12_en_sel(0);
                exec_scrxxx_map(MT6311_VDVFS12_INDEX);                       

                printk("[mt6311_set_vdvfs12_en_sel(1)]\n");
                mt6311_set_vdvfs12_en_sel(1);
                exec_scrxxx_map(MT6311_VDVFS12_INDEX);

                printk("[mt6311_set_vdvfs12_en_sel(2)]\n");
                mt6311_set_vdvfs12_en_sel(2);
                exec_scrxxx_map(MT6311_VDVFS12_INDEX);

                printk("[mt6311_set_vdvfs12_en_sel(3)]\n");
                mt6311_set_vdvfs12_en_sel(3);
                exec_scrxxx_map(MT6311_VDVFS12_INDEX);
                #endif                
                break;                

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void PMIC_BUCK_ON_OFF(int index_val)
{
    printk("[PMIC_BUCK_ON_OFF] start....\n");

    //      
    set_srclken_sw_mode();

    //      
    set_mt6311_srclken_sw_mode();

    switch(index_val){
      //      
      case 0 : exec_vcore1_en_test();   break;
      case 1 : exec_vcore2_en_test();   break;
      case 2 : exec_vdram_en_test();    break;
      case 3 : exec_vgpu_en_test();     break;
      case 4 : exec_vrf18_0_en_test();  break;
      case 5 : exec_vio18_en_test();    break;
      case 6 : exec_vdvfs11_en_test();  break;
      case 7 : exec_vdvfs12_en_test();  break;
      case 8 : exec_vpa_en_test();      break;

      //        
      case MT6311_VDVFS11_INDEX : exec_mt6311_vdvfs11_en_test();  break;
      case MT6311_VDVFS12_INDEX : exec_mt6311_vdvfs12_en_test();  break;  

	  default:
        printk("[PMIC_BUCK_ON_OFF] Invalid channel value(%d)\n", index_val);
        break;

    }

    printk("[PMIC_BUCK_ON_OFF] end....\n");
}
#endif

#ifdef BUCK_VOSEL_DVT_ENABLE
void exec_vcore1_vosel_test_sub_vosel(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VCORE1_VOSEL_SLEEP_MASK;k++) {
        mt6325_upmu_set_vcore1_vosel_sleep(k); printk("[mt6325_upmu_set_vcore1_vosel_sleep] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VCORE1_INDEX);
    }
    for(k=0;k<=MT6325_PMIC_VCORE1_VOSEL_ON_MASK;k++) {
        mt6325_upmu_set_vcore1_vosel_on(k); printk("[mt6325_upmu_set_vcore1_vosel_on] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VCORE1_INDEX);
    }
}

void exec_vcore1_vosel_test_sub(void)
{
#if 1
    printk("[exec_vcore1_vosel_test_sub] ----------------------------------------------------\n");
    mt6325_upmu_set_vcore1_vosel_ctrl(0);
    printk("[exec_vcore1_vosel_test_sub] mt6325_upmu_set_vcore1_vosel_sleep(0x30); mt6325_upmu_set_vcore1_vosel_on(0x50);\n");
                                         mt6325_upmu_set_vcore1_vosel_sleep(0x30); mt6325_upmu_set_vcore1_vosel_on(0x50);
    mt6325_upmu_set_vcore1_vosel_ctrl(1);                                     
    printk("[exec_vcore1_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(VCORE1_INDEX);
    printk("[exec_vcore1_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(0); read_adc_value(VCORE1_INDEX);
    printk("[exec_vcore1_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(1); read_adc_value(VCORE1_INDEX);
    printk("[exec_vcore1_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(VCORE1_INDEX);

    printk("[exec_vcore1_vosel_test_sub] ----------------------------------------------------\n");
    mt6325_upmu_set_vcore1_vosel_ctrl(0);
    printk("[exec_vcore1_vosel_test_sub] mt6325_upmu_set_vcore1_vosel_sleep(0x40); mt6325_upmu_set_vcore1_vosel_on(0x20);\n");
                                         mt6325_upmu_set_vcore1_vosel_sleep(0x40); mt6325_upmu_set_vcore1_vosel_on(0x20);
    mt6325_upmu_set_vcore1_vosel_ctrl(1);                                     
    printk("[exec_vcore1_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(VCORE1_INDEX);
    printk("[exec_vcore1_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(0); read_adc_value(VCORE1_INDEX);
    printk("[exec_vcore1_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(1); read_adc_value(VCORE1_INDEX);
    printk("[exec_vcore1_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(VCORE1_INDEX);   
#else
    printk("[exec_vcore1_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vcore1_vosel_test_sub_vosel();

    printk("[exec_vcore1_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vcore1_vosel_test_sub_vosel();

    printk("[exec_vcore1_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vcore1_vosel_test_sub_vosel();

    printk("[exec_vcore1_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vcore1_vosel_test_sub_vosel();
#endif                              
}

void exec_vcore1_vosel_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vcore1_vosel_ctrl] %d\n", i);
        mt6325_upmu_set_vcore1_vosel_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VCORE1_VOSEL_MASK;k++) {
                    mt6325_upmu_set_vcore1_vosel(k); printk("[mt6325_upmu_set_vcore1_vosel] k=%d, ",k);
                    if(k==0) mdelay(5000);
                    read_adc_value(VCORE1_INDEX);
                }
                break;

            case 1:
                printk("[mt6325_upmu_set_vcore1_vosel_sel(0)]\n");
                mt6325_upmu_set_vcore1_vosel_sel(0);
                exec_vcore1_vosel_test_sub(); 

                printk("[mt6325_upmu_set_vcore1_vosel_sel(1)]\n");
                mt6325_upmu_set_vcore1_vosel_sel(1);                
                exec_vcore1_vosel_test_sub();

                printk("[mt6325_upmu_set_vcore1_vosel_sel(2)]\n");
                mt6325_upmu_set_vcore1_vosel_sel(2);
                exec_vcore1_vosel_test_sub();

                printk("[mt6325_upmu_set_vcore1_vosel_sel(3)]\n");
                mt6325_upmu_set_vcore1_vosel_sel(3);
                exec_vcore1_vosel_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vcore2_vosel_test_sub_vosel(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VCORE2_VOSEL_SLEEP_MASK;k++) {
        mt6325_upmu_set_vcore2_vosel_sleep(k); printk("[mt6325_upmu_set_vcore2_vosel_sleep] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VCORE2_INDEX);
    }
    for(k=0;k<=MT6325_PMIC_VCORE2_VOSEL_AUD_MASK;k++) {
        mt6325_upmu_set_vcore2_vosel_aud(k); printk("[mt6325_upmu_set_vcore2_vosel_aud] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VCORE2_INDEX);
    }
    for(k=0;k<=MT6325_PMIC_VCORE2_VOSEL_ON_MASK;k++) {
        mt6325_upmu_set_vcore2_vosel_on(k); printk("[mt6325_upmu_set_vcore2_vosel_on] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VCORE2_INDEX);
    }
}

void exec_vcore2_vosel_test_sub(void)
{
#if 1
    printk("[exec_vcore2_vosel_test_sub] ----------------------------------------------------\n");
    mt6325_upmu_set_vcore2_vosel_ctrl(0);
    printk("[exec_vcore2_vosel_test_sub] mt6325_upmu_set_vcore2_vosel_sleep(0x30); mt6325_upmu_set_vcore2_vosel_on(0x50); mt6325_upmu_set_vcore2_vosel_aud(0x10);\n");
                                         mt6325_upmu_set_vcore2_vosel_sleep(0x30); mt6325_upmu_set_vcore2_vosel_on(0x50); mt6325_upmu_set_vcore2_vosel_aud(0x10);
    mt6325_upmu_set_vcore2_vosel_ctrl(1);                                     
    printk("[exec_vcore2_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(VCORE2_INDEX);
    printk("[exec_vcore2_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(0); read_adc_value(VCORE2_INDEX);
    printk("[exec_vcore2_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(1); read_adc_value(VCORE2_INDEX);
    printk("[exec_vcore2_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(VCORE2_INDEX);

    printk("[exec_vcore2_vosel_test_sub] ----------------------------------------------------\n");
    mt6325_upmu_set_vcore2_vosel_ctrl(0);
    printk("[exec_vcore2_vosel_test_sub] mt6325_upmu_set_vcore2_vosel_sleep(0x40); mt6325_upmu_set_vcore2_vosel_on(0x20); mt6325_upmu_set_vcore2_vosel_aud(0x15);\n");
                                         mt6325_upmu_set_vcore2_vosel_sleep(0x40); mt6325_upmu_set_vcore2_vosel_on(0x20); mt6325_upmu_set_vcore2_vosel_aud(0x15);
    mt6325_upmu_set_vcore2_vosel_ctrl(1);                                     
    printk("[exec_vcore2_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(VCORE2_INDEX);
    printk("[exec_vcore2_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(0); read_adc_value(VCORE2_INDEX);
    printk("[exec_vcore2_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(1); read_adc_value(VCORE2_INDEX);
    printk("[exec_vcore2_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(VCORE2_INDEX);                                                    
#else

    printk("[exec_vcore2_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vcore2_vosel_test_sub_vosel();

    printk("[exec_vcore2_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vcore2_vosel_test_sub_vosel();

    printk("[exec_vcore2_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vcore2_vosel_test_sub_vosel();

    printk("[exec_vcore2_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vcore2_vosel_test_sub_vosel();
#endif                              
}

void exec_vcore2_vosel_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vcore2_vosel_ctrl] %d\n", i);
        mt6325_upmu_set_vcore2_vosel_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VCORE2_VOSEL_MASK;k++) {
                    mt6325_upmu_set_vcore2_vosel(k); printk("[mt6325_upmu_set_vcore2_vosel] k=%d, ",k);
                    if(k==0) mdelay(5000);
                    read_adc_value(VCORE2_INDEX);
                }
                break;

            case 1:
                //                                                 
                mt6325_upmu_set_rg_vowen_mode(1);
                mt6325_upmu_set_rg_srcvolten_mode(1);
                mt6325_upmu_set_rg_vowen_sw(0);
                mt6325_upmu_set_rg_srcvolten_sw(0);
                printk("[ZF_1] Reg[0x%x]=0x%x\n", MT6325_TOP_CKSEL_CON2,upmu_get_reg_value(MT6325_TOP_CKSEL_CON2));
                //                                                 
                printk("[mt6325_upmu_set_vcore2_vosel_sel(0)]\n");
                mt6325_upmu_set_vcore2_vosel_sel(0);
                exec_vcore2_vosel_test_sub(); 
                printk("[mt6325_upmu_set_vcore2_vosel_sel(1)]\n");
                mt6325_upmu_set_vcore2_vosel_sel(1);                
                exec_vcore2_vosel_test_sub();
                printk("[mt6325_upmu_set_vcore2_vosel_sel(2)]\n");
                mt6325_upmu_set_vcore2_vosel_sel(2);
                exec_vcore2_vosel_test_sub();
                printk("[mt6325_upmu_set_vcore2_vosel_sel(3)]\n");
                mt6325_upmu_set_vcore2_vosel_sel(3);
                exec_vcore2_vosel_test_sub();
                //                                                 

                printk("[ZF_2_3] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                 set_srclken_1_val(0); set_srclken_0_val(0);
                printk("[mt6325_upmu_set_vcore2_vosel_sel(0)]\n");
                mt6325_upmu_set_vcore2_vosel_sel(0);
                //                                                 
                mt6325_upmu_set_rg_vowen_mode(1);
                mt6325_upmu_set_rg_srcvolten_mode(1);
                mt6325_upmu_set_rg_vowen_sw(1);
                mt6325_upmu_set_rg_srcvolten_sw(0);
                printk("[ZF_2] Reg[0x%x]=0x%x\n", MT6325_TOP_CKSEL_CON2,upmu_get_reg_value(MT6325_TOP_CKSEL_CON2));
                //                                                                
                mt6325_upmu_set_vcore2_vosel_ctrl(0);
                printk("[exec_vcore2_vosel_test_sub] mt6325_upmu_set_vcore2_vosel_sleep(0x30); mt6325_upmu_set_vcore2_vosel_on(0x50); mt6325_upmu_set_vcore2_vosel_aud(0x10);\n");
                                                     mt6325_upmu_set_vcore2_vosel_sleep(0x30); mt6325_upmu_set_vcore2_vosel_on(0x50); mt6325_upmu_set_vcore2_vosel_aud(0x10);
                mt6325_upmu_set_vcore2_vosel_ctrl(1);                                     
                read_adc_value(VCORE2_INDEX);
                
                mt6325_upmu_set_vcore2_vosel_ctrl(0);
                printk("[exec_vcore2_vosel_test_sub] mt6325_upmu_set_vcore2_vosel_sleep(0x40); mt6325_upmu_set_vcore2_vosel_on(0x20); mt6325_upmu_set_vcore2_vosel_aud(0x15);\n");
                                                     mt6325_upmu_set_vcore2_vosel_sleep(0x40); mt6325_upmu_set_vcore2_vosel_on(0x20); mt6325_upmu_set_vcore2_vosel_aud(0x15);
                mt6325_upmu_set_vcore2_vosel_ctrl(1);                                     
                read_adc_value(VCORE2_INDEX);
                //                                                 

                //                                                 
                mt6325_upmu_set_rg_vowen_sw(0);
                printk("[ZF_3] Reg[0x%x]=0x%x\n", MT6325_TOP_CKSEL_CON2,upmu_get_reg_value(MT6325_TOP_CKSEL_CON2));
                //                                                                
                mt6325_upmu_set_vcore2_vosel_ctrl(0);
                printk("[exec_vcore2_vosel_test_sub] mt6325_upmu_set_vcore2_vosel_sleep(0x30); mt6325_upmu_set_vcore2_vosel_on(0x50); mt6325_upmu_set_vcore2_vosel_aud(0x10);\n");
                                                     mt6325_upmu_set_vcore2_vosel_sleep(0x30); mt6325_upmu_set_vcore2_vosel_on(0x50); mt6325_upmu_set_vcore2_vosel_aud(0x10);
                mt6325_upmu_set_vcore2_vosel_ctrl(1);                                     
                read_adc_value(VCORE2_INDEX);
                
                mt6325_upmu_set_vcore2_vosel_ctrl(0);
                printk("[exec_vcore2_vosel_test_sub] mt6325_upmu_set_vcore2_vosel_sleep(0x40); mt6325_upmu_set_vcore2_vosel_on(0x20); mt6325_upmu_set_vcore2_vosel_aud(0x15);\n");
                                                     mt6325_upmu_set_vcore2_vosel_sleep(0x40); mt6325_upmu_set_vcore2_vosel_on(0x20); mt6325_upmu_set_vcore2_vosel_aud(0x15);
                mt6325_upmu_set_vcore2_vosel_ctrl(1);                                     
                read_adc_value(VCORE2_INDEX);
                //                                                 
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vdram_vosel_test_sub_vosel(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VDRAM_VOSEL_SLEEP_MASK;k++) {
        mt6325_upmu_set_vdram_vosel_sleep(k); printk("[mt6325_upmu_set_vdram_vosel_sleep] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VDRAM_INDEX);
    }
    for(k=0;k<=MT6325_PMIC_VDRAM_VOSEL_ON_MASK;k++) {
        mt6325_upmu_set_vdram_vosel_on(k); printk("[mt6325_upmu_set_vdram_vosel_on] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VDRAM_INDEX);
    }
}

void exec_vdram_vosel_test_sub(void)
{
#if 1
    printk("[exec_vdram_vosel_test_sub] ----------------------------------------------------\n");
    mt6325_upmu_set_vdram_vosel_ctrl(0);
    printk("[exec_vdram_vosel_test_sub] mt6325_upmu_set_vdram_vosel_sleep(0x30); mt6325_upmu_set_vdram_vosel_on(0x50);\n");
                                         mt6325_upmu_set_vdram_vosel_sleep(0x30); mt6325_upmu_set_vdram_vosel_on(0x50);
    mt6325_upmu_set_vdram_vosel_ctrl(1);                                     
    printk("[exec_vdram_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(VDRAM_INDEX);
    printk("[exec_vdram_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(0); read_adc_value(VDRAM_INDEX);
    printk("[exec_vdram_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(1); read_adc_value(VDRAM_INDEX);
    printk("[exec_vdram_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(VDRAM_INDEX);

    printk("[exec_vdram_vosel_test_sub] ----------------------------------------------------\n");
    mt6325_upmu_set_vdram_vosel_ctrl(0);
    printk("[exec_vdram_vosel_test_sub] mt6325_upmu_set_vdram_vosel_sleep(0x40); mt6325_upmu_set_vdram_vosel_on(0x20);\n");
                                         mt6325_upmu_set_vdram_vosel_sleep(0x40); mt6325_upmu_set_vdram_vosel_on(0x20);
    mt6325_upmu_set_vdram_vosel_ctrl(1);                                     
    printk("[exec_vdram_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(VDRAM_INDEX);
    printk("[exec_vdram_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(0); read_adc_value(VDRAM_INDEX);
    printk("[exec_vdram_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(1); read_adc_value(VDRAM_INDEX);
    printk("[exec_vdram_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(VDRAM_INDEX);                                                       
#else

    printk("[exec_vdram_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vdram_vosel_test_sub_vosel();

    printk("[exec_vdram_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vdram_vosel_test_sub_vosel();

    printk("[exec_vdram_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vdram_vosel_test_sub_vosel();

    printk("[exec_vdram_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vdram_vosel_test_sub_vosel();
#endif                              
}

void exec_vdram_vosel_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vdram_vosel_ctrl] %d\n", i);
        mt6325_upmu_set_vdram_vosel_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VDRAM_VOSEL_MASK;k++) {
                    mt6325_upmu_set_vdram_vosel(k); printk("[mt6325_upmu_set_vdram_vosel] k=%d, ",k);
                    if(k==0) mdelay(5000);
                    read_adc_value(VDRAM_INDEX);
                }
                break;

            case 1:
                printk("[mt6325_upmu_set_vdram_vosel_sel(0)]\n");
                mt6325_upmu_set_vdram_vosel_sel(0);
                exec_vdram_vosel_test_sub(); 

                printk("[mt6325_upmu_set_vdram_vosel_sel(1)]\n");
                mt6325_upmu_set_vdram_vosel_sel(1);                
                exec_vdram_vosel_test_sub();

                printk("[mt6325_upmu_set_vdram_vosel_sel(2)]\n");
                mt6325_upmu_set_vdram_vosel_sel(2);
                exec_vdram_vosel_test_sub();

                printk("[mt6325_upmu_set_vdram_vosel_sel(3)]\n");
                mt6325_upmu_set_vdram_vosel_sel(3);
                exec_vdram_vosel_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vgpu_vosel_test_sub_vosel(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VGPU_VOSEL_SLEEP_MASK;k++) {
        mt6325_upmu_set_vgpu_vosel_sleep(k); printk("[mt6325_upmu_set_vgpu_vosel_sleep] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VGPU_INDEX);
    }
    for(k=0;k<=MT6325_PMIC_VGPU_VOSEL_ON_MASK;k++) {
        mt6325_upmu_set_vgpu_vosel_on(k); printk("[mt6325_upmu_set_vgpu_vosel_on] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VGPU_INDEX);
    }
}

void exec_vgpu_vosel_test_sub(void)
{
#if 1
    printk("[exec_vgpu_vosel_test_sub] ----------------------------------------------------\n");
    mt6325_upmu_set_vgpu_vosel_ctrl(0);
    printk("[exec_vgpu_vosel_test_sub] mt6325_upmu_set_vgpu_vosel_sleep(0x30); mt6325_upmu_set_vgpu_vosel_on(0x50);\n");
                                         mt6325_upmu_set_vgpu_vosel_sleep(0x30); mt6325_upmu_set_vgpu_vosel_on(0x50);
    mt6325_upmu_set_vgpu_vosel_ctrl(1);                                     
    printk("[exec_vgpu_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(VGPU_INDEX);
    printk("[exec_vgpu_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(0); read_adc_value(VGPU_INDEX);
    printk("[exec_vgpu_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(1); read_adc_value(VGPU_INDEX);
    printk("[exec_vgpu_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(VGPU_INDEX);

    printk("[exec_vgpu_vosel_test_sub] ----------------------------------------------------\n");
    mt6325_upmu_set_vgpu_vosel_ctrl(0);
    printk("[exec_vgpu_vosel_test_sub] mt6325_upmu_set_vgpu_vosel_sleep(0x40); mt6325_upmu_set_vgpu_vosel_on(0x20);\n");
                                         mt6325_upmu_set_vgpu_vosel_sleep(0x40); mt6325_upmu_set_vgpu_vosel_on(0x20);
    mt6325_upmu_set_vgpu_vosel_ctrl(1);                                     
    printk("[exec_vgpu_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(VGPU_INDEX);
    printk("[exec_vgpu_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(0); read_adc_value(VGPU_INDEX);
    printk("[exec_vgpu_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(1); read_adc_value(VGPU_INDEX);
    printk("[exec_vgpu_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(VGPU_INDEX);                                                        
#else

    printk("[exec_vgpu_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vgpu_vosel_test_sub_vosel();

    printk("[exec_vgpu_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vgpu_vosel_test_sub_vosel();

    printk("[exec_vgpu_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vgpu_vosel_test_sub_vosel();

    printk("[exec_vgpu_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vgpu_vosel_test_sub_vosel();
#endif                              
}

void exec_vgpu_vosel_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vgpu_vosel_ctrl] %d\n", i);
        mt6325_upmu_set_vgpu_vosel_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VGPU_VOSEL_MASK;k++) {
                    mt6325_upmu_set_vgpu_vosel(k); printk("[mt6325_upmu_set_vgpu_vosel] k=%d, ",k);
                    if(k==0) mdelay(5000);
                    read_adc_value(VGPU_INDEX);
                }
                break;

            case 1:
                printk("[mt6325_upmu_set_vgpu_vosel_sel(0)]\n");
                mt6325_upmu_set_vgpu_vosel_sel(0);
                exec_vgpu_vosel_test_sub(); 

                printk("[mt6325_upmu_set_vgpu_vosel_sel(1)]\n");
                mt6325_upmu_set_vgpu_vosel_sel(1);                
                exec_vgpu_vosel_test_sub();

                printk("[mt6325_upmu_set_vgpu_vosel_sel(2)]\n");
                mt6325_upmu_set_vgpu_vosel_sel(2);
                exec_vgpu_vosel_test_sub();

                printk("[mt6325_upmu_set_vgpu_vosel_sel(3)]\n");
                mt6325_upmu_set_vgpu_vosel_sel(3);
                exec_vgpu_vosel_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vrf18_0_vosel_test_sub_vosel(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VRF18_0_VOSEL_SLEEP_MASK;k++) {
        mt6325_upmu_set_vrf18_0_vosel_sleep(k); printk("[mt6325_upmu_set_vrf18_0_vosel_sleep] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VRF18_0_INDEX);
    }
    for(k=0;k<=MT6325_PMIC_VRF18_0_VOSEL_ON_MASK;k++) {
        mt6325_upmu_set_vrf18_0_vosel_on(k); printk("[mt6325_upmu_set_vrf18_0_vosel_on] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VRF18_0_INDEX);
    }
}

void exec_vrf18_0_vosel_test_sub(void)
{
    printk("[exec_vrf18_0_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vrf18_0_vosel_test_sub_vosel();

    printk("[exec_vrf18_0_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vrf18_0_vosel_test_sub_vosel();

    printk("[exec_vrf18_0_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vrf18_0_vosel_test_sub_vosel();

    printk("[exec_vrf18_0_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vrf18_0_vosel_test_sub_vosel();
}

void exec_vrf18_0_vosel_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vrf18_0_vosel_ctrl] %d\n", i);
        mt6325_upmu_set_vrf18_0_vosel_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VRF18_0_VOSEL_MASK;k++) {
                    mt6325_upmu_set_vrf18_0_vosel(k); printk("[mt6325_upmu_set_vrf18_0_vosel] k=%d, ",k);
                    if(k==0) mdelay(5000);
                    read_adc_value(VRF18_0_INDEX);
                }
                break;

            case 1:
                printk("[mt6325_upmu_set_vrf18_0_vosel_sel(0)]\n");
                mt6325_upmu_set_vrf18_0_vosel_sel(0);
                exec_vrf18_0_vosel_test_sub(); 

                printk("[mt6325_upmu_set_vrf18_0_vosel_sel(1)]\n");
                mt6325_upmu_set_vrf18_0_vosel_sel(1);                
                exec_vrf18_0_vosel_test_sub();

                printk("[mt6325_upmu_set_vrf18_0_vosel_sel(2)]\n");
                mt6325_upmu_set_vrf18_0_vosel_sel(2);
                exec_vrf18_0_vosel_test_sub();

                printk("[mt6325_upmu_set_vrf18_0_vosel_sel(3)]\n");
                mt6325_upmu_set_vrf18_0_vosel_sel(3);
                exec_vrf18_0_vosel_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vio18_vosel_test_sub_vosel(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VIO18_VOSEL_SLEEP_MASK;k++) {
        mt6325_upmu_set_vio18_vosel_sleep(k); printk("[mt6325_upmu_set_vio18_vosel_sleep] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VIO18_INDEX);
    }
    for(k=0;k<=MT6325_PMIC_VIO18_VOSEL_ON_MASK;k++) {
        mt6325_upmu_set_vio18_vosel_on(k); printk("[mt6325_upmu_set_vio18_vosel_on] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VIO18_INDEX);
    }
}

void exec_vio18_vosel_test_sub(void)
{
    printk("[exec_vio18_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vio18_vosel_test_sub_vosel();

    printk("[exec_vio18_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vio18_vosel_test_sub_vosel();

    printk("[exec_vio18_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vio18_vosel_test_sub_vosel();

    printk("[exec_vio18_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vio18_vosel_test_sub_vosel();
}

void exec_vio18_vosel_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vio18_vosel_ctrl] %d\n", i);
        mt6325_upmu_set_vio18_vosel_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VIO18_VOSEL_MASK;k++) {
                    mt6325_upmu_set_vio18_vosel(k); printk("[mt6325_upmu_set_vio18_vosel] k=%d, ",k);
                    if(k==0) mdelay(5000);
                    read_adc_value(VIO18_INDEX);
                }
                break;

            case 1:
                printk("[mt6325_upmu_set_vio18_vosel_sel(0)]\n");
                mt6325_upmu_set_vio18_vosel_sel(0);
                exec_vio18_vosel_test_sub(); 

                printk("[mt6325_upmu_set_vio18_vosel_sel(1)]\n");
                mt6325_upmu_set_vio18_vosel_sel(1);                
                exec_vio18_vosel_test_sub();

                printk("[mt6325_upmu_set_vio18_vosel_sel(2)]\n");
                mt6325_upmu_set_vio18_vosel_sel(2);
                exec_vio18_vosel_test_sub();

                printk("[mt6325_upmu_set_vio18_vosel_sel(3)]\n");
                mt6325_upmu_set_vio18_vosel_sel(3);
                exec_vio18_vosel_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vdvfs11_vosel_test_sub_vosel(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VDVFS11_VOSEL_SLEEP_MASK;k++) {
        mt6325_upmu_set_vdvfs11_vosel_sleep(k); printk("[mt6325_upmu_set_vdvfs11_vosel_sleep] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VDVFS11_INDEX);
    }
    for(k=0;k<=MT6325_PMIC_VDVFS11_VOSEL_ON_MASK;k++) {
        mt6325_upmu_set_vdvfs11_vosel_on(k); printk("[mt6325_upmu_set_vdvfs11_vosel_on] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VDVFS11_INDEX);
    }
}

void exec_vdvfs11_vosel_test_sub(void)
{
#if 1
    printk("[exec_vdvfs11_vosel_test_sub] ----------------------------------------------------\n");
    mt6325_upmu_set_vdvfs11_vosel_ctrl(0);
    printk("[exec_vdvfs11_vosel_test_sub] mt6325_upmu_set_vdvfs11_vosel_sleep(0x30); mt6325_upmu_set_vdvfs11_vosel_on(0x50);\n");
                                         mt6325_upmu_set_vdvfs11_vosel_sleep(0x30); mt6325_upmu_set_vdvfs11_vosel_on(0x50);
    mt6325_upmu_set_vdvfs11_vosel_ctrl(1);                                     
    printk("[exec_vdvfs11_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(VDVFS11_INDEX);
    printk("[exec_vdvfs11_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(0); read_adc_value(VDVFS11_INDEX);
    printk("[exec_vdvfs11_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(1); read_adc_value(VDVFS11_INDEX);
    printk("[exec_vdvfs11_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(VDVFS11_INDEX);

    printk("[exec_vdvfs11_vosel_test_sub] ----------------------------------------------------\n");
    mt6325_upmu_set_vdvfs11_vosel_ctrl(0);
    printk("[exec_vdvfs11_vosel_test_sub] mt6325_upmu_set_vdvfs11_vosel_sleep(0x40); mt6325_upmu_set_vdvfs11_vosel_on(0x20);\n");
                                         mt6325_upmu_set_vdvfs11_vosel_sleep(0x40); mt6325_upmu_set_vdvfs11_vosel_on(0x20);
    mt6325_upmu_set_vdvfs11_vosel_ctrl(1);                                     
    printk("[exec_vdvfs11_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(VDVFS11_INDEX);
    printk("[exec_vdvfs11_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(0); read_adc_value(VDVFS11_INDEX);
    printk("[exec_vdvfs11_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(1); read_adc_value(VDVFS11_INDEX);
    printk("[exec_vdvfs11_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(VDVFS11_INDEX);                                                       
#else

    printk("[exec_vdvfs11_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vdvfs11_vosel_test_sub_vosel();

    printk("[exec_vdvfs11_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vdvfs11_vosel_test_sub_vosel();

    printk("[exec_vdvfs11_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vdvfs11_vosel_test_sub_vosel();

    printk("[exec_vdvfs11_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vdvfs11_vosel_test_sub_vosel();
#endif                              
}

void exec_vdvfs11_vosel_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vdvfs11_vosel_ctrl] %d\n", i);
        mt6325_upmu_set_vdvfs11_vosel_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VDVFS11_VOSEL_MASK;k++) {
                    mt6325_upmu_set_vdvfs11_vosel(k); printk("[mt6325_upmu_set_vdvfs11_vosel] k=%d, ",k);
                    if(k==0) mdelay(10000);
                    read_adc_value(VDVFS11_INDEX);
                }
                break;

            case 1:
                printk("[mt6325_upmu_set_vdvfs11_vosel_sel(0)]\n");
                mt6325_upmu_set_vdvfs11_vosel_sel(0);
                exec_vdvfs11_vosel_test_sub(); 

                printk("[mt6325_upmu_set_vdvfs11_vosel_sel(1)]\n");
                mt6325_upmu_set_vdvfs11_vosel_sel(1);                
                exec_vdvfs11_vosel_test_sub();

                printk("[mt6325_upmu_set_vdvfs11_vosel_sel(2)]\n");
                mt6325_upmu_set_vdvfs11_vosel_sel(2);
                exec_vdvfs11_vosel_test_sub();

                printk("[mt6325_upmu_set_vdvfs11_vosel_sel(3)]\n");
                mt6325_upmu_set_vdvfs11_vosel_sel(3);
                exec_vdvfs11_vosel_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vdvfs12_vosel_test_sub_vosel(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VDVFS12_VOSEL_SLEEP_MASK;k++) {
        mt6325_upmu_set_vdvfs12_vosel_sleep(k); printk("[mt6325_upmu_set_vdvfs12_vosel_sleep] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VDVFS12_INDEX);
    }
    for(k=0;k<=MT6325_PMIC_VDVFS12_VOSEL_ON_MASK;k++) {
        mt6325_upmu_set_vdvfs12_vosel_on(k); printk("[mt6325_upmu_set_vdvfs12_vosel_on] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VDVFS12_INDEX);
    }
}

void exec_vdvfs12_vosel_test_sub(void)
{
#if 1
    printk("[exec_vdvfs12_vosel_test_sub] ----------------------------------------------------\n");
    mt6325_upmu_set_vdvfs12_vosel_ctrl(0);
    printk("[exec_vdvfs12_vosel_test_sub] mt6325_upmu_set_vdvfs12_vosel_sleep(0x30); mt6325_upmu_set_vdvfs12_vosel_on(0x50);\n");
                                         mt6325_upmu_set_vdvfs12_vosel_sleep(0x30); mt6325_upmu_set_vdvfs12_vosel_on(0x50);
    mt6325_upmu_set_vdvfs12_vosel_ctrl(1);                                     
    printk("[exec_vdvfs12_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(VDVFS12_INDEX);
    printk("[exec_vdvfs12_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(0); read_adc_value(VDVFS12_INDEX);
    printk("[exec_vdvfs12_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(1); read_adc_value(VDVFS12_INDEX);
    printk("[exec_vdvfs12_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(VDVFS12_INDEX);

    printk("[exec_vdvfs12_vosel_test_sub] ----------------------------------------------------\n");
    mt6325_upmu_set_vdvfs12_vosel_ctrl(0);
    printk("[exec_vdvfs12_vosel_test_sub] mt6325_upmu_set_vdvfs12_vosel_sleep(0x40); mt6325_upmu_set_vdvfs12_vosel_on(0x20);\n");
                                         mt6325_upmu_set_vdvfs12_vosel_sleep(0x40); mt6325_upmu_set_vdvfs12_vosel_on(0x20);
    mt6325_upmu_set_vdvfs12_vosel_ctrl(1);                                     
    printk("[exec_vdvfs12_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(1); read_adc_value(VDVFS12_INDEX);
    printk("[exec_vdvfs12_vosel_test_sub] set_srclken_1_val(1); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(1); set_srclken_0_val(0); read_adc_value(VDVFS12_INDEX);
    printk("[exec_vdvfs12_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(1);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(1); read_adc_value(VDVFS12_INDEX);
    printk("[exec_vdvfs12_vosel_test_sub] set_srclken_1_val(0); set_srclken_0_val(0);\n");
                                         set_srclken_1_val(0); set_srclken_0_val(0); read_adc_value(VDVFS12_INDEX);                                                        
#else

    printk("[exec_vdvfs12_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vdvfs12_vosel_test_sub_vosel();

    printk("[exec_vdvfs12_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vdvfs12_vosel_test_sub_vosel();

    printk("[exec_vdvfs12_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vdvfs12_vosel_test_sub_vosel();

    printk("[exec_vdvfs12_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vdvfs12_vosel_test_sub_vosel();
#endif                              
}

void exec_vdvfs12_vosel_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vdvfs12_vosel_ctrl] %d\n", i);
        mt6325_upmu_set_vdvfs12_vosel_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VDVFS12_VOSEL_MASK;k++) {
                    mt6325_upmu_set_vdvfs12_vosel(k); printk("[mt6325_upmu_set_vdvfs12_vosel] k=%d, ",k);
                    if(k==0) mdelay(5000);
                    read_adc_value(VDVFS12_INDEX);
                }
                break;

            case 1:
                printk("[mt6325_upmu_set_vdvfs12_vosel_sel(0)]\n");
                mt6325_upmu_set_vdvfs12_vosel_sel(0);
                exec_vdvfs12_vosel_test_sub(); 

                printk("[mt6325_upmu_set_vdvfs12_vosel_sel(1)]\n");
                mt6325_upmu_set_vdvfs12_vosel_sel(1);                
                exec_vdvfs12_vosel_test_sub();

                printk("[mt6325_upmu_set_vdvfs12_vosel_sel(2)]\n");
                mt6325_upmu_set_vdvfs12_vosel_sel(2);
                exec_vdvfs12_vosel_test_sub();

                printk("[mt6325_upmu_set_vdvfs12_vosel_sel(3)]\n");
                mt6325_upmu_set_vdvfs12_vosel_sel(3);
                exec_vdvfs12_vosel_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vpa_vosel_test_sub_vosel(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VPA_VOSEL_SLEEP_MASK;k++) {
        mt6325_upmu_set_vpa_vosel_sleep(k); printk("[mt6325_upmu_set_vpa_vosel_sleep] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VPA_INDEX);
    }
    for(k=0;k<=MT6325_PMIC_VPA_VOSEL_ON_MASK;k++) {
        mt6325_upmu_set_vpa_vosel_on(k); printk("[mt6325_upmu_set_vpa_vosel_on] k=%d, ",k);
        if(k==0) mdelay(500);
        read_adc_value(VPA_INDEX);
    }
}

void exec_vpa_vosel_test_sub(void)
{
    printk("[exec_vpa_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vpa_vosel_test_sub_vosel();

    printk("[exec_vpa_vosel_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vpa_vosel_test_sub_vosel();

    printk("[exec_vpa_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vpa_vosel_test_sub_vosel();

    printk("[exec_vpa_vosel_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vpa_vosel_test_sub_vosel();
}

void exec_vpa_vosel_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vpa_vosel_ctrl] %d\n", i);
        mt6325_upmu_set_vpa_vosel_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VPA_VOSEL_MASK;k++) {
                    mt6325_upmu_set_vpa_vosel(k); printk("[mt6325_upmu_set_vpa_vosel] k=%d, ",k);
                    if(k==0) mdelay(5000);
                    read_adc_value(VPA_INDEX);
                }
                break;

            case 1:
                printk("[mt6325_upmu_set_vpa_vosel_sel(0)]\n");
                mt6325_upmu_set_vpa_vosel_sel(0);
                exec_vpa_vosel_test_sub(); 

                printk("[mt6325_upmu_set_vpa_vosel_sel(1)]\n");
                mt6325_upmu_set_vpa_vosel_sel(1);                
                exec_vpa_vosel_test_sub();

                printk("[mt6325_upmu_set_vpa_vosel_sel(2)]\n");
                mt6325_upmu_set_vpa_vosel_sel(2);
                exec_vpa_vosel_test_sub();

                printk("[mt6325_upmu_set_vpa_vosel_sel(3)]\n");
                mt6325_upmu_set_vpa_vosel_sel(3);
                exec_vpa_vosel_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_mt6311_vdvfs11_vosel_test_sub_vosel(void)
{
    int k=0;
        
    for(k=0;k<=MT6311_PMIC_VDVFS11_VOSEL_SLEEP_MASK;k++) {
        mt6311_set_vdvfs11_vosel_sleep(k); printk("[mt6311_set_vdvfs11_vosel_sleep] k=%d, ",k);
        if(k==0) mdelay(5000);
        read_adc_value(MT6311_VDVFS11_INDEX);
    }
    for(k=0;k<=MT6311_PMIC_VDVFS11_VOSEL_ON_MASK;k++) {
        mt6311_set_vdvfs11_vosel_on(k); printk("[mt6311_set_vdvfs11_vosel_on] k=%d, ",k);
        if(k==0) mdelay(5000);
        read_adc_value(MT6311_VDVFS11_INDEX);
    }
}

void exec_mt6311_vdvfs11_vosel_test_sub(void)
{
#if 1
    printk("[exec_mt6311_vdvfs11_vosel_test_sub] ----------------------------------------------------\n");
    mt6311_set_vdvfs11_vosel_ctrl(0);
    printk("[exec_mt6311_vdvfs11_vosel_test_sub] mt6311_set_vdvfs11_vosel_sleep(0x30); mt6311_set_vdvfs11_vosel_on(0x50);\n");
                                                 mt6311_set_vdvfs11_vosel_sleep(0x30); mt6311_set_vdvfs11_vosel_on(0x50);
    mt6311_set_vdvfs11_vosel_ctrl(1);                                     
    printk("[exec_mt6311_vdvfs11_vosel_test_sub] set_mt6311_srclken_val(1); \n");
                                                 set_mt6311_srclken_val(1); read_adc_value(MT6311_VDVFS11_INDEX);
    printk("[exec_mt6311_vdvfs11_vosel_test_sub] set_mt6311_srclken_val(0); \n");
                                                 set_mt6311_srclken_val(0); read_adc_value(MT6311_VDVFS11_INDEX);

    printk("[exec_mt6311_vdvfs11_vosel_test_sub] ----------------------------------------------------\n");
    mt6311_set_vdvfs11_vosel_ctrl(0);
    printk("[exec_mt6311_vdvfs11_vosel_test_sub] mt6311_set_vdvfs11_vosel_sleep(0x40); mt6311_set_vdvfs11_vosel_on(0x20);\n");
                                                 mt6311_set_vdvfs11_vosel_sleep(0x40); mt6311_set_vdvfs11_vosel_on(0x20);
    mt6311_set_vdvfs11_vosel_ctrl(1);                                     
    printk("[exec_mt6311_vdvfs11_vosel_test_sub] set_mt6311_srclken_val(1); \n");
                                                 set_mt6311_srclken_val(1); read_adc_value(MT6311_VDVFS11_INDEX);
    printk("[exec_mt6311_vdvfs11_vosel_test_sub] set_mt6311_srclken_val(0); \n");
                                                 set_mt6311_srclken_val(0); read_adc_value(MT6311_VDVFS11_INDEX);

    printk("[exec_mt6311_vdvfs11_vosel_test_sub] restore to set_mt6311_srclken_val(1); \n");
                                                 set_mt6311_srclken_val(1);                                             
#else
    printk("[exec_mt6311_vdvfs11_vosel_test_sub] set_mt6311_srclken_val(0);\n");
                                                 set_mt6311_srclken_val(0); exec_mt6311_vdvfs11_vosel_test_sub_vosel();

    printk("[exec_mt6311_vdvfs11_vosel_test_sub] set_mt6311_srclken_val(1);\n");
                                                 set_mt6311_srclken_val(1); exec_mt6311_vdvfs11_vosel_test_sub_vosel();
#endif                                                 
}

void exec_mt6311_vdvfs11_vosel_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6311_set_vdvfs11_vosel_ctrl] %d\n", i);
        mt6311_set_vdvfs11_vosel_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6311_PMIC_VDVFS11_VOSEL_MASK;k++) {
                    mt6311_set_vdvfs11_vosel(k); printk("[mt6311_set_vdvfs11_vosel] k=%d, ",k);
                    if(k==0) mdelay(5000);
                    read_adc_value(MT6311_VDVFS11_INDEX);
                }
                break;

            case 1:
                #if 1
                exec_mt6311_vdvfs11_vosel_test_sub();
                #else
                printk("[mt6311_set_vdvfs11_vosel_sel(0)]\n");
                mt6311_set_vdvfs11_vosel_sel(0);
                exec_mt6311_vdvfs11_vosel_test_sub(); 

                printk("[mt6311_set_vdvfs11_vosel_sel(1)]\n");
                mt6311_set_vdvfs11_vosel_sel(1);                
                exec_mt6311_vdvfs11_vosel_test_sub();

                printk("[mt6311_set_vdvfs11_vosel_sel(2)]\n");
                mt6311_set_vdvfs11_vosel_sel(2);
                exec_mt6311_vdvfs11_vosel_test_sub();

                printk("[mt6311_set_vdvfs11_vosel_sel(3)]\n");
                mt6311_set_vdvfs11_vosel_sel(3);
                exec_mt6311_vdvfs11_vosel_test_sub();
                #endif
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_mt6311_vdvfs12_vosel_test_sub_vosel(void)
{
    int k=0;
        
    for(k=0;k<=MT6311_PMIC_VDVFS12_VOSEL_SLEEP_MASK;k++) {
        mt6311_set_vdvfs12_vosel_sleep(k); printk("[mt6311_set_vdvfs12_vosel_sleep] k=%d, ",k);
        if(k==0) mdelay(5000);
        read_adc_value(MT6311_VDVFS12_INDEX);
    }
    for(k=0;k<=MT6311_PMIC_VDVFS12_VOSEL_ON_MASK;k++) {
        mt6311_set_vdvfs12_vosel_on(k); printk("[mt6311_set_vdvfs12_vosel_on] k=%d, ",k);
        if(k==0) mdelay(5000);
        read_adc_value(MT6311_VDVFS12_INDEX);
    }
}

void exec_mt6311_vdvfs12_vosel_test_sub(void)
{
#if 1
    printk("[exec_mt6311_vdvfs12_vosel_test_sub] ----------------------------------------------------\n");
    mt6311_set_vdvfs12_vosel_ctrl(0);
    printk("[exec_mt6311_vdvfs12_vosel_test_sub] mt6311_set_vdvfs12_vosel_sleep(0x30); mt6311_set_vdvfs12_vosel_on(0x50);\n");
                                                 mt6311_set_vdvfs12_vosel_sleep(0x30); mt6311_set_vdvfs12_vosel_on(0x50);
    mt6311_set_vdvfs12_vosel_ctrl(1);                                     
    printk("[exec_mt6311_vdvfs12_vosel_test_sub] set_mt6311_srclken_val(1); \n");
                                                 set_mt6311_srclken_val(1); read_adc_value(MT6311_VDVFS12_INDEX);
    printk("[exec_mt6311_vdvfs12_vosel_test_sub] set_mt6311_srclken_val(0); \n");
                                                 set_mt6311_srclken_val(0); read_adc_value(MT6311_VDVFS12_INDEX);

    printk("[exec_mt6311_vdvfs12_vosel_test_sub] ----------------------------------------------------\n");
    mt6311_set_vdvfs12_vosel_ctrl(0);
    printk("[exec_mt6311_vdvfs12_vosel_test_sub] mt6311_set_vdvfs12_vosel_sleep(0x40); mt6311_set_vdvfs12_vosel_on(0x20);\n");
                                                 mt6311_set_vdvfs12_vosel_sleep(0x40); mt6311_set_vdvfs12_vosel_on(0x20);
    mt6311_set_vdvfs12_vosel_ctrl(1);                                     
    printk("[exec_mt6311_vdvfs12_vosel_test_sub] set_mt6311_srclken_val(1); \n");
                                                 set_mt6311_srclken_val(1); read_adc_value(MT6311_VDVFS12_INDEX);
    printk("[exec_mt6311_vdvfs12_vosel_test_sub] set_mt6311_srclken_val(0); \n");
                                                 set_mt6311_srclken_val(0); read_adc_value(MT6311_VDVFS12_INDEX);

    printk("[exec_mt6311_vdvfs12_vosel_test_sub] restore to set_mt6311_srclken_val(1); \n");
                                                 set_mt6311_srclken_val(1);                                                 
#else

    printk("[exec_mt6311_vdvfs12_vosel_test_sub] set_mt6311_srclken_val(0);\n");
                                                 set_mt6311_srclken_val(0); exec_mt6311_vdvfs12_vosel_test_sub_vosel();

    printk("[exec_mt6311_vdvfs12_vosel_test_sub] set_mt6311_srclken_val(1);\n");
                                                 set_mt6311_srclken_val(1); exec_mt6311_vdvfs12_vosel_test_sub_vosel();
#endif                                                 
}

void exec_mt6311_vdvfs12_vosel_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6311_set_vdvfs12_vosel_ctrl] %d\n", i);
        mt6311_set_vdvfs12_vosel_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6311_PMIC_VDVFS12_VOSEL_MASK;k++) {
                    mt6311_set_vdvfs12_vosel(k); printk("[mt6311_set_vdvfs12_vosel] k=%d, ",k);
                    if(k==0) mdelay(5000);
                    read_adc_value(MT6311_VDVFS12_INDEX);
                }
                break;

            case 1:
                #if 1
                exec_mt6311_vdvfs12_vosel_test_sub();
                #else
                printk("[mt6311_set_vdvfs12_vosel_sel(0)]\n");
                mt6311_set_vdvfs12_vosel_sel(0);
                exec_mt6311_vdvfs12_vosel_test_sub(); 

                printk("[mt6311_set_vdvfs12_vosel_sel(1)]\n");
                mt6311_set_vdvfs12_vosel_sel(1);                
                exec_mt6311_vdvfs12_vosel_test_sub();

                printk("[mt6311_set_vdvfs12_vosel_sel(2)]\n");
                mt6311_set_vdvfs12_vosel_sel(2);
                exec_mt6311_vdvfs12_vosel_test_sub();

                printk("[mt6311_set_vdvfs12_vosel_sel(3)]\n");
                mt6311_set_vdvfs12_vosel_sel(3);
                exec_mt6311_vdvfs12_vosel_test_sub();
                #endif
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void PMIC_BUCK_VOSEL(int index_val)
{
    printk("[PMIC_BUCK_VOSEL] start....\n");

    //      
    set_srclken_0_val(1); 
    set_srclken_1_val(1);
    set_srclken_sw_mode();
    
    mt6325_upmu_set_vcore1_en_ctrl(0);
    mt6325_upmu_set_vcore2_en_ctrl(0);
    mt6325_upmu_set_vdram_en_ctrl(0);
    mt6325_upmu_set_vgpu_en_ctrl(0);
    mt6325_upmu_set_vrf18_0_en_ctrl(0);
    mt6325_upmu_set_vio18_en_ctrl(0);
    mt6325_upmu_set_vdvfs11_en_ctrl(0);
    mt6325_upmu_set_vdvfs12_en_ctrl(0);
    mt6325_upmu_set_vpa_en_ctrl(0);

    //      
    set_mt6311_srclken_val(1);
    set_mt6311_srclken_sw_mode();
    
    mt6311_set_vdvfs11_en_ctrl(0);
    mt6311_set_vdvfs12_en_ctrl(0);
    mt6311_set_vdvfs11_en(1);
    mt6311_set_vdvfs12_en(1);

    switch(index_val){
      //      
      case 0 : mt6325_upmu_set_vcore1_en(1);  exec_vcore1_vosel_test();   break;
      case 1 : mt6325_upmu_set_vcore2_en(1);  exec_vcore2_vosel_test();   break;
      case 2 : mt6325_upmu_set_vdram_en(1);   exec_vdram_vosel_test();    break;
      case 3 : mt6325_upmu_set_vgpu_en(1);    exec_vgpu_vosel_test();     break;
      case 4 : mt6325_upmu_set_vrf18_0_en(1); exec_vrf18_0_vosel_test();  break;
      case 5 : mt6325_upmu_set_vio18_en(1);   exec_vio18_vosel_test();    break;
      case 6 : mt6325_upmu_set_vdvfs11_en(1); exec_vdvfs11_vosel_test();  break;
      case 7 : mt6325_upmu_set_vdvfs12_en(1); exec_vdvfs12_vosel_test();  break;
      case 8 : mt6325_upmu_set_vpa_en(1);     exec_vpa_vosel_test();      break;

      //        
      case MT6311_VDVFS11_INDEX : exec_mt6311_vdvfs11_vosel_test();  break;
      case MT6311_VDVFS12_INDEX : exec_mt6311_vdvfs12_vosel_test();  break;    

	  default:
        printk("[PMIC_BUCK_VOSEL] Invalid channel value(%d)\n", index_val);
        break;
    }

    printk("[PMIC_BUCK_VOSEL] end....\n");
}
#endif

#ifdef BUCK_DLC_DVT_ENABLE
void exec_vpa_dlc_test_sub_dlc(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VPA_DLC_SLEEP_MASK;k++) {
        mt6325_upmu_set_vpa_dlc_sleep(k);         
        printk("[exec_vpa_dlc_test_sub_dlc] dlc_sleep=%d, qi_dlc=%d\n", k, mt6325_upmu_get_qi_vpa_dlc());
    }
    
    for(k=0;k<=MT6325_PMIC_VPA_DLC_ON_MASK;k++) {
        mt6325_upmu_set_vpa_dlc_on(k);         
        printk("[exec_vpa_dlc_test_sub_dlc] dlc_on=%d, qi_dlc=%d\n", k, mt6325_upmu_get_qi_vpa_dlc());
    }
}

void exec_vpa_dlc_test_sub(void)
{
    printk("[exec_vpa_dlc_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vpa_dlc_test_sub_dlc();

    printk("[exec_vpa_dlc_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vpa_dlc_test_sub_dlc();

    printk("[exec_vpa_dlc_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vpa_dlc_test_sub_dlc();

    printk("[exec_vpa_dlc_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vpa_dlc_test_sub_dlc();
}

void exec_vpa_dlc_test(void)
{
    int i=0, k=0;

//                                                                 
    printk("[exec_vpa_dlc_test] mt6325_upmu_set_vpa_dlc_map_en(0);\n");
    mt6325_upmu_set_vpa_dlc_map_en(0);
//                                                                 
    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vpa_dlc_ctrl] %d\n", i);
        mt6325_upmu_set_vpa_dlc_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VPA_DLC_MASK;k++) {
                    mt6325_upmu_set_vpa_dlc(k);                     
                    printk("[exec_vpa_dlc_test] dlc=%d, qi_dlc=%d\n", k, mt6325_upmu_get_qi_vpa_dlc());
                }                    
                break;

            case 1:
                printk("[mt6325_upmu_set_vpa_dlc_sel(0)]\n");
                mt6325_upmu_set_vpa_dlc_sel(0);
                exec_vpa_dlc_test_sub(); 

                printk("[mt6325_upmu_set_vpa_dlc_sel(1)]\n");
                mt6325_upmu_set_vpa_dlc_sel(1);                
                exec_vpa_dlc_test_sub();

                printk("[mt6325_upmu_set_vpa_dlc_sel(2)]\n");
                mt6325_upmu_set_vpa_dlc_sel(2);
                exec_vpa_dlc_test_sub();

                printk("[mt6325_upmu_set_vpa_dlc_sel(3)]\n");
                mt6325_upmu_set_vpa_dlc_sel(3);
                exec_vpa_dlc_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    
//                                                                 
    printk("[exec_vpa_dlc_test] mt6325_upmu_set_vpa_dlc_map_en(1);\n");
    mt6325_upmu_set_vpa_dlc_map_en(1);
//                                                                 
    printk("[exec_vpa_dlc_test] mt6325_upmu_set_vpa_vosel_dlc001(20);\n");
    printk("[exec_vpa_dlc_test] mt6325_upmu_set_vpa_vosel_dlc011(30);\n");
    printk("[exec_vpa_dlc_test] mt6325_upmu_set_vpa_vosel_dlc111(50);\n");
    mt6325_upmu_set_vpa_vosel_dlc001(20);
    mt6325_upmu_set_vpa_vosel_dlc011(30);
    mt6325_upmu_set_vpa_vosel_dlc111(50);

    printk("[exec_vpa_dlc_test] mt6325_upmu_set_vpa_vosel_ctrl(0);\n");
    mt6325_upmu_set_vpa_vosel_ctrl(0);
    
    for(k=0;k<=MT6325_PMIC_VPA_VOSEL_MASK;k++) {
        mt6325_upmu_set_vpa_vosel(k); 
        printk("[exec_vpa_dlc_test] ni_vosel=%d, qi_dlc=%d\n", mt6325_upmu_get_ni_vpa_vosel(), mt6325_upmu_get_qi_vpa_dlc());
    }

    
//                                                                 
    printk("[exec_vpa_dlc_test] mt6325_upmu_set_vpa_dlc_map_en(0);\n");
    mt6325_upmu_set_vpa_dlc_map_en(0);
//                                                                 

}

void PMIC_BUCK_DLC(int index_val)
{
    printk("[PMIC_BUCK_DLC] start....\n");

    set_srclken_0_val(1); 
    set_srclken_1_val(1);
    set_srclken_sw_mode();

    //      
    mt6325_upmu_set_vcore1_en_ctrl(0);
    mt6325_upmu_set_vcore2_en_ctrl(0);
    mt6325_upmu_set_vdram_en_ctrl(0);
    mt6325_upmu_set_vgpu_en_ctrl(0);
    mt6325_upmu_set_vrf18_0_en_ctrl(0);
    mt6325_upmu_set_vio18_en_ctrl(0);
    mt6325_upmu_set_vdvfs11_en_ctrl(0);
    mt6325_upmu_set_vdvfs12_en_ctrl(0);
    mt6325_upmu_set_vpa_en_ctrl(0);

    switch(index_val){
      //      
      case 0 : printk("[PMIC_BUCK_DLC] no DLC function\n");                break;
      case 1 : printk("[PMIC_BUCK_DLC] no DLC function\n");                break;
      case 2 : printk("[PMIC_BUCK_DLC] no DLC function\n");                break;
      case 3 : printk("[PMIC_BUCK_DLC] no DLC function\n");                break;
      case 4 : printk("[PMIC_BUCK_DLC] no DLC function\n");                break;
      case 5 : printk("[PMIC_BUCK_DLC] no DLC function\n");                break;
      case 6 : printk("[PMIC_BUCK_DLC] no DLC function\n");                break;
      case 7 : printk("[PMIC_BUCK_DLC] no DLC function\n");                break;
      case 8 : mt6325_upmu_set_vpa_en(1);   exec_vpa_dlc_test();           break;

	  default:
        printk("[PMIC_BUCK_DLC] Invalid channel value(%d)\n", index_val);
        break;
    }

    printk("[PMIC_BUCK_DLC] end....\n");
}
#endif

#ifdef BUCK_BURST_DVT_ENABLE
void exec_vcore1_burst_test_sub_burst(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VCORE1_BURST_SLEEP_MASK;k++) {
        mt6325_upmu_set_vcore1_burst_sleep(k);         
        printk("[exec_vcore1_burst_test_sub_burst] burst_sleep=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vcore1_burst());
    }
    
    for(k=0;k<=MT6325_PMIC_VCORE1_BURST_ON_MASK;k++) {
        mt6325_upmu_set_vcore1_burst_on(k);        
        printk("[exec_vcore1_burst_test_sub_burst] burst_on=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vcore1_burst());
    }
}

void exec_vcore1_burst_test_sub(void)
{
    printk("[exec_vcore1_burst_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vcore1_burst_test_sub_burst();

    printk("[exec_vcore1_burst_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vcore1_burst_test_sub_burst();

    printk("[exec_vcore1_burst_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vcore1_burst_test_sub_burst();

    printk("[exec_vcore1_burst_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vcore1_burst_test_sub_burst();
}

void exec_vcore1_burst_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vcore1_burst_ctrl] %d\n", i);
        mt6325_upmu_set_vcore1_burst_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VCORE1_BURST_MASK;k++) {
                    mt6325_upmu_set_vcore1_burst(k);                     
                    printk("[exec_vcore1_burst_test] burst=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vcore1_burst());
                }                    
                break;

            case 1:
                printk("[mt6325_upmu_set_vcore1_burst_sel(0)]\n");
                mt6325_upmu_set_vcore1_burst_sel(0);
                exec_vcore1_burst_test_sub(); 

                printk("[mt6325_upmu_set_vcore1_burst_sel(1)]\n");
                mt6325_upmu_set_vcore1_burst_sel(1);                
                exec_vcore1_burst_test_sub();

                printk("[mt6325_upmu_set_vcore1_burst_sel(2)]\n");
                mt6325_upmu_set_vcore1_burst_sel(2);
                exec_vcore1_burst_test_sub();

                printk("[mt6325_upmu_set_vcore1_burst_sel(3)]\n");
                mt6325_upmu_set_vcore1_burst_sel(3);
                exec_vcore1_burst_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vcore2_burst_test_sub_burst(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VCORE2_BURST_SLEEP_MASK;k++) {
        mt6325_upmu_set_vcore2_burst_sleep(k);         
        printk("[exec_vcore2_burst_test_sub_burst] burst_sleep=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vcore2_burst());
    }
    
    for(k=0;k<=MT6325_PMIC_VCORE2_BURST_ON_MASK;k++) {
        mt6325_upmu_set_vcore2_burst_on(k);        
        printk("[exec_vcore2_burst_test_sub_burst] burst_on=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vcore2_burst());
    }
}

void exec_vcore2_burst_test_sub(void)
{
    printk("[exec_vcore2_burst_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vcore2_burst_test_sub_burst();

    printk("[exec_vcore2_burst_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vcore2_burst_test_sub_burst();

    printk("[exec_vcore2_burst_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vcore2_burst_test_sub_burst();

    printk("[exec_vcore2_burst_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vcore2_burst_test_sub_burst();
}

void exec_vcore2_burst_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vcore2_burst_ctrl] %d\n", i);
        mt6325_upmu_set_vcore2_burst_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VCORE2_BURST_MASK;k++) {
                    mt6325_upmu_set_vcore2_burst(k);                     
                    printk("[exec_vcore2_burst_test] burst=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vcore2_burst());
                }                    
                break;

            case 1:
                printk("[mt6325_upmu_set_vcore2_burst_sel(0)]\n");
                mt6325_upmu_set_vcore2_burst_sel(0);
                exec_vcore2_burst_test_sub(); 

                printk("[mt6325_upmu_set_vcore2_burst_sel(1)]\n");
                mt6325_upmu_set_vcore2_burst_sel(1);                
                exec_vcore2_burst_test_sub();

                printk("[mt6325_upmu_set_vcore2_burst_sel(2)]\n");
                mt6325_upmu_set_vcore2_burst_sel(2);
                exec_vcore2_burst_test_sub();

                printk("[mt6325_upmu_set_vcore2_burst_sel(3)]\n");
                mt6325_upmu_set_vcore2_burst_sel(3);
                exec_vcore2_burst_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vdram_burst_test_sub_burst(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VDRAM_BURST_SLEEP_MASK;k++) {
        mt6325_upmu_set_vdram_burst_sleep(k);         
        printk("[exec_vdram_burst_test_sub_burst] burst_sleep=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vdram_burst());
    }
    
    for(k=0;k<=MT6325_PMIC_VDRAM_BURST_ON_MASK;k++) {
        mt6325_upmu_set_vdram_burst_on(k);        
        printk("[exec_vdram_burst_test_sub_burst] burst_on=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vdram_burst());
    }
}

void exec_vdram_burst_test_sub(void)
{
    printk("[exec_vdram_burst_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vdram_burst_test_sub_burst();

    printk("[exec_vdram_burst_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vdram_burst_test_sub_burst();

    printk("[exec_vdram_burst_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vdram_burst_test_sub_burst();

    printk("[exec_vdram_burst_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vdram_burst_test_sub_burst();
}

void exec_vdram_burst_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vdram_burst_ctrl] %d\n", i);
        mt6325_upmu_set_vdram_burst_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VDRAM_BURST_MASK;k++) {
                    mt6325_upmu_set_vdram_burst(k);                     
                    printk("[exec_vdram_burst_test] burst=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vdram_burst());
                }                    
                break;

            case 1:
                printk("[mt6325_upmu_set_vdram_burst_sel(0)]\n");
                mt6325_upmu_set_vdram_burst_sel(0);
                exec_vdram_burst_test_sub(); 

                printk("[mt6325_upmu_set_vdram_burst_sel(1)]\n");
                mt6325_upmu_set_vdram_burst_sel(1);                
                exec_vdram_burst_test_sub();

                printk("[mt6325_upmu_set_vdram_burst_sel(2)]\n");
                mt6325_upmu_set_vdram_burst_sel(2);
                exec_vdram_burst_test_sub();

                printk("[mt6325_upmu_set_vdram_burst_sel(3)]\n");
                mt6325_upmu_set_vdram_burst_sel(3);
                exec_vdram_burst_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vgpu_burst_test_sub_burst(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VGPU_BURST_SLEEP_MASK;k++) {
        mt6325_upmu_set_vgpu_burst_sleep(k);         
        printk("[exec_vgpu_burst_test_sub_burst] burst_sleep=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vgpu_burst());
    }
    
    for(k=0;k<=MT6325_PMIC_VGPU_BURST_ON_MASK;k++) {
        mt6325_upmu_set_vgpu_burst_on(k);        
        printk("[exec_vgpu_burst_test_sub_burst] burst_on=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vgpu_burst());
    }
}

void exec_vgpu_burst_test_sub(void)
{
    printk("[exec_vgpu_burst_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vgpu_burst_test_sub_burst();

    printk("[exec_vgpu_burst_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vgpu_burst_test_sub_burst();

    printk("[exec_vgpu_burst_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vgpu_burst_test_sub_burst();

    printk("[exec_vgpu_burst_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vgpu_burst_test_sub_burst();
}

void exec_vgpu_burst_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vgpu_burst_ctrl] %d\n", i);
        mt6325_upmu_set_vgpu_burst_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VGPU_BURST_MASK;k++) {
                    mt6325_upmu_set_vgpu_burst(k);                     
                    printk("[exec_vgpu_burst_test] burst=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vgpu_burst());
                }                    
                break;

            case 1:
                printk("[mt6325_upmu_set_vgpu_burst_sel(0)]\n");
                mt6325_upmu_set_vgpu_burst_sel(0);
                exec_vgpu_burst_test_sub(); 

                printk("[mt6325_upmu_set_vgpu_burst_sel(1)]\n");
                mt6325_upmu_set_vgpu_burst_sel(1);                
                exec_vgpu_burst_test_sub();

                printk("[mt6325_upmu_set_vgpu_burst_sel(2)]\n");
                mt6325_upmu_set_vgpu_burst_sel(2);
                exec_vgpu_burst_test_sub();

                printk("[mt6325_upmu_set_vgpu_burst_sel(3)]\n");
                mt6325_upmu_set_vgpu_burst_sel(3);
                exec_vgpu_burst_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void exec_vio18_burst_test_sub_burst(void)
{
    int k=0;
        
    for(k=0;k<=MT6325_PMIC_VIO18_BURST_SLEEP_MASK;k++) {
        mt6325_upmu_set_vio18_burst_sleep(k);         
        printk("[exec_vio18_burst_test_sub_burst] burst_sleep=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vio18_burst());
    }
    
    for(k=0;k<=MT6325_PMIC_VIO18_BURST_ON_MASK;k++) {
        mt6325_upmu_set_vio18_burst_on(k);        
        printk("[exec_vio18_burst_test_sub_burst] burst_on=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vio18_burst());
    }
}

void exec_vio18_burst_test_sub(void)
{
    printk("[exec_vio18_burst_test_sub] set_srclken_0_val(0); set_srclken_1_val(0);\n");
                              set_srclken_0_val(0); set_srclken_1_val(0); exec_vio18_burst_test_sub_burst();

    printk("[exec_vio18_burst_test_sub] set_srclken_0_val(0); set_srclken_1_val(1);\n");
                              set_srclken_0_val(0); set_srclken_1_val(1); exec_vio18_burst_test_sub_burst();

    printk("[exec_vio18_burst_test_sub] set_srclken_0_val(1); set_srclken_1_val(0);\n");
                              set_srclken_0_val(1); set_srclken_1_val(0); exec_vio18_burst_test_sub_burst();

    printk("[exec_vio18_burst_test_sub] set_srclken_0_val(1); set_srclken_1_val(1);\n");
                              set_srclken_0_val(1); set_srclken_1_val(1); exec_vio18_burst_test_sub_burst();
}

void exec_vio18_burst_test(void)
{
    int i=0, k=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_vio18_burst_ctrl] %d\n", i);
        mt6325_upmu_set_vio18_burst_ctrl(i);

        switch(i){
            case 0:
                for(k=0;k<=MT6325_PMIC_VIO18_BURST_MASK;k++) {
                    mt6325_upmu_set_vio18_burst(k);                     
                    printk("[exec_vio18_burst_test] burst=%d, qi_burst=%d\n", k, mt6325_upmu_get_qi_vio18_burst());
                }                    
                break;

            case 1:
                printk("[mt6325_upmu_set_vio18_burst_sel(0)]\n");
                mt6325_upmu_set_vio18_burst_sel(0);
                exec_vio18_burst_test_sub(); 

                printk("[mt6325_upmu_set_vio18_burst_sel(1)]\n");
                mt6325_upmu_set_vio18_burst_sel(1);                
                exec_vio18_burst_test_sub();

                printk("[mt6325_upmu_set_vio18_burst_sel(2)]\n");
                mt6325_upmu_set_vio18_burst_sel(2);
                exec_vio18_burst_test_sub();

                printk("[mt6325_upmu_set_vio18_burst_sel(3)]\n");
                mt6325_upmu_set_vio18_burst_sel(3);
                exec_vio18_burst_test_sub();
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }
}

void PMIC_BUCK_BURST(int index_val)
{
    printk("[PMIC_BUCK_BURST] start....\n");

    set_srclken_0_val(1); 
    set_srclken_1_val(1);
    set_srclken_sw_mode();

    //      
    mt6325_upmu_set_vcore1_en_ctrl(0);
    mt6325_upmu_set_vcore2_en_ctrl(0);
    mt6325_upmu_set_vdram_en_ctrl(0);
    mt6325_upmu_set_vgpu_en_ctrl(0);
    mt6325_upmu_set_vrf18_0_en_ctrl(0);
    mt6325_upmu_set_vio18_en_ctrl(0);
    mt6325_upmu_set_vdvfs11_en_ctrl(0);
    mt6325_upmu_set_vdvfs12_en_ctrl(0);
    mt6325_upmu_set_vpa_en_ctrl(0);

    switch(index_val){
      //      
      case 0 : mt6325_upmu_set_vcore1_en(1); exec_vcore1_burst_test();  break;
      case 1 : mt6325_upmu_set_vcore2_en(1); exec_vcore2_burst_test();  break;
      case 2 : mt6325_upmu_set_vdram_en(1);  exec_vdram_burst_test();   break;
      case 3 : mt6325_upmu_set_vgpu_en(1);   exec_vgpu_burst_test();    break;
      case 4 : printk("[PMIC_BUCK_BURST] no BURST function\n");         break;
      case 5 : mt6325_upmu_set_vio18_en(1);  exec_vio18_burst_test();   break;
      case 6 : printk("[PMIC_BUCK_BURST] no BURST function\n");         break;
      case 7 : printk("[PMIC_BUCK_BURST] no BURST function\n");         break;
      case 8 : printk("[PMIC_BUCK_BURST] no BURST function\n");         break;

	  default:
        printk("[PMIC_BUCK_BURST] Invalid channel value(%d)\n", index_val);
        break;
    }

    printk("[PMIC_BUCK_BURST] end....\n");
}
#endif

//                                        
//    
//                                        
#ifdef LDO_ON_OFF_DVT_ENABLE
void exec_6325_ldo_vtcxo0_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vtcxo0_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vtcxo0_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VTCXO0]\n");
                        hwPowerOn(MT6325_POWER_LDO_VTCXO0, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VTCXO0]\n");
                        hwPowerDown(MT6325_POWER_LDO_VTCXO0, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VTCXO0]\n");
                        hwPowerOn(MT6325_POWER_LDO_VTCXO0, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VTCXO0]\n");
                        hwPowerDown(MT6325_POWER_LDO_VTCXO0, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vtcxo0_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vtcxo0_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vtcxo0_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vtcxo0_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vtcxo0_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vtcxo0_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vtcxo0_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vtcxo0_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vtcxo0_on_ctrl(0);\n"); mt6325_upmu_set_rg_vtcxo0_on_ctrl(0);
}

void exec_6325_ldo_vtcxo1_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vtcxo1_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vtcxo1_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VTCXO1]\n");
                        hwPowerOn(MT6325_POWER_LDO_VTCXO1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VTCXO1]\n");
                        hwPowerDown(MT6325_POWER_LDO_VTCXO1, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VTCXO1]\n");
                        hwPowerOn(MT6325_POWER_LDO_VTCXO1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VTCXO1]\n");
                        hwPowerDown(MT6325_POWER_LDO_VTCXO1, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vtcxo1_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vtcxo1_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vtcxo1_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vtcxo1_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vtcxo1_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vtcxo1_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vtcxo1_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vtcxo1_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vtcxo1_on_ctrl(0);\n"); mt6325_upmu_set_rg_vtcxo1_on_ctrl(0);
}

void exec_6325_ldo_vaud28_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vaud28_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vaud28_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VAUD28]\n");
                        hwPowerOn(MT6325_POWER_LDO_VAUD28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VAUD28]\n");
                        hwPowerDown(MT6325_POWER_LDO_VAUD28, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VAUD28]\n");
                        hwPowerOn(MT6325_POWER_LDO_VAUD28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VAUD28]\n");
                        hwPowerDown(MT6325_POWER_LDO_VAUD28, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vaud28_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vaud28_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vaud28_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vaud28_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vaud28_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vaud28_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vaud28_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vaud28_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vaud28_on_ctrl(0);\n"); mt6325_upmu_set_rg_vaud28_on_ctrl(0);
}

void exec_6325_ldo_vauxa28_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vauxa28_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vauxa28_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VAUXA28]\n");
                        hwPowerOn(MT6325_POWER_LDO_VAUXA28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VAUXA28]\n");
                        hwPowerDown(MT6325_POWER_LDO_VAUXA28, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VAUXA28]\n");
                        hwPowerOn(MT6325_POWER_LDO_VAUXA28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VAUXA28]\n");
                        hwPowerDown(MT6325_POWER_LDO_VAUXA28, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vauxa28_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vauxa28_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vauxa28_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vauxa28_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vauxa28_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vauxa28_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vauxa28_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vauxa28_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vauxa28_on_ctrl(0);\n"); mt6325_upmu_set_rg_vauxa28_on_ctrl(0);
}

void exec_6325_ldo_vbif28_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vbif28_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vbif28_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VBIF28]\n");
                        hwPowerOn(MT6325_POWER_LDO_VBIF28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VBIF28]\n");
                        hwPowerDown(MT6325_POWER_LDO_VBIF28, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VBIF28]\n");
                        hwPowerOn(MT6325_POWER_LDO_VBIF28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VBIF28]\n");
                        hwPowerDown(MT6325_POWER_LDO_VBIF28, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vbif28_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vbif28_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vbif28_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vbif28_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vbif28_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vbif28_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vbif28_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vbif28_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vbif28_on_ctrl(0);\n"); mt6325_upmu_set_rg_vbif28_on_ctrl(0);
}

void exec_6325_ldo_vcama_en_test(void)
{
    printk("hwPowerOn(MT6325_POWER_LDO_VCAMA]\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMA, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

    printk("hwPowerDown(MT6325_POWER_LDO_VCAMA]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMA, "ldo_dvt"); read_adc_value(LDO_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAMA]\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMA, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

    printk("hwPowerDown(MT6325_POWER_LDO_VCAMA]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMA, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
}

void exec_6325_ldo_vcn28_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vcn28_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vcn28_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VCN28]\n");
                        hwPowerOn(MT6325_POWER_LDO_VCN28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VCN28]\n");
                        hwPowerDown(MT6325_POWER_LDO_VCN28, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VCN28]\n");
                        hwPowerOn(MT6325_POWER_LDO_VCN28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VCN28]\n");
                        hwPowerDown(MT6325_POWER_LDO_VCN28, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vcn28_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vcn28_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vcn28_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vcn28_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vcn28_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vcn28_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vcn28_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vcn28_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vcn28_on_ctrl(0);\n"); mt6325_upmu_set_rg_vcn28_on_ctrl(0);
}

void exec_6325_ldo_vcn33_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vcn33_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vcn33_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VCN33]\n");
                        hwPowerOn(MT6325_POWER_LDO_VCN33, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VCN33]\n");
                        hwPowerDown(MT6325_POWER_LDO_VCN33, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VCN33]\n");
                        hwPowerOn(MT6325_POWER_LDO_VCN33, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VCN33]\n");
                        hwPowerDown(MT6325_POWER_LDO_VCN33, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vcn33_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vcn33_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vcn33_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vcn33_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vcn33_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vcn33_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vcn33_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vcn33_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vcn33_on_ctrl(0);\n"); mt6325_upmu_set_rg_vcn33_on_ctrl(0);
}

void exec_6325_ldo_vrf18_1_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vrf18_1_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vrf18_1_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VRF18_1]\n");
                        hwPowerOn(MT6325_POWER_LDO_VRF18_1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VRF18_1]\n");
                        hwPowerDown(MT6325_POWER_LDO_VRF18_1, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VRF18_1]\n");
                        hwPowerOn(MT6325_POWER_LDO_VRF18_1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VRF18_1]\n");
                        hwPowerDown(MT6325_POWER_LDO_VRF18_1, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vrf18_1_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vrf18_1_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vrf18_1_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vrf18_1_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vrf18_1_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vrf18_1_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vrf18_1_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vrf18_1_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vrf18_1_on_ctrl(0);\n"); mt6325_upmu_set_rg_vrf18_1_on_ctrl(0);
}

void exec_6325_ldo_vusb33_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vusb33_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vusb33_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VUSB33]\n");
                        hwPowerOn(MT6325_POWER_LDO_VUSB33, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VUSB33]\n");
                        hwPowerDown(MT6325_POWER_LDO_VUSB33, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VUSB33]\n");
                        hwPowerOn(MT6325_POWER_LDO_VUSB33, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VUSB33]\n");
                        hwPowerDown(MT6325_POWER_LDO_VUSB33, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vusb33_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vusb33_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vusb33_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vusb33_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vusb33_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vusb33_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vusb33_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vusb33_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vusb33_on_ctrl(0);\n"); mt6325_upmu_set_rg_vusb33_on_ctrl(0);
}

void exec_6325_ldo_vmch_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vmch_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vmch_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VMCH]\n");
                        hwPowerOn(MT6325_POWER_LDO_VMCH, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VMCH]\n");
                        hwPowerDown(MT6325_POWER_LDO_VMCH, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VMCH]\n");
                        hwPowerOn(MT6325_POWER_LDO_VMCH, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VMCH]\n");
                        hwPowerDown(MT6325_POWER_LDO_VMCH, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vmch_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vmch_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vmch_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vmch_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vmch_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vmch_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vmch_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vmch_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vmch_on_ctrl(0);\n"); mt6325_upmu_set_rg_vmch_on_ctrl(0);
}

void exec_6325_ldo_vmc_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vmc_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vmc_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VMC]\n");
                        hwPowerOn(MT6325_POWER_LDO_VMC, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VMC]\n");
                        hwPowerDown(MT6325_POWER_LDO_VMC, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VMC]\n");
                        hwPowerOn(MT6325_POWER_LDO_VMC, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VMC]\n");
                        hwPowerDown(MT6325_POWER_LDO_VMC, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vmc_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vmc_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vmc_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vmc_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vmc_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vmc_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vmc_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vmc_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vmc_on_ctrl(0);\n"); mt6325_upmu_set_rg_vmc_on_ctrl(0);
}

void exec_6325_ldo_vemc33_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vemc33_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vemc33_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VEMC33]\n");
                        hwPowerOn(MT6325_POWER_LDO_VEMC33, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VEMC33]\n");
                        hwPowerDown(MT6325_POWER_LDO_VEMC33, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VEMC33]\n");
                        hwPowerOn(MT6325_POWER_LDO_VEMC33, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VEMC33]\n");
                        hwPowerDown(MT6325_POWER_LDO_VEMC33, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vemc33_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vemc33_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vemc33_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vemc33_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vemc33_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vemc33_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vemc33_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vemc33_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vemc33_on_ctrl(0);\n"); mt6325_upmu_set_rg_vemc33_on_ctrl(0);
}

void exec_6325_ldo_vio28_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vio28_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vio28_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VIO28]\n");
                        hwPowerOn(MT6325_POWER_LDO_VIO28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VIO28]\n");
                        hwPowerDown(MT6325_POWER_LDO_VIO28, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VIO28]\n");
                        hwPowerOn(MT6325_POWER_LDO_VIO28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VIO28]\n");
                        hwPowerDown(MT6325_POWER_LDO_VIO28, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vio28_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vio28_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vio28_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vio28_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vio28_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vio28_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vio28_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vio28_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vio28_on_ctrl(0);\n"); mt6325_upmu_set_rg_vio28_on_ctrl(0);
}

void exec_6325_ldo_vcam_af_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vcam_af_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vcam_af_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VCAM_AF]\n");
                        hwPowerOn(MT6325_POWER_LDO_VCAM_AF, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VCAM_AF]\n");
                        hwPowerDown(MT6325_POWER_LDO_VCAM_AF, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VCAM_AF]\n");
                        hwPowerOn(MT6325_POWER_LDO_VCAM_AF, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VCAM_AF]\n");
                        hwPowerDown(MT6325_POWER_LDO_VCAM_AF, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vcam_af_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vcam_af_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vcam_af_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vcam_af_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vcam_af_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vcam_af_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vcam_af_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vcam_af_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vcam_af_on_ctrl(0);\n"); mt6325_upmu_set_rg_vcam_af_on_ctrl(0);
}

void exec_6325_ldo_vgp1_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vgp1_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vgp1_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VGP1]\n");
                        hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VGP1]\n");
                        hwPowerDown(MT6325_POWER_LDO_VGP1, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VGP1]\n");
                        hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VGP1]\n");
                        hwPowerDown(MT6325_POWER_LDO_VGP1, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vgp1_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vgp1_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vgp1_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vgp1_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vgp1_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vgp1_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vgp1_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vgp1_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vgp1_on_ctrl(0);\n"); mt6325_upmu_set_rg_vgp1_on_ctrl(0);
}

void exec_6325_ldo_vefuse_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vefuse_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vefuse_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VEFUSE]\n");
                        hwPowerOn(MT6325_POWER_LDO_VEFUSE, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VEFUSE]\n");
                        hwPowerDown(MT6325_POWER_LDO_VEFUSE, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VEFUSE]\n");
                        hwPowerOn(MT6325_POWER_LDO_VEFUSE, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VEFUSE]\n");
                        hwPowerDown(MT6325_POWER_LDO_VEFUSE, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vefuse_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vefuse_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vefuse_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vefuse_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vefuse_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vefuse_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vefuse_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vefuse_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vefuse_on_ctrl(0);\n"); mt6325_upmu_set_rg_vefuse_on_ctrl(0);
}

void exec_6325_ldo_vsim1_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vsim1_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vsim1_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VSIM1]\n");
                        hwPowerOn(MT6325_POWER_LDO_VSIM1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VSIM1]\n");
                        hwPowerDown(MT6325_POWER_LDO_VSIM1, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VSIM1]\n");
                        hwPowerOn(MT6325_POWER_LDO_VSIM1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VSIM1]\n");
                        hwPowerDown(MT6325_POWER_LDO_VSIM1, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vsim1_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vsim1_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vsim1_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vsim1_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vsim1_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vsim1_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vsim1_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vsim1_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vsim1_on_ctrl(0);\n"); mt6325_upmu_set_rg_vsim1_on_ctrl(0);
}

void exec_6325_ldo_vsim2_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vsim2_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vsim2_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VSIM2]\n");
                        hwPowerOn(MT6325_POWER_LDO_VSIM2, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VSIM2]\n");
                        hwPowerDown(MT6325_POWER_LDO_VSIM2, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VSIM2]\n");
                        hwPowerOn(MT6325_POWER_LDO_VSIM2, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VSIM2]\n");
                        hwPowerDown(MT6325_POWER_LDO_VSIM2, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vsim2_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vsim2_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vsim2_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vsim2_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vsim2_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vsim2_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vsim2_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vsim2_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vsim2_on_ctrl(0);\n"); mt6325_upmu_set_rg_vsim2_on_ctrl(0);
}

void exec_6325_ldo_vmipi_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vmipi_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vmipi_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VMIPI]\n");
                        hwPowerOn(MT6325_POWER_LDO_VMIPI, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VMIPI]\n");
                        hwPowerDown(MT6325_POWER_LDO_VMIPI, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VMIPI]\n");
                        hwPowerOn(MT6325_POWER_LDO_VMIPI, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VMIPI]\n");
                        hwPowerDown(MT6325_POWER_LDO_VMIPI, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vmipi_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vmipi_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vmipi_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vmipi_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vmipi_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vmipi_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vmipi_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vmipi_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vmipi_on_ctrl(0);\n"); mt6325_upmu_set_rg_vmipi_on_ctrl(0);
}

void exec_6325_ldo_vcn18_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vcn18_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vcn18_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VCN18]\n");
                        hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VCN18]\n");
                        hwPowerDown(MT6325_POWER_LDO_VCN18, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VCN18]\n");
                        hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VCN18]\n");
                        hwPowerDown(MT6325_POWER_LDO_VCN18, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vcn18_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vcn18_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vcn18_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vcn18_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vcn18_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vcn18_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vcn18_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vcn18_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vcn18_on_ctrl(0);\n"); mt6325_upmu_set_rg_vcn18_on_ctrl(0);
}

void exec_6325_ldo_vgp2_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vgp2_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vgp2_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VGP2]\n");
                        hwPowerOn(MT6325_POWER_LDO_VGP2, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VGP2]\n");
                        hwPowerDown(MT6325_POWER_LDO_VGP2, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VGP2]\n");
                        hwPowerOn(MT6325_POWER_LDO_VGP2, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VGP2]\n");
                        hwPowerDown(MT6325_POWER_LDO_VGP2, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vgp2_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vgp2_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vgp2_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vgp2_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vgp2_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vgp2_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vgp2_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vgp2_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vgp2_on_ctrl(0);\n"); mt6325_upmu_set_rg_vgp2_on_ctrl(0);
}

void exec_6325_ldo_vcamd_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vcamd_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vcamd_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VCAMD]\n");
                        hwPowerOn(MT6325_POWER_LDO_VCAMD, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VCAMD]\n");
                        hwPowerDown(MT6325_POWER_LDO_VCAMD, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VCAMD]\n");
                        hwPowerOn(MT6325_POWER_LDO_VCAMD, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VCAMD]\n");
                        hwPowerDown(MT6325_POWER_LDO_VCAMD, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vcamd_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vcamd_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vcamd_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vcamd_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vcamd_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vcamd_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vcamd_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vcamd_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vcamd_on_ctrl(0);\n"); mt6325_upmu_set_rg_vcamd_on_ctrl(0);
}

void exec_6325_ldo_vcam_io_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vcam_io_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vcam_io_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VCAM_IO]\n");
                        hwPowerOn(MT6325_POWER_LDO_VCAM_IO, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VCAM_IO]\n");
                        hwPowerDown(MT6325_POWER_LDO_VCAM_IO, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VCAM_IO]\n");
                        hwPowerOn(MT6325_POWER_LDO_VCAM_IO, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VCAM_IO]\n");
                        hwPowerDown(MT6325_POWER_LDO_VCAM_IO, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vcam_io_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vcam_io_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vcam_io_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vcam_io_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vcam_io_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vcam_io_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vcam_io_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vcam_io_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vcam_io_on_ctrl(0);\n"); mt6325_upmu_set_rg_vcam_io_on_ctrl(0);
}

void exec_6325_ldo_vsram_dvfs1_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vsram_dvfs1_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vsram_dvfs1_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VSRAM_DVFS1]\n");
                        hwPowerOn(MT6325_POWER_LDO_VSRAM_DVFS1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VSRAM_DVFS1]\n");
                        hwPowerDown(MT6325_POWER_LDO_VSRAM_DVFS1, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VSRAM_DVFS1]\n");
                        hwPowerOn(MT6325_POWER_LDO_VSRAM_DVFS1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VSRAM_DVFS1]\n");
                        hwPowerDown(MT6325_POWER_LDO_VSRAM_DVFS1, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vsram_dvfs1_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vsram_dvfs1_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vsram_dvfs1_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vsram_dvfs1_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vsram_dvfs1_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vsram_dvfs1_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vsram_dvfs1_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vsram_dvfs1_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vsram_dvfs1_on_ctrl(0);\n"); mt6325_upmu_set_rg_vsram_dvfs1_on_ctrl(0);
}

void exec_6325_ldo_vgp3_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vgp3_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vgp3_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VGP3]\n");
                        hwPowerOn(MT6325_POWER_LDO_VGP3, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VGP3]\n");
                        hwPowerDown(MT6325_POWER_LDO_VGP3, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VGP3]\n");
                        hwPowerOn(MT6325_POWER_LDO_VGP3, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VGP3]\n");
                        hwPowerDown(MT6325_POWER_LDO_VGP3, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vgp3_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vgp3_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vgp3_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vgp3_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vgp3_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vgp3_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vgp3_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vgp3_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vgp3_on_ctrl(0);\n"); mt6325_upmu_set_rg_vgp3_on_ctrl(0);
}

void exec_6325_ldo_vbiasn_en_test(void)
{
    int i=0;

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vbiasn_on_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vbiasn_on_ctrl(i);

        switch(i){
            case 0:
                printk("hwPowerOn(MT6325_POWER_LDO_VBIASN]\n");
                        hwPowerOn(MT6325_POWER_LDO_VBIASN, VOL_0800, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VBIASN]\n");
                        hwPowerDown(MT6325_POWER_LDO_VBIASN, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerOn(MT6325_POWER_LDO_VBIASN]\n");
                        hwPowerOn(MT6325_POWER_LDO_VBIASN, VOL_0800, "ldo_dvt"); read_adc_value(LDO_ONLY);

                printk("hwPowerDown(MT6325_POWER_LDO_VBIASN]\n");
                        hwPowerDown(MT6325_POWER_LDO_VBIASN, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vbiasn_srclk_en_sel(0)]\n");
                         mt6325_upmu_set_rg_vbiasn_srclk_en_sel(0); exec_scrxxx_map(LDO_ONLY);                       

                printk("[mt6325_upmu_set_rg_vbiasn_srclk_en_sel(1)]\n");
                         mt6325_upmu_set_rg_vbiasn_srclk_en_sel(1); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vbiasn_srclk_en_sel(2)]\n");
                         mt6325_upmu_set_rg_vbiasn_srclk_en_sel(2); exec_scrxxx_map(LDO_ONLY);

                printk("[mt6325_upmu_set_rg_vbiasn_srclk_en_sel(3)]\n");
                         mt6325_upmu_set_rg_vbiasn_srclk_en_sel(3); exec_scrxxx_map(LDO_ONLY);
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vbiasn_on_ctrl(0);\n"); mt6325_upmu_set_rg_vbiasn_on_ctrl(0);
}

void exec_6325_ldo_vrtc_en_test(void)
{
    printk("hwPowerOn(MT6325_POWER_LDO_VRTC]\n");
            hwPowerOn(MT6325_POWER_LDO_VRTC, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

    printk("hwPowerDown(MT6325_POWER_LDO_VRTC]\n");
            hwPowerDown(MT6325_POWER_LDO_VRTC, "ldo_dvt"); read_adc_value(LDO_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VRTC]\n");
            hwPowerOn(MT6325_POWER_LDO_VRTC, VOL_DEFAULT, "ldo_dvt"); read_adc_value(LDO_ONLY);

    printk("hwPowerDown(MT6325_POWER_LDO_VRTC]\n");
            hwPowerDown(MT6325_POWER_LDO_VRTC, "ldo_dvt"); read_adc_value(LDO_ONLY);                        
}

void exec_6311_ldo_vbiasn_en_test(void)
{
    int i=0;

    mt6311_set_rg_vbiasn_vosel(0x1F);

    for(i=0;i<=1;i++)
    {
        printk("[mt6311_set_rg_vbiasn_on_ctrl] %d\n", i);
        mt6311_set_rg_vbiasn_on_ctrl(i);

        switch(i){
            case 0:
                printk("mt6311_set_rg_vbiasn_en(1);\n");
                        mt6311_set_rg_vbiasn_en(1); read_adc_value(LDO_ONLY);

                printk("mt6311_set_rg_vbiasn_en(0);\n");
                        mt6311_set_rg_vbiasn_en(0); read_adc_value(LDO_ONLY);

                printk("mt6311_set_rg_vbiasn_en(1);\n");
                        mt6311_set_rg_vbiasn_en(1); read_adc_value(LDO_ONLY);

                printk("mt6311_set_rg_vbiasn_en(0);\n");
                        mt6311_set_rg_vbiasn_en(0); read_adc_value(LDO_ONLY);                        
                break;

            case 1:
                set_mt6311_srclken_sw_mode();

                printk("set_mt6311_srclken_val(0);\n");
                        set_mt6311_srclken_val(0); read_adc_value(LDO_ONLY);

                printk("set_mt6311_srclken_val(1);\n");
                        set_mt6311_srclken_val(1); read_adc_value(LDO_ONLY);

                printk("set_mt6311_srclken_val(0);\n");
                        set_mt6311_srclken_val(0); read_adc_value(LDO_ONLY);

                printk("set_mt6311_srclken_val(1);\n");
                        set_mt6311_srclken_val(1); read_adc_value(LDO_ONLY);                          
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6311_set_rg_vbiasn_on_ctrl(0);\n"); mt6311_set_rg_vbiasn_on_ctrl(0);
}
#endif

#ifdef LDO_VOSEL_DVT_ENABLE
void exec_6325_ldo_vcama_vosel_test(void)
{
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VCAMA] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMA, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMA]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMA, "ldo_dvt"); read_adc_value(ADC_ONLY);
            
    printk("hwPowerOn(MT6325_POWER_LDO_VCAMA] VOL_1500\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMA, VOL_1500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMA]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMA, "ldo_dvt"); read_adc_value(ADC_ONLY);
            
    printk("hwPowerOn(MT6325_POWER_LDO_VCAMA] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMA, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMA]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMA, "ldo_dvt"); read_adc_value(ADC_ONLY);
            
    printk("hwPowerOn(MT6325_POWER_LDO_VCAMA] VOL_2500\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMA, VOL_2500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMA]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMA, "ldo_dvt"); read_adc_value(ADC_ONLY);
            
    printk("hwPowerOn(MT6325_POWER_LDO_VCAMA] VOL_2800\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMA, VOL_2800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMA]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMA, "ldo_dvt"); read_adc_value(ADC_ONLY);        
    //                                                                               
}

void exec_6325_ldo_vcn33_vosel_test(void)
{
    mt6325_upmu_set_rg_vcn33_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VCN33] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VCN33, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN33]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN33, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN33] VOL_3300\n");
            hwPowerOn(MT6325_POWER_LDO_VCN33, VOL_3300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN33]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN33, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN33] VOL_3400\n");
            hwPowerOn(MT6325_POWER_LDO_VCN33, VOL_3400, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN33]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN33, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN33] VOL_3500\n");
            hwPowerOn(MT6325_POWER_LDO_VCN33, VOL_3500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN33]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN33, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN33] VOL_3600\n");
            hwPowerOn(MT6325_POWER_LDO_VCN33, VOL_3600, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN33]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN33, "ldo_dvt"); read_adc_value(ADC_ONLY);            
    //                                                                           
}

void exec_6325_ldo_vrf18_1_vosel_test(void)
{
    mt6325_upmu_set_rg_vrf18_1_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VRF18_1] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VRF18_1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VRF18_1]\n");
            hwPowerDown(MT6325_POWER_LDO_VRF18_1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VRF18_1] VOL_1500\n");
            hwPowerOn(MT6325_POWER_LDO_VRF18_1, VOL_1500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VRF18_1]\n");
            hwPowerDown(MT6325_POWER_LDO_VRF18_1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VRF18_1] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VRF18_1, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VRF18_1]\n");
            hwPowerDown(MT6325_POWER_LDO_VRF18_1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VRF18_1] VOL_2500\n");
            hwPowerOn(MT6325_POWER_LDO_VRF18_1, VOL_2500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VRF18_1]\n");
            hwPowerDown(MT6325_POWER_LDO_VRF18_1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VRF18_1] VOL_2800\n");
            hwPowerOn(MT6325_POWER_LDO_VRF18_1, VOL_2800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VRF18_1]\n");
            hwPowerDown(MT6325_POWER_LDO_VRF18_1, "ldo_dvt"); read_adc_value(ADC_ONLY);            
    //                                                                           
}

void exec_6325_ldo_vmch_vosel_test(void)
{
    mt6325_upmu_set_rg_vmch_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VMCH] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VMCH, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VMCH]\n");
            hwPowerDown(MT6325_POWER_LDO_VMCH, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VMCH] VOL_3000\n");
            hwPowerOn(MT6325_POWER_LDO_VMCH, VOL_3000, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VMCH]\n");
            hwPowerDown(MT6325_POWER_LDO_VMCH, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VMCH] VOL_3300\n");
            hwPowerOn(MT6325_POWER_LDO_VMCH, VOL_3300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VMCH]\n");
            hwPowerDown(MT6325_POWER_LDO_VMCH, "ldo_dvt"); read_adc_value(ADC_ONLY);           
    //                                                                           
}

void exec_6325_ldo_vmc_vosel_test(void)
{
    mt6325_upmu_set_rg_vmc_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VMC] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VMC, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VMC]\n");
            hwPowerDown(MT6325_POWER_LDO_VMC, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VMC] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VMC, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VMC]\n");
            hwPowerDown(MT6325_POWER_LDO_VMC, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VMC] VOL_3300\n");
            hwPowerOn(MT6325_POWER_LDO_VMC, VOL_3300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VMC]\n");
            hwPowerDown(MT6325_POWER_LDO_VMC, "ldo_dvt"); read_adc_value(ADC_ONLY);           
    //                                                                           
}

void exec_6325_ldo_vemc33_vosel_test(void)
{
    mt6325_upmu_set_rg_vemc33_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VEMC33] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VEMC33, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VEMC33]\n");
            hwPowerDown(MT6325_POWER_LDO_VEMC33, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VEMC33] VOL_3000\n");
            hwPowerOn(MT6325_POWER_LDO_VEMC33, VOL_3000, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VEMC33]\n");
            hwPowerDown(MT6325_POWER_LDO_VEMC33, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VEMC33] VOL_3300\n");
            hwPowerOn(MT6325_POWER_LDO_VEMC33, VOL_3300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VEMC33]\n");
            hwPowerDown(MT6325_POWER_LDO_VEMC33, "ldo_dvt"); read_adc_value(ADC_ONLY);           
    //                                                                           
}

void exec_6325_ldo_vcam_af_vosel_test(void)
{
    mt6325_upmu_set_rg_vcam_af_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_AF] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_AF, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_AF]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_AF, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_AF] VOL_1200\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_AF, VOL_1200, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_AF]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_AF, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_AF] VOL_1300\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_AF, VOL_1300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_AF]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_AF, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_AF] VOL_1500\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_AF, VOL_1500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_AF]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_AF, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_AF] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_AF, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_AF]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_AF, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_AF] VOL_2500\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_AF, VOL_2500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_AF]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_AF, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_AF] VOL_2800\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_AF, VOL_2800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_AF]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_AF, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_AF] VOL_3000\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_AF, VOL_3000, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_AF]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_AF, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_AF] VOL_3300\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_AF, VOL_3300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_AF]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_AF, "ldo_dvt"); read_adc_value(ADC_ONLY);            
    //                                                                           
}

void exec_6325_ldo_vgp1_vosel_test(void)
{
    mt6325_upmu_set_rg_vgp1_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VGP1] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP1]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP1] VOL_1200\n");
            hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_1200, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP1]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP1] VOL_1300\n");
            hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_1300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP1]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP1] VOL_1500\n");
            hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_1500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP1]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP1] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP1]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP1] VOL_2500\n");
            hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_2500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP1]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP1] VOL_2800\n");
            hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_2800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP1]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP1] VOL_3000\n");
            hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_3000, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP1]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP1] VOL_3300\n");
            hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_3300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP1]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP1, "ldo_dvt"); read_adc_value(ADC_ONLY);            
    //                                                                           
}

void exec_6325_ldo_vefuse_vosel_test(void)
{
    mt6325_upmu_set_rg_vefuse_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VEFUSE] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VEFUSE, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VEFUSE]\n");
            hwPowerDown(MT6325_POWER_LDO_VEFUSE, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VEFUSE] VOL_1200\n");
            hwPowerOn(MT6325_POWER_LDO_VEFUSE, VOL_1200, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VEFUSE]\n");
            hwPowerDown(MT6325_POWER_LDO_VEFUSE, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VEFUSE] VOL_1600\n");
            hwPowerOn(MT6325_POWER_LDO_VEFUSE, VOL_1600, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VEFUSE]\n");
            hwPowerDown(MT6325_POWER_LDO_VEFUSE, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VEFUSE] VOL_1700\n");
            hwPowerOn(MT6325_POWER_LDO_VEFUSE, VOL_1700, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VEFUSE]\n");
            hwPowerDown(MT6325_POWER_LDO_VEFUSE, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VEFUSE] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VEFUSE, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VEFUSE]\n");
            hwPowerDown(MT6325_POWER_LDO_VEFUSE, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VEFUSE] VOL_1900\n");
            hwPowerOn(MT6325_POWER_LDO_VEFUSE, VOL_1900, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VEFUSE]\n");
            hwPowerDown(MT6325_POWER_LDO_VEFUSE, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VEFUSE] VOL_2000\n");
            hwPowerOn(MT6325_POWER_LDO_VEFUSE, VOL_2000, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VEFUSE]\n");
            hwPowerDown(MT6325_POWER_LDO_VEFUSE, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VEFUSE] VOL_2100\n");
            hwPowerOn(MT6325_POWER_LDO_VEFUSE, VOL_2100, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VEFUSE]\n");
            hwPowerDown(MT6325_POWER_LDO_VEFUSE, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VEFUSE] VOL_2200\n");
            hwPowerOn(MT6325_POWER_LDO_VEFUSE, VOL_2200, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VEFUSE]\n");
            hwPowerDown(MT6325_POWER_LDO_VEFUSE, "ldo_dvt"); read_adc_value(ADC_ONLY);            
    //                                                                           
}

void exec_6325_ldo_vsim1_vosel_test(void)
{
    mt6325_upmu_set_rg_vsim1_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VSIM1] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM1]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM1] VOL_1650\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM1, VOL_1650, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM1]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM1] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM1, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM1]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM1] VOL_1850\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM1, VOL_1850, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM1]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM1] VOL_2750\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM1, VOL_2750, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM1]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM1] VOL_3000\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM1, VOL_3000, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM1]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM1, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM1] VOL_3100\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM1, VOL_3100, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM1]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM1, "ldo_dvt"); read_adc_value(ADC_ONLY);           
    //                                                                           
}

void exec_6325_ldo_vsim2_vosel_test(void)
{
    mt6325_upmu_set_rg_vsim2_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VSIM2] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM2, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM2]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM2, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM2] VOL_1650\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM2, VOL_1650, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM2]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM2, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM2] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM2, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM2]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM2, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM2] VOL_1850\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM2, VOL_1850, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM2]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM2, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM2] VOL_2750\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM2, VOL_2750, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM2]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM2, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM2] VOL_3000\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM2, VOL_3000, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM2]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM2, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM2] VOL_3100\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM2, VOL_3100, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM2]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM2, "ldo_dvt"); read_adc_value(ADC_ONLY);           
    //                                                                           
}

void exec_6325_ldo_vmipi_vosel_test(void)
{
    mt6325_upmu_set_rg_vmipi_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VMIPI] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VMIPI, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VMIPI]\n");
            hwPowerDown(MT6325_POWER_LDO_VMIPI, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VMIPI] VOL_1200\n");
            hwPowerOn(MT6325_POWER_LDO_VMIPI, VOL_1200, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VMIPI]\n");
            hwPowerDown(MT6325_POWER_LDO_VMIPI, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VMIPI] VOL_1300\n");
            hwPowerOn(MT6325_POWER_LDO_VMIPI, VOL_1300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VMIPI]\n");
            hwPowerDown(MT6325_POWER_LDO_VMIPI, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VMIPI] VOL_1500\n");
            hwPowerOn(MT6325_POWER_LDO_VMIPI, VOL_1500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VMIPI]\n");
            hwPowerDown(MT6325_POWER_LDO_VMIPI, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VMIPI] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VMIPI, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VMIPI]\n");
            hwPowerDown(MT6325_POWER_LDO_VMIPI, "ldo_dvt"); read_adc_value(ADC_ONLY);          
    //                                                                           
}

void exec_6325_ldo_vcn18_vosel_test(void)
{
    mt6325_upmu_set_rg_vcn18_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VCN18] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN18]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN18, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN18] VOL_1200\n");
            hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_1200, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN18]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN18, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN18] VOL_1300\n");
            hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_1300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN18]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN18, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN18] VOL_1500\n");
            hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_1500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN18]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN18, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN18] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN18]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN18, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN18] VOL_2500\n");
            hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_2500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN18]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN18, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN18] VOL_2800\n");
            hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_2800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN18]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN18, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN18] VOL_3000\n");
            hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_3000, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN18]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN18, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN18] VOL_3300\n");
            hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_3300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCN18]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN18, "ldo_dvt"); read_adc_value(ADC_ONLY);            
    //                                                                           
}

void exec_6325_ldo_vgp2_vosel_test(void)
{
    mt6325_upmu_set_rg_vgp2_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VGP2] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VGP2, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP2]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP2, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP2] VOL_1200\n");
            hwPowerOn(MT6325_POWER_LDO_VGP2, VOL_1200, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP2]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP2, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP2] VOL_1300\n");
            hwPowerOn(MT6325_POWER_LDO_VGP2, VOL_1300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP2]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP2, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP2] VOL_1500\n");
            hwPowerOn(MT6325_POWER_LDO_VGP2, VOL_1500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP2]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP2, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP2] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VGP2, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP2]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP2, "ldo_dvt"); read_adc_value(ADC_ONLY);          
    //                                                                           
}

void exec_6325_ldo_vcamd_vosel_test(void)
{
    int i=0;

    mt6325_upmu_set_rg_vcamd_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VCAMD] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMD, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMD]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMD, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAMD] VOL_0900\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMD, VOL_0900, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMD]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMD, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAMD] VOL_1000\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMD, VOL_1000, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMD]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMD, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAMD] VOL_1100\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMD, VOL_1100, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMD]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMD, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAMD] VOL_1220\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMD, VOL_1220, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMD]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMD, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAMD] VOL_1300\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMD, VOL_1300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMD]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMD, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAMD] VOL_1500\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMD, VOL_1500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMD]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMD, "ldo_dvt"); read_adc_value(ADC_ONLY);          
    //                                                                           
    mt6325_upmu_set_rg_vcamd_en(1);
    for(i=0;i<=7;i++)
    {
        mdelay(100);
        printk("mt6325_upmu_set_rg_vcamd_vosel=%d\n", i);
        mt6325_upmu_set_rg_vcamd_vosel(i); 
        read_adc_value(ADC_ONLY);
    }
}

void exec_6325_ldo_vcam_io_vosel_test(void)
{
    mt6325_upmu_set_rg_vcam_io_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_IO] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_IO, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_IO]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_IO, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_IO] VOL_1200\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_IO, VOL_1200, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_IO]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_IO, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_IO] VOL_1300\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_IO, VOL_1300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_IO]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_IO, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_IO] VOL_1500\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_IO, VOL_1500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_IO]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_IO, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_IO] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_IO, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_IO]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_IO, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_IO] VOL_2500\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_IO, VOL_2500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_IO]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_IO, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_IO] VOL_2800\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_IO, VOL_2800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_IO]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_IO, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_IO] VOL_3000\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_IO, VOL_3000, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_IO]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_IO, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_IO] VOL_3300\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_IO, VOL_3300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_IO]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_IO, "ldo_dvt"); read_adc_value(ADC_ONLY);            
    //                                                                           
}

void exec_6325_ldo_vsram_dvfs1_vosel_test(void)
{
    int i=0;

    mt6325_upmu_set_rg_vsram_dvfs1_on_ctrl(0);
    mt6325_upmu_set_rg_vsram_dvfs1_en(1);
    for(i=0;i<=0x7F;i++)
    {
        mdelay(500);
        printk("mt6325_upmu_set_rg_vsram_dvfs1_vosel=%d\n", i);
        mt6325_upmu_set_rg_vsram_dvfs1_vosel(i); 
        read_adc_value(ADC_ONLY);
    }
}

void exec_6325_ldo_vgp3_vosel_test(void)
{
    mt6325_upmu_set_rg_vgp3_on_ctrl(0);
    
    //                                                                           
    printk("hwPowerOn(MT6325_POWER_LDO_VGP3] VOL_DEFAULT\n");
            hwPowerOn(MT6325_POWER_LDO_VGP3, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP3]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP3, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP3] VOL_1200\n");
            hwPowerOn(MT6325_POWER_LDO_VGP3, VOL_1200, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP3]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP3, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP3] VOL_1300\n");
            hwPowerOn(MT6325_POWER_LDO_VGP3, VOL_1300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP3]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP3, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP3] VOL_1500\n");
            hwPowerOn(MT6325_POWER_LDO_VGP3, VOL_1500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP3]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP3, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP3] VOL_1800\n");
            hwPowerOn(MT6325_POWER_LDO_VGP3, VOL_1800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP3]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP3, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP3] VOL_2500\n");
            hwPowerOn(MT6325_POWER_LDO_VGP3, VOL_2500, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP3]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP3, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP3] VOL_2800\n");
            hwPowerOn(MT6325_POWER_LDO_VGP3, VOL_2800, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP3]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP3, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP3] VOL_3000\n");
            hwPowerOn(MT6325_POWER_LDO_VGP3, VOL_3000, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP3]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP3, "ldo_dvt"); read_adc_value(ADC_ONLY);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP3] VOL_3300\n");
            hwPowerOn(MT6325_POWER_LDO_VGP3, VOL_3300, "ldo_dvt"); read_adc_value(ADC_ONLY);
    printk("hwPowerDown(MT6325_POWER_LDO_VGP3]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP3, "ldo_dvt"); read_adc_value(ADC_ONLY);            
    //                                                                           
}

void exec_6325_ldo_vbiasn_vosel_test(void)
{
    int i=0;

    mt6325_upmu_set_rg_vbiasn_on_ctrl(0);
    mt6325_upmu_set_rg_vbiasn_en(1);
    for(i=0;i<=0x1F;i++)
    {
        mdelay(500);
        printk("mt6325_upmu_set_rg_vbiasn_vosel=%d\n", i);
        mt6325_upmu_set_rg_vbiasn_vosel(i); 
        read_adc_value(ADC_ONLY);
    }
}

void exec_6311_ldo_vbiasn_vosel_test(void)
{
    int i=0;

    mt6311_set_rg_vbiasn_on_ctrl(0);
    mt6311_set_rg_vbiasn_en(1);
    for(i=0;i<=MT6311_PMIC_RG_VBIASN_VOSEL_MASK;i++)
    {
        mdelay(500);
        printk("mt6311_set_rg_vbiasn_vosel=%d\n", i);
        mt6311_set_rg_vbiasn_vosel(i); 
        read_adc_value(ADC_ONLY);
    }
}

//                                   
void exec_6311_ldo_vdig18_vosel_test(void)
{
    int i=0;

    for(i=0;i<=MT6311_PMIC_RG_VDIG18_VOSEL_MASK;i++)
    {
        mdelay(500);
        printk("mt6311_set_rg_vdig18_vosel=%d\n", i);
        mt6311_set_rg_vdig18_vosel(i); 
        read_adc_value(ADC_ONLY);
    }
}
#endif

#ifdef LDO_CAL_DVT_ENABLE
void exec_6325_ldo_vtcxo0_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vtcxo0_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VTCXO0]\n");
            hwPowerOn(MT6325_POWER_LDO_VTCXO0, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VTCXO0_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vtcxo0_cal=%d, ", i);
        mt6325_upmu_set_rg_vtcxo0_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VTCXO0]\n");
            hwPowerDown(MT6325_POWER_LDO_VTCXO0, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vtcxo1_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vtcxo1_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VTCXO1]\n");
            hwPowerOn(MT6325_POWER_LDO_VTCXO1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VTCXO1_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vtcxo1_cal=%d, ", i);
        mt6325_upmu_set_rg_vtcxo1_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VTCXO1]\n");
            hwPowerDown(MT6325_POWER_LDO_VTCXO1, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vaud28_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vaud28_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VAUD28]\n");
            hwPowerOn(MT6325_POWER_LDO_VAUD28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VAUD28_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vaud28_cal=%d, ", i);
        mt6325_upmu_set_rg_vaud28_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VAUD28]\n");
            hwPowerDown(MT6325_POWER_LDO_VAUD28, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vauxa28_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vauxa28_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VAUXA28]\n");
            hwPowerOn(MT6325_POWER_LDO_VAUXA28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VAUXA28_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vauxa28_cal=%d, ", i);
        mt6325_upmu_set_rg_vauxa28_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VAUXA28]\n");
            hwPowerDown(MT6325_POWER_LDO_VAUXA28, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vbif28_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vbif28_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VBIF28]\n");
            hwPowerOn(MT6325_POWER_LDO_VBIF28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VBIF28_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vbif28_cal=%d, ", i);
        mt6325_upmu_set_rg_vbif28_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VBIF28]\n");
            hwPowerDown(MT6325_POWER_LDO_VBIF28, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vcama_cal_test(void)
{
    int i=0;    

    printk("hwPowerOn(MT6325_POWER_LDO_VCAMA]\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMA, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VCAMA_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vcama_cal=%d, ", i);
        mt6325_upmu_set_rg_vcama_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMA]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMA, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vcn28_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vcn28_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN28]\n");
            hwPowerOn(MT6325_POWER_LDO_VCN28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VCN28_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vcn28_cal=%d, ", i);
        mt6325_upmu_set_rg_vcn28_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VCN28]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN28, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vcn33_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vcn33_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN33]\n");
            hwPowerOn(MT6325_POWER_LDO_VCN33, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VCN33_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vcn33_cal=%d, ", i);
        mt6325_upmu_set_rg_vcn33_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VCN33]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN33, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vrf18_1_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vrf18_1_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VRF18_1]\n");
            hwPowerOn(MT6325_POWER_LDO_VRF18_1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VRF18_1_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vrf18_1_cal=%d, ", i);
        mt6325_upmu_set_rg_vrf18_1_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VRF18_1]\n");
            hwPowerDown(MT6325_POWER_LDO_VRF18_1, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vusb33_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vusb33_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VUSB33]\n");
            hwPowerOn(MT6325_POWER_LDO_VUSB33, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VUSB33_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vusb33_cal=%d, ", i);
        mt6325_upmu_set_rg_vusb33_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VUSB33]\n");
            hwPowerDown(MT6325_POWER_LDO_VUSB33, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vmch_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vmch_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VMCH]\n");
            hwPowerOn(MT6325_POWER_LDO_VMCH, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VMCH_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vmch_cal=%d, ", i);
        mt6325_upmu_set_rg_vmch_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VMCH]\n");
            hwPowerDown(MT6325_POWER_LDO_VMCH, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vmc_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vmc_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VMC]\n");
            hwPowerOn(MT6325_POWER_LDO_VMC, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VMC_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vmc_cal=%d, ", i);
        mt6325_upmu_set_rg_vmc_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VMC]\n");
            hwPowerDown(MT6325_POWER_LDO_VMC, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vemc33_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vemc33_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VEMC33]\n");
            hwPowerOn(MT6325_POWER_LDO_VEMC33, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VEMC_3V3_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vemc33_cal=%d, ", i);
        mt6325_upmu_set_rg_vemc_3v3_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VEMC33]\n");
            hwPowerDown(MT6325_POWER_LDO_VEMC33, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vio28_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vio28_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VIO28]\n");
            hwPowerOn(MT6325_POWER_LDO_VIO28, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VIO28_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vio28_cal=%d, ", i);
        mt6325_upmu_set_rg_vio28_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VIO28]\n");
            hwPowerDown(MT6325_POWER_LDO_VIO28, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vcam_af_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vcam_af_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_AF]\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_AF, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VCAMAF_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vcam_af_cal=%d, ", i);
        mt6325_upmu_set_rg_vcamaf_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_AF]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_AF, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vgp1_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vgp1_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP1]\n");
            hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VGP1_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vgp1_cal=%d, ", i);
        mt6325_upmu_set_rg_vgp1_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VGP1]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP1, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vefuse_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vefuse_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VEFUSE]\n");
            hwPowerOn(MT6325_POWER_LDO_VEFUSE, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VEFUSE_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vefuse_cal=%d, ", i);
        mt6325_upmu_set_rg_vefuse_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VEFUSE]\n");
            hwPowerDown(MT6325_POWER_LDO_VEFUSE, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vsim1_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vsim1_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM1]\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM1, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VSIM1_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vsim1_cal=%d, ", i);
        mt6325_upmu_set_rg_vsim1_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM1]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM1, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vsim2_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vsim2_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VSIM2]\n");
            hwPowerOn(MT6325_POWER_LDO_VSIM2, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VSIM2_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vsim2_cal=%d, ", i);
        mt6325_upmu_set_rg_vsim2_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VSIM2]\n");
            hwPowerDown(MT6325_POWER_LDO_VSIM2, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vmipi_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vmipi_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VMIPI]\n");
            hwPowerOn(MT6325_POWER_LDO_VMIPI, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VMIPI_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vmipi_cal=%d, ", i);
        mt6325_upmu_set_rg_vmipi_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VMIPI]\n");
            hwPowerDown(MT6325_POWER_LDO_VMIPI, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vcn18_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vcn18_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VCN18]\n");
            hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VCN18_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vcn18_cal=%d, ", i);
        mt6325_upmu_set_rg_vcn18_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VCN18]\n");
            hwPowerDown(MT6325_POWER_LDO_VCN18, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vgp2_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vgp2_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP2]\n");
            hwPowerOn(MT6325_POWER_LDO_VGP2, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VGP2_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vgp2_cal=%d, ", i);
        mt6325_upmu_set_rg_vgp2_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VGP2]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP2, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vcamd_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vcamd_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAMD]\n");
            hwPowerOn(MT6325_POWER_LDO_VCAMD, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VCAMD_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vcamd_cal=%d, ", i);
        mt6325_upmu_set_rg_vcamd_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VCAMD]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAMD, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vcam_io_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vcam_io_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VCAM_IO]\n");
            hwPowerOn(MT6325_POWER_LDO_VCAM_IO, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VCAMIO_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vcam_io_cal=%d, ", i);
        mt6325_upmu_set_rg_vcamio_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VCAM_IO]\n");
            hwPowerDown(MT6325_POWER_LDO_VCAM_IO, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vsram_dvfs1_cal_test(void)
{
    printk("[exec_6325_ldo_vsram_dvfs1_cal_test] no CAL\n");
}

void exec_6325_ldo_vgp3_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vgp3_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VGP3]\n");
            hwPowerOn(MT6325_POWER_LDO_VGP3, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        
            
    for(i=0;i<=MT6325_PMIC_RG_VGP3_CAL_MASK;i++)
    {
        printk("mt6325_upmu_set_rg_vgp3_cal=%d, ", i);
        mt6325_upmu_set_rg_vgp3_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VGP3]\n");
            hwPowerDown(MT6325_POWER_LDO_VGP3, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vbiasn_cal_test(void)
{
    int i=0;    
    mt6325_upmu_set_rg_vbiasn_on_ctrl(0);

    printk("hwPowerOn(MT6325_POWER_LDO_VBIASN]\n");
            hwPowerOn(MT6325_POWER_LDO_VBIASN, VOL_DEFAULT, "ldo_dvt"); read_adc_value(ADC_ONLY);        

    mt6325_upmu_set_rg_vbiasn_vosel(0x1F);
            
    for(i=0;i<=MT6325_PMIC_RG_VBIASN_CAL_MASK;i++)
    {
        mdelay(500);
        printk("mt6325_upmu_set_rg_vbiasn_cal=%d, ", i);
        mt6325_upmu_set_rg_vbiasn_cal(i); 
        read_adc_value(ADC_ONLY);
    }
    
    printk("hwPowerDown(MT6325_POWER_LDO_VBIASN]\n");
            hwPowerDown(MT6325_POWER_LDO_VBIASN, "ldo_dvt"); read_adc_value(ADC_ONLY);
}

void exec_6325_ldo_vrtc_cal_test(void)
{
    printk("[exec_6325_ldo_vrtc_cal_test] no CAL\n");
}

//                                   
void exec_6311_ldo_vdig18_cal_test(void)
{
    int i=0;    
           
    for(i=0;i<=MT6311_PMIC_RG_VDIG18_CAL_MASK;i++)
    {
        printk("mt6311_set_rg_vdig18_cal=%d, ", i);
        mt6311_set_rg_vdig18_cal(i); 
        read_adc_value(ADC_ONLY);
    }    
}
#endif

#ifdef LDO_MODE_DVT_ENABLE
void exec_6325_ldo_vtcxo0_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vtcxo0_mode = %d\n", mt6325_upmu_get_qi_vtcxo0_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vtcxo0_mode = %d\n", mt6325_upmu_get_qi_vtcxo0_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vtcxo0_mode = %d\n", mt6325_upmu_get_qi_vtcxo0_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vtcxo0_mode = %d\n", mt6325_upmu_get_qi_vtcxo0_mode()); 
}

void exec_6325_ldo_vtcxo0_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vtcxo0_on_ctrl(0);
    mt6325_upmu_set_rg_vtcxo0_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vtcxo0_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vtcxo0_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vtcxo0_mode_set(0); ");
                        mt6325_upmu_set_rg_vtcxo0_mode_set(0);
                printk("mt6325_upmu_get_qi_vtcxo0_mode = %d\n", mt6325_upmu_get_qi_vtcxo0_mode());

                printk("mt6325_upmu_set_rg_vtcxo0_mode_set(1); ");
                        mt6325_upmu_set_rg_vtcxo0_mode_set(1);
                printk("mt6325_upmu_get_qi_vtcxo0_mode = %d\n", mt6325_upmu_get_qi_vtcxo0_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vtcxo0_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vtcxo0_srclk_mode_sel(0); 
                exec_6325_ldo_vtcxo0_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vtcxo0_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vtcxo0_srclk_mode_sel(1);
                exec_6325_ldo_vtcxo0_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vtcxo0_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vtcxo0_srclk_mode_sel(2);
                exec_6325_ldo_vtcxo0_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vtcxo0_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vtcxo0_srclk_mode_sel(3);
                exec_6325_ldo_vtcxo0_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vtcxo0_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vtcxo0_mode_ctrl(0);
}

void exec_6325_ldo_vtcxo1_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vtcxo1_mode = %d\n", mt6325_upmu_get_qi_vtcxo1_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vtcxo1_mode = %d\n", mt6325_upmu_get_qi_vtcxo1_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vtcxo1_mode = %d\n", mt6325_upmu_get_qi_vtcxo1_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vtcxo1_mode = %d\n", mt6325_upmu_get_qi_vtcxo1_mode()); 
}

void exec_6325_ldo_vtcxo1_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vtcxo1_on_ctrl(0);
    mt6325_upmu_set_rg_vtcxo1_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vtcxo1_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vtcxo1_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vtcxo1_mode_set(0); ");
                        mt6325_upmu_set_rg_vtcxo1_mode_set(0);
                printk("mt6325_upmu_get_qi_vtcxo1_mode = %d\n", mt6325_upmu_get_qi_vtcxo1_mode());

                printk("mt6325_upmu_set_rg_vtcxo1_mode_set(1); ");
                        mt6325_upmu_set_rg_vtcxo1_mode_set(1);
                printk("mt6325_upmu_get_qi_vtcxo1_mode = %d\n", mt6325_upmu_get_qi_vtcxo1_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vtcxo1_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vtcxo1_srclk_mode_sel(0); 
                exec_6325_ldo_vtcxo1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vtcxo1_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vtcxo1_srclk_mode_sel(1);
                exec_6325_ldo_vtcxo1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vtcxo1_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vtcxo1_srclk_mode_sel(2);
                exec_6325_ldo_vtcxo1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vtcxo1_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vtcxo1_srclk_mode_sel(3);
                exec_6325_ldo_vtcxo1_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vtcxo1_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vtcxo1_mode_ctrl(0);
}

void exec_6325_ldo_vaud28_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vaud28_mode = %d\n", mt6325_upmu_get_qi_vaud28_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vaud28_mode = %d\n", mt6325_upmu_get_qi_vaud28_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vaud28_mode = %d\n", mt6325_upmu_get_qi_vaud28_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vaud28_mode = %d\n", mt6325_upmu_get_qi_vaud28_mode()); 
}

void exec_6325_ldo_vaud28_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vaud28_on_ctrl(0);
    mt6325_upmu_set_rg_vaud28_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vaud28_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vaud28_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vaud28_mode_set(0); ");
                        mt6325_upmu_set_rg_vaud28_mode_set(0);
                printk("mt6325_upmu_get_qi_vaud28_mode = %d\n", mt6325_upmu_get_qi_vaud28_mode());

                printk("mt6325_upmu_set_rg_vaud28_mode_set(1); ");
                        mt6325_upmu_set_rg_vaud28_mode_set(1);
                printk("mt6325_upmu_get_qi_vaud28_mode = %d\n", mt6325_upmu_get_qi_vaud28_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vaud28_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vaud28_srclk_mode_sel(0); 
                exec_6325_ldo_vaud28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vaud28_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vaud28_srclk_mode_sel(1);
                exec_6325_ldo_vaud28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vaud28_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vaud28_srclk_mode_sel(2);
                exec_6325_ldo_vaud28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vaud28_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vaud28_srclk_mode_sel(3);
                exec_6325_ldo_vaud28_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vaud28_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vaud28_mode_ctrl(0);
}


//                                                                               

void exec_6325_ldo_vauxa28_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vauxa28_mode = %d\n", mt6325_upmu_get_qi_vauxa28_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vauxa28_mode = %d\n", mt6325_upmu_get_qi_vauxa28_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vauxa28_mode = %d\n", mt6325_upmu_get_qi_vauxa28_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vauxa28_mode = %d\n", mt6325_upmu_get_qi_vauxa28_mode()); 
}

void exec_6325_ldo_vauxa28_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vauxa28_on_ctrl(0);
    mt6325_upmu_set_rg_vauxa28_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vauxa28_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vauxa28_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vauxa28_mode_set(0); ");
                        mt6325_upmu_set_rg_vauxa28_mode_set(0);
                printk("mt6325_upmu_get_qi_vauxa28_mode = %d\n", mt6325_upmu_get_qi_vauxa28_mode());

                printk("mt6325_upmu_set_rg_vauxa28_mode_set(1); ");
                        mt6325_upmu_set_rg_vauxa28_mode_set(1);
                printk("mt6325_upmu_get_qi_vauxa28_mode = %d\n", mt6325_upmu_get_qi_vauxa28_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vauxa28_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vauxa28_srclk_mode_sel(0); 
                exec_6325_ldo_vauxa28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vauxa28_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vauxa28_srclk_mode_sel(1);
                exec_6325_ldo_vauxa28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vauxa28_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vauxa28_srclk_mode_sel(2);
                exec_6325_ldo_vauxa28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vauxa28_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vauxa28_srclk_mode_sel(3);
                exec_6325_ldo_vauxa28_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vauxa28_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vauxa28_mode_ctrl(0);
}

void exec_6325_ldo_vauxa28_mode_test_2(void)
{
    printk("[mt6325_upmu_set_rg_vauxa28_auxadc_pwdb_en(0)]\n");
             mt6325_upmu_set_rg_vauxa28_auxadc_pwdb_en(0);
    //                           
    printk("mt6325_upmu_get_qi_vauxa28_mode = %d\n", mt6325_upmu_get_qi_vauxa28_mode());


    printk("[mt6325_upmu_set_rg_vauxa28_auxadc_pwdb_en(1)]\n");
             mt6325_upmu_set_rg_vauxa28_auxadc_pwdb_en(1);
    printk("mt6325_upmu_get_qi_vauxa28_mode = %d\n", mt6325_upmu_get_qi_vauxa28_mode());
}

void exec_6325_ldo_vauxa28_mode_test_main(void)
{
#if 1
    exec_6325_ldo_vauxa28_mode_test();
#else
    printk("[exec_6325_ldo_vauxa28_mode_test_main] mt6325_upmu_set_rg_vauxa28_auxadc_pwdb_en(1);\n");
    mt6325_upmu_set_rg_vauxa28_auxadc_pwdb_en(1);
    exec_6325_ldo_vauxa28_mode_test();
    
    printk("[exec_6325_ldo_vauxa28_mode_test_main] mt6325_upmu_set_rg_vauxa28_mode_ctrl(0);\n");
    mt6325_upmu_set_rg_vauxa28_mode_ctrl(0);
    printk("[exec_6325_ldo_vauxa28_mode_test_main] mt6325_upmu_set_rg_vauxa28_mode_set(1);\n");
    mt6325_upmu_set_rg_vauxa28_mode_set(1);
    exec_6325_ldo_vauxa28_mode_test_2();
#endif    
}

//                                                                               

void exec_6325_ldo_vbif28_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vbif28_mode = %d\n", mt6325_upmu_get_qi_vbif28_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vbif28_mode = %d\n", mt6325_upmu_get_qi_vbif28_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vbif28_mode = %d\n", mt6325_upmu_get_qi_vbif28_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vbif28_mode = %d\n", mt6325_upmu_get_qi_vbif28_mode()); 
}

void exec_6325_ldo_vbif28_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vbif28_on_ctrl(0);
    mt6325_upmu_set_rg_vbif28_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vbif28_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vbif28_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vbif28_mode_set(0); ");
                        mt6325_upmu_set_rg_vbif28_mode_set(0);
                printk("mt6325_upmu_get_qi_vbif28_mode = %d\n", mt6325_upmu_get_qi_vbif28_mode());

                printk("mt6325_upmu_set_rg_vbif28_mode_set(1); ");
                        mt6325_upmu_set_rg_vbif28_mode_set(1);
                printk("mt6325_upmu_get_qi_vbif28_mode = %d\n", mt6325_upmu_get_qi_vbif28_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vbif28_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vbif28_srclk_mode_sel(0); 
                exec_6325_ldo_vbif28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vbif28_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vbif28_srclk_mode_sel(1);
                exec_6325_ldo_vbif28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vbif28_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vbif28_srclk_mode_sel(2);
                exec_6325_ldo_vbif28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vbif28_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vbif28_srclk_mode_sel(3);
                exec_6325_ldo_vbif28_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vbif28_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vbif28_mode_ctrl(0);
}

void exec_6325_ldo_vcn28_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vcn28_mode = %d\n", mt6325_upmu_get_qi_vcn28_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vcn28_mode = %d\n", mt6325_upmu_get_qi_vcn28_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vcn28_mode = %d\n", mt6325_upmu_get_qi_vcn28_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vcn28_mode = %d\n", mt6325_upmu_get_qi_vcn28_mode()); 
}

void exec_6325_ldo_vcn28_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vcn28_on_ctrl(0);
    mt6325_upmu_set_rg_vcn28_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vcn28_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vcn28_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vcn28_mode_set(0); ");
                        mt6325_upmu_set_rg_vcn28_mode_set(0);
                printk("mt6325_upmu_get_qi_vcn28_mode = %d\n", mt6325_upmu_get_qi_vcn28_mode());

                printk("mt6325_upmu_set_rg_vcn28_mode_set(1); ");
                        mt6325_upmu_set_rg_vcn28_mode_set(1);
                printk("mt6325_upmu_get_qi_vcn28_mode = %d\n", mt6325_upmu_get_qi_vcn28_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vcn28_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vcn28_srclk_mode_sel(0); 
                exec_6325_ldo_vcn28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcn28_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vcn28_srclk_mode_sel(1);
                exec_6325_ldo_vcn28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcn28_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vcn28_srclk_mode_sel(2);
                exec_6325_ldo_vcn28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcn28_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vcn28_srclk_mode_sel(3);
                exec_6325_ldo_vcn28_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vcn28_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vcn28_mode_ctrl(0);
}

void exec_6325_ldo_vcn33_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vcn33_mode = %d\n", mt6325_upmu_get_qi_vcn33_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vcn33_mode = %d\n", mt6325_upmu_get_qi_vcn33_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vcn33_mode = %d\n", mt6325_upmu_get_qi_vcn33_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vcn33_mode = %d\n", mt6325_upmu_get_qi_vcn33_mode()); 
}

void exec_6325_ldo_vcn33_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vcn33_on_ctrl(0);
    mt6325_upmu_set_rg_vcn33_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vcn33_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vcn33_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vcn33_mode_set(0); ");
                        mt6325_upmu_set_rg_vcn33_mode_set(0);
                printk("mt6325_upmu_get_qi_vcn33_mode = %d\n", mt6325_upmu_get_qi_vcn33_mode());

                printk("mt6325_upmu_set_rg_vcn33_mode_set(1); ");
                        mt6325_upmu_set_rg_vcn33_mode_set(1);
                printk("mt6325_upmu_get_qi_vcn33_mode = %d\n", mt6325_upmu_get_qi_vcn33_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vcn33_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vcn33_srclk_mode_sel(0); 
                exec_6325_ldo_vcn33_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcn33_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vcn33_srclk_mode_sel(1);
                exec_6325_ldo_vcn33_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcn33_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vcn33_srclk_mode_sel(2);
                exec_6325_ldo_vcn33_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcn33_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vcn33_srclk_mode_sel(3);
                exec_6325_ldo_vcn33_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vcn33_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vcn33_mode_ctrl(0);
}

void exec_6325_ldo_vrf18_1_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vrf18_1_mode = %d\n", mt6325_upmu_get_qi_vrf18_1_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vrf18_1_mode = %d\n", mt6325_upmu_get_qi_vrf18_1_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vrf18_1_mode = %d\n", mt6325_upmu_get_qi_vrf18_1_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vrf18_1_mode = %d\n", mt6325_upmu_get_qi_vrf18_1_mode()); 
}

void exec_6325_ldo_vrf18_1_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vrf18_1_on_ctrl(0);
    mt6325_upmu_set_rg_vrf18_1_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vrf18_1_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vrf18_1_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vrf18_1_mode_set(0); ");
                        mt6325_upmu_set_rg_vrf18_1_mode_set(0);
                printk("mt6325_upmu_get_qi_vrf18_1_mode = %d\n", mt6325_upmu_get_qi_vrf18_1_mode());

                printk("mt6325_upmu_set_rg_vrf18_1_mode_set(1); ");
                        mt6325_upmu_set_rg_vrf18_1_mode_set(1);
                printk("mt6325_upmu_get_qi_vrf18_1_mode = %d\n", mt6325_upmu_get_qi_vrf18_1_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vrf18_1_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vrf18_1_srclk_mode_sel(0); 
                exec_6325_ldo_vrf18_1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vrf18_1_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vrf18_1_srclk_mode_sel(1);
                exec_6325_ldo_vrf18_1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vrf18_1_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vrf18_1_srclk_mode_sel(2);
                exec_6325_ldo_vrf18_1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vrf18_1_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vrf18_1_srclk_mode_sel(3);
                exec_6325_ldo_vrf18_1_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vrf18_1_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vrf18_1_mode_ctrl(0);
}

void exec_6325_ldo_vusb33_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vusb33_mode = %d\n", mt6325_upmu_get_qi_vusb33_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vusb33_mode = %d\n", mt6325_upmu_get_qi_vusb33_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vusb33_mode = %d\n", mt6325_upmu_get_qi_vusb33_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vusb33_mode = %d\n", mt6325_upmu_get_qi_vusb33_mode()); 
}

void exec_6325_ldo_vusb33_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vusb33_on_ctrl(0);
    mt6325_upmu_set_rg_vusb33_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vusb33_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vusb33_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vusb33_mode_set(0); ");
                        mt6325_upmu_set_rg_vusb33_mode_set(0);
                printk("mt6325_upmu_get_qi_vusb33_mode = %d\n", mt6325_upmu_get_qi_vusb33_mode());

                printk("mt6325_upmu_set_rg_vusb33_mode_set(1); ");
                        mt6325_upmu_set_rg_vusb33_mode_set(1);
                printk("mt6325_upmu_get_qi_vusb33_mode = %d\n", mt6325_upmu_get_qi_vusb33_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vusb33_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vusb33_srclk_mode_sel(0); 
                exec_6325_ldo_vusb33_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vusb33_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vusb33_srclk_mode_sel(1);
                exec_6325_ldo_vusb33_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vusb33_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vusb33_srclk_mode_sel(2);
                exec_6325_ldo_vusb33_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vusb33_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vusb33_srclk_mode_sel(3);
                exec_6325_ldo_vusb33_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vusb33_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vusb33_mode_ctrl(0);
}

void exec_6325_ldo_vmch_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vmch_mode = %d\n", mt6325_upmu_get_qi_vmch_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vmch_mode = %d\n", mt6325_upmu_get_qi_vmch_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vmch_mode = %d\n", mt6325_upmu_get_qi_vmch_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vmch_mode = %d\n", mt6325_upmu_get_qi_vmch_mode()); 
}

void exec_6325_ldo_vmch_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vmch_on_ctrl(0);
    mt6325_upmu_set_rg_vmch_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vmch_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vmch_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vmch_mode_set(0); ");
                        mt6325_upmu_set_rg_vmch_mode_set(0);
                printk("mt6325_upmu_get_qi_vmch_mode = %d\n", mt6325_upmu_get_qi_vmch_mode());

                printk("mt6325_upmu_set_rg_vmch_mode_set(1); ");
                        mt6325_upmu_set_rg_vmch_mode_set(1);
                printk("mt6325_upmu_get_qi_vmch_mode = %d\n", mt6325_upmu_get_qi_vmch_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vmch_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vmch_srclk_mode_sel(0); 
                exec_6325_ldo_vmch_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vmch_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vmch_srclk_mode_sel(1);
                exec_6325_ldo_vmch_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vmch_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vmch_srclk_mode_sel(2);
                exec_6325_ldo_vmch_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vmch_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vmch_srclk_mode_sel(3);
                exec_6325_ldo_vmch_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vmch_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vmch_mode_ctrl(0);
}

void exec_6325_ldo_vmc_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vmc_mode = %d\n", mt6325_upmu_get_qi_vmc_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vmc_mode = %d\n", mt6325_upmu_get_qi_vmc_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vmc_mode = %d\n", mt6325_upmu_get_qi_vmc_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vmc_mode = %d\n", mt6325_upmu_get_qi_vmc_mode()); 
}

void exec_6325_ldo_vmc_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vmc_on_ctrl(0);
    mt6325_upmu_set_rg_vmc_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vmc_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vmc_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vmc_mode_set(0); ");
                        mt6325_upmu_set_rg_vmc_mode_set(0);
                printk("mt6325_upmu_get_qi_vmc_mode = %d\n", mt6325_upmu_get_qi_vmc_mode());

                printk("mt6325_upmu_set_rg_vmc_mode_set(1); ");
                        mt6325_upmu_set_rg_vmc_mode_set(1);
                printk("mt6325_upmu_get_qi_vmc_mode = %d\n", mt6325_upmu_get_qi_vmc_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vmc_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vmc_srclk_mode_sel(0); 
                exec_6325_ldo_vmc_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vmc_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vmc_srclk_mode_sel(1);
                exec_6325_ldo_vmc_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vmc_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vmc_srclk_mode_sel(2);
                exec_6325_ldo_vmc_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vmc_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vmc_srclk_mode_sel(3);
                exec_6325_ldo_vmc_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vmc_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vmc_mode_ctrl(0);
}

void exec_6325_ldo_vemc33_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vemc_3v3_mode = %d\n", mt6325_upmu_get_qi_vemc_3v3_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vemc_3v3_mode = %d\n", mt6325_upmu_get_qi_vemc_3v3_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vemc_3v3_mode = %d\n", mt6325_upmu_get_qi_vemc_3v3_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vemc_3v3_mode = %d\n", mt6325_upmu_get_qi_vemc_3v3_mode()); 
}

void exec_6325_ldo_vemc33_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vemc33_on_ctrl(0);
    mt6325_upmu_set_rg_vemc33_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vemc33_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vemc33_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vemc33_mode_set(0); ");
                        mt6325_upmu_set_rg_vemc33_mode_set(0);
                printk("mt6325_upmu_get_qi_vemc_3v3_mode = %d\n", mt6325_upmu_get_qi_vemc_3v3_mode());

                printk("mt6325_upmu_set_rg_vemc33_mode_set(1); ");
                        mt6325_upmu_set_rg_vemc33_mode_set(1);
                printk("mt6325_upmu_get_qi_vemc_3v3_mode = %d\n", mt6325_upmu_get_qi_vemc_3v3_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vemc33_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vemc33_srclk_mode_sel(0); 
                exec_6325_ldo_vemc33_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vemc33_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vemc33_srclk_mode_sel(1);
                exec_6325_ldo_vemc33_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vemc33_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vemc33_srclk_mode_sel(2);
                exec_6325_ldo_vemc33_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vemc33_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vemc33_srclk_mode_sel(3);
                exec_6325_ldo_vemc33_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vemc33_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vemc33_mode_ctrl(0);
}

void exec_6325_ldo_vio28_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vio28_mode = %d\n", mt6325_upmu_get_qi_vio28_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vio28_mode = %d\n", mt6325_upmu_get_qi_vio28_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vio28_mode = %d\n", mt6325_upmu_get_qi_vio28_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vio28_mode = %d\n", mt6325_upmu_get_qi_vio28_mode()); 
}

void exec_6325_ldo_vio28_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vio28_on_ctrl(0);
    mt6325_upmu_set_rg_vio28_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vio28_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vio28_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vio28_mode_set(0); ");
                        mt6325_upmu_set_rg_vio28_mode_set(0);
                printk("mt6325_upmu_get_qi_vio28_mode = %d\n", mt6325_upmu_get_qi_vio28_mode());

                printk("mt6325_upmu_set_rg_vio28_mode_set(1); ");
                        mt6325_upmu_set_rg_vio28_mode_set(1);
                printk("mt6325_upmu_get_qi_vio28_mode = %d\n", mt6325_upmu_get_qi_vio28_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vio28_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vio28_srclk_mode_sel(0); 
                exec_6325_ldo_vio28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vio28_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vio28_srclk_mode_sel(1);
                exec_6325_ldo_vio28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vio28_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vio28_srclk_mode_sel(2);
                exec_6325_ldo_vio28_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vio28_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vio28_srclk_mode_sel(3);
                exec_6325_ldo_vio28_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vio28_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vio28_mode_ctrl(0);
}

void exec_6325_ldo_vcam_af_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vcamaf_mode = %d\n", mt6325_upmu_get_qi_vcamaf_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vcamaf_mode = %d\n", mt6325_upmu_get_qi_vcamaf_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vcamaf_mode = %d\n", mt6325_upmu_get_qi_vcamaf_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vcamaf_mode = %d\n", mt6325_upmu_get_qi_vcamaf_mode()); 
}

void exec_6325_ldo_vcam_af_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vcam_af_on_ctrl(0);
    mt6325_upmu_set_rg_vcam_af_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vcam_af_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vcam_af_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vcam_af_mode_set(0); ");
                        mt6325_upmu_set_rg_vcam_af_mode_set(0);
                printk("mt6325_upmu_get_qi_vcamaf_mode = %d\n", mt6325_upmu_get_qi_vcamaf_mode());

                printk("mt6325_upmu_set_rg_vcam_af_mode_set(1); ");
                        mt6325_upmu_set_rg_vcam_af_mode_set(1);
                printk("mt6325_upmu_get_qi_vcamaf_mode = %d\n", mt6325_upmu_get_qi_vcamaf_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vcam_af_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vcam_af_srclk_mode_sel(0); 
                exec_6325_ldo_vcam_af_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcam_af_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vcam_af_srclk_mode_sel(1);
                exec_6325_ldo_vcam_af_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcam_af_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vcam_af_srclk_mode_sel(2);
                exec_6325_ldo_vcam_af_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcam_af_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vcam_af_srclk_mode_sel(3);
                exec_6325_ldo_vcam_af_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vcam_af_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vcam_af_mode_ctrl(0);
}

void exec_6325_ldo_vgp1_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vgp1_mode = %d\n", mt6325_upmu_get_qi_vgp1_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vgp1_mode = %d\n", mt6325_upmu_get_qi_vgp1_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vgp1_mode = %d\n", mt6325_upmu_get_qi_vgp1_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vgp1_mode = %d\n", mt6325_upmu_get_qi_vgp1_mode()); 
}

void exec_6325_ldo_vgp1_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vgp1_on_ctrl(0);
    mt6325_upmu_set_rg_vgp1_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vgp1_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vgp1_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vgp1_mode_set(0); ");
                        mt6325_upmu_set_rg_vgp1_mode_set(0);
                printk("mt6325_upmu_get_qi_vgp1_mode = %d\n", mt6325_upmu_get_qi_vgp1_mode());

                printk("mt6325_upmu_set_rg_vgp1_mode_set(1); ");
                        mt6325_upmu_set_rg_vgp1_mode_set(1);
                printk("mt6325_upmu_get_qi_vgp1_mode = %d\n", mt6325_upmu_get_qi_vgp1_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vgp1_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vgp1_srclk_mode_sel(0); 
                exec_6325_ldo_vgp1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vgp1_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vgp1_srclk_mode_sel(1);
                exec_6325_ldo_vgp1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vgp1_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vgp1_srclk_mode_sel(2);
                exec_6325_ldo_vgp1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vgp1_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vgp1_srclk_mode_sel(3);
                exec_6325_ldo_vgp1_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vgp1_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vgp1_mode_ctrl(0);
}

void exec_6325_ldo_vefuse_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vefuse_mode = %d\n", mt6325_upmu_get_qi_vefuse_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vefuse_mode = %d\n", mt6325_upmu_get_qi_vefuse_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vefuse_mode = %d\n", mt6325_upmu_get_qi_vefuse_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vefuse_mode = %d\n", mt6325_upmu_get_qi_vefuse_mode()); 
}

void exec_6325_ldo_vefuse_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vefuse_on_ctrl(0);
    mt6325_upmu_set_rg_vefuse_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vefuse_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vefuse_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vefuse_mode_set(0); ");
                        mt6325_upmu_set_rg_vefuse_mode_set(0);
                printk("mt6325_upmu_get_qi_vefuse_mode = %d\n", mt6325_upmu_get_qi_vefuse_mode());

                printk("mt6325_upmu_set_rg_vefuse_mode_set(1); ");
                        mt6325_upmu_set_rg_vefuse_mode_set(1);
                printk("mt6325_upmu_get_qi_vefuse_mode = %d\n", mt6325_upmu_get_qi_vefuse_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vefuse_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vefuse_srclk_mode_sel(0); 
                exec_6325_ldo_vefuse_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vefuse_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vefuse_srclk_mode_sel(1);
                exec_6325_ldo_vefuse_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vefuse_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vefuse_srclk_mode_sel(2);
                exec_6325_ldo_vefuse_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vefuse_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vefuse_srclk_mode_sel(3);
                exec_6325_ldo_vefuse_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vefuse_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vefuse_mode_ctrl(0);
}

void exec_6325_ldo_vsim1_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vsim1_mode = %d\n", mt6325_upmu_get_qi_vsim1_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vsim1_mode = %d\n", mt6325_upmu_get_qi_vsim1_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vsim1_mode = %d\n", mt6325_upmu_get_qi_vsim1_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vsim1_mode = %d\n", mt6325_upmu_get_qi_vsim1_mode()); 
}

void exec_6325_ldo_vsim1_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vsim1_on_ctrl(0);
    mt6325_upmu_set_rg_vsim1_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vsim1_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vsim1_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vsim1_mode_set(0); ");
                        mt6325_upmu_set_rg_vsim1_mode_set(0);
                printk("mt6325_upmu_get_qi_vsim1_mode = %d\n", mt6325_upmu_get_qi_vsim1_mode());

                printk("mt6325_upmu_set_rg_vsim1_mode_set(1); ");
                        mt6325_upmu_set_rg_vsim1_mode_set(1);
                printk("mt6325_upmu_get_qi_vsim1_mode = %d\n", mt6325_upmu_get_qi_vsim1_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vsim1_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vsim1_srclk_mode_sel(0); 
                exec_6325_ldo_vsim1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vsim1_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vsim1_srclk_mode_sel(1);
                exec_6325_ldo_vsim1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vsim1_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vsim1_srclk_mode_sel(2);
                exec_6325_ldo_vsim1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vsim1_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vsim1_srclk_mode_sel(3);
                exec_6325_ldo_vsim1_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vsim1_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vsim1_mode_ctrl(0);
}

void exec_6325_ldo_vsim2_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vsim2_mode = %d\n", mt6325_upmu_get_qi_vsim2_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vsim2_mode = %d\n", mt6325_upmu_get_qi_vsim2_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vsim2_mode = %d\n", mt6325_upmu_get_qi_vsim2_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vsim2_mode = %d\n", mt6325_upmu_get_qi_vsim2_mode()); 
}

void exec_6325_ldo_vsim2_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vsim2_on_ctrl(0);
    mt6325_upmu_set_rg_vsim2_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vsim2_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vsim2_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vsim2_mode_set(0); ");
                        mt6325_upmu_set_rg_vsim2_mode_set(0);
                printk("mt6325_upmu_get_qi_vsim2_mode = %d\n", mt6325_upmu_get_qi_vsim2_mode());

                printk("mt6325_upmu_set_rg_vsim2_mode_set(1); ");
                        mt6325_upmu_set_rg_vsim2_mode_set(1);
                printk("mt6325_upmu_get_qi_vsim2_mode = %d\n", mt6325_upmu_get_qi_vsim2_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vsim2_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vsim2_srclk_mode_sel(0); 
                exec_6325_ldo_vsim2_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vsim2_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vsim2_srclk_mode_sel(1);
                exec_6325_ldo_vsim2_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vsim2_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vsim2_srclk_mode_sel(2);
                exec_6325_ldo_vsim2_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vsim2_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vsim2_srclk_mode_sel(3);
                exec_6325_ldo_vsim2_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vsim2_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vsim2_mode_ctrl(0);
}

void exec_6325_ldo_vmipi_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vmipi_mode = %d\n", mt6325_upmu_get_qi_vmipi_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vmipi_mode = %d\n", mt6325_upmu_get_qi_vmipi_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vmipi_mode = %d\n", mt6325_upmu_get_qi_vmipi_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vmipi_mode = %d\n", mt6325_upmu_get_qi_vmipi_mode()); 
}

void exec_6325_ldo_vmipi_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vmipi_on_ctrl(0);
    mt6325_upmu_set_rg_vmipi_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vmipi_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vmipi_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vmipi_mode_set(0); ");
                        mt6325_upmu_set_rg_vmipi_mode_set(0);
                printk("mt6325_upmu_get_qi_vmipi_mode = %d\n", mt6325_upmu_get_qi_vmipi_mode());

                printk("mt6325_upmu_set_rg_vmipi_mode_set(1); ");
                        mt6325_upmu_set_rg_vmipi_mode_set(1);
                printk("mt6325_upmu_get_qi_vmipi_mode = %d\n", mt6325_upmu_get_qi_vmipi_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vmipi_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vmipi_srclk_mode_sel(0); 
                exec_6325_ldo_vmipi_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vmipi_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vmipi_srclk_mode_sel(1);
                exec_6325_ldo_vmipi_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vmipi_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vmipi_srclk_mode_sel(2);
                exec_6325_ldo_vmipi_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vmipi_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vmipi_srclk_mode_sel(3);
                exec_6325_ldo_vmipi_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vmipi_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vmipi_mode_ctrl(0);
}

void exec_6325_ldo_vcn18_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vcn18_mode = %d\n", mt6325_upmu_get_qi_vcn18_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vcn18_mode = %d\n", mt6325_upmu_get_qi_vcn18_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vcn18_mode = %d\n", mt6325_upmu_get_qi_vcn18_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vcn18_mode = %d\n", mt6325_upmu_get_qi_vcn18_mode()); 
}

void exec_6325_ldo_vcn18_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vcn18_on_ctrl(0);
    mt6325_upmu_set_rg_vcn18_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vcn18_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vcn18_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vcn18_mode_set(0); ");
                        mt6325_upmu_set_rg_vcn18_mode_set(0);
                printk("mt6325_upmu_get_qi_vcn18_mode = %d\n", mt6325_upmu_get_qi_vcn18_mode());

                printk("mt6325_upmu_set_rg_vcn18_mode_set(1); ");
                        mt6325_upmu_set_rg_vcn18_mode_set(1);
                printk("mt6325_upmu_get_qi_vcn18_mode = %d\n", mt6325_upmu_get_qi_vcn18_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vcn18_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vcn18_srclk_mode_sel(0); 
                exec_6325_ldo_vcn18_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcn18_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vcn18_srclk_mode_sel(1);
                exec_6325_ldo_vcn18_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcn18_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vcn18_srclk_mode_sel(2);
                exec_6325_ldo_vcn18_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcn18_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vcn18_srclk_mode_sel(3);
                exec_6325_ldo_vcn18_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vcn18_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vcn18_mode_ctrl(0);
}

void exec_6325_ldo_vgp2_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vgp2_mode = %d\n", mt6325_upmu_get_qi_vgp2_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vgp2_mode = %d\n", mt6325_upmu_get_qi_vgp2_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vgp2_mode = %d\n", mt6325_upmu_get_qi_vgp2_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vgp2_mode = %d\n", mt6325_upmu_get_qi_vgp2_mode()); 
}

void exec_6325_ldo_vgp2_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vgp2_on_ctrl(0);
    mt6325_upmu_set_rg_vgp2_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vgp2_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vgp2_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vgp2_mode_set(0); ");
                        mt6325_upmu_set_rg_vgp2_mode_set(0);
                printk("mt6325_upmu_get_qi_vgp2_mode = %d\n", mt6325_upmu_get_qi_vgp2_mode());

                printk("mt6325_upmu_set_rg_vgp2_mode_set(1); ");
                        mt6325_upmu_set_rg_vgp2_mode_set(1);
                printk("mt6325_upmu_get_qi_vgp2_mode = %d\n", mt6325_upmu_get_qi_vgp2_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vgp2_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vgp2_srclk_mode_sel(0); 
                exec_6325_ldo_vgp2_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vgp2_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vgp2_srclk_mode_sel(1);
                exec_6325_ldo_vgp2_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vgp2_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vgp2_srclk_mode_sel(2);
                exec_6325_ldo_vgp2_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vgp2_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vgp2_srclk_mode_sel(3);
                exec_6325_ldo_vgp2_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vgp2_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vgp2_mode_ctrl(0);
}

void exec_6325_ldo_vcamd_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vcamd_mode = %d\n", mt6325_upmu_get_qi_vcamd_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vcamd_mode = %d\n", mt6325_upmu_get_qi_vcamd_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vcamd_mode = %d\n", mt6325_upmu_get_qi_vcamd_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vcamd_mode = %d\n", mt6325_upmu_get_qi_vcamd_mode()); 
}

void exec_6325_ldo_vcamd_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vcamd_on_ctrl(0);
    mt6325_upmu_set_rg_vcamd_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vcamd_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vcamd_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vcamd_mode_set(0); ");
                        mt6325_upmu_set_rg_vcamd_mode_set(0);
                printk("mt6325_upmu_get_qi_vcamd_mode = %d\n", mt6325_upmu_get_qi_vcamd_mode());

                printk("mt6325_upmu_set_rg_vcamd_mode_set(1); ");
                        mt6325_upmu_set_rg_vcamd_mode_set(1);
                printk("mt6325_upmu_get_qi_vcamd_mode = %d\n", mt6325_upmu_get_qi_vcamd_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vcamd_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vcamd_srclk_mode_sel(0); 
                exec_6325_ldo_vcamd_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcamd_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vcamd_srclk_mode_sel(1);
                exec_6325_ldo_vcamd_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcamd_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vcamd_srclk_mode_sel(2);
                exec_6325_ldo_vcamd_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcamd_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vcamd_srclk_mode_sel(3);
                exec_6325_ldo_vcamd_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vcamd_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vcamd_mode_ctrl(0);
}

void exec_6325_ldo_vcam_io_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vcamio_mode = %d\n", mt6325_upmu_get_qi_vcamio_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vcamio_mode = %d\n", mt6325_upmu_get_qi_vcamio_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vcamio_mode = %d\n", mt6325_upmu_get_qi_vcamio_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vcamio_mode = %d\n", mt6325_upmu_get_qi_vcamio_mode()); 
}

void exec_6325_ldo_vcam_io_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vcam_io_on_ctrl(0);
    mt6325_upmu_set_rg_vcam_io_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vcam_io_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vcam_io_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vcam_io_mode_set(0); ");
                        mt6325_upmu_set_rg_vcam_io_mode_set(0);
                printk("mt6325_upmu_get_qi_vcamio_mode = %d\n", mt6325_upmu_get_qi_vcamio_mode());

                printk("mt6325_upmu_set_rg_vcam_io_mode_set(1); ");
                        mt6325_upmu_set_rg_vcam_io_mode_set(1);
                printk("mt6325_upmu_get_qi_vcamio_mode = %d\n", mt6325_upmu_get_qi_vcamio_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vcam_io_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vcam_io_srclk_mode_sel(0); 
                exec_6325_ldo_vcam_io_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcam_io_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vcam_io_srclk_mode_sel(1);
                exec_6325_ldo_vcam_io_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcam_io_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vcam_io_srclk_mode_sel(2);
                exec_6325_ldo_vcam_io_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vcam_io_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vcam_io_srclk_mode_sel(3);
                exec_6325_ldo_vcam_io_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vcam_io_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vcam_io_mode_ctrl(0);
}

void exec_6325_ldo_vsram_dvfs1_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vsram_dvfs1_mode = %d\n", mt6325_upmu_get_qi_vsram_dvfs1_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vsram_dvfs1_mode = %d\n", mt6325_upmu_get_qi_vsram_dvfs1_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vsram_dvfs1_mode = %d\n", mt6325_upmu_get_qi_vsram_dvfs1_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vsram_dvfs1_mode = %d\n", mt6325_upmu_get_qi_vsram_dvfs1_mode()); 
}

void exec_6325_ldo_vsram_dvfs1_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vsram_dvfs1_on_ctrl(0);
    mt6325_upmu_set_rg_vsram_dvfs1_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vsram_dvfs1_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vsram_dvfs1_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vsram_dvfs1_mode_set(0); ");
                        mt6325_upmu_set_rg_vsram_dvfs1_mode_set(0);
                printk("mt6325_upmu_get_qi_vsram_dvfs1_mode = %d\n", mt6325_upmu_get_qi_vsram_dvfs1_mode());

                printk("mt6325_upmu_set_rg_vsram_dvfs1_mode_set(1); ");
                        mt6325_upmu_set_rg_vsram_dvfs1_mode_set(1);
                printk("mt6325_upmu_get_qi_vsram_dvfs1_mode = %d\n", mt6325_upmu_get_qi_vsram_dvfs1_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vsram_dvfs1_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vsram_dvfs1_srclk_mode_sel(0); 
                exec_6325_ldo_vsram_dvfs1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vsram_dvfs1_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vsram_dvfs1_srclk_mode_sel(1);
                exec_6325_ldo_vsram_dvfs1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vsram_dvfs1_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vsram_dvfs1_srclk_mode_sel(2);
                exec_6325_ldo_vsram_dvfs1_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vsram_dvfs1_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vsram_dvfs1_srclk_mode_sel(3);
                exec_6325_ldo_vsram_dvfs1_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vsram_dvfs1_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vsram_dvfs1_mode_ctrl(0);
}

void exec_6325_ldo_vgp3_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vgp3_mode = %d\n", mt6325_upmu_get_qi_vgp3_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vgp3_mode = %d\n", mt6325_upmu_get_qi_vgp3_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vgp3_mode = %d\n", mt6325_upmu_get_qi_vgp3_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vgp3_mode = %d\n", mt6325_upmu_get_qi_vgp3_mode()); 
}

void exec_6325_ldo_vgp3_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vgp3_on_ctrl(0);
    mt6325_upmu_set_rg_vgp3_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vgp3_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vgp3_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vgp3_mode_set(0); ");
                        mt6325_upmu_set_rg_vgp3_mode_set(0);
                printk("mt6325_upmu_get_qi_vgp3_mode = %d\n", mt6325_upmu_get_qi_vgp3_mode());

                printk("mt6325_upmu_set_rg_vgp3_mode_set(1); ");
                        mt6325_upmu_set_rg_vgp3_mode_set(1);
                printk("mt6325_upmu_get_qi_vgp3_mode = %d\n", mt6325_upmu_get_qi_vgp3_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vgp3_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vgp3_srclk_mode_sel(0); 
                exec_6325_ldo_vgp3_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vgp3_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vgp3_srclk_mode_sel(1);
                exec_6325_ldo_vgp3_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vgp3_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vgp3_srclk_mode_sel(2);
                exec_6325_ldo_vgp3_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vgp3_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vgp3_srclk_mode_sel(3);
                exec_6325_ldo_vgp3_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vgp3_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vgp3_mode_ctrl(0);
}

void exec_6325_ldo_vbiasn_mode_test_sub(void)
{
    set_srclken_sw_mode();

    printk("set_srclken_0_val(0); set_srclken_1_val(0); \n"); set_srclken_0_val(0); set_srclken_1_val(0);
    printk("mt6325_upmu_get_qi_vbiasn_mode = %d\n", mt6325_upmu_get_qi_vbiasn_mode()); 
    printk("set_srclken_0_val(0); set_srclken_1_val(1); \n"); set_srclken_0_val(0); set_srclken_1_val(1);
    printk("mt6325_upmu_get_qi_vbiasn_mode = %d\n", mt6325_upmu_get_qi_vbiasn_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(0); \n"); set_srclken_0_val(1); set_srclken_1_val(0); 
    printk("mt6325_upmu_get_qi_vbiasn_mode = %d\n", mt6325_upmu_get_qi_vbiasn_mode()); 
    printk("set_srclken_0_val(1); set_srclken_1_val(1); \n"); set_srclken_0_val(1); set_srclken_1_val(1); 
    printk("mt6325_upmu_get_qi_vbiasn_mode = %d\n", mt6325_upmu_get_qi_vbiasn_mode()); 
}

void exec_6325_ldo_vbiasn_mode_test(void)
{
    int i=0;    

    mt6325_upmu_set_rg_vbiasn_on_ctrl(0);
    mt6325_upmu_set_rg_vbiasn_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6325_upmu_set_rg_vbiasn_mode_ctrl] %d\n", i);
        mt6325_upmu_set_rg_vbiasn_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6325_upmu_set_rg_vbiasn_mode_set(0); ");
                        mt6325_upmu_set_rg_vbiasn_mode_set(0);
                printk("mt6325_upmu_get_qi_vbiasn_mode = %d\n", mt6325_upmu_get_qi_vbiasn_mode());

                printk("mt6325_upmu_set_rg_vbiasn_mode_set(1); ");
                        mt6325_upmu_set_rg_vbiasn_mode_set(1);
                printk("mt6325_upmu_get_qi_vbiasn_mode = %d\n", mt6325_upmu_get_qi_vbiasn_mode());
                break;

            case 1:
                printk("[mt6325_upmu_set_rg_vbiasn_srclk_mode_sel(0)]\n"); 
                         mt6325_upmu_set_rg_vbiasn_srclk_mode_sel(0); 
                exec_6325_ldo_vbiasn_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vbiasn_srclk_mode_sel(1)]\n");
                         mt6325_upmu_set_rg_vbiasn_srclk_mode_sel(1);
                exec_6325_ldo_vbiasn_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vbiasn_srclk_mode_sel(2)]\n");
                         mt6325_upmu_set_rg_vbiasn_srclk_mode_sel(2);
                exec_6325_ldo_vbiasn_mode_test_sub();
                
                printk("[mt6325_upmu_set_rg_vbiasn_srclk_mode_sel(3)]\n");
                         mt6325_upmu_set_rg_vbiasn_srclk_mode_sel(3);
                exec_6325_ldo_vbiasn_mode_test_sub();  
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6325_upmu_set_rg_vbiasn_mode_ctrl(0);\n"); mt6325_upmu_set_rg_vbiasn_mode_ctrl(0);
}

void exec_6311_ldo_vbiasn_mode_test(void)
{
    int i=0;    

    mt6311_set_rg_vbiasn_on_ctrl(0);
    mt6311_set_rg_vbiasn_en(1);

    for(i=0;i<=1;i++)
    {
        printk("[mt6311_set_rg_vbiasn_mode_ctrl] %d\n", i);
        mt6311_set_rg_vbiasn_mode_ctrl(i);

        switch(i){
            case 0:
                printk("mt6311_set_rg_vbiasn_mode_set(0); ");
                        mt6311_set_rg_vbiasn_mode_set(0);
                printk("mt6311_get_qi_vbiasn_mode = %d\n", mt6311_get_qi_vbiasn_mode());

                printk("mt6311_set_rg_vbiasn_mode_set(1); ");
                        mt6311_set_rg_vbiasn_mode_set(1);
                printk("mt6311_get_qi_vbiasn_mode = %d\n", mt6311_get_qi_vbiasn_mode());
                break;

            case 1:
                set_mt6311_srclken_sw_mode();

                printk("set_mt6311_srclken_val(0); ");
                        set_mt6311_srclken_val(0); 
                printk("mt6311_get_qi_vbiasn_mode = %d\n", mt6311_get_qi_vbiasn_mode());

                printk("set_mt6311_srclken_val(1); ");
                        set_mt6311_srclken_val(1); 
                printk("mt6311_get_qi_vbiasn_mode = %d\n", mt6311_get_qi_vbiasn_mode());

                printk("set_mt6311_srclken_val(0); ");
                        set_mt6311_srclken_val(0);
                printk("mt6311_get_qi_vbiasn_mode = %d\n", mt6311_get_qi_vbiasn_mode());

                printk("set_mt6311_srclken_val(1); ");
                        set_mt6311_srclken_val(1);
                printk("mt6311_get_qi_vbiasn_mode = %d\n", mt6311_get_qi_vbiasn_mode());
                break;

            default:
                printk("Invalid channel value(%d)\n", i);
                break;
        }
    }

    printk("Final reset to 0: mt6311_set_rg_vbiasn_mode_ctrl(0);\n"); mt6311_set_rg_vbiasn_mode_ctrl(0);
}
#endif

//                                        
//    
//                                        
#ifdef BIF_DVT_ENABLE
int g_loop_out = 50;

void reset_bif_irq(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int loop_i=0;
    
    printk("reset BIF_IRQ : BIF_IRQ_CLR=1\n");
    ret=pmic_config_interface(MT6325_BIF_CON31, 0x1, 0x1, 1);
    
    reg_val=0;
    while(reg_val != 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    printk("When BIF_IRQ=0 : BIF_IRQ_CLR =0\n");
    ret=pmic_config_interface(MT6325_BIF_CON31, 0x0, 0x1, 1); 
}

void set_bif_cmd(int bif_cmd[], int bif_cmd_len)
{
    int i=0;
    int con_index=0;
    U32 ret=0;
    
    for(i=0;i<bif_cmd_len;i++)
    {
        ret=pmic_config_interface(MT6325_BIF_CON0+con_index, bif_cmd[i], 0x07FF, 0);
        con_index += 0x2;
    }
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "\n Reg[0x%x]=0x%x,Reg[0x%x]=0x%x,Reg[0x%x]=0x%x,Reg[0x%x]=0x%x \n Reg[0x%x]=0x%x,Reg[0x%x]=0x%x,Reg[0x%x]=0x%x,Reg[0x%x]=0x%x \n Reg[0x%x]=0x%x,Reg[0x%x]=0x%x,Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON0, upmu_get_reg_value(MT6325_BIF_CON0),
        MT6325_BIF_CON1, upmu_get_reg_value(MT6325_BIF_CON1),
        MT6325_BIF_CON2, upmu_get_reg_value(MT6325_BIF_CON2),
        MT6325_BIF_CON3, upmu_get_reg_value(MT6325_BIF_CON3),
        MT6325_BIF_CON4, upmu_get_reg_value(MT6325_BIF_CON4),
        MT6325_BIF_CON5, upmu_get_reg_value(MT6325_BIF_CON5),
        MT6325_BIF_CON6, upmu_get_reg_value(MT6325_BIF_CON6),
        MT6325_BIF_CON7, upmu_get_reg_value(MT6325_BIF_CON7),
        MT6325_BIF_CON8, upmu_get_reg_value(MT6325_BIF_CON8),
        MT6325_BIF_CON9, upmu_get_reg_value(MT6325_BIF_CON9),
        MT6325_BIF_CON10,upmu_get_reg_value(MT6325_BIF_CON10),
        MT6325_BIF_CON11,upmu_get_reg_value(MT6325_BIF_CON11)
        );
}

void check_bat_lost(void)
{
    U32 ret=0;
    U32 reg_val=0;
        
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 13);
    printk("[check_bat_lost] BIF_BAT_LOST : BIF_CON31[13]=0x%x - battery is detected(0)/battery is undetected(1)\n", reg_val);
}

void bif_debug(void)
{
    U32 ret=0;    
    U32 qi_bif_tx_en=0;
    U32 qi_bif_tx_data=0;
    U32 qi_bif_rx_en=0;
    U32 qi_bif_rx_data=0;
    U32 bif_tx_state=0;
    U32 bif_flow_ctl_state=0;    
    U32 bif_rx_state=0;
    U32 bif_data_num=0;
    U32 rgs_baton_hv=0;
    U32 repeat=10;
    int i=0;

    printk("[bif_debug] qi_bif_tx_en, qi_bif_tx_data, qi_bif_rx_en, qi_bif_rx_data, bif_tx_state, bif_flow_ctl_state, bif_rx_state, bif_data_num, rgs_baton_hv\n");

    for(i=0;i<repeat;i++)
    {
        ret=pmic_read_interface(MT6325_BIF_CON33, &qi_bif_tx_en, 0x1, 15);
        ret=pmic_read_interface(MT6325_BIF_CON33, &qi_bif_tx_data, 0x1, 14);
        ret=pmic_read_interface(MT6325_BIF_CON33, &qi_bif_rx_en, 0x1, 13);
        ret=pmic_read_interface(MT6325_BIF_CON33, &qi_bif_rx_data, 0x1, 12);
        ret=pmic_read_interface(MT6325_BIF_CON33, &bif_tx_state, 0x3, 10);
        ret=pmic_read_interface(MT6325_BIF_CON33, &bif_flow_ctl_state, 0x3, 8);        
        ret=pmic_read_interface(MT6325_BIF_CON33, &bif_rx_state, 0x7, 5);
        ret=pmic_read_interface(MT6325_BIF_CON19, &bif_data_num, 0xF, 0);
        ret=pmic_read_interface(MT6325_BATON_CON0,&rgs_baton_hv, 0x1, 6);

        printk("[bif_debug] %d,%d,%d,%d,%d,%d,%d,%d,%d\n", 
            qi_bif_tx_en, qi_bif_tx_data, qi_bif_rx_en, qi_bif_rx_data, bif_tx_state, bif_flow_ctl_state, bif_rx_state, bif_data_num, rgs_baton_hv);        
    }    
}

void tc_bif_reset_slave(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[1]={0};
    int loop_i=0;
    
    //                           
    printk("[tc_bif_reset_slave] 1. set command sequence\n");
    bif_cmd[0]=0x0400;
    set_bif_cmd(bif_cmd,1);

    //                    
    printk("[tc_bif_reset_slave] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x0, 0x3, 8);
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_reset_slave] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0); 

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_reset_slave] 4. polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                  
    printk("[tc_bif_reset_slave] 5. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0); 

    check_bat_lost();

    //             
    reset_bif_irq();
}

void debug_dump_reg(void)
{
    U32 ret=0;
    U32 reg_val=0;
    
    ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xFFFF, 0);
    printk("[debug_dump_reg] Reg[0x%x]=0x%x\n", MT6325_BIF_CON19, reg_val);

    ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFFFF, 0);
    printk("[debug_dump_reg] Reg[0x%x]=0x%x\n", MT6325_BIF_CON20, reg_val);

    ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0xFFFF, 0);
    printk("[debug_dump_reg] Reg[0x%x]=0x%x\n", MT6325_BIF_CON21, reg_val);

    ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0xFFFF, 0);
    printk("[debug_dump_reg] Reg[0x%x]=0x%x\n", MT6325_BIF_CON22, reg_val);

    ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0xFFFF, 0);
    printk("[debug_dump_reg] Reg[0x%x]=0x%x\n", MT6325_BIF_CON23, reg_val);
}

#if 1 //            
void tc_bif_1000_step_0(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int loop_i=0;
    
    printk("[tc_bif_1000_step_0]-----------------------------------\n");

    printk("[tc_bif_1000_step_0] 1. set power up regiser\n");
    ret=pmic_config_interface(MT6325_BIF_CON32, 0x1, 0x1, 15);

    printk("[tc_bif_1000_step_0] 2. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);

    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON32,upmu_get_reg_value(MT6325_BIF_CON32),
        MT6325_BIF_CON18,upmu_get_reg_value(MT6325_BIF_CON18)
        );

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1000_step_0] 3. polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    printk("[tc_bif_1000_step_0] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);

    printk("[tc_bif_1000_step_0] 5. to disable power up mode\n");
    ret=pmic_config_interface(MT6325_BIF_CON32, 0x0, 0x1, 15);

    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON32,upmu_get_reg_value(MT6325_BIF_CON32),
        MT6325_BIF_CON18,upmu_get_reg_value(MT6325_BIF_CON18)
        );

    check_bat_lost();

    reset_bif_irq();
}

void tc_bif_1000_step_1(void)
{
    printk("[tc_bif_1000_step_1]-----------------------------------\n");

    tc_bif_reset_slave();    
}

void tc_bif_1000_step_2(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[3]={0,0,0};
    int loop_i=0;
    
    printk("[tc_bif_1000_step_2]-----------------------------------\n");

    //                           
    printk("[tc_bif_1000_step_2] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x0100;
    bif_cmd[2]=0x0300;
    set_bif_cmd(bif_cmd,3);

    //                    
    printk("[tc_bif_1000_step_2] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x3, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);    
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x1, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );

    //                     
    printk("[tc_bif_1000_step_2] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1000_step_2] 4. polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                  
    printk("[tc_bif_1000_step_2] 5. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    

    //                
    printk("[tc_bif_1000_step_2] 6. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1000_step_2] BIF_CON31[14:12]=0x%x\n", reg_val);

    if(reg_val == 0)
    {
        //                                         
        printk("[tc_bif_1000_step_2] 7. read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1000_step_2] BIF_DATA_NUM (1) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //              
        printk("[tc_bif_1000_step_2] 8. read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1000_step_2] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1000_step_2] BIF_ACK_0 (1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1000_step_2] BIF_DATA_0 (10H) : BIF_CON20[7:0]=0x%x\n", reg_val);
    }
    else
    {
        printk("[tc_bif_1000_step_2] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);

        debug_dump_reg();
    }
 
    //             
    reset_bif_irq();
}
#endif

#if 1 //            
int pcap_15_8 = 0;
int pcap_7_0 = 0;
int pcreg_15_8 = 0;
int pcreg_7_0 = 0;

void tc_bif_1001_step_1(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[4]={0,0,0,0};
    int loop_i=0;
	
    printk("[tc_bif_1001_step_1]-----------------------------------\n");
	
    //                           
    printk("[tc_bif_1001_step_1] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x0424;
    bif_cmd[2]=0x0100;
    bif_cmd[3]=0x030A;
    set_bif_cmd(bif_cmd,4);
        
    //                    
    printk("[tc_bif_1001_step_1] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x4, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);
    //                                                           
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x4, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1001_step_1] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1001_step_1] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1001_step_1] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                   
    printk("[tc_bif_1001_step_1] 5. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1001_step_1] BIF_CON31[14:12]=0x%x\n", reg_val);    
    
    if(reg_val == 0)
    {
        //                                            
        printk("[tc_bif_1001_step_1] 6. read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1001_step_1] BIF_DATA_NUM (4) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //                 
        printk("[tc_bif_1001_step_1] 7. read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1001_step_1] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1001_step_1] BIF_ACK_0 (1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1001_step_1] BIF_DATA_0 (01H) : BIF_CON20[7:0]=0x%x\n", reg_val);
    
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 15);
        printk("[tc_bif_1001_step_1] BIF_DATA_ERROR_0 (0) : BIF_CON21[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 8);
        printk("[tc_bif_1001_step_1] BIF_ACK_0 (1) : BIF_CON21[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0xFF, 0);
        printk("[tc_bif_1001_step_1] BIF_DATA_0 (01H) : BIF_CON21[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0x1, 15);
        printk("[tc_bif_1001_step_1] BIF_DATA_ERROR_0 (0) : BIF_CON22[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0x1, 8);
        printk("[tc_bif_1001_step_1] BIF_ACK_0 (1) : BIF_CON22[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0xFF, 0);
        printk("[tc_bif_1001_step_1] BIF_DATA_0 (pcap[15:8]=01H) : BIF_CON22[7:0]=0x%x\n", reg_val);
        pcap_15_8=reg_val;
    
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0x1, 15);
        printk("[tc_bif_1001_step_1] BIF_DATA_ERROR_0 (0) : BIF_CON23[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0x1, 8);
        printk("[tc_bif_1001_step_1] BIF_ACK_0 (1) : BIF_CON23[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0xFF, 0);
        printk("[tc_bif_1001_step_1] BIF_DATA_0 (pcap[7:0]=00H) : BIF_CON23[7:0]=0x%x\n", reg_val);
        pcap_7_0=reg_val;
    }
    else
    {
        printk("[tc_bif_1001_step_1] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);

        debug_dump_reg();
    }
 
    //                
    reset_bif_irq();
}

void tc_bif_1001_step_2(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[4]={0,0,0,0};
    int loop_i=0;
	
    printk("[tc_bif_1001_step_2]-----------------------------------\n");
	
    //                           
    printk("[tc_bif_1001_step_2] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x0422;
    bif_cmd[2]=(0x0100|pcap_15_8);
    bif_cmd[3]=(0x0300|pcap_7_0);
    set_bif_cmd(bif_cmd,4);
        
    //                    
    printk("[tc_bif_1001_step_2] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x4, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x2, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1001_step_2] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1001_step_2] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1001_step_2] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                   
    printk("[tc_bif_1001_step_2] 5. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1001_step_2] BIF_CON31[14:12]=0x%x\n", reg_val);    
    
    if(reg_val == 0)
    {
        //                                            
        printk("[tc_bif_1001_step_2] 6. read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1001_step_2] BIF_DATA_NUM (2) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //                 
        printk("[tc_bif_1001_step_2] 7. read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1001_step_2] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1001_step_2] BIF_ACK_0 (1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1001_step_2] BIF_DATA_0 (0A) : BIF_CON20[7:0]=0x%x\n", reg_val);
        pcreg_15_8=reg_val;
    
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 15);
        printk("[tc_bif_1001_step_2] BIF_DATA_ERROR_0 (0) : BIF_CON21[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 8);
        printk("[tc_bif_1001_step_2] BIF_ACK_0 (1) : BIF_CON21[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0xFF, 0);
        printk("[tc_bif_1001_step_2] BIF_DATA_0 (00) : BIF_CON21[7:0]=0x%x\n", reg_val);
        pcreg_7_0=reg_val;
    }
    else
    {
        printk("[tc_bif_1001_step_2] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);

        debug_dump_reg();
    }
 
    //                
    reset_bif_irq();
}


void tc_bif_1001_step_3(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[4]={0,0,0,0};
    int loop_i=0;
	
	  printk("[tc_bif_1001_step_3]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1001_step_3] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=(0x0100|pcreg_15_8);
    bif_cmd[2]=(0x0200|pcreg_7_0);
    bif_cmd[3]=0x0000;
    set_bif_cmd(bif_cmd,4);
        
    //                    
    printk("[tc_bif_1001_step_3] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x4, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x0, 0x3, 8);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15)
        );
        
    //                     
    printk("[tc_bif_1001_step_3] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1001_step_3] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1001_step_3] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);

    check_bat_lost();
    
    //                
    reset_bif_irq();
}

void tc_bif_1001_step_4(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[3]={0,0,0};
    int loop_i=0;
	
	  printk("[tc_bif_1001_step_4]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1001_step_4] 1. set command sequence\n");
    bif_cmd[0]=0x0600;
    bif_cmd[1]=(0x0100|pcreg_15_8);
    bif_cmd[2]=(0x0300|pcreg_7_0);    
    set_bif_cmd(bif_cmd,3);       
        
    //                    
    printk("[tc_bif_1001_step_4] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x3, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);    
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x1, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
        
    //                     
    printk("[tc_bif_1001_step_4] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1001_step_4] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1001_step_4] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                   
    printk("[tc_bif_1001_step_4] 5. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1001_step_4] BIF_CON31[14:12]=0x%x\n", reg_val);    
    
    if(reg_val == 0)
    {
        //                                            
        printk("[tc_bif_1001_step_4] 6. read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1001_step_4] BIF_DATA_NUM (1) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //                 
        printk("[tc_bif_1001_step_4] 7. read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1001_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1001_step_4] BIF_ACK_0 (1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1001_step_4] BIF_DATA_0 (00H) : BIF_CON20[7:0]=0x%x\n", reg_val);
    }
    else
    {
        printk("[tc_bif_1001_step_4] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);

        debug_dump_reg();
    }
 
    //                
    reset_bif_irq(); 
}

void tc_bif_1001_step_5(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[5]={0,0,0,0,0};
    int loop_i=0;
	
	  printk("[tc_bif_1001_step_5]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1001_step_5] 1. set command sequence\n");
    bif_cmd[0]=0x0600;
    bif_cmd[1]=(0x0100|pcreg_15_8);
    bif_cmd[2]=(0x0200|pcreg_7_0);
    bif_cmd[3]=0x0001;
    bif_cmd[4]=0x04C2;
    set_bif_cmd(bif_cmd,5);
        
    //                    
    printk("[tc_bif_1001_step_5] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x5, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);    
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x1, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
        
    //                     
    printk("[tc_bif_1001_step_5] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1001_step_5] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1001_step_5] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                   
    printk("[tc_bif_1001_step_5] 5. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1001_step_5] BIF_CON31[14:12]=0x%x\n", reg_val); 
    
    if(reg_val == 0)
    {
        //                                            
        printk("[tc_bif_1001_step_5] 6. read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1001_step_5] BIF_DATA_NUM (1) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //                 
        printk("[tc_bif_1001_step_5] 7. read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1001_step_5] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1001_step_5] BIF_ACK_0 (1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1001_step_5] BIF_DATA_0 (04h) : BIF_CON20[7:0]=0x%x\n", reg_val);
    }
    else
    {
        printk("[tc_bif_1001_step_5] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);

        debug_dump_reg();
    }
    
    //                
    reset_bif_irq();
}
#endif

#if 1 //            
int nvmcap_15_8 = 0;
int nvmcap_7_0 = 0;
int nvmreg_15_8 = 0;
int nvmreg_7_0 = 0;

void tc_bif_1002_step_1(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[4]={0,0,0,0};
    int loop_i=0;
	
	  printk("[tc_bif_1002_step_1]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1002_step_1] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x0424;
    bif_cmd[2]=0x0100;
    bif_cmd[3]=0x0316;    
    set_bif_cmd(bif_cmd,4);
        
    //                    
    printk("[tc_bif_1002_step_1] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x4, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x4, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1002_step_1] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1002_step_1] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1002_step_1] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                   
    printk("[tc_bif_1002_step_1] 5. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1002_step_1] BIF_CON31[14:12]=0x%x\n", reg_val);    
    
    if(reg_val == 0)
    {
        //                                            
        printk("[tc_bif_1002_step_1] 6. read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1002_step_1] BIF_DATA_NUM (4) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //                 
        printk("[tc_bif_1002_step_1] 7. read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_1] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_1] BIF_ACK_0 (1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_1] BIF_DATA_0 (04H) : BIF_CON20[7:0]=0x%x\n", reg_val);
    
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_1] BIF_DATA_ERROR_0 (0) : BIF_CON21[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_1] BIF_ACK_0 (1) : BIF_CON21[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_1] BIF_DATA_0 (01H) : BIF_CON21[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_1] BIF_DATA_ERROR_0 (0) : BIF_CON22[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_1] BIF_ACK_0 (1) : BIF_CON22[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_1] BIF_DATA_0 (pcap[15:8]=04H) : BIF_CON22[7:0]=0x%x\n", reg_val);
        nvmcap_15_8=reg_val;
    
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_1] BIF_DATA_ERROR_0 (0) : BIF_CON23[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_1] BIF_ACK_0 (1) : BIF_CON23[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_1] BIF_DATA_0 (pcap[7:0]=00H) : BIF_CON23[7:0]=0x%x\n", reg_val);
        nvmcap_7_0=reg_val;
    }
    else
    {
        printk("[tc_bif_1002_step_1] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);
    }
 
    //                
    reset_bif_irq(); 
}

void tc_bif_1002_step_2(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[4]={0,0,0,0};
    int loop_i=0;
	
	  printk("[tc_bif_1002_step_2]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1002_step_2] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x0422;
    bif_cmd[2]=(0x0100|nvmcap_15_8);
    bif_cmd[3]=(0x0300|nvmcap_7_0);    
    set_bif_cmd(bif_cmd,4);
        
    //                    
    printk("[tc_bif_1002_step_2] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x4, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x2, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1002_step_2] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1002_step_2] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1002_step_2] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                   
    printk("[tc_bif_1002_step_2] 5. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1002_step_2] BIF_CON31[14:12]=0x%x\n", reg_val);    
    
    if(reg_val == 0)
    {
        //                                            
        printk("[tc_bif_1002_step_2] 6. read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1002_step_2] BIF_DATA_NUM (2) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //                 
        printk("[tc_bif_1002_step_2] 7. read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_2] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_2] BIF_ACK_0 (1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_2] BIF_DATA_0 (nvmreg[15:8]=0FH) : BIF_CON20[7:0]=0x%x\n", reg_val);
        nvmreg_15_8=reg_val;
    
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_2] BIF_DATA_ERROR_0 (0) : BIF_CON21[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_2] BIF_ACK_0 (1) : BIF_CON21[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_2] BIF_DATA_0 (nvmreg[7:0]=00H) : BIF_CON21[7:0]=0x%x\n", reg_val);
        nvmreg_7_0=reg_val;
    }
    else
    {
        printk("[tc_bif_1002_step_2] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);
    }
 
    //                
    reset_bif_irq();
}

void tc_bif_1002_step_3(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[11]={0,0,0,0,0,0,0,0,0,0,0};
    int loop_i=0;
	
	  printk("[tc_bif_1002_step_3]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1002_step_3] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=(0x0100|nvmreg_15_8);
    bif_cmd[2]=(0x0200|nvmcap_7_0) + 0x09;
    bif_cmd[3]=0x0000;
    bif_cmd[4]=0x0001;
    bif_cmd[5]=0x0002;
    bif_cmd[6]=0x0003;
    bif_cmd[7]=0x0004;
    bif_cmd[8]=0x0005;
    bif_cmd[9]=0x0006;
    bif_cmd[10]=0x04C2;
    set_bif_cmd(bif_cmd,11);
        
    //                    
    printk("[tc_bif_1002_step_3] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0xB, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x1, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1002_step_3] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1002_step_3] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1002_step_3] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                   
    printk("[tc_bif_1002_step_3] 5. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1002_step_3] BIF_CON31[14:12]=0x%x\n", reg_val);    
    
    if(reg_val == 0)
    {
        //                                            
        printk("[tc_bif_1002_step_3] 6. read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1002_step_3] BIF_DATA_NUM (1) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //                 
        printk("[tc_bif_1002_step_3] 7. read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_3] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_3] BIF_ACK_0 (0/1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_3] BIF_DATA_0 (0Ah) : BIF_CON20[7:0]=0x%x\n", reg_val);        
    }
    else
    {
        printk("[tc_bif_1002_step_3] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);
    }
 
    //                
    reset_bif_irq();
}

void tc_bif_1002_step_4(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[4]={0,0,0,0};
    int loop_i=0;
	
	  printk("[tc_bif_1002_step_4]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1002_step_4] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x0427;
    bif_cmd[2]=(0x0100|nvmreg_15_8);
    bif_cmd[3]=(0x0300|nvmcap_7_0) + 0x09;
    set_bif_cmd(bif_cmd,4);
        
    //                    
    printk("[tc_bif_1002_step_4] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x4, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x7, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1002_step_4] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1002_step_4] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1002_step_4] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                   
    printk("[tc_bif_1002_step_4] 5. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1002_step_4] BIF_CON31[14:12]=0x%x\n", reg_val);    
    
    if(reg_val == 0)
    {
        //                                            
        printk("[tc_bif_1002_step_4] 6. read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1002_step_4] BIF_DATA_NUM (7) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //                 
        printk("[tc_bif_1002_step_4] 7. read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_4] BIF_ACK_0 (1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_4] BIF_DATA_0 (00h) : BIF_CON20[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON21[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_4] BIF_ACK_0 (1) : BIF_CON21[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_4] BIF_DATA_0 (01h) : BIF_CON21[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON22[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_4] BIF_ACK_0 (1) : BIF_CON22[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_4] BIF_DATA_0 (02h) : BIF_CON22[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON23[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_4] BIF_ACK_0 (1) : BIF_CON23[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_4] BIF_DATA_0 (03h) : BIF_CON23[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON24, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON24[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON24, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_4] BIF_ACK_0 (1) : BIF_CON24[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON24, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_4] BIF_DATA_0 (04h) : BIF_CON24[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON25, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON25[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON25, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_4] BIF_ACK_0 (1) : BIF_CON25[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON25, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_4] BIF_DATA_0 (05h) : BIF_CON25[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON26, &reg_val, 0x1, 15);
        printk("[tc_bif_1002_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON26[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON26, &reg_val, 0x1, 8);
        printk("[tc_bif_1002_step_4] BIF_ACK_0 (1) : BIF_CON26[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON26, &reg_val, 0xFF, 0);
        printk("[tc_bif_1002_step_4] BIF_DATA_0 (06h) : BIF_CON26[7:0]=0x%x\n", reg_val);
    }
    else
    {
        printk("[tc_bif_1002_step_4] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);
    }
 
    //                
    reset_bif_irq(); 
}

#endif

#if 1 //            
int sccap_15_8 = 0;
int sccap_7_0 = 0;
int screg_15_8 = 0;
int screg_7_0 = 0;

void tc_bif_1003_step_2(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[4]={0,0,0,0};
    int loop_i=0;
	
	  printk("[tc_bif_1003_step_2]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1003_step_2] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x0424;
    bif_cmd[2]=0x0100;
    bif_cmd[3]=0x030E;
    set_bif_cmd(bif_cmd,4);
        
    //                    
    printk("[tc_bif_1003_step_2] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x4, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x4, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1003_step_2] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1003_step_2] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1003_step_2] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                   
    printk("[tc_bif_1003_step_2] 5. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1003_step_2] BIF_CON31[14:12]=0x%x\n", reg_val);    
    
    if(reg_val == 0)
    {
        //                                            
        printk("[tc_bif_1003_step_2] 6. read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1003_step_2] BIF_DATA_NUM (4) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //                 
        printk("[tc_bif_1003_step_2] 7. read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1003_step_2] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1003_step_2] BIF_ACK_0 (1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1003_step_2] BIF_DATA_0 (02H) : BIF_CON20[7:0]=0x%x\n", reg_val);
    
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 15);
        printk("[tc_bif_1003_step_2] BIF_DATA_ERROR_0 (0) : BIF_CON21[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 8);
        printk("[tc_bif_1003_step_2] BIF_ACK_0 (1) : BIF_CON21[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0xFF, 0);
        printk("[tc_bif_1003_step_2] BIF_DATA_0 (01H) : BIF_CON21[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0x1, 15);
        printk("[tc_bif_1003_step_2] BIF_DATA_ERROR_0 (0) : BIF_CON22[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0x1, 8);
        printk("[tc_bif_1003_step_2] BIF_ACK_0 (1) : BIF_CON22[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0xFF, 0);
        printk("[tc_bif_1003_step_2] BIF_DATA_0 (sccap[15:8]=02H) : BIF_CON22[7:0]=0x%x\n", reg_val);
        sccap_15_8=reg_val;
    
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0x1, 15);
        printk("[tc_bif_1003_step_2] BIF_DATA_ERROR_0 (0) : BIF_CON23[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0x1, 8);
        printk("[tc_bif_1003_step_2] BIF_ACK_0 (1) : BIF_CON23[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0xFF, 0);
        printk("[tc_bif_1003_step_2] BIF_DATA_0 (sccap[7:0]=00H) : BIF_CON23[7:0]=0x%x\n", reg_val);
        sccap_7_0=reg_val;
    }
    else
    {
        printk("[tc_bif_1003_step_2] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);
    }
 
    //                
    reset_bif_irq();
}

void tc_bif_1003_step_3(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[4]={0,0,0,0};
    int loop_i=0;
	
	  printk("[tc_bif_1003_step_3]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1003_step_3] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x0422;
    bif_cmd[2]=(0x0100|sccap_15_8);
    bif_cmd[3]=(0x0300|sccap_7_0);
    set_bif_cmd(bif_cmd,4);
        
    //                    
    printk("[tc_bif_1003_step_3] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x4, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x2, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1003_step_3] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1003_step_3] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1003_step_3] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                   
    printk("[tc_bif_1003_step_3] 5. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1003_step_3] BIF_CON31[14:12]=0x%x\n", reg_val);    
    
    if(reg_val == 0)
    {
        //                                            
        printk("[tc_bif_1003_step_3] 6. read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1003_step_3] BIF_DATA_NUM (2) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //                 
        printk("[tc_bif_1003_step_3] 7. read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1003_step_3] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1003_step_3] BIF_ACK_0 (1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1003_step_3] BIF_DATA_0 (screg[15:8]=0CH) : BIF_CON20[7:0]=0x%x\n", reg_val);
        screg_15_8=reg_val;
    
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 15);
        printk("[tc_bif_1003_step_3] BIF_DATA_ERROR_0 (0) : BIF_CON21[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 8);
        printk("[tc_bif_1003_step_3] BIF_ACK_0 (1) : BIF_CON21[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0xFF, 0);
        printk("[tc_bif_1003_step_3] BIF_DATA_0 (screg[7:0]=00H) : BIF_CON21[7:0]=0x%x\n", reg_val);
        screg_7_0=reg_val;
    }
    else
    {
        printk("[tc_bif_1003_step_3] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);
    }
 
    //                
    reset_bif_irq();
}

void tc_bif_1003_step_4(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[4]={0,0,0,0};
    int loop_i=0;
	
	  printk("[tc_bif_1003_step_4]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1003_step_4] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=(0x0100|screg_15_8);
    bif_cmd[2]=(0x0200|screg_7_0);
    bif_cmd[3]=0x00FF;
    set_bif_cmd(bif_cmd,4);
        
    //                    
    printk("[tc_bif_1003_step_4] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x4, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x0, 0x3, 8);
    //                                                              
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1003_step_4] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1003_step_4] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1003_step_4] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    

    check_bat_lost();
     
    //                
    reset_bif_irq(); 
}

void tc_bif_1003_step_5(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[1]={0};
    int loop_i=0;
	
	  printk("[tc_bif_1003_step_5]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1003_step_5] 1. set command sequence\n");
    bif_cmd[0]=0x0411;
    set_bif_cmd(bif_cmd,1);      
        
    //                    
    printk("[tc_bif_1003_step_5] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x2, 0x3, 8);
        
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1003_step_5] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1003_step_5] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1003_step_5] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    

    //                   
    printk("[tc_bif_1003_step_5] 5. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 13);
    printk("[tc_bif_1003_step_5] BIF_BAT_LOST : BIF_CON31[13]=0x%x - battery is detected(0)/battery is undetected(1)\n", reg_val);
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 12);
    printk("[tc_bif_1003_step_5] BIF_TIMEOUT : BIF_CON31[12]=0x%x - positive(0)/negative(1)\n", reg_val);
    ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0x1, 12);
    printk("[tc_bif_1003_step_5] BIF_RESPONSE : BIF_CON19[12]=0x%x - positive(1)/negative(0)\n", reg_val);
     
    //                
    reset_bif_irq();
}

void tc_bif_1003_step_5_positive(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[1]={0};
    int loop_i=0;
	
	printk("[tc_bif_1003_step_5_positive]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1003_step_5_positive] 1. set command sequence\n");
    bif_cmd[0]=0x0411;
    set_bif_cmd(bif_cmd,1);      
        
    //                    
    printk("[tc_bif_1003_step_5_positive] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x2, 0x3, 8);
    
    //   
    ret=pmic_config_interface(MT6325_BIF_CON15,0x7F, 0x7F,0);

    //       
    ret=pmic_config_interface(MT6325_BIF_CON37,0x1FF, 0x1FF,0);

    ret=pmic_config_interface(MT6325_BIF_CON30, 0x1, 0x1, 12);
    ret=pmic_config_interface(MT6325_BIF_CON30, 0x1, 0x1, 4);
        
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x,Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17),
        MT6325_BIF_CON30,upmu_get_reg_value(MT6325_BIF_CON30),
        MT6325_BIF_CON37,upmu_get_reg_value(MT6325_BIF_CON37)
        );
    
    //                     
    printk("[tc_bif_1003_step_5_positive] 5. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();

    //   
    mdelay(10);
    ret=pmic_config_interface(MT6325_BIF_CON30, 0x0, 0x1, 12);
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "BIF_RX_DATA_SW=0 : Reg[0x%x]=0x%x\n",         
        MT6325_BIF_CON30,upmu_get_reg_value(MT6325_BIF_CON30)
        );
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1003_step_5_positive] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                  
    printk("[tc_bif_1003_step_5_positive] disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0); 

    printk("[tc_bif_1003_step_5_positive] disable SW control\n");
    ret=pmic_config_interface(MT6325_BIF_CON30, 0x0, 0x1, 4);

    //                
    printk("[tc_bif_1003_step_5_positive] 8. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 13);
    printk("[tc_bif_1003_step_5_positive] BIF_BAT_LOST : BIF_CON31[13]=0x%x - battery is detected(0)/battery is undetected(1)\n", reg_val);
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 12);
    printk("[tc_bif_1003_step_5_positive] BIF_TIMEOUT : BIF_CON31[12]=0x%x - positive(0)/negative(1)\n", reg_val);
    ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0x1, 12);
    printk("[tc_bif_1003_step_5_positive] BIF_RESPONSE : BIF_CON19[12]=0x%x - positive(1)/negative(0)\n", reg_val);
     
    //                
    reset_bif_irq();
}

void tc_bif_1003_step_5_negative(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[1]={0};
    int loop_i=0;
	
	  printk("[tc_bif_1003_step_5_negative]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1003_step_5_negative] 1. set command sequence\n");
    bif_cmd[0]=0x0411;
    set_bif_cmd(bif_cmd,1);      
        
    //                    
    printk("[tc_bif_1003_step_5_negative] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x2, 0x3, 8);
        
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1003_step_5_negative] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1003_step_5_negative] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1003_step_5_negative] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    

    //                   
    printk("[tc_bif_1003_step_5_negative] 5. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 13);
    printk("[tc_bif_1003_step_5_negative] BIF_BAT_LOST : BIF_CON31[13]=0x%x - battery is detected(0)/battery is undetected(1)\n", reg_val);
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 12);
    printk("[tc_bif_1003_step_5_negative] BIF_TIMEOUT : BIF_CON31[12]=0x%x - positive(0)/negative(1)\n", reg_val);
    ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0x1, 12);
    printk("[tc_bif_1003_step_5_negative] BIF_RESPONSE : BIF_CON19[12]=0x%x - positive(1)/negative(0)\n", reg_val);
     
    //                
    reset_bif_irq();
}
#endif

#if 1 //            
void tc_bif_1004_step_1_positive(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[1]={0};
    int loop_i=0;
	
	  printk("[tc_bif_1004_step_1_positive]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1004_step_1_positive] 1. set command sequence\n");
    bif_cmd[0]=0x0410;    
    set_bif_cmd(bif_cmd,1);
        
    //                    
    printk("[tc_bif_1004_step_1_positive] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x2, 0x3, 8);
        
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1004_step_1_positive] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    //                                                                
    udelay(10);
    bif_debug();
    mdelay(1000);
    printk("[tc_bif_1004_step_1_positive] wait 1s and then\n");

    //   
    ret=pmic_config_interface(MT6325_BIF_CON30, 0x0, 0x1, 12);
    ret=pmic_config_interface(MT6325_BIF_CON30, 0x1, 0x1, 4);
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "after set, Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON30,upmu_get_reg_value(MT6325_BIF_CON30)
        );
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1004_step_1_positive] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1004_step_1_positive] 5. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);   

    printk("[tc_bif_1004_step_1_positive] disable SW control\n");
    ret=pmic_config_interface(MT6325_BIF_CON30, 0x0, 0x1, 4);
    
    //                   
    printk("[tc_bif_1004_step_1_positive] 6. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 13);
    printk("[tc_bif_1004_step_1_positive] BIF_BAT_LOST : BIF_CON31[13]=0x%x - battery is detected(0)/battery is undetected(1)\n", reg_val);
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 12);
    printk("[tc_bif_1004_step_1_positive] BIF_TIMEOUT : BIF_CON31[12]=0x%x - positive(0)/negative(1)\n", reg_val);
    ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0x1, 12);
    printk("[tc_bif_1004_step_1_positive] BIF_RESPONSE : BIF_CON19[12]=0x%x - positive(1)/negative(0)\n", reg_val);
     
    //                
    reset_bif_irq();
}

void tc_bif_1004_step_1_negative(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[1]={0};
    int loop_i=0;
	
	  printk("[tc_bif_1004_step_1_negative]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1004_step_1_negative] 1. set command sequence\n");
    bif_cmd[0]=0x0410;    
    set_bif_cmd(bif_cmd,1);
        
    //                    
    printk("[tc_bif_1004_step_1_negative] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x2, 0x3, 8);
        
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1004_step_1_negative] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    //                                                                
    udelay(10);
    bif_debug();
    mdelay(1000);
    printk("[tc_bif_1004_step_1_negative] wait 1s and then\n");

    //                      
    printk("[tc_bif_1004_step_1_negative] 4. BIF_BACK_NORMAL = 1\n");
    ret=pmic_config_interface(MT6325_BIF_CON31, 0x1, 0x1, 0);
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON31,upmu_get_reg_value(MT6325_BIF_CON31)
        );
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1004_step_1_negative] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }
    
    //                     
    printk("[tc_bif_1004_step_1_negative] 5. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    printk("[tc_bif_1004_step_1_negative] clear rg_back_normal\n");
    ret=pmic_config_interface(MT6325_BIF_CON31, 0x0, 0x1, 0);
 
    //                   
    printk("[tc_bif_1004_step_1_negative] 6. data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 13);
    printk("[tc_bif_1004_step_1_negative] BIF_BAT_LOST : BIF_CON31[13]=0x%x - battery is detected(0)/battery is undetected(1)\n", reg_val);
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 12);
    printk("[tc_bif_1004_step_1_negative] BIF_TIMEOUT : BIF_CON31[12]=0x%x - positive(0)/negative(1)\n", reg_val);
    ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0x1, 12);
    printk("[tc_bif_1004_step_1_negative] BIF_RESPONSE : BIF_CON19[12]=0x%x - positive(1)/negative(0)\n", reg_val);
     
    //                
    reset_bif_irq();
}

#endif

#if 1 //            
void tc_bif_1005_step1(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[11]={0,0,0,0,0,0,0,0,0,0,0};
	
	printk("[tc_bif_1005_step1]-----------------------------------\n");

    printk("[tc_bif_1005_step1] 1. sw control and switch to sw mode\n");
    ret=pmic_config_interface(MT6325_BIF_CON30, 0x1, 0x1, 11);
    ret=pmic_config_interface(MT6325_BIF_CON30, 0x1, 0x1, 6);
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON30,upmu_get_reg_value(MT6325_BIF_CON30)
        );
    
    //                    
    printk("[tc_bif_1005_step1] 2. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x010F;
    bif_cmd[2]=0x0209;
    bif_cmd[3]=0x0000;
    bif_cmd[4]=0x0001;
    bif_cmd[5]=0x0002;
    bif_cmd[6]=0x0003;
    bif_cmd[7]=0x0004;
    bif_cmd[8]=0x0005;
    bif_cmd[9]=0x0006;
    bif_cmd[10]=0x04C2;
    set_bif_cmd(bif_cmd,11);
        
    //                 
    printk("[tc_bif_1005_step1] 3. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0xB, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x0, 0x3, 8);
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x1, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1005_step1] 4. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    mdelay(1000);

    printk("[tc_bif_1005_step1] After 1s\n");
    
    reg_val=0;    
    //                   
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1005_step1] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);
        printk("[tc_bif_1005_step1] if bif_irq == 0, pass\n");
    }
    
    //                  
    printk("[tc_bif_1005_step1] disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    

    //             
    reset_bif_irq();
}
#endif

#if 1 //            
void tc_bif_1006_step_1(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[11]={0,0,0,0,0,0,0,0,0,0,0};
    int loop_i=0;
	
	printk("[tc_bif_1006_step_1]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1006_step_1] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x010F;
    bif_cmd[2]=0x0209;
    bif_cmd[3]=0x0000;
    bif_cmd[4]=0x0001;
    bif_cmd[5]=0x0002;
    bif_cmd[6]=0x0003;
    bif_cmd[7]=0x0004;
    bif_cmd[8]=0x0005;
    bif_cmd[9]=0x0006;
    bif_cmd[10]=0x04C2;
    set_bif_cmd(bif_cmd,11);
        
    //                    
    printk("[tc_bif_1006_step_1] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0xB, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x0, 0x3, 8);
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x1, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1006_step_1] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    mdelay(10);
    printk("[tc_bif_1006_step_1] wait 10ms and then\n");

    ret=pmic_config_interface(MT6325_BIF_CON30, 0x1, 0x1, 11);
    ret=pmic_config_interface(MT6325_BIF_CON30, 0x1, 0x1, 6);    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON30,upmu_get_reg_value(MT6325_BIF_CON30)
        );
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1006_step_1] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }

    printk("[tc_bif_1006_step_1] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

    ret=pmic_config_interface(MT6325_BIF_CON30, 0x0, 0x1, 6);
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "disable sw control : Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON30,upmu_get_reg_value(MT6325_BIF_CON30)
        );
    
    //                  
    printk("[tc_bif_1006_step_1] disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                
    printk("[tc_bif_1006_step_1] data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 12);
    printk("[tc_bif_1006_step_1] BIF_CON31[12]=0x%x, need 0\n", reg_val);    
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 13);
    printk("[tc_bif_1006_step_1] BIF_CON31[13]=0x%x, need 1\n", reg_val);
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 14);
    printk("[tc_bif_1006_step_1] BIF_CON31[14]=0x%x, need 0\n", reg_val);
 
    //             
    reset_bif_irq();
}

void tc_bif_1006_step_2(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[11]={0,0,0,0,0,0,0,0,0,0,0};
    int loop_i=0;
	
	printk("[tc_bif_1006_step_2]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1006_step_2] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x010F;
    bif_cmd[2]=0x0209;
    bif_cmd[3]=0x0000;
    bif_cmd[4]=0x0001;
    bif_cmd[5]=0x0002;
    bif_cmd[6]=0x0003;
    bif_cmd[7]=0x0004;
    bif_cmd[8]=0x0005;
    bif_cmd[9]=0x0006;
    bif_cmd[10]=0x04C2;
    set_bif_cmd(bif_cmd,11);
        
    //                    
    printk("[tc_bif_1006_step_2] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0xB, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x0, 0x3, 8);
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x1, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1006_step_2] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1006_step_2] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }

    printk("[tc_bif_1006_step_2] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);
    
    //                  
    printk("[tc_bif_1006_step_2] disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                
    printk("[tc_bif_1006_step_2] data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1006_step_2] BIF_CON31[14:12]=0x%x\n", reg_val);    
    
    if(reg_val == 0)
    {
        //                                            
        printk("[tc_bif_1006_step_2] read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1006_step_2] BIF_DATA_NUM (1) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //                 
        printk("[tc_bif_1006_step_2] read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1006_step_2] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1006_step_2] BIF_ACK_0 (1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1006_step_2] BIF_DATA_0 (0AH) : BIF_CON20[7:0]=0x%x\n", reg_val);
    }
    else
    {
        printk("[tc_bif_1006_step_2] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);
    }
 
    //             
    reset_bif_irq();
}

void tc_bif_1006_step_3(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[4]={0,0,0,0};
    int loop_i=0;
	
	printk("[tc_bif_1006_step_3]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1006_step_3] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x0427;
    bif_cmd[2]=0x010F;
    bif_cmd[3]=0x0309;
    set_bif_cmd(bif_cmd,4);
        
    //                    
    printk("[tc_bif_1006_step_3] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x4, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x7, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1006_step_3] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
    
    mdelay(10);
    printk("[tc_bif_1006_step_3] wait 10ms and then\n");

    ret=pmic_config_interface(MT6325_BIF_CON30, 0x1, 0x1, 11);
    ret=pmic_config_interface(MT6325_BIF_CON30, 0x1, 0x1, 6);    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON30,upmu_get_reg_value(MT6325_BIF_CON30)
        );
    
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1006_step_3] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }

    printk("[tc_bif_1006_step_3] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

    ret=pmic_config_interface(MT6325_BIF_CON30, 0x0, 0x1, 6);
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "disable sw control : Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON30,upmu_get_reg_value(MT6325_BIF_CON30)
        );
    
    //                  
    printk("[tc_bif_1006_step_3] disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                
    printk("[tc_bif_1006_step_3] data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 12);
    printk("[tc_bif_1006_step_3] BIF_CON31[12]=0x%x, need 0\n", reg_val);    
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 13);
    printk("[tc_bif_1006_step_3] BIF_CON31[13]=0x%x, need 1\n", reg_val);
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 14);
    printk("[tc_bif_1006_step_3] BIF_CON31[14]=0x%x, need 0\n", reg_val);
 
    //             
    reset_bif_irq();
}

void tc_bif_1006_step_4(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int bif_cmd[4]={0,0,0,0};
    int loop_i=0;
	
	printk("[tc_bif_1006_step_4]-----------------------------------\n");
	
    //                       
    printk("[tc_bif_1006_step_4] 1. set command sequence\n");
    bif_cmd[0]=0x0601;
    bif_cmd[1]=0x0427;
    bif_cmd[2]=0x010F;
    bif_cmd[3]=0x0309;
    set_bif_cmd(bif_cmd,4);
        
    //                    
    printk("[tc_bif_1006_step_4] 2. parameter setting\n");
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x4, 0xF, 12);
    ret=pmic_config_interface(MT6325_BIF_CON15, 0x1, 0x3, 8);
    ret=pmic_config_interface(MT6325_BIF_CON17, 0x7, 0xF, 12);    
    
    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON15,upmu_get_reg_value(MT6325_BIF_CON15),
        MT6325_BIF_CON17,upmu_get_reg_value(MT6325_BIF_CON17)
        );
    
    //                     
    printk("[tc_bif_1006_step_4] 3. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);    

    udelay(10);
    bif_debug();
        
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1006_step_4] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

        if(loop_i++ > g_loop_out) break;
    }

    printk("[tc_bif_1006_step_4] polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);

    //                  
    printk("[tc_bif_1006_step_4] disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);    
    
    //                
    printk("[tc_bif_1006_step_4] data valid check\n");
    ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x7, 12);
    printk("[tc_bif_1006_step_4] BIF_CON31[14:12]=0x%x\n", reg_val);    
    
    if(reg_val == 0)
    {
        //                                            
        printk("[tc_bif_1006_step_4] 6. read the number of read back data package\n");
        ret=pmic_read_interface(MT6325_BIF_CON19, &reg_val, 0xF, 0);
        printk("[tc_bif_1006_step_4] BIF_DATA_NUM (7) : BIF_CON19[3:0]=0x%x\n", reg_val);        
        
        //                 
        printk("[tc_bif_1006_step_4] 7. read data back\n");
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 15);
        printk("[tc_bif_1006_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON20[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0x1, 8);
        printk("[tc_bif_1006_step_4] BIF_ACK_0 (1) : BIF_CON20[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON20, &reg_val, 0xFF, 0);
        printk("[tc_bif_1006_step_4] BIF_DATA_0 (00h) : BIF_CON20[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 15);
        printk("[tc_bif_1006_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON21[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0x1, 8);
        printk("[tc_bif_1006_step_4] BIF_ACK_0 (1) : BIF_CON21[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON21, &reg_val, 0xFF, 0);
        printk("[tc_bif_1006_step_4] BIF_DATA_0 (01h) : BIF_CON21[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0x1, 15);
        printk("[tc_bif_1006_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON22[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0x1, 8);
        printk("[tc_bif_1006_step_4] BIF_ACK_0 (1) : BIF_CON22[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON22, &reg_val, 0xFF, 0);
        printk("[tc_bif_1006_step_4] BIF_DATA_0 (02h) : BIF_CON22[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0x1, 15);
        printk("[tc_bif_1006_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON23[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0x1, 8);
        printk("[tc_bif_1006_step_4] BIF_ACK_0 (1) : BIF_CON23[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON23, &reg_val, 0xFF, 0);
        printk("[tc_bif_1006_step_4] BIF_DATA_0 (03h) : BIF_CON23[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON24, &reg_val, 0x1, 15);
        printk("[tc_bif_1006_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON24[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON24, &reg_val, 0x1, 8);
        printk("[tc_bif_1006_step_4] BIF_ACK_0 (1) : BIF_CON24[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON24, &reg_val, 0xFF, 0);
        printk("[tc_bif_1006_step_4] BIF_DATA_0 (04h) : BIF_CON24[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON25, &reg_val, 0x1, 15);
        printk("[tc_bif_1006_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON25[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON25, &reg_val, 0x1, 8);
        printk("[tc_bif_1006_step_4] BIF_ACK_0 (1) : BIF_CON25[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON25, &reg_val, 0xFF, 0);
        printk("[tc_bif_1006_step_4] BIF_DATA_0 (05h) : BIF_CON25[7:0]=0x%x\n", reg_val);
        
        ret=pmic_read_interface(MT6325_BIF_CON26, &reg_val, 0x1, 15);
        printk("[tc_bif_1006_step_4] BIF_DATA_ERROR_0 (0) : BIF_CON26[15]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON26, &reg_val, 0x1, 8);
        printk("[tc_bif_1006_step_4] BIF_ACK_0 (1) : BIF_CON26[8]=0x%x\n", reg_val);
        ret=pmic_read_interface(MT6325_BIF_CON26, &reg_val, 0xFF, 0);
        printk("[tc_bif_1006_step_4] BIF_DATA_0 (06h) : BIF_CON26[7:0]=0x%x\n", reg_val);
    }
    else
    {
        printk("[tc_bif_1006_step_4] Fail : BIF_CON31[14:12]=0x%x\n", reg_val);
    }
 
    //             
    reset_bif_irq();
}
#endif

#if 1
void tc_bif_1008_step_0(void)
{
    U32 ret=0;

    
    printk("[tc_bif_1008_step_0]-----------------------------------\n");

    printk("[tc_bif_1008_step_0] 1. set power up regiser\n");
    ret=pmic_config_interface(MT6325_BIF_CON32, 0x1, 0x1, 15);

    printk("[tc_bif_1008_step_0] 2. trigger BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x1, 0x1, 0);

    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON32,upmu_get_reg_value(MT6325_BIF_CON32),
        MT6325_BIF_CON18,upmu_get_reg_value(MT6325_BIF_CON18)
        );

    udelay(10);
    bif_debug();

    #if 0
    reg_val=0;    
    while(reg_val == 0)
    {
        ret=pmic_read_interface(MT6325_BIF_CON31, &reg_val, 0x1, 11);
        printk("[tc_bif_1008_step_0] 3. polling BIF_IRQ : BIF_CON31[11]=0x%x\n", reg_val);
    }
    #else
    printk("[tc_bif_1008_step_0] wait EINT\n");   
    #endif    
}

void tc_bif_1008_step_1(void)
{
    U32 ret=0;

    
    printk("[tc_bif_1008_step_1]-----------------------------------\n");
    
    printk("[tc_bif_1008_step_1] 4. disable BIF module\n");
    ret=pmic_config_interface(MT6325_BIF_CON18, 0x0, 0x1, 0);

    printk("[tc_bif_1008_step_1] 5. to disable power up mode\n");
    ret=pmic_config_interface(MT6325_BIF_CON32, 0x0, 0x1, 15);

    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        MT6325_BIF_CON32,upmu_get_reg_value(MT6325_BIF_CON32),
        MT6325_BIF_CON18,upmu_get_reg_value(MT6325_BIF_CON18)
        );

    check_bat_lost();

    reset_bif_irq();
}

#endif

void bif_init(void)
{
    //                 
    //                                 
    //                            
    //                            
    //                            
    //                            

    //                    
    mt6325_upmu_set_rg_int_en_bif(1);

    //                
    upmu_set_reg_value(MT6325_TOP_CKPDN_CON2_CLR, 0x0070);

    //                    
    pmic_config_interface(MT6325_BATON_CON0, 0xF, 0xF, 0);

    //           
    mt6325_upmu_set_rg_vbif28_en(1);

    //                 
    pmic_config_interface(0x0ABE,0x8017,0xFFFF,0);

    //                 
    pmic_config_interface(0x0F00,0x0005,0xFFFF,0);
    pmic_config_interface(0x0ABA,0x500F,0xFFFF,0);

    printk("[bif_init] done. [0x%x]=0x%x,[0x%x]=0x%x,[0x%x]=0x%x,[0x%x]=0x%x,[0x%x]=0x%x,[0x%x]=0x%x,[0x%x]=0x%x\n",
        MT6325_INT_CON0, upmu_get_reg_value(MT6325_INT_CON0),
        MT6325_TOP_CKPDN_CON2, upmu_get_reg_value(MT6325_TOP_CKPDN_CON2),
        MT6325_BATON_CON0, upmu_get_reg_value(MT6325_BATON_CON0),
        MT6325_LDO_CON4, upmu_get_reg_value(MT6325_LDO_CON4),
        0x0ABE, upmu_get_reg_value(0x0ABE),
        0x0F00, upmu_get_reg_value(0x0F00),
        0x0ABA, upmu_get_reg_value(0x0ABA)
        );
}

void tc_bif_1000(void)
{
    printk("[tc_bif_1000] run read/write function\n");

    bif_init();

    tc_bif_1000_step_0();
    mdelay(10);
    tc_bif_1000_step_1();    
    tc_bif_1000_step_2();
}

void tc_bif_1001(void)
{
    printk("[tc_bif_1001] run read/write function\n");

    tc_bif_reset_slave();
    mdelay(1);
    
    tc_bif_1001_step_1();//           
    mdelay(500);
    tc_bif_1001_step_1();//           
    mdelay(500);
    tc_bif_1001_step_1();//           
    mdelay(500);
    tc_bif_1001_step_1();//           
    mdelay(500);
    tc_bif_1001_step_1();//           
    mdelay(500);
    
    tc_bif_1001_step_2();//           
    mdelay(500);
    tc_bif_1001_step_2();//           
    mdelay(500);
    tc_bif_1001_step_2();//           
    mdelay(500);
    tc_bif_1001_step_2();//           
    mdelay(500);
    tc_bif_1001_step_2();//           
    mdelay(500);
    
    tc_bif_1001_step_3();//     
    tc_bif_1001_step_4();//                        
    tc_bif_1001_step_5();//                                                  
}

void tc_bif_1002(void)
{
    printk("[tc_bif_1002] run burst write/read function\n");

    tc_bif_reset_slave();
    mdelay(1);
    
    tc_bif_1002_step_1(); //                 
    mdelay(500);
    tc_bif_1002_step_1(); //                 
    mdelay(500);
    tc_bif_1002_step_1(); //                 
    mdelay(500);
    tc_bif_1002_step_1(); //                 
    mdelay(500);
    tc_bif_1002_step_1(); //                 
    mdelay(500);
    
    tc_bif_1002_step_2(); //                 
    mdelay(500);
    tc_bif_1002_step_2(); //                 
    mdelay(500);
    tc_bif_1002_step_2(); //                 
    mdelay(500);
    tc_bif_1002_step_2(); //                 
    mdelay(500);
    tc_bif_1002_step_2(); //                 
    mdelay(500);
    
    tc_bif_1002_step_3(); //                  
    
    tc_bif_1002_step_4(); //                 
    mdelay(500);
    tc_bif_1002_step_4(); //                 
    mdelay(500);
    tc_bif_1002_step_4(); //                 
    mdelay(500);
    tc_bif_1002_step_4(); //                 
    mdelay(500);
    tc_bif_1002_step_4(); //                 
    mdelay(500);
}

void tc_bif_1003(void)
{
    printk("[tc_bif_1003] run bus command (ISTS)\n");

    pmic_config_interface(MT6325_TOP_CKSEL_CON0, 0x4, MT6325_PMIC_RG_BIF_X4_CK_DIVSEL_MASK, MT6325_PMIC_RG_BIF_X4_CK_DIVSEL_SHIFT);
    printk("[RG_BIF_X4_CK_DIVSEL] set done. Reg[0x%x]=0x%x\n",
        MT6325_TOP_CKSEL_CON0, upmu_get_reg_value(MT6325_TOP_CKSEL_CON0)
        );

    pmic_config_interface(MT6325_BIF_CON15,0x7F,0x7F, 0);    
    printk("[RG_BIF_X4_CK_DIVSEL] Reg[0x%x]=0x%x\n",
            MT6325_BIF_CON15, upmu_get_reg_value(MT6325_BIF_CON15)
            );

    tc_bif_reset_slave();
    mdelay(1);
    mdelay(500);
    
    tc_bif_1003_step_2(); //                 
    mdelay(500);
    tc_bif_1003_step_2(); //                 
    mdelay(500);
    tc_bif_1003_step_2(); //                 
    mdelay(500);
    tc_bif_1003_step_2(); //                 
    mdelay(500);
    tc_bif_1003_step_2(); //                 
    mdelay(500);
    
    tc_bif_1003_step_3(); //                 
    mdelay(500);
    tc_bif_1003_step_3(); //                 
    mdelay(500);
    tc_bif_1003_step_3(); //                 
    mdelay(500);
    tc_bif_1003_step_3(); //                 
    mdelay(500);
    tc_bif_1003_step_3(); //                 
    mdelay(500);
    
    tc_bif_1003_step_4(); //                
    
    //                                              
    mdelay(2000);    
    tc_bif_1003_step_5_negative(); //                                          
    mdelay(1000);    
    tc_bif_1003_step_5_positive(); //                                              
}

void tc_bif_1004(void)
{
    printk("[tc_bif_1004] run interrupt mode (EINT)\n");
    
    tc_bif_1004_step_1_positive();
    tc_bif_1004_step_1_negative();
}

void tc_bif_1005(void)
{
    printk("[tc_bif_1005] battery lost\n");

    tc_bif_1005_step1();
}

void tc_bif_1006(void)
{
    printk("[tc_bif_1006] battery lost test : E1 option\n");

    tc_bif_reset_slave();
    mdelay(1);
    tc_bif_1006_step_1();
    tc_bif_1006_step_2();
    tc_bif_1006_step_3();
    tc_bif_1006_step_4();    
}

void tc_bif_1007(void)
{
    printk("[tc_bif_1007] BIF EINT test\n");

    bif_init();

    tc_bif_1000_step_0();
}

void tc_bif_1008(void)
{
    printk("[tc_bif_1008] BIF EINT test\n");

    bif_init();

    tc_bif_1008_step_0();
}

#endif


#ifdef TOP_DVT_ENABLE
static kal_uint32 mt6325_aPMURegDef[783][2]= {  /*                     */
  {0x000A, 0x0000}, /*          */
  {0x000C, 0x0000}, /*          */
  {0x000E, 0x0000}, /*          */
  {0x0010, 0x0000}, /*          */
  {0x0012, 0x0000}, /*           */
  {0x0014, 0x0020}, /*           */
  {0x0016, 0x0000}, /*           */
  {0x0018, 0x17C0}, /*           */
  {0x001A, 0x0000}, /*           */
  {0x001C, 0x0000}, /*           */
  {0x001E, 0x0800}, /*           */
  {0x0020, 0x0000}, /*           */
  {0x0022, 0x0000}, /*           */
  {0x0024, 0x009A}, /*           */
  {0x0026, 0x0000}, /*           */
  {0x0028, 0x0000}, /*           */
  {0x002A, 0x0044}, /*           */
  {0x002C, 0x0020}, /*           */
  {0x002E, 0x0000}, /*              */
  {0x0200, 0x2510}, /*     */
  {0x0202, 0x2510}, /*     */
  {0x0204, 0x0103}, /*       */
  {0x0206, 0x0000}, /*        */
  {0x0208, 0x0000}, /*         */
  {0x020A, 0x0000}, /*         */
  {0x020C, 0x0000}, /*           */
  {0x020E, 0x0000}, /*          */
  {0x0210, 0x0000}, /*          */
  {0x0212, 0x0000}, /*          */
  {0x0214, 0x0000}, /*         */
  {0x0216, 0x0000}, /*         */
  {0x0218, 0x0000}, /*         */
  {0x021A, 0x0000}, /*        */
  {0x021C, 0x0000}, /*         */
  {0x021E, 0x0000}, /*         */
  {0x0220, 0x0000}, /*         */
  {0x0222, 0x003C}, /*        */
  {0x0224, 0x0000}, /*        */
  {0x0226, 0x0000}, /*        */
  {0x0228, 0xCCCC}, /*        */
  {0x022A, 0xCCCC}, /*        */
  {0x022C, 0xCCCC}, /*        */
  {0x022E, 0x0CCC}, /*        */
  {0x0230, 0x0000}, /*          */
  {0x0232, 0x0000}, /*              */
  {0x0234, 0x0000}, /*              */
  {0x0236, 0x0000}, /*           */
  {0x0238, 0xEEFC}, /*              */
  {0x023A, 0x0000}, /*                  */
  {0x023C, 0x0000}, /*                  */
  {0x023E, 0x00A0}, /*              */
  {0x0240, 0x0000}, /*                  */
  {0x0242, 0x0000}, /*                  */
  {0x0244, 0x177F}, /*              */
  {0x0246, 0x0000}, /*                  */
  {0x0248, 0x0000}, /*                  */
  {0x024A, 0x05E0}, /*              */
  {0x024C, 0x0000}, /*                  */
  {0x024E, 0x0000}, /*                  */
  {0x0250, 0x0000}, /*              */
  {0x0252, 0x0000}, /*                  */
  {0x0254, 0x0000}, /*                  */
  {0x0256, 0x0000}, /*              */
  {0x0258, 0x0000}, /*                  */
  {0x025A, 0x0000}, /*                  */
  {0x025C, 0x0000}, /*              */
  {0x0262, 0x0401}, /*                */
  {0x0268, 0x1AFF}, /*              */
  {0x026A, 0x0000}, /*                  */
  {0x026C, 0x0000}, /*                  */
  {0x026E, 0x0000}, /*              */
  {0x0270, 0x0000}, /*              */
  {0x0272, 0x0000}, /*              */
  {0x0274, 0x0000}, /*         */
  {0x0276, 0x0000}, /*             */
  {0x0278, 0x0000}, /*             */
  {0x027A, 0x0002}, /*             */
  {0x027C, 0x0000}, /*                 */
  {0x027E, 0x0000}, /*                 */
  {0x0280, 0x0065}, /*            */
  {0x0282, 0x0000}, /*            */
  {0x0284, 0x0000}, /*                */
  {0x0286, 0x0000}, /*                */
  {0x0288, 0x0000}, /*            */
  {0x028A, 0x0300}, /*            */
  {0x028C, 0x0000}, /*                */
  {0x028E, 0x0000}, /*                */
  {0x0290, 0x0000}, /*              */
  {0x0292, 0x0000}, /*                  */
  {0x0294, 0x0000}, /*                  */
  {0x0296, 0x0015}, /*        */
  {0x0298, 0x0000}, /*            */
  {0x029A, 0x0000}, /*            */
  {0x029C, 0x0080}, /*        */
  {0x029E, 0x0000}, /*            */
  {0x02A0, 0x0000}, /*            */
  {0x02A2, 0x002F}, /*        */
  {0x02A4, 0x0000}, /*            */
  {0x02A6, 0x0000}, /*            */
  {0x02A8, 0x0000}, /*            */
  {0x02AA, 0x0000}, /*                */
  {0x02AC, 0x0000}, /*                */
  {0x02AE, 0x0000}, /*           */
  {0x02B0, 0x0000}, /*           */
  {0x02B2, 0x0000}, /*           */
  {0x02B4, 0x0001}, /*         */
  {0x02B6, 0x0000}, /*          */
  {0x02B8, 0x0000}, /*          */
  {0x02BA, 0x0000}, /*          */
  {0x02BC, 0x0000}, /*          */
  {0x02BE, 0x0000}, /*          */
  {0x02C0, 0x5AA5}, /*             */
  {0x02C2, 0x0000}, /*              */
  {0x02C4, 0x0000}, /*             */
  {0x02C6, 0x0000}, /*          */
  {0x02C8, 0x0083}, /*           */
  {0x02CA, 0x0000}, /*               */
  {0x02CC, 0x0000}, /*                  */
  {0x02CE, 0x0000}, /*                 */
  {0x02D0, 0x0000}, /*             */
  {0x02D2, 0x0000}, /*              */
  {0x02D4, 0x0000}, /*               */
  {0x02D6, 0x0000}, /*                */
  {0x02D8, 0x000F}, /*            */
  {0x02DA, 0x0000}, /*             */
  {0x02E0, 0x0000}, /*             */
  {0x02E6, 0x0000}, /*             */
  {0x02EC, 0x0000}, /*       */
  {0x0400, 0x0000}, /*             */
  {0x0402, 0xB100}, /*             */
  {0x0404, 0x110A}, /*             */
  {0x0406, 0xC10B}, /*             */
  {0x0408, 0x0000}, /*             */
  {0x040A, 0x0000}, /*             */
  {0x040C, 0x0000}, /*             */
  {0x040E, 0x0000}, /*             */
  {0x0410, 0x0000}, /*             */
  {0x0412, 0x000A}, /*             */
  {0x0414, 0x000A}, /*              */
  {0x0416, 0x000A}, /*              */
  {0x0418, 0x000A}, /*              */
  {0x041A, 0x000A}, /*              */
  {0x041C, 0x000A}, /*              */
  {0x041E, 0x000A}, /*              */
  {0x0420, 0x000A}, /*              */
  {0x0422, 0x000A}, /*              */
  {0x0424, 0x0000}, /*              */
  {0x0426, 0x0000}, /*              */
  {0x0428, 0x0000}, /*              */
  {0x042A, 0x01FF}, /*              */
  {0x042C, 0x0000}, /*              */
  {0x042E, 0x0000}, /*              */
  {0x0430, 0x0000}, /*              */
  {0x0432, 0x0000}, /*              */
  {0x0434, 0x0000}, /*              */
  {0x0436, 0x0000}, /*              */
  {0x0438, 0x0000}, /*              */
  {0x043A, 0x1482}, /*              */
  {0x043C, 0x01AC}, /*              */
  {0x043E, 0x7801}, /*              */
  {0x0440, 0x805C}, /*              */
  {0x0442, 0x0000}, /*              */
  {0x0444, 0x1482}, /*               */
  {0x0446, 0x01AC}, /*               */
  {0x0448, 0x7801}, /*               */
  {0x044A, 0x805C}, /*               */
  {0x044C, 0x0000}, /*               */
  {0x044E, 0x0000}, /*                 */
  {0x0450, 0x0000}, /*                 */
  {0x0452, 0x0000}, /*                 */
  {0x0454, 0x0000}, /*                 */
  {0x0456, 0x0000}, /*                 */
  {0x0458, 0x0000}, /*                 */
  {0x045A, 0x0000}, /*                 */
  {0x045C, 0x0000}, /*                 */
  {0x045E, 0x0000}, /*                 */
  {0x0460, 0x0000}, /*                 */
  {0x0462, 0x9102}, /*               */
  {0x0464, 0xC6ED}, /*               */
  {0x0466, 0x3D2C}, /*               */
  {0x0468, 0x03FF}, /*               */
  {0x046A, 0x03FF}, /*               */
  {0x046C, 0xF0C0}, /*               */
  {0x046E, 0x00F0}, /*               */
  {0x0470, 0x0000}, /*               */
  {0x0472, 0x14C2}, /*             */
  {0x0474, 0x01AC}, /*             */
  {0x0476, 0x7801}, /*             */
  {0x0478, 0x805C}, /*             */
  {0x047A, 0x0000}, /*             */
  {0x047C, 0x0000}, /*            */
  {0x047E, 0x8000}, /*            */
  {0x0480, 0x0002}, /*            */
  {0x0482, 0x0000}, /*            */
  {0x0484, 0x1482}, /*               */
  {0x0486, 0x01AC}, /*               */
  {0x0488, 0x7801}, /*               */
  {0x048A, 0x805C}, /*               */
  {0x048C, 0x0000}, /*               */
  {0x048E, 0x0C12}, /*              */
  {0x0490, 0x041C}, /*              */
  {0x0492, 0x3C00}, /*              */
  {0x0494, 0x1280}, /*              */
  {0x0496, 0x0020}, /*              */
  {0x0498, 0x145A}, /*                */
  {0x049A, 0x002C}, /*                */
  {0x049C, 0x7801}, /*                */
  {0x049E, 0x805A}, /*                */
  {0x04A0, 0x0000}, /*                */
  {0x04A2, 0x0000}, /*            */
  {0x04B0, 0x0008}, /*            */
  {0x04B2, 0x3000}, /*            */
  {0x04B4, 0x3021}, /*            */
  {0x04B6, 0x8888}, /*             */
  {0x04B8, 0x0040}, /*             */
  {0x04BA, 0x0040}, /*             */
  {0x04BC, 0x0040}, /*             */
  {0x04BE, 0x0040}, /*             */
  {0x04C6, 0x0001}, /*             */
  {0x04C8, 0x0000}, /*            */
  {0x04D6, 0x0008}, /*            */
  {0x04D8, 0x3000}, /*            */
  {0x04DA, 0x3021}, /*            */
  {0x04DC, 0x8888}, /*             */
  {0x04DE, 0x0040}, /*             */
  {0x04E0, 0x0040}, /*             */
  {0x04E2, 0x0040}, /*             */
  {0x04E4, 0x0040}, /*             */
  {0x04EC, 0x0001}, /*             */
  {0x04EE, 0x0000}, /*                */
  {0x04FC, 0x0008}, /*                */
  {0x04FE, 0x3000}, /*                */
  {0x0500, 0x3021}, /*                */
  {0x0502, 0x8888}, /*                 */
  {0x0504, 0x0042}, /*                 */
  {0x0506, 0x0042}, /*                 */
  {0x0508, 0x0042}, /*                 */
  {0x050A, 0x0042}, /*                 */
  {0x0512, 0x0001}, /*                 */
  {0x0514, 0x0000}, /*          */
  {0x0522, 0x0000}, /*          */
  {0x0524, 0x0000}, /*          */
  {0x0526, 0x3021}, /*          */
  {0x0528, 0x8888}, /*           */
  {0x052A, 0x0063}, /*           */
  {0x052C, 0x0063}, /*           */
  {0x052E, 0x0063}, /*           */
  {0x0530, 0x0063}, /*           */
  {0x0532, 0x3333}, /*           */
  {0x0538, 0x0001}, /*           */
  {0x053A, 0x0000}, /*            */
  {0x0548, 0x0000}, /*            */
  {0x054A, 0x0000}, /*            */
  {0x054C, 0x1020}, /*            */
  {0x054E, 0x8888}, /*             */
  {0x0550, 0x0032}, /*             */
  {0x0552, 0x0032}, /*             */
  {0x0554, 0x0032}, /*             */
  {0x0556, 0x0032}, /*             */
  {0x0558, 0x3333}, /*             */
  {0x055E, 0x0001}, /*             */
  {0x0600, 0x0000}, /*         */
  {0x060E, 0x0000}, /*         */
  {0x0610, 0x0000}, /*         */
  {0x0612, 0x3021}, /*         */
  {0x0614, 0x8888}, /*          */
  {0x0616, 0x0040}, /*          */
  {0x0618, 0x0040}, /*          */
  {0x061A, 0x0040}, /*          */
  {0x061C, 0x0040}, /*          */
  {0x061E, 0x3333}, /*          */
  {0x0620, 0x3333}, /*          */
  {0x0622, 0x3333}, /*          */
  {0x0624, 0x0001}, /*          */
  {0x0626, 0x0000}, /*           */
  {0x0634, 0x0000}, /*           */
  {0x0636, 0x0000}, /*           */
  {0x0638, 0x3021}, /*           */
  {0x063A, 0x8888}, /*            */
  {0x063C, 0x0040}, /*            */
  {0x063E, 0x0040}, /*            */
  {0x0640, 0x0040}, /*            */
  {0x0642, 0x0040}, /*            */
  {0x0644, 0x3333}, /*            */
  {0x0646, 0x3333}, /*            */
  {0x0648, 0x3333}, /*            */
  {0x064A, 0x0001}, /*            */
  {0x064C, 0x0000}, /*           */
  {0x065A, 0x0000}, /*           */
  {0x065C, 0x0000}, /*           */
  {0x065E, 0x3021}, /*           */
  {0x0660, 0x8888}, /*            */
  {0x0662, 0x0040}, /*            */
  {0x0664, 0x0040}, /*            */
  {0x0666, 0x0040}, /*            */
  {0x0668, 0x0040}, /*            */
  {0x066A, 0x3333}, /*            */
  {0x066C, 0x3333}, /*            */
  {0x066E, 0x3333}, /*            */
  {0x0670, 0x0001}, /*            */
  {0x0672, 0x0030}, /*            */
  {0x0674, 0x0000}, /*            */
  {0x0676, 0x0000}, /*          */
  {0x0684, 0x0000}, /*          */
  {0x0686, 0x0000}, /*          */
  {0x0688, 0x3021}, /*          */
  {0x068A, 0x8888}, /*           */
  {0x068C, 0x0030}, /*           */
  {0x068E, 0x0030}, /*           */
  {0x0690, 0x0030}, /*           */
  {0x0692, 0x0030}, /*           */
  {0x0694, 0x3333}, /*           */
  {0x0696, 0x3333}, /*           */
  {0x0698, 0x3333}, /*           */
  {0x069A, 0x0001}, /*           */
  {0x069C, 0x0000}, /*        */
  {0x06AA, 0x0000}, /*        */
  {0x06AC, 0x0000}, /*        */
  {0x06AE, 0x0020}, /*        */
  {0x06B0, 0x8888}, /*         */
  {0x06B2, 0x0000}, /*         */
  {0x06B4, 0x0000}, /*         */
  {0x06B6, 0x0000}, /*         */
  {0x06B8, 0x0000}, /*         */
  {0x06BA, 0x0000}, /*         */
  {0x06BC, 0x0000}, /*         */
  {0x06BE, 0xFF00}, /*         */
  {0x06C0, 0xCC01}, /*         */
  {0x06C2, 0x0000}, /*         */
  {0x06C4, 0x0000}, /*         */
  {0x06C6, 0x2E14}, /*         */
  {0x06C8, 0x0E01}, /*         */
  {0x06CA, 0xF001}, /*         */
  {0x06CC, 0x0000}, /*           */
  {0x06CE, 0x0000}, /*           */
  {0x06D0, 0x0000}, /*           */
  {0x06D2, 0x016F}, /*           */
  {0x0800, 0x0000}, /*        */
  {0x0802, 0x0F9F}, /*        */
  {0x0804, 0x0F9F}, /*        */
  {0x0806, 0x001F}, /*        */
  {0x0808, 0x0707}, /*        */
  {0x080A, 0x3F3F}, /*        */
  {0x080C, 0x0000}, /*           */
  {0x080E, 0x0000}, /*           */
  {0x0810, 0x0000}, /*           */
  {0x0812, 0x0000}, /*           */
  {0x0814, 0x0000}, /*           */
  {0x0816, 0x0000}, /*           */
  {0x0818, 0x0000}, /*           */
  {0x081A, 0x0000}, /*           */
  {0x081C, 0x0000}, /*           */
  {0x081E, 0x0000}, /*           */
  {0x0820, 0x0000}, /*           */
  {0x0822, 0x0000}, /*           */
  {0x0824, 0x0000}, /*           */
  {0x0826, 0x0000}, /*           */
  {0x0828, 0x0000}, /*           */
  {0x082A, 0x0000}, /*           */
  {0x082C, 0x0000}, /*          */
  {0x082E, 0x0000}, /*          */
  {0x0830, 0x0000}, /*               */
  {0x0832, 0x0000}, /*           */
  {0x0834, 0x0000}, /*             */
  {0x0836, 0x0000}, /*               */
  {0x0838, 0x0000}, /*              */
  {0x0A00, 0x8102}, /*        */
  {0x0A02, 0x2120}, /*        */
  {0x0A04, 0x8102}, /*        */
  {0x0A06, 0x8102}, /*        */
  {0x0A08, 0x0100}, /*        */
  {0x0A0A, 0x0100}, /*        */
  {0x0A0C, 0x0100}, /*        */
  {0x0A0E, 0x0100}, /*        */
  {0x0A10, 0x0100}, /*        */
  {0x0A12, 0x8102}, /*        */
  {0x0A14, 0x8102}, /*         */
  {0x0A16, 0x8502}, /*         */
  {0x0A18, 0x8102}, /*         */
  {0x0A1A, 0x8122}, /*         */
  {0x0A1C, 0x0120}, /*         */
  {0x0A1E, 0x0100}, /*         */
  {0x0A20, 0x8102}, /*         */
  {0x0A22, 0x0100}, /*         */
  {0x0A24, 0x0100}, /*         */
  {0x0A26, 0x0100}, /*         */
  {0x0A28, 0x0900}, /*         */
  {0x0A2A, 0x0100}, /*         */
  {0x0A2C, 0x6706}, /*         */
  {0x0A2E, 0x0100}, /*         */
  {0x0A30, 0x0100}, /*         */
  {0x0A32, 0x0100}, /*         */
  {0x0A34, 0x8102}, /*         */
  {0x0A36, 0x0100}, /*         */
  {0x0A38, 0x8102}, /*         */
  {0x0A3A, 0x8002}, /*         */
  {0x0A3C, 0x0061}, /*         */
  {0x0A3E, 0x0101}, /*         */
  {0x0A40, 0x0101}, /*         */
  {0x0A42, 0x0107}, /*         */
  {0x0A44, 0x0101}, /*         */
  {0x0A46, 0xF301}, /*         */
  {0x0A48, 0x0B03}, /*         */
  {0x0A4A, 0x0730}, /*         */
  {0x0A4C, 0x010B}, /*         */
  {0x0A4E, 0x0B01}, /*         */
  {0x0A50, 0x0505}, /*         */
  {0x0A52, 0x070A}, /*          */
  {0x0A54, 0x0707}, /*          */
  {0x0A56, 0x0709}, /*          */
  {0x0A58, 0x8401}, /*          */
  {0x0A5A, 0x0501}, /*          */
  {0x0A5C, 0x0000}, /*          */
  {0x0A5E, 0xF800}, /*        */
  {0x0A60, 0xF800}, /*        */
  {0x0A62, 0x7000}, /*        */
  {0x0A64, 0xF800}, /*        */
  {0x0A66, 0x0000}, /*         */
  {0x0A68, 0x0000}, /*         */
  {0x0A6A, 0x0000}, /*         */
  {0x0A6C, 0x0000}, /*         */
  {0x0A6E, 0x0000}, /*            */
  {0x0A70, 0x0000}, /*        */
  {0x0A72, 0x0000}, /*        */
  {0x0A74, 0x0000}, /*        */
  {0x0A76, 0x0000}, /*        */
  {0x0A78, 0x0000}, /*        */
  {0x0A7A, 0x0000}, /*        */
  {0x0A7C, 0x0000}, /*        */
  {0x0A7E, 0x0000}, /*        */
  {0x0A80, 0x0000}, /*        */
  {0x0A82, 0x0000}, /*        */
  {0x0A84, 0x0000}, /*         */
  {0x0A86, 0x0000}, /*         */
  {0x0A88, 0x0000}, /*         */
  {0x0A8A, 0x0000}, /*         */
  {0x0A8C, 0x0000}, /*         */
  {0x0A8E, 0x4000}, /*         */
  {0x0A90, 0x1431}, /*         */
  {0x0A92, 0x1011}, /*         */
  {0x0A94, 0x0000}, /*         */
  {0x0A96, 0x0000}, /*         */
  {0x0A98, 0x0100}, /*         */
  {0x0A9A, 0x0100}, /*         */
  {0x0A9C, 0x0100}, /*         */
  {0x0A9E, 0x0100}, /*         */
  {0x0AA0, 0x0100}, /*         */
  {0x0AA2, 0x0100}, /*         */
  {0x0AA4, 0x0100}, /*         */
  {0x0AA6, 0x0100}, /*         */
  {0x0AA8, 0x0100}, /*         */
  {0x0AAA, 0x0100}, /*         */
  {0x0AAC, 0x0000}, /*         */
  {0x0AAE, 0x0000}, /*         */
  {0x0AB0, 0x0010}, /*         */
  {0x0AB2, 0x0000}, /*         */
  {0x0AB4, 0x0000}, /*         */
  {0x0AB6, 0x0000}, /*         */
  {0x0AB8, 0x0000}, /*         */
  {0x0ABA, 0x0000}, /*          */
  {0x0ABC, 0x000B}, /*         */
  {0x0ABE, 0x8022}, /*         */
  {0x0AC0, 0x0000}, /*         */
  {0x0AC2, 0x0000}, /*        */
  {0x0AC4, 0x0000}, /*        */
  {0x0AC6, 0x0014}, /*        */
  {0x0AC8, 0x0000}, /*        */
  {0x0ACA, 0x0000}, /*        */
  {0x0ACC, 0x0094}, /*        */
  {0x0ACE, 0x0000}, /*        */
  {0x0AD0, 0x4531}, /*        */
  {0x0AD2, 0x0000}, /*        */
  {0x0AD4, 0x2000}, /*        */
  {0x0AD6, 0x0000}, /*         */
  {0x0AD8, 0x0000}, /*         */
  {0x0ADA, 0x0000}, /*         */
  {0x0ADC, 0x0000}, /*         */
  {0x0ADE, 0x0000}, /*         */
  {0x0AE0, 0x0000}, /*         */
  {0x0AE2, 0x0000}, /*         */
  {0x0AE4, 0x0000}, /*            */
  {0x0AE6, 0x0000}, /*            */
  {0x0AE8, 0x0000}, /*            */
  {0x0C00, 0x0000}, /*        */
  {0x0C02, 0x0000}, /*        */
  {0x0C04, 0x0000}, /*        */
  {0x0C06, 0x0000}, /*        */
  {0x0C08, 0x0000}, /*        */
  {0x0C0A, 0x0000}, /*        */
  {0x0C0C, 0x0000}, /*        */
  {0x0C0E, 0x0000}, /*        */
  {0x0C10, 0x0000}, /*        */
  {0x0C12, 0x0000}, /*        */
  {0x0C14, 0x0000}, /*         */
  {0x0C16, 0x0000}, /*         */
  {0x0C18, 0x0000}, /*         */
  {0x0C1A, 0x0000}, /*         */
  {0x0C1C, 0x0000}, /*         */
  {0x0C1E, 0x0000}, /*             */
  {0x0C20, 0x0000}, /*              */
  {0x0C22, 0x0000}, /*              */
  {0x0C24, 0x0000}, /*              */
  {0x0C26, 0x0000}, /*              */
  {0x0C28, 0x0000}, /*              */
  {0x0C2A, 0x0000}, /*               */
  {0x0C2C, 0x0000}, /*                */
  {0x0C2E, 0x0000}, /*                */
  {0x0C30, 0x0000}, /*                */
  {0x0C32, 0x0000}, /*                */
  {0x0C34, 0x0000}, /*                */
  {0x0C36, 0x0000}, /*                */
  {0x0C38, 0x0000}, /*                */
  {0x0C3A, 0x0000}, /*                */
  {0x0C3C, 0x0000}, /*                */
  {0x0C3E, 0x0000}, /*                */
  {0x0C40, 0x0000}, /*                */
  {0x0C42, 0x0000}, /*                */
  {0x0C44, 0x0000}, /*                */
  {0x0C46, 0x0000}, /*                */
  {0x0C48, 0x0000}, /*                */
  {0x0C4A, 0x0000}, /*                */
  {0x0C4C, 0x0000}, /*                */
  {0x0C4E, 0x0000}, /*                */
  {0x0C50, 0x0000}, /*                */
  {0x0C52, 0x0000}, /*                */
  {0x0C54, 0x0000}, /*                */
  {0x0C56, 0x0000}, /*                */
  {0x0C58, 0x0000}, /*                */
  {0x0C5A, 0x0000}, /*                */
  {0x0C5C, 0x0000}, /*                */
  {0x0C5E, 0x0001}, /*            */
  {0x0C60, 0x0002}, /*             */
  {0x0C62, 0x0004}, /*             */
  {0x0C64, 0x0008}, /*             */
  {0x0C66, 0x0010}, /*             */
  {0x0C68, 0x6000}, /*             */
  {0x0C6A, 0x000C}, /*              */
  {0x0C6C, 0x0000}, /*               */
  {0x0C6E, 0x0000}, /*               */
  {0x0C70, 0x0000}, /*               */
  {0x0C72, 0x0000}, /*               */
  {0x0C74, 0x0800}, /*               */
  {0x0C76, 0x0001}, /*               */
  {0x0C78, 0x0000}, /*               */
  {0x0C7A, 0x2800}, /*               */
  {0x0C7C, 0x0000}, /*               */
  {0x0C7E, 0x0000}, /*               */
  {0x0C80, 0x0000}, /*               */
  {0x0C82, 0x0000}, /*               */
  {0x0C84, 0x0000}, /*               */
  {0x0C86, 0x0000}, /*               */
  {0x0C88, 0x0000}, /*               */
  {0x0C8A, 0x0000}, /*               */
  {0x0C8C, 0x0000}, /*               */
  {0x0C8E, 0x0000}, /*               */
  {0x0C90, 0x0000}, /*               */
  {0x0C92, 0x0000}, /*               */
  {0x0C94, 0x0000}, /*               */
  {0x0C96, 0x0000}, /*               */
  {0x0C98, 0x0000}, /*               */
  {0x0C9A, 0x0000}, /*               */
  {0x0C9C, 0x0000}, /*               */
  {0x0C9E, 0x0788}, /*            */
  {0x0CA0, 0x0000}, /*            */
  {0x0CA2, 0x0200}, /*            */
  {0x0CA4, 0x0020}, /*          */
  {0x0CA6, 0x0000}, /*          */
  {0x0CA8, 0x0000}, /*          */
  {0x0CAA, 0x0000}, /*          */
  {0x0CAC, 0x0000}, /*          */
  {0x0CAE, 0x0000}, /*          */
  {0x0CB0, 0x0000}, /*          */
  {0x0CB2, 0x0000}, /*          */
  {0x0CB4, 0x0000}, /*          */
  {0x0CB6, 0x0000}, /*          */
  {0x0CB8, 0x0000}, /*           */
  {0x0CBA, 0x000B}, /*           */
  {0x0CBC, 0x0000}, /*           */
  {0x0CBE, 0x0000}, /*           */
  {0x0CC0, 0x0000}, /*           */
  {0x0CC2, 0x0000}, /*           */
  {0x0CC4, 0x0000}, /*           */
  {0x0CC6, 0x0000}, /*           */
  {0x0CC8, 0x0000}, /*           */
  {0x0CCA, 0x0000}, /*           */
  {0x0CCC, 0x0000}, /*           */
  {0x0CCE, 0x0000}, /*           */
  {0x0CD0, 0x0400}, /*           */
  {0x0CD2, 0x0000}, /*           */
  {0x0CD4, 0x0000}, /*           */
  {0x0CD6, 0x001D}, /*           */
  {0x0CD8, 0x0000}, /*           */
  {0x0CDA, 0x0000}, /*           */
  {0x0CDC, 0x0000}, /*              */
  {0x0CDE, 0x0000}, /*               */
  {0x0CE0, 0x0000}, /*               */
  {0x0CE2, 0x0001}, /*               */
  {0x0CE4, 0x0000}, /*               */
  {0x0CE6, 0x0000}, /*               */
  {0x0CE8, 0x5520}, /*               */
  {0x0CEA, 0x0001}, /*               */
  {0x0CEC, 0x0000}, /*               */
  {0x0CEE, 0x0000}, /*               */
  {0x0CF0, 0x0000}, /*               */
  {0x0CF2, 0x0000}, /*               */
  {0x0CF4, 0x0000}, /*               */
  {0x0CF6, 0x0000}, /*               */
  {0x0CF8, 0x0800}, /*               */
  {0x0CFA, 0x0000}, /*               */
  {0x0CFC, 0x1515}, /*               */
  {0x0CFE, 0x0000}, /*               */
  {0x0D00, 0x0004}, /*               */
  {0x0D02, 0x0000}, /*               */
  {0x0D04, 0x0000}, /*                */
  {0x0D06, 0x0310}, /*                */
  {0x0D08, 0x0180}, /*                */
  {0x0D0A, 0x0013}, /*                */
  {0x0D0C, 0x0000}, /*                */
  {0x0D0E, 0x0000}, /*                  */
  {0x0D10, 0x0000}, /*                  */
  {0x0D12, 0x0020}, /*                  */
  {0x0D14, 0x0001}, /*                  */
  {0x0D16, 0x0000}, /*                  */
  {0x0E00, 0x0000}, /*           */
  {0x0E02, 0x0000}, /*           */
  {0x0E04, 0x0000}, /*           */
  {0x0E06, 0x00FF}, /*            */
  {0x0E08, 0x0000}, /*                */
  {0x0E0A, 0x0000}, /*                */
  {0x0E0C, 0x0000}, /*            */
  {0x0E0E, 0x0000}, /*                */
  {0x0E10, 0x0000}, /*                */
  {0x0E12, 0x8014}, /*          */
  {0x0E14, 0xFC00}, /*           */
  {0x0E16, 0x0000}, /*           */
  {0x0E18, 0x0000}, /*           */
  {0x0E1A, 0x0000}, /*           */
  {0x0E1C, 0x8000}, /*           */
  {0x0E1E, 0x8000}, /*           */
  {0x0E20, 0x0000}, /*           */
  {0x0E22, 0x0000}, /*           */
  {0x0E24, 0x01E0}, /*           */
  {0x0E26, 0x0003}, /*               */
  {0x0E28, 0x0003}, /*           */
  {0x0E2A, 0x0000}, /*                */
  {0x0E2C, 0x0000}, /*                */
  {0x0E2E, 0x0000}, /*                */
  {0x0E30, 0x8000}, /*                */
  {0x0E32, 0x8000}, /*                */
  {0x0E34, 0x0000}, /*                */
  {0x0E36, 0x0000}, /*                */
  {0x0E38, 0x0000}, /*              */
  {0x0E3A, 0x0000}, /*              */
  {0x0E3C, 0x0000}, /*              */
  {0x0E3E, 0x8000}, /*              */
  {0x0E40, 0x8000}, /*              */
  {0x0E42, 0x0000}, /*              */
  {0x0E44, 0x0000}, /*              */
  {0x0E46, 0x0000}, /*           */
  {0x0E48, 0x0000}, /*           */
  {0x0E4A, 0x0000}, /*           */
  {0x0E4C, 0x0000}, /*           */
  {0x0E4E, 0x0000}, /*           */
  {0x0E50, 0x0000}, /*           */
  {0x0E52, 0x0000}, /*           */
  {0x0E54, 0x0000}, /*           */
  {0x0E56, 0x0000}, /*           */
  {0x0E58, 0x0000}, /*           */
  {0x0E5A, 0x0000}, /*            */
  {0x0E5C, 0x0000}, /*            */
  {0x0E5E, 0x0000}, /*            */
  {0x0E60, 0x0000}, /*            */
  {0x0E62, 0x0000}, /*            */
  {0x0E64, 0x0000}, /*            */
  {0x0E66, 0x0000}, /*            */
  {0x0E68, 0x0000}, /*            */
  {0x0E6A, 0x0000}, /*            */
  {0x0E6C, 0x0000}, /*            */
  {0x0E6E, 0x0000}, /*            */
  {0x0E70, 0x0000}, /*            */
  {0x0E72, 0x0000}, /*            */
  {0x0E74, 0x0000}, /*            */
  {0x0E76, 0x0000}, /*            */
  {0x0E78, 0x0000}, /*            */
  {0x0E7A, 0x0000}, /*            */
  {0x0E7C, 0x0000}, /*            */
  {0x0E7E, 0x0000}, /*            */
  {0x0E80, 0x0000}, /*            */
  {0x0E82, 0x0000}, /*            */
  {0x0E84, 0x0000}, /*           */
  {0x0E86, 0x0000}, /*           */
  {0x0E88, 0x0000}, /*           */
  {0x0E8A, 0x0000}, /*           */
  {0x0E8C, 0x0000}, /*           */
  {0x0E8E, 0x0000}, /*           */
  {0x0E90, 0x0000}, /*           */
  {0x0E92, 0x8000}, /*           */
  {0x0E94, 0x8000}, /*           */
  {0x0E96, 0x0000}, /*           */
  {0x0E98, 0x0000}, /*           */
  {0x0E9A, 0x0020}, /*           */
  {0x0E9C, 0x0042}, /*            */
  {0x0E9E, 0x0000}, /*            */
  {0x0EA0, 0x0000}, /*            */
  {0x0EA2, 0x0000}, /*            */
  {0x0EA4, 0x0000}, /*            */
  {0x0EA6, 0x0000}, /*            */
  {0x0EA8, 0x0000}, /*            */
  {0x0EAA, 0x0000}, /*            */
  {0x0EAC, 0x0020}, /*            */
  {0x0EAE, 0x0000}, /*            */
  {0x0EB0, 0x0000}, /*            */
  {0x0EB2, 0x0000}, /*            */
  {0x0EB4, 0x0000}, /*            */
  {0x0EB6, 0x0000}, /*            */
  {0x0EB8, 0x0012}, /*            */
  {0x0EBA, 0x0000}, /*            */
  {0x0EBC, 0x0000}, /*            */
  {0x0EBE, 0x0000}, /*            */
  {0x0EC0, 0x0010}, /*           */
  {0x0EC2, 0x0000}, /*           */
  {0x0EC4, 0x0000}, /*           */
  {0x0EC6, 0x0000}, /*           */
  {0x0EC8, 0x0000}, /*           */
  {0x0ECA, 0x0101}, /*           */
  {0x0ECC, 0x0010}, /*           */
  {0x0ECE, 0x0010}, /*           */
  {0x0ED0, 0x0010}, /*           */
  {0x0ED2, 0x0010}, /*           */
  {0x0ED4, 0x0005}, /*            */
  {0x0ED6, 0x0777}, /*            */
  {0x0ED8, 0x0000}, /*            */
  {0x0EDA, 0x0000}, /*            */
  {0x0EDC, 0x00FF}, /*            */
  {0x0EDE, 0x0061}, /*            */
  {0x0EE0, 0x0161}, /*            */
  {0x0EE2, 0x0000}, /*            */
  {0x0EE4, 0x0000}, /*            */
  {0x0EE6, 0x0700}, /*            */
  {0x0EE8, 0x0000}, /*            */
  {0x0EEA, 0x0010}, /*            */
  {0x0EEC, 0x03FF}, /*            */
  {0x0EEE, 0x0000}, /*            */
  {0x0EF0, 0x0000}, /*            */
  {0x0EF4, 0x00F2}, /*        */
  {0x0EF6, 0x0004}, /*        */
  {0x0EFA, 0x000F}, /*        */
  {0x0EFC, 0x0000}, /*        */
  {0x0EFE, 0x0003}, /*        */
  {0x0F02, 0x0000}, /*        */
  {0x0F04, 0x0000}, /*        */
  {0x0F08, 0x0000}, /*         */
  {0x0F0A, 0x0000}, /*         */
  {0x0F0C, 0x0010}, /*         */
  {0x0F0E, 0x0000}, /*         */
  {0x0F10, 0x0000}, /*         */
  {0x0F12, 0x0000}, /*         */
  {0x0F14, 0x0002}, /*         */
  {0x0F16, 0x0003}, /*         */
  {0x0F18, 0x0000}, /*         */
  {0x0F1C, 0x0000}, /*         */
  {0x0F1E, 0x0022}, /*         */
  {0x0F20, 0x0024}, /*         */
  {0x0F22, 0x0044}, /*         */
  {0x0F24, 0x0010}, /*         */
  {0x0F26, 0x0000}, /*         */
  {0x0F28, 0x0000}, /*         */
  {0x0F2A, 0x0000}, /*         */
  {0x0F2C, 0x0000}, /*         */
  {0x0F2E, 0x0055}, /*         */
  {0x0F30, 0x0000}, /*         */
  {0x0F32, 0x0000}, /*         */
  {0x0F34, 0x1D1D}, /*         */
  {0x0F36, 0x091D}, /*         */
  {0x0F38, 0x3109}, /*         */
  {0x0F3A, 0x0909}, /*         */
  {0x0F3C, 0x1D09}, /*         */
  {0x0F3E, 0x311D}, /*         */
  {0x0F40, 0x0009}, /*         */
  {0x0F42, 0x00AA}, /*         */
  {0x0F44, 0x0000}, /*         */
  {0x0F46, 0x00C0}, /*              */
  {0x0F48, 0x0000}, /*              */
};

static kal_uint32 mt6325_aPMURegDef_mask[783] = { /*     */
  0xBFFF, /*          */
  0x3FF3, /*          */
  0x7FF3, /*          */
  0x4030, /*          */
  0x7F03, /*           */
  0x00F0, /*           */
  0x0003, /*           */
  0xFFFF, /*           */
  0xFFE3, /*           */
  0x00FF, /*           */
  0xF80F, /*           */
  0xFFFF, /*           */
  0x8002, /*           */
  0xFFFF, /*           */
  0x001F, /*           */
  0x0007, /*           */
  0x00FF, /*           */
  0x007F, /*           */
  0x1000, /*              */
  0xFFFF, /*     */
  0xFFFF, /*     */
  0x0377, /*       */
  0x0000, /*        */
  0x1FFF, /*         */
  0x000F, /*         */
  0x0001, /*           */
  0x0000, /*          */
  0x0000, /*          */
  0x0000, /*          */
  0x0000, /*         */
  0x0000, /*         */
  0x0000, /*         */
  0x0000, /*        */
  0x0F00, /*         */
  0x000F, /*         */
  0x000F, /*         */
  0x003F, /*        */
  0x000F, /*        */
  0x003F, /*        */
  0xFFFF, /*        */
  0xFFFF, /*        */
  0xFFFF, /*        */
  0x0FFF, /*        */
  0x000F, /*          */
  0x0000, /*              */
  0x0000, /*              */
  0x0000, /*           */
  0xFFFF, /*              */
  0x0000, /*                  */
  0x0000, /*                  */
  0xFFFF, /*              */
  0x0000, /*                  */
  0x0000, /*                  */
  0xFFFF, /*              */
  0x0000, /*                  */
  0x0000, /*                  */
  0xFFFF, /*              */
  0x0000, /*                  */
  0x0000, /*                  */
  0xFFF3, /*              */
  0x0000, /*                  */
  0x0000, /*                  */
  0x031F, /*              */
  0x0000, /*                  */
  0x0000, /*                  */
  0x03FF, /*              */
  0xFF7F, /*                */
  0xFFFF, /*              */
  0x0000, /*                  */
  0x0000, /*                  */
  0xFFFF, /*              */
  0xFFFF, /*              */
  0x07FF, /*              */
  0xFF3F, /*         */
  0x0000, /*             */
  0x0000, /*             */
  0x8F3F, /*             */
  0x0000, /*                 */
  0x0000, /*                 */
  0x00FF, /*            */
  0xFFFF, /*            */
  0x0000, /*                */
  0x0000, /*                */
  0x00FF, /*            */
  0x371B, /*            */
  0x0000, /*                */
  0x0000, /*                */
  0x00FF, /*              */
  0x0000, /*                  */
  0x0000, /*                  */
  0xFFFF, /*        */
  0x0000, /*            */
  0x0000, /*            */
  0x7FFF, /*        */
  0x0000, /*            */
  0x0000, /*            */
  0x3FFF, /*        */
  0x0000, /*            */
  0x0000, /*            */
  0x003F, /*            */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*           */
  0x0000, /*           */
  0x0000, /*           */
  0x0003, /*         */
  0x800F, /*          */
  0xFFFF, /*          */
  0xFFFF, /*          */
  0xFFFF, /*          */
  0x0001, /*          */
  0xFFFF, /*             */
  0xFFFF, /*              */
  0x0001, /*             */
  0x0001, /*          */
  0x00FF, /*           */
  0x000F, /*               */
  0x0003, /*                  */
  0x0003, /*                 */
  0x0001, /*             */
  0x0001, /*              */
  0x0001, /*               */
  0x0001, /*                */
  0x000F, /*            */
  0xFFFF, /*             */
  0x7FFF, /*             */
  0x3FFF, /*             */
  0x0003, /*       */
  0xFF00, /*             */
  0xF1FF, /*             */
  0xF1FF, /*             */
  0xF1FF, /*             */
  0xFFFF, /*             */
  0x0000, /*             */
  0x0000, /*             */
  0x0000, /*             */
  0x0000, /*             */
  0x00CF, /*             */
  0x00CF, /*              */
  0x00CF, /*              */
  0x00CF, /*              */
  0x00CF, /*              */
  0x00CF, /*              */
  0x00CF, /*              */
  0x00CF, /*              */
  0x00CF, /*              */
  0x01FF, /*              */
  0x01FF, /*              */
  0x01FF, /*              */
  0x01FF, /*              */
  0x01FF, /*              */
  0x0007, /*              */
  0x7F7F, /*              */
  0x7F7F, /*              */
  0x007F, /*              */
  0x3FFF, /*              */
  0x00FF, /*              */
  0x1FFF, /*              */
  0xFFFF, /*              */
  0x7FFF, /*              */
  0xFFFF, /*              */
  0x0001, /*              */
  0x1FFF, /*               */
  0xFFFF, /*               */
  0x7FFF, /*               */
  0xFFFF, /*               */
  0x0001, /*               */
  0x3FFF, /*                 */
  0x7FFF, /*                 */
  0x7FFF, /*                 */
  0x7FFF, /*                 */
  0x3FFF, /*                 */
  0x3FFF, /*                 */
  0x1FFF, /*                 */
  0x3FFF, /*                 */
  0xFFFF, /*                 */
  0x003F, /*                 */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0x03FF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0x3FFF, /*               */
  0x1FFF, /*             */
  0xFFFF, /*             */
  0x7FFF, /*             */
  0xFFFF, /*             */
  0x0001, /*             */
  0x0FFF, /*            */
  0xFFFF, /*            */
  0x1FFF, /*            */
  0x00FF, /*            */
  0x1FFF, /*               */
  0xFFFF, /*               */
  0x7FFF, /*               */
  0xFFFF, /*               */
  0x0001, /*               */
  0x1FFF, /*              */
  0x07FF, /*              */
  0x3FFF, /*              */
  0xFFFF, /*              */
  0x007F, /*              */
  0x1FFF, /*                */
  0xFFFF, /*                */
  0x7FFF, /*                */
  0xFFFF, /*                */
  0x0001, /*                */
  0xFF00, /*            */
  0x000F, /*            */
  0x3333, /*            */
  0xB031, /*            */
  0xFFFF, /*             */
  0x007F, /*             */
  0x007F, /*             */
  0x007F, /*             */
  0x007F, /*             */
  0xCDF3, /*             */
  0xFF00, /*            */
  0x000F, /*            */
  0x3333, /*            */
  0xB031, /*            */
  0xFFFF, /*             */
  0x007F, /*             */
  0x007F, /*             */
  0x007F, /*             */
  0x007F, /*             */
  0xCDF3, /*             */
  0xFF00, /*                */
  0x000F, /*                */
  0x3333, /*                */
  0xB031, /*                */
  0xFFFF, /*                 */
  0x007F, /*                 */
  0x007F, /*                 */
  0x007F, /*                 */
  0x007F, /*                 */
  0xCDF3, /*                 */
  0xFF00, /*          */
  0x000F, /*          */
  0x3333, /*          */
  0xB031, /*          */
  0xFFFF, /*           */
  0x007F, /*           */
  0x007F, /*           */
  0x007F, /*           */
  0x007F, /*           */
  0x7777, /*           */
  0xCDF3, /*           */
  0xFF00, /*            */
  0x000F, /*            */
  0x3333, /*            */
  0xB031, /*            */
  0xFFFF, /*             */
  0x007F, /*             */
  0x007F, /*             */
  0x007F, /*             */
  0x007F, /*             */
  0x7777, /*             */
  0xCDF3, /*             */
  0xFF00, /*         */
  0x000F, /*         */
  0x3333, /*         */
  0xB031, /*         */
  0xFFFF, /*          */
  0x007F, /*          */
  0x007F, /*          */
  0x007F, /*          */
  0x007F, /*          */
  0x7777, /*          */
  0x3333, /*          */
  0x3333, /*          */
  0xCDF3, /*          */
  0xFF00, /*           */
  0x000F, /*           */
  0x3333, /*           */
  0xB031, /*           */
  0xFFFF, /*            */
  0x007F, /*            */
  0x007F, /*            */
  0x007F, /*            */
  0x007F, /*            */
  0x7777, /*            */
  0x3333, /*            */
  0x3333, /*            */
  0xCDF3, /*            */
  0xFF00, /*           */
  0x000F, /*           */
  0x3333, /*           */
  0xB031, /*           */
  0xFFFF, /*            */
  0x007F, /*            */
  0x007F, /*            */
  0x007F, /*            */
  0x007F, /*            */
  0x7777, /*            */
  0x3333, /*            */
  0x3333, /*            */
  0xCDF3, /*            */
  0x007F, /*            */
  0x0003, /*            */
  0xFF00, /*          */
  0x000F, /*          */
  0x3333, /*          */
  0xB031, /*          */
  0xFFFF, /*           */
  0x007F, /*           */
  0x007F, /*           */
  0x007F, /*           */
  0x007F, /*           */
  0x7777, /*           */
  0x3333, /*           */
  0x3333, /*           */
  0xCDF3, /*           */
  0xFF00, /*        */
  0x000F, /*        */
  0x3333, /*        */
  0xB031, /*        */
  0xFFFF, /*         */
  0x003F, /*         */
  0x003F, /*         */
  0x003F, /*         */
  0x003F, /*         */
  0x3800, /*         */
  0x7777, /*         */
  0xFFFF, /*         */
  0xCCF3, /*         */
  0x3333, /*         */
  0x3333, /*         */
  0x3F3F, /*         */
  0x3F01, /*         */
  0xFFF3, /*         */
  0x00FF, /*           */
  0x3F00, /*           */
  0x0000, /*           */
  0x03FF, /*           */
  0x07FF, /*        */
  0x0F9F, /*        */
  0x0F9F, /*        */
  0x001F, /*        */
  0x0707, /*        */
  0x3F3F, /*        */
  0xFFFF, /*           */
  0x7FFF, /*           */
  0xFFFF, /*           */
  0x0F0F, /*           */
  0xFFFF, /*           */
  0x7FFF, /*           */
  0xFFFF, /*           */
  0x0F0F, /*           */
  0xFFFF, /*           */
  0x7FFF, /*           */
  0xFFFF, /*           */
  0x0F0F, /*           */
  0xFFFF, /*           */
  0x7FFF, /*           */
  0xFFFF, /*           */
  0x0F0F, /*           */
  0xFFFF, /*          */
  0x000F, /*          */
  0xF03F, /*               */
  0x7777, /*           */
  0x0FFF, /*             */
  0xFF0F, /*               */
  0x0FFF, /*              */
  0xB33F, /*        */
  0xB33F, /*        */
  0xB33F, /*        */
  0xBB3F, /*        */
  0xB33F, /*        */
  0x8302, /*        */
  0xB33F, /*        */
  0xB33F, /*        */
  0xB33F, /*        */
  0xB33F, /*        */
  0xB33F, /*         */
  0xBF3F, /*         */
  0xB33F, /*         */
  0xB3BF, /*         */
  0xB33F, /*         */
  0xB33F, /*         */
  0xB33F, /*         */
  0xB33F, /*         */
  0xB3BF, /*         */
  0xB33F, /*         */
  0xBF3B, /*         */
  0xB33F, /*         */
  0xFF87, /*         */
  0xB33F, /*         */
  0xB3BF, /*         */
  0xB33F, /*         */
  0xB33F, /*         */
  0xB33F, /*         */
  0xB3BF, /*         */
  0x8002, /*         */
  0x00F7, /*         */
  0xF1F1, /*         */
  0xF3F1, /*         */
  0xF3F7, /*         */
  0xF1F7, /*         */
  0xF7F1, /*         */
  0xFFF7, /*         */
  0xF730, /*         */
  0xF1FF, /*         */
  0xFFF7, /*         */
  0xFFFF, /*         */
  0xF7FE, /*          */
  0xFFFF, /*          */
  0xFFFF, /*          */
  0xFE31, /*          */
  0xFFF1, /*          */
  0xF800, /*          */
  0xFFFF, /*        */
  0xFFFF, /*        */
  0x77FF, /*        */
  0xFFFF, /*        */
  0xFFFF, /*         */
  0x3FFE, /*         */
  0x3FFF, /*         */
  0x3FFE, /*         */
  0x0003, /*            */
  0x07FF, /*        */
  0x07FF, /*        */
  0x07FF, /*        */
  0x07FF, /*        */
  0x07FF, /*        */
  0x07FF, /*        */
  0x07FF, /*        */
  0x07FF, /*        */
  0x07FF, /*        */
  0x07FF, /*        */
  0x07FF, /*         */
  0x07FF, /*         */
  0x07FF, /*         */
  0x07FF, /*         */
  0x07FF, /*         */
  0xF37F, /*         */
  0xFDFF, /*         */
  0xF01F, /*         */
  0x0001, /*         */
  0x100F, /*         */
  0x81FF, /*         */
  0x81FF, /*         */
  0x81FF, /*         */
  0x81FF, /*         */
  0x81FF, /*         */
  0x81FF, /*         */
  0x81FF, /*         */
  0x81FF, /*         */
  0x81FF, /*         */
  0x81FF, /*         */
  0xF9FF, /*         */
  0xFC03, /*         */
  0x801F, /*         */
  0x0000, /*         */
  0x0000, /*         */
  0x0000, /*         */
  0x0000, /*         */
  0xFF7D, /*          */
  0xFFFF, /*         */
  0x83FF, /*         */
  0xFF00, /*         */
  0x370D, /*        */
  0xFFFF, /*        */
  0x07FF, /*        */
  0x330D, /*        */
  0xFFFF, /*        */
  0x7FFF, /*        */
  0xFF37, /*        */
  0x7FFF, /*        */
  0x7FFF, /*        */
  0xFF00, /*        */
  0x0077, /*         */
  0xFFFF, /*         */
  0xFF80, /*         */
  0xFFFF, /*         */
  0xFFF0, /*         */
  0xFFFF, /*         */
  0xFFFF, /*         */
  0x7800, /*            */
  0x08FF, /*            */
  0x03F0, /*            */
  0x003F, /*        */
  0x00FF, /*        */
  0x0003, /*        */
  0x0003, /*        */
  0x0001, /*        */
  0x0001, /*        */
  0xFFFF, /*        */
  0xFFFF, /*        */
  0x0001, /*        */
  0x0001, /*        */
  0x0001, /*         */
  0x0001, /*         */
  0xFFFF, /*         */
  0x0000, /*         */
  0x001F, /*         */
  0x0000, /*             */
  0x0000, /*              */
  0x0000, /*              */
  0x0000, /*              */
  0x0000, /*              */
  0x0000, /*              */
  0x0000, /*               */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0x0000, /*                */
  0xFFFF, /*            */
  0xFFFF, /*             */
  0xFFFF, /*             */
  0xFFFF, /*             */
  0xFFFF, /*             */
  0xFFFF, /*             */
  0xFFFF, /*              */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0x1FEC, /*            */
  0x07FF, /*            */
  0x1BB0, /*            */
  0xFF7D, /*          */
  0xFFFF, /*          */
  0xFFFF, /*          */
  0x3FFF, /*          */
  0xFFFF, /*          */
  0xFFFF, /*          */
  0xFFFF, /*          */
  0xFFFF, /*          */
  0xFFFF, /*          */
  0xFFFF, /*          */
  0xFFF1, /*           */
  0x030F, /*           */
  0x77BF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0x00FF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xC0FF, /*           */
  0x1FFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0x003F, /*           */
  0x7FFF, /*           */
  0xFFFF, /*           */
  0x0010, /*              */
  0xFFFF, /*               */
  0x7FFF, /*               */
  0xFFFF, /*               */
  0x0FFF, /*               */
  0xFFFF, /*               */
  0xFFF0, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0x0003, /*               */
  0x07FF, /*               */
  0x07FF, /*               */
  0xFFFF, /*               */
  0xFF3F, /*               */
  0x3FFF, /*               */
  0xFFFF, /*               */
  0x3F3F, /*               */
  0xFFFF, /*               */
  0xFFFF, /*               */
  0x1FFF, /*               */
  0xFFFF, /*                */
  0xFFFF, /*                */
  0xFFFF, /*                */
  0xFFFF, /*                */
  0x0077, /*                */
  0x0001, /*                  */
  0x0001, /*                  */
  0x01FF, /*                  */
  0x0001, /*                  */
  0x0003, /*                  */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xF800, /*           */
  0xFFFF, /*            */
  0x0000, /*                */
  0x0000, /*                */
  0xFF9F, /*            */
  0x0000, /*                */
  0x0000, /*                */
  0xFC3F, /*          */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0x000F, /*           */
  0xBFFF, /*           */
  0xBFFF, /*           */
  0x01FF, /*           */
  0x01FF, /*           */
  0x07FF, /*           */
  0x83FF, /*               */
  0xC1FF, /*           */
  0xFFFF, /*                */
  0xFFFF, /*                */
  0x000F, /*                */
  0xBFFF, /*                */
  0xBFFF, /*                */
  0x01FF, /*                */
  0x01FF, /*                */
  0xFFFF, /*              */
  0xFFFF, /*              */
  0x000F, /*              */
  0xBFFF, /*              */
  0xBFFF, /*              */
  0x01FF, /*              */
  0x01FF, /*              */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0x8FFF, /*            */
  0xFFFF, /*            */
  0x8000, /*            */
  0xFFFF, /*            */
  0x8000, /*            */
  0xFFFF, /*            */
  0x8001, /*            */
  0xFFFF, /*            */
  0xFFFF, /*            */
  0xFFFF, /*            */
  0x8FFF, /*            */
  0x8FFF, /*            */
  0xFFFF, /*            */
  0xFFFF, /*            */
  0xFFFF, /*            */
  0xFFFF, /*            */
  0x8FFF, /*            */
  0xFFFF, /*            */
  0xFFFF, /*            */
  0xFFFF, /*            */
  0xFFFF, /*            */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0x1FF5, /*           */
  0xF037, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0x000F, /*           */
  0xBFFF, /*           */
  0xBFFF, /*           */
  0x01FF, /*           */
  0x01FF, /*           */
  0xFC38, /*           */
  0xCFFF, /*            */
  0x8017, /*            */
  0x01FF, /*            */
  0x00FF, /*            */
  0x01FF, /*            */
  0x00FF, /*            */
  0xFFFF, /*            */
  0xFFFF, /*            */
  0xC3FF, /*            */
  0xD0BF, /*            */
  0x8000, /*            */
  0x8000, /*            */
  0x81FF, /*            */
  0x00FF, /*            */
  0x00FE, /*            */
  0x00FF, /*            */
  0xFFFF, /*            */
  0xFFFF, /*            */
  0x7FFF, /*           */
  0x003F, /*           */
  0x00FF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*           */
  0xFFFF, /*            */
  0xC777, /*            */
  0x8707, /*            */
  0xF3FF, /*            */
  0xF7FF, /*            */
  0x3771, /*            */
  0xFFFF, /*            */
  0xEEEE, /*            */
  0xFF00, /*            */
  0x8707, /*            */
  0x0000, /*            */
  0xFFFF, /*            */
  0x7FFF, /*            */
  0xFFFF, /*            */
  0xFFFF, /*            */
  0x00FF, /*        */
  0x00EE, /*        */
  0x000F, /*        */
  0x0077, /*        */
  0x007F, /*        */
  0x03FF, /*        */
  0x0001, /*        */
  0x003F, /*         */
  0x0077, /*         */
  0x011F, /*         */
  0x00FF, /*         */
  0x0007, /*         */
  0x1F0C, /*         */
  0x009F, /*         */
  0x001F, /*         */
  0x0037, /*         */
  0x01FF, /*         */
  0x0077, /*         */
  0x0077, /*         */
  0x00FF, /*         */
  0x00D7, /*         */
  0x00F1, /*         */
  0x001F, /*         */
  0x000F, /*         */
  0x00FF, /*         */
  0x03FF, /*         */
  0x0031, /*         */
  0x0031, /*         */
  0x3F3F, /*         */
  0x3F3F, /*         */
  0x3F3F, /*         */
  0x3F3F, /*         */
  0x3F3F, /*         */
  0x3F3F, /*         */
  0x003F, /*         */
  0x00FF, /*         */
  0x00FF, /*         */
  0x1FE3, /*              */
  0x7F1F, /*              */
};

static kal_uint32 mt6325_aPMURegWr[10][2]= {  /*                      */
  {0x0020, 0xFFFF}, /*           */
  {0x02DA, 0xFFFF}, /*             */
  {0x0408, 0xFFFE}, /*             */  
  {0x0600, 0xFF00}, /*         */    
  {0x0836, 0x000F}, /*               */    
  {0x0A8E, 0x407F}, /*         */    
  {0x0C5E, 0xFFFF}, /*            */     
  {0x0E00, 0xFFFF}, /*           */      
  {0x203E, 0x078D}, /*                   */          
  {0x6080, 0x355A}, /*          */ 
};

static kal_uint32 mt6325_aPMURegWr_mask[10] = { /*     */
  0xFFFF, /*           */
  0xFFFF, /*             */
  0xFFFF, /*             */
  0xFF00, /*         */  
  0xFFFF, /*               */  
  0xF37F, /*         */    
  0xFFFF, /*            */    
  0xFFFF, /*           */     
  0xFFFF, /*                   */       
  0x3FFF, /*          */ 
};





void top_6325_read(void)
{
    kal_uint32 u2PMUReg = 0;
    kal_uint32 u2Cnt = 0;
    kal_uint32 default_value_mask = 0;

    printk("RegNum,DefaultValue,Mask,GotValue,TestValue,Ans\n");

    for(u2Cnt = 0; u2Cnt < (sizeof(mt6325_aPMURegDef)/sizeof(*mt6325_aPMURegDef)); ++u2Cnt)
    {
       u2PMUReg = upmu_get_reg_value(    (mt6325_aPMURegDef[u2Cnt][0])  );

       //                                                                                                           
       //                        
       u2PMUReg &= mt6325_aPMURegDef_mask[u2Cnt];       
       //                                                                                                         

       default_value_mask = ((mt6325_aPMURegDef[u2Cnt][1]) & mt6325_aPMURegDef_mask[u2Cnt]);      

       if(u2PMUReg != default_value_mask)
       {
           printk("[error] %x,%x,%x,%x,%x,%x\n",
            (mt6325_aPMURegDef[u2Cnt][0]), 
            (mt6325_aPMURegDef[u2Cnt][1]),
            mt6325_aPMURegDef_mask[u2Cnt],
            upmu_get_reg_value(    (mt6325_aPMURegDef[u2Cnt][0])  ),              
            u2PMUReg,            
            default_value_mask
            );
       }
    }        
}

void top_6325_write(int test_value)
{
    kal_uint32 u2PMUReg = 0;
    kal_uint32 u2Cnt = 0;
    kal_uint32 default_value_mask = 0;

    printk("RegNum,write_value(default_value_mask),Mask,GotValue,TestValue,Ans\n");

    for(u2Cnt = 0; u2Cnt < (sizeof(mt6325_aPMURegWr)/sizeof(*mt6325_aPMURegWr)); ++u2Cnt)
    {
       //                
       upmu_set_reg_value( (mt6325_aPMURegWr[u2Cnt][0]), test_value );
    
       //                
       u2PMUReg = upmu_get_reg_value(    (mt6325_aPMURegWr[u2Cnt][0])  );

       //                                                                                                                
       //                        
       u2PMUReg &= mt6325_aPMURegWr_mask[u2Cnt];
       //                                                                                                       
       
       default_value_mask = (test_value & mt6325_aPMURegWr_mask[u2Cnt]);

       if(u2PMUReg != default_value_mask)
       {              
           printk("[error] %x,%x(%x),%x,%x,%x,%x\r\n",
            (mt6325_aPMURegWr[u2Cnt][0]),             
            test_value,
            default_value_mask,
            mt6325_aPMURegWr_mask[u2Cnt],
            upmu_get_reg_value(    (mt6325_aPMURegWr[u2Cnt][0])  ),              
            u2PMUReg, 
            default_value_mask
            );
       }
    }

    #if 0 //           
    for(u2Cnt = 0; u2Cnt < (sizeof(mt6325_aPMURegWr)/sizeof(*mt6325_aPMURegWr)); ++u2Cnt)
    {
        printk("Reg[%x] %x\n", 
            (mt6325_aPMURegWr[u2Cnt][0]), 
            upmu_get_reg_value(    (mt6325_aPMURegWr[u2Cnt][0])  )
            );
    }
    #endif
}

static kal_uint32 mt6311_aPMURegDef[190][2]= {  /*                     */
  {0x00, 0x11}, /*   */
  {0x01, 0x01}, /*     */
  {0x02, 0x01}, /*     */
  {0x1A, 0x00}, /*          */
  {0x1B, 0xF9}, /*          */
  {0x1C, 0x00}, /*          */
  {0x1D, 0x00}, /*          */
  {0x1E, 0x00}, /*          */
  {0x1F, 0x00}, /*          */
  {0x20, 0x00}, /*          */
  {0x21, 0x00}, /*          */
  {0x22, 0x00}, /*          */
  {0x23, 0x20}, /*          */
  {0x24, 0x03}, /*           */
  {0x25, 0x00}, /*           */
  {0x26, 0x00}, /*           */
  {0x27, 0x00}, /*           */
  {0x28, 0x40}, /*           */
  {0x29, 0x00}, /*             */
  {0x2A, 0x65}, /*             */
  {0x2B, 0x00}, /*          */
  {0x2C, 0x00}, /*          */
  {0x2D, 0x00}, /*          */
  {0x2E, 0x00}, /*          */
  {0x2F, 0x00}, /*          */
  {0x30, 0x00}, /*          */
  {0x31, 0x00}, /*          */
  {0x32, 0x00}, /*          */
  {0x33, 0x00}, /*          */
  {0x34, 0x00}, /*          */
  {0x35, 0x00}, /*           */
  {0x36, 0x00}, /*           */
  {0x37, 0x00}, /*           */
  {0x38, 0x00}, /*           */
  {0x39, 0x00}, /*              */
  {0x3A, 0x00}, /*               */
  {0x3B, 0x00}, /*                */
  {0x3C, 0x00}, /*                */
  {0x3D, 0x00}, /*                */
  {0x3E, 0x00}, /*                */
  {0x3F, 0x00}, /*                */
  {0x40, 0x00}, /*                */
  {0x41, 0x00}, /*                */
  {0x42, 0x00}, /*                */
  {0x43, 0x00}, /*                */
  {0x44, 0x00}, /*                */
  {0x45, 0x00}, /*                 */
  {0x46, 0x00}, /*                  */
  {0x47, 0x00}, /*                  */
  {0x48, 0x00}, /*                  */
  {0x49, 0x01}, /*             */
  {0x4A, 0x00}, /*              */
  {0x4B, 0x02}, /*               */
  {0x4C, 0x00}, /*               */
  {0x4D, 0x00}, /*               */
  {0x4E, 0x00}, /*               */
  {0x4F, 0x00}, /*               */
  {0x50, 0x00}, /*               */
  {0x51, 0x00}, /*               */
  {0x52, 0x00}, /*               */
  {0x53, 0x00}, /*               */
  {0x54, 0x00}, /*               */
  {0x55, 0x04}, /*                */
  {0x56, 0x00}, /*                 */
  {0x57, 0x10}, /*                 */
  {0x58, 0x02}, /*                 */
  {0x59, 0x00}, /*             */
  {0x5A, 0x17}, /*             */
  {0x5B, 0x00}, /*             */
  {0x5C, 0x03}, /*             */
  {0x5D, 0x0A}, /*             */
  {0x5E, 0x19}, /*             */
  {0x5F, 0x0B}, /*             */
  {0x60, 0x00}, /*             */
  {0x61, 0x00}, /*             */
  {0x62, 0x0A}, /*             */
  {0x63, 0x0A}, /*              */
  {0x64, 0x00}, /*              */
  {0x65, 0x00}, /*              */
  {0x66, 0x00}, /*              */
  {0x67, 0x03}, /*              */
  {0x68, 0x00}, /*              */
  {0x69, 0xFE}, /*              */
  {0x6A, 0x00}, /*              */
  {0x6B, 0x00}, /*            */
  {0x6C, 0x01}, /*              */
  {0x6D, 0x20}, /*              */
  {0x6E, 0x01}, /*              */
  {0x6F, 0x02}, /*              */
  {0x70, 0x00}, /*              */
  {0x71, 0x00}, /*              */
  {0x72, 0x00}, /*              */
  {0x73, 0x00}, /*              */
  {0x74, 0x00}, /*              */
  {0x75, 0x00}, /*              */
  {0x76, 0xF0}, /*               */
  {0x77, 0x14}, /*               */
  {0x78, 0x00}, /*               */
  {0x79, 0x0D}, /*               */
  {0x7A, 0x88}, /*               */
  {0x7B, 0x0A}, /*               */
  {0x7C, 0x3A}, /*               */
  {0x7D, 0x03}, /*               */
  {0x7E, 0xFF}, /*               */
  {0x7F, 0xFF}, /*               */
  {0x80, 0x00}, /*               */
  {0x81, 0x00}, /*               */
  {0x82, 0x00}, /*               */
  {0x83, 0x00}, /*               */
  {0x84, 0x00}, /*                */
  {0x85, 0x00}, /*                */
  {0x86, 0x00}, /*                */
  {0x87, 0x00}, /*            */
  {0x88, 0x00}, /*            */
  {0x89, 0x00}, /*            */
  {0x8A, 0x1D}, /*            */
  {0x8B, 0x88}, /*             */
  {0x8C, 0x88}, /*             */
  {0x8D, 0x40}, /*             */
  {0x8E, 0x40}, /*             */
  {0x8F, 0x40}, /*             */
  {0x90, 0x40}, /*             */
  {0x91, 0x33}, /*             */
  {0x92, 0x33}, /*             */
  {0x93, 0x00}, /*             */
  {0x94, 0x01}, /*             */
  {0x95, 0x00}, /*            */
  {0x96, 0x00}, /*            */
  {0x97, 0x00}, /*            */
  {0x98, 0x1D}, /*            */
  {0x99, 0x88}, /*             */
  {0x9A, 0x88}, /*             */
  {0x9B, 0x40}, /*             */
  {0x9C, 0x40}, /*             */
  {0x9D, 0x40}, /*             */
  {0x9E, 0x40}, /*             */
  {0x9F, 0x33}, /*             */
  {0xA0, 0x33}, /*             */
  {0xA1, 0x00}, /*             */
  {0xA2, 0x01}, /*             */
  {0xA3, 0x00}, /*           */
  {0xA4, 0x00}, /*           */
  {0xA5, 0x00}, /*           */
  {0xA6, 0x00}, /*           */
  {0xA7, 0x01}, /*           */
  {0xA8, 0x6F}, /*           */
  {0xA9, 0x00}, /*           */
  {0xAA, 0x00}, /*           */
  {0xAB, 0x00}, /*           */
  {0xAC, 0x00}, /*           */
  {0xAD, 0x00}, /*           */
  {0xAE, 0x00}, /*            */
  {0xAF, 0x20}, /*           */
  {0xB0, 0x14}, /*           */
  {0xB1, 0x00}, /*           */
  {0xB2, 0x30}, /*           */
  {0xB3, 0x00}, /*           */
  {0xB4, 0x40}, /*           */
  {0xB5, 0x02}, /*           */
  {0xB6, 0x00}, /*           */
  {0xB7, 0x00}, /*           */
  {0xB8, 0x00}, /*           */
  {0xB9, 0x00}, /*            */
  {0xBA, 0x00}, /*            */
  {0xBB, 0x00}, /*            */
  {0xBC, 0x00}, /*            */
  {0xBD, 0xE0}, /*            */
  {0xBE, 0x00}, /*            */
  {0xBF, 0x20}, /*            */
  {0xC0, 0x00}, /*            */
  {0xC1, 0x00}, /*            */
  {0xC2, 0x00}, /*            */
  {0xC3, 0x00}, /*            */
  {0xC4, 0x00}, /*            */
  {0xC5, 0x01}, /*            */
  {0xC6, 0x00}, /*            */
  {0xC7, 0x01}, /*            */
  {0xC8, 0x00}, /*            */
  {0xC9, 0x00}, /*            */
  {0xCA, 0x00}, /*            */
  {0xCB, 0x00}, /*            */
  {0xCD, 0x00}, /*         */
  {0xCE, 0x61}, /*        */
  {0xCF, 0x01}, /*        */
  {0xD0, 0x00}, /*        */
  {0xD1, 0x00}, /*          */
  {0xD2, 0x00}, /*          */
  {0xD3, 0x00}, /*          */
  {0xD4, 0x00}, /*          */
  {0xD5, 0x00}  /*          */ 
};

static kal_uint32 mt6311_aPMURegDef_mask[190] = { /*     */
  0xFF, /*   */
  0xFF, /*     */
  0xFF, /*     */
  0xFF, /*          */
  0xFF, /*          */
  0x03, /*          */
  0x7F, /*          */
  0xFF, /*          */
  0x07, /*          */
  0x0F, /*          */
  0x73, /*          */
  0x03, /*          */
  0xF0, /*          */
  0xFF, /*           */
  0x02, /*           */
  0xFF, /*           */
  0xFF, /*           */
  0xCF, /*           */
  0x00, /*             */
  0xFF, /*             */
  0x7F, /*          */
  0x01, /*          */
  0x01, /*          */
  0x01, /*          */
  0x01, /*          */
  0xFF, /*          */
  0xFF, /*          */
  0x01, /*          */
  0x01, /*          */
  0x01, /*          */
  0x01, /*           */
  0x01, /*           */
  0x05, /*           */
  0x01, /*           */
  0xFF, /*              */
  0xFF, /*               */
  0xFF, /*                */
  0xFF, /*                */
  0xFF, /*                */
  0xFF, /*                */
  0xFF, /*                */
  0xFF, /*                */
  0xFF, /*                */
  0xFF, /*                */
  0xFF, /*                */
  0xFF, /*                */
  0xFF, /*                 */
  0xFF, /*                  */
  0xFF, /*                  */
  0xFF, /*                  */
  0xFF, /*             */
  0xFF, /*              */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*                */
  0xFF, /*                 */
  0xFF, /*                 */
  0xFF, /*                 */
  0xFF, /*             */
  0x1F, /*             */
  0xFF, /*             */
  0x1F, /*             */
  0xFF, /*             */
  0x1F, /*             */
  0xFF, /*             */
  0x00, /*             */
  0x00, /*             */
  0xCF, /*             */
  0xCF, /*              */
  0x0F, /*              */
  0x03, /*              */
  0x03, /*              */
  0x03, /*              */
  0x03, /*              */
  0xFF, /*              */
  0x3F, /*              */
  0xFF, /*            */
  0x0F, /*              */
  0x7F, /*              */
  0x7F, /*              */
  0x7F, /*              */
  0x1F, /*              */
  0x1F, /*              */
  0x1F, /*              */
  0xFF, /*              */
  0x07, /*              */
  0x3F, /*              */
  0xFF, /*               */
  0x1F, /*               */
  0x1F, /*               */
  0x3F, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0x03, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0x3F, /*               */
  0xFF, /*               */
  0xFF, /*                */
  0x03, /*                */
  0x07, /*                */
  0xFF, /*            */
  0x0F, /*            */
  0xFF, /*            */
  0x9F, /*            */
  0xFF, /*             */
  0xFF, /*             */
  0x7F, /*             */
  0x7F, /*             */
  0x7F, /*             */
  0x7F, /*             */
  0x77, /*             */
  0x77, /*             */
  0x1F, /*             */
  0xF3, /*             */
  0xFF, /*            */
  0x0F, /*            */
  0xFF, /*            */
  0x9F, /*            */
  0xFF, /*             */
  0xFF, /*             */
  0x7F, /*             */
  0x7F, /*             */
  0x7F, /*             */
  0x7F, /*             */
  0x77, /*             */
  0x77, /*             */
  0x1F, /*             */
  0xF3, /*             */
  0xFF, /*           */
  0x3F, /*           */
  0x00, /*           */
  0x00, /*           */
  0x01, /*           */
  0xFF, /*           */
  0xBF, /*           */
  0xBF, /*           */
  0xBF, /*           */
  0xBF, /*           */
  0xFF, /*           */
  0x07, /*            */
  0x3F, /*           */
  0x3F, /*           */
  0xFF, /*           */
  0x3F, /*           */
  0xFF, /*           */
  0xFF, /*           */
  0xFF, /*           */
  0xFF, /*           */
  0xFF, /*           */
  0xFF, /*           */
  0x03, /*            */
  0xFF, /*            */
  0x3F, /*            */
  0x07, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0x0F, /*            */
  0x7F, /*            */
  0xFF, /*            */
  0x7F, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0x0F, /*            */
  0xE8, /*         */
  0xF7, /*        */
  0x01, /*        */
  0xFF, /*        */
  0x8F, /*          */
  0xFF, /*          */
  0xFF, /*          */
  0xFF, /*          */
  0xFF  /*          */
};

static kal_uint32 mt6311_aPMURegWr[135][2]= {  /*                      */
  {0x1A, 0xFF}, /*          */
  {0x1B, 0x06}, /*          */
  {0x1E, 0xFF}, /*          */
  {0x1F, 0xFF}, /*          */
  {0x20, 0xFF}, /*          */
  {0x22, 0xFF}, /*          */
  {0x23, 0xDF}, /*          */
  {0x26, 0xFF}, /*           */
  {0x2A, 0x9A}, /*             */
  {0x2B, 0xFF}, /*          */
  {0x2C, 0xFF}, /*          */
  {0x2D, 0xFF}, /*          */
  {0x2E, 0xFF}, /*          */
  {0x2F, 0xFF}, /*          */
  {0x30, 0xFF}, /*          */
  {0x31, 0xFF}, /*          */
  {0x32, 0xFF}, /*          */
  {0x33, 0xFF}, /*          */
  {0x34, 0xFF}, /*          */
  {0x35, 0xFF}, /*           */
  {0x36, 0xFF}, /*           */
  {0x38, 0xFF}, /*           */
  {0x49, 0xFE}, /*             */
  {0x4A, 0xFF}, /*              */
  {0x4B, 0xFD}, /*               */
  {0x4C, 0xFF}, /*               */
  {0x4D, 0xFF}, /*               */
  {0x4E, 0xFF}, /*               */
  {0x4F, 0xFF}, /*               */
  {0x50, 0xFF}, /*               */
  {0x51, 0xFF}, /*               */
  {0x52, 0xFF}, /*               */
  {0x53, 0xFF}, /*               */
  {0x54, 0xFF}, /*               */
  {0x55, 0xFB}, /*                */
  {0x56, 0xFF}, /*                 */
  {0x57, 0xEF}, /*                 */
  {0x58, 0xFD}, /*                 */
  {0x59, 0xFF}, /*             */
  {0x5A, 0xE8}, /*             */
  {0x5B, 0xFF}, /*             */
  {0x5C, 0xFC}, /*             */
  {0x5D, 0xF5}, /*             */
  {0x5E, 0xE6}, /*             */
  {0x5F, 0xF4}, /*             */
  {0x62, 0xF5}, /*             */
  {0x63, 0xF5}, /*              */
  {0x64, 0xFF}, /*              */
  {0x65, 0xFF}, /*              */
  {0x67, 0xFC}, /*              */
  {0x68, 0xFF}, /*              */
  {0x69, 0x01}, /*              */
  {0x6A, 0xFF}, /*              */
  {0x6B, 0xFF}, /*            */
  {0x6C, 0xFE}, /*              */
  {0x6D, 0xDF}, /*              */
  {0x6E, 0xFE}, /*              */
  {0x6F, 0xFD}, /*              */
  {0x70, 0xFF}, /*              */
  {0x71, 0xFF}, /*              */
  {0x72, 0xFF}, /*              */
  {0x73, 0xFF}, /*              */
  {0x74, 0xFF}, /*              */
  {0x75, 0xFF}, /*              */
  {0x76, 0x0F}, /*               */
  {0x77, 0xEB}, /*               */
  {0x78, 0xFF}, /*               */
  {0x79, 0xF2}, /*               */
  {0x7A, 0x77}, /*               */
  {0x7B, 0xF5}, /*               */
  {0x7C, 0xC5}, /*               */
  {0x7D, 0xFC}, /*               */
  {0x7E, 0x00}, /*               */
  {0x7F, 0x00}, /*               */
  {0x80, 0xFF}, /*               */
  {0x81, 0xFF}, /*               */
  {0x82, 0xFF}, /*               */
  {0x83, 0xFF}, /*               */
  {0x84, 0xFF}, /*                */
  {0x85, 0xFF}, /*                */
  {0x87, 0xFF}, /*            */
  {0x88, 0xFF}, /*            */
  {0x89, 0xFF}, /*            */
  {0x8B, 0x77}, /*             */
  {0x8C, 0x77}, /*             */
  {0x8D, 0xBF}, /*             */
  {0x8E, 0xBF}, /*             */
  {0x8F, 0xBF}, /*             */
  {0x92, 0xCC}, /*             */
  {0x95, 0xFF}, /*            */
  {0x96, 0xFF}, /*            */
  {0x97, 0xFF}, /*            */
  {0x99, 0x77}, /*             */
  {0x9A, 0x77}, /*             */
  {0x9B, 0xBF}, /*             */
  {0x9C, 0xBF}, /*             */
  {0x9D, 0xBF}, /*             */
  {0xA0, 0xCC}, /*             */
  {0xA3, 0xFF}, /*           */
  {0xA4, 0xFF}, /*           */
  {0xA7, 0xFE}, /*           */
  {0xA8, 0x90}, /*           */
  {0xAE, 0xFF}, /*            */
  {0xAF, 0xDF}, /*           */
  {0xB0, 0xEB}, /*           */
  {0xB1, 0xFF}, /*           */
  {0xB2, 0xCF}, /*           */
  {0xB3, 0xFF}, /*           */
  {0xB4, 0xBF}, /*           */
  {0xB5, 0xFD}, /*           */
  {0xB6, 0xFF}, /*           */
  {0xB7, 0xFF}, /*           */
  {0xB8, 0xFF}, /*           */
  {0xB9, 0xFF}, /*            */
  {0xBA, 0xFF}, /*            */
  {0xBB, 0xFF}, /*            */
  {0xBC, 0xFF}, /*            */
  {0xBD, 0x1F}, /*            */
  {0xBF, 0xDF}, /*            */
  {0xC0, 0xFF}, /*            */
  {0xC1, 0xFF}, /*            */
  {0xC2, 0xFF}, /*            */
  {0xC3, 0xFF}, /*            */
  {0xC4, 0xFF}, /*            */
  {0xC5, 0xFE}, /*            */
  {0xC6, 0xFF}, /*            */
  {0xC7, 0xFE}, /*            */
  {0xC8, 0xFF}, /*            */
  {0xCB, 0xFF}, /*            */
  {0xCD, 0xFF}, /*         */
  {0xCE, 0x9E}, /*        */
  {0xCF, 0xFE}, /*        */
  {0xD0, 0xFF}, /*        */
  {0xD2, 0xFF}, /*          */
  {0xD3, 0xFF}  /*          */
};

static kal_uint32 mt6311_aPMURegWr_mask[135] = { /*     */
  0xFF, /*          */
  0xFF, /*          */
  0xFF, /*          */
  0x07, /*          */
  0x0F, /*          */
  0x03, /*          */
  0xF0, /*          */
  0xFF, /*           */
  0xFF, /*             */
  0x7F, /*          */
  0x01, /*          */
  0x01, /*          */
  0x01, /*          */
  0x01, /*          */
  0xFF, /*          */
  0xFF, /*          */
  0x01, /*          */
  0x01, /*          */
  0x01, /*          */
  0x01, /*           */
  0x01, /*           */
  0x01, /*           */
  0xFF, /*             */
  0xFF, /*              */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*                */
  0xFF, /*                 */
  0xFF, /*                 */
  0xFF, /*                 */
  0xFF, /*             */
  0x1F, /*             */
  0xFF, /*             */
  0x1F, /*             */
  0xFF, /*             */
  0x1F, /*             */
  0xFF, /*             */
  0xCF, /*             */
  0xCF, /*              */
  0x0F, /*              */
  0x03, /*              */
  0x03, /*              */
  0x03, /*              */
  0xFF, /*              */
  0x3F, /*              */
  0xFF, /*            */
  0x0F, /*              */
  0x7F, /*              */
  0x7F, /*              */
  0x7F, /*              */
  0x1F, /*              */
  0x1F, /*              */
  0x1F, /*              */
  0xFF, /*              */
  0x07, /*              */
  0x3F, /*              */
  0xFF, /*               */
  0x1F, /*               */
  0x1F, /*               */
  0x3F, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0x03, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0xFF, /*               */
  0x3F, /*               */
  0xFF, /*               */
  0xFF, /*                */
  0x03, /*                */
  0xFF, /*            */
  0x0F, /*            */
  0xFF, /*            */
  0xFF, /*             */
  0xFF, /*             */
  0x7F, /*             */
  0x7F, /*             */
  0x7F, /*             */
  0x77, /*             */
  0xFF, /*            */
  0x0F, /*            */
  0xFF, /*            */
  0xFF, /*             */
  0xFF, /*             */
  0x7F, /*             */
  0x7F, /*             */
  0x7F, /*             */
  0x77, /*             */
  0xFF, /*           */
  0x3F, /*           */
  0x01, /*           */
  0xFF, /*           */
  0x07, /*            */
  0x3F, /*           */
  0x3F, /*           */
  0xFF, /*           */
  0x3F, /*           */
  0xFF, /*           */
  0xFF, /*           */
  0xFF, /*           */
  0xFF, /*           */
  0xFF, /*           */
  0xFF, /*           */
  0x03, /*            */
  0xFF, /*            */
  0x3F, /*            */
  0x07, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0xFF, /*            */
  0x0F, /*            */
  0x7F, /*            */
  0xFF, /*            */
  0x7F, /*            */
  0xFF, /*            */
  0x0F, /*            */
  0xE8, /*         */
  0xF7, /*        */
  0x01, /*        */
  0xFF, /*        */
  0xFF, /*          */
  0xFF  /*          */
};

void top_6311_read(void)
{
    kal_uint32 u2PMUReg = 0;
    kal_uint32 u2Cnt = 0;
    kal_uint32 default_value_mask = 0;

    printk("RegNum,DefaultValue,Mask,GotValue,TestValue,Ans\n");

    for(u2Cnt = 0; u2Cnt < (sizeof(mt6311_aPMURegDef)/sizeof(*mt6311_aPMURegDef)); ++u2Cnt)
    {
       u2PMUReg = mt6311_get_reg_value(    (mt6311_aPMURegDef[u2Cnt][0])  );

       //                                                                                                           
       //                        
       u2PMUReg &= mt6311_aPMURegDef_mask[u2Cnt];       
       //                                                                                                         

       default_value_mask = ((mt6311_aPMURegDef[u2Cnt][1]) & mt6311_aPMURegDef_mask[u2Cnt]);      

       if(u2PMUReg != default_value_mask)
       {
           printk("[error] %x,%x,%x,%x,%x,%x\n",
            (mt6311_aPMURegDef[u2Cnt][0]), 
            (mt6311_aPMURegDef[u2Cnt][1]),
            mt6311_aPMURegDef_mask[u2Cnt],
            mt6311_get_reg_value(    (mt6311_aPMURegDef[u2Cnt][0])  ),              
            u2PMUReg,            
            default_value_mask
            );
       }
    }        
}

void top_6311_write(int test_value)
{
    kal_uint32 u2PMUReg = 0;
    kal_uint32 u2Cnt = 0;
    kal_uint32 default_value_mask = 0;

    printk("RegNum,write_value(default_value_mask),Mask,GotValue,TestValue,Ans\n");

    for(u2Cnt = 0; u2Cnt < (sizeof(mt6311_aPMURegWr)/sizeof(*mt6311_aPMURegWr)); ++u2Cnt)
    {
       //                
       mt6311_set_reg_value( (mt6311_aPMURegWr[u2Cnt][0]), test_value );
    
       //                
       u2PMUReg = mt6311_get_reg_value(    (mt6311_aPMURegWr[u2Cnt][0])  );

       //                                                                                                                
       //                        
       u2PMUReg &= mt6311_aPMURegWr_mask[u2Cnt];
       //                                                                                                       
       
       default_value_mask = (test_value & mt6311_aPMURegWr_mask[u2Cnt]);

       if(u2PMUReg != default_value_mask)
       {              
           printk("[error] %x,%x(%x),%x,%x,%x,%x\r\n",
            (mt6311_aPMURegWr[u2Cnt][0]),             
            test_value,
            default_value_mask,
            mt6311_aPMURegWr_mask[u2Cnt],
            mt6311_get_reg_value(    (mt6311_aPMURegWr[u2Cnt][0])  ),              
            u2PMUReg, 
            default_value_mask
            );
           
           //                              
           //             
       }
       else
       {
           printk("[pass]%x\n",(mt6311_aPMURegWr[u2Cnt][0]));
       }
    }

    #if 0 //           
    printk("debug check------------------------\n");
    for(u2Cnt = 0; u2Cnt < (sizeof(mt6311_aPMURegWr)/sizeof(*mt6311_aPMURegWr)); ++u2Cnt)
    {
        printk("Reg[%x] %x\n", 
            (mt6311_aPMURegWr[u2Cnt][0]), 
            upmu_get_reg_value(    (mt6311_aPMURegWr[u2Cnt][0])  )
            );
    }
    #endif
}

#endif 

#ifdef EFUSE_DVT_ENABLE
void exec_6325_efuse_test(void)
{
    U32 ret=0;
    U32 reg_val=0;
    int i=0;
    
    printk("[exec_6325_efuse_test] start\n");

    //                                 
    ret=pmic_config_interface(0x026C, 0x0040, 0xFFFF, 0);
    ret=pmic_config_interface(0x024E, 0x0004, 0xFFFF, 0);

    //  
    ret=pmic_config_interface(0x0C16, 0x1, 0x1, 0);

    //    
    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x%x]=0x%x,Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
        0x026C,upmu_get_reg_value(0x026C),
        0x024C,upmu_get_reg_value(0x024C),
        0x0C16,upmu_get_reg_value(0x0C16)
        );

    for(i=0;i<=0x1F;i++)
    {
        //                  
        ret=pmic_config_interface(0x0C00, i, 0x1F, 1);

        //         
        ret=pmic_read_interface(0xC10, &reg_val, 0x1, 0);
        if(reg_val==0)
            ret=pmic_config_interface(0xC10, 1, 0x1, 0);
        else
            ret=pmic_config_interface(0xC10, 0, 0x1, 0);

        reg_val=1;    
        while(reg_val == 1)
        {
            ret=pmic_read_interface(0xC1A, &reg_val, 0x1, 0);
            printk("5. polling Reg[0x61A][0]=0x%x\n", reg_val);
        }

        //            
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "i=%d,Reg[0x%x]=0x%x,Reg[0x%x]=0x%x,Reg[0x%x]=0x%x\n", 
            i,
            0x0C00,upmu_get_reg_value(0x0C00),
            0x0C18,upmu_get_reg_value(0x0C18),
            0x0C1C,upmu_get_reg_value(0x0C1C)
            );
    }

    //                                  
    ret=pmic_config_interface(0x024C, 0x0004, 0xFFFF, 0);
    ret=pmic_config_interface(0x026A, 0x0040, 0xFFFF, 0);

    printk("[exec_6325_efuse_test] Done\n");
}
#endif

//                                        
//       
//                                        
#ifdef AUXADC_DVT_ENABLE
#define MAX_CHANNEL_NUM 13

//         
#define GAIN_CH0	0x1f4		//    
#define OFFSET_CH0	0x1e
#define GAIN_CH4	0x3e8	//    
#define OFFSET_CH4	0x1e
#define GAIN_CH7	0x5dc  //    
#define OFFSET_CH7	0x1e  //   
#define SW_GAIN		0x7d0	//    
#define SW_OFFSET	0x1e

extern kal_uint32  pmic_is_auxadc_ready(kal_int32 channel_num, upmu_adc_chip_list_enum chip_num, upmu_adc_user_list_enum user_num);
extern kal_uint32  pmic_get_adc_output(kal_int32 channel_num, upmu_adc_chip_list_enum chip_num, upmu_adc_user_list_enum user_num);
extern int PMIC_IMM_GetChannelNumber(upmu_adc_chl_list_enum dwChannel);
extern upmu_adc_user_list_enum PMIC_IMM_GetUserNumber(upmu_adc_chl_list_enum dwChannel);

static upmu_adc_chl_list_enum eChannelEnumList[MAX_CHANNEL_NUM] =
{
	AUX_BATSNS_AP,
	AUX_ISENSE_AP,
	AUX_VCDT_AP,
	AUX_BATON_AP,
	AUX_TSENSE_AP,
	AUX_VACCDET_AP,
	AUX_VISMPS_AP,
	AUX_ADCVIN0_GPS,
	AUX_ICLASSAB_AP,
	AUX_HP_AP,
	AUX_VBIF_AP,

	AUX_ADCVIN0_MD,
	AUX_TSENSE_MD,
};

int auxadc_request_one_channel(int index) {
	int ret = 0;
    switch (index){
	case 0:
		ret = PMIC_IMM_GetOneChannelValue(AUX_BATSNS_AP, 1, 0);
		break;
	case 1:	
		ret = PMIC_IMM_GetOneChannelValue(AUX_ISENSE_AP, 1, 0);
		break;
	case 2:	
		ret = PMIC_IMM_GetOneChannelValue(AUX_VCDT_AP, 1, 0);
		break;
	case 3:	
		ret = PMIC_IMM_GetOneChannelValue(AUX_BATON_AP, 1, 0);
		break;
	case 4:	
		ret = PMIC_IMM_GetOneChannelValue(AUX_TSENSE_AP, 1, 0);
		break;
	case 5:	
		ret = PMIC_IMM_GetOneChannelValue(AUX_VACCDET_AP, 1, 0);
		break;
	case 6:	
		
		ret = PMIC_IMM_GetOneChannelValue(AUX_VISMPS_AP, 1, 0);
		break;
	case 7:	
		ret = PMIC_IMM_GetOneChannelValue(AUX_ADCVIN0_GPS, 1, 0);
		
		break;
	case 8:	
		ret = PMIC_IMM_GetOneChannelValue(AUX_ICLASSAB_AP, 1, 0);
		
		break;
	case 9:	
		ret = PMIC_IMM_GetOneChannelValue(AUX_HP_AP, 1, 0);
		break;
	case 10:	
		ret = PMIC_IMM_GetOneChannelValue(AUX_VBIF_AP, 1, 0);
		break;
	case 11:	
		ret = PMIC_IMM_GetOneChannelValue(AUX_ADCVIN0_MD, 1, 0);
		break;
	case 12:
		ret = PMIC_IMM_GetOneChannelValue(AUX_TSENSE_MD, 1, 0);
		break;
	case 13:
		ret = PMIC_IMM_GetOneChannelValue(AUX_CH12, 1, 0);
		break;
	case 14:
		ret = PMIC_IMM_GetOneChannelValue(AUX_CH13, 1, 0);
		break;
	case 15:
		ret = PMIC_IMM_GetOneChannelValue(AUX_CH14, 1, 0);
		break;
	case 16:
		ret = PMIC_IMM_GetOneChannelValue(AUX_CH15, 1, 0);
		break;
	//                                           
	default :
		printk("[auxadc_request_test] no such channel number!!\n");
		break;
    }
    
    return ret;	
}
//                                      
void auxadc_request_test(void)
{
	int i;
	int ret[MAX_CHANNEL_NUM] = {0};
	printk("[auxadc_request_test] start \n");
	printk("[auxadc_request_test] Process Result: ");
	for (i=0; i< MAX_CHANNEL_NUM; i++) {
		ret[i] = auxadc_request_one_channel(i);
		printk("ret[%d]: %d ", i, ret[i]);
	}
	printk("\n[auxadc_request_test] end !! \n");
}

void auxadc_trimm_channel_test(void)
{
	int i, j;
	int ret[MAX_CHANNEL_NUM] = {0}, ret_raw[MAX_CHANNEL_NUM] = {0}, ret_trim[MAX_CHANNEL_NUM] = {0}, ret_cali[MAX_CHANNEL_NUM] = {0};
	int channel_num[MAX_CHANNEL_NUM];
	upmu_adc_user_list_enum user_num[MAX_CHANNEL_NUM];
	U32 temp_result[4] = {0.0};
	U32 gain_value, offset_value; 
	U32 adc_output_16bit=0, adc_output_15bit=0, adc_output_12bit=0, adc_out=0;
	
	printk("[auxadc_trimm_channel_test] start\n");
	
	temp_result[0] = 1015;
	temp_result[1] = 1030;
	temp_result[2] = 1045;
	temp_result[3] = 1060;
	//
	mt6325_upmu_set_efuse_gain_ch4_trim(GAIN_CH4);
	mt6325_upmu_set_efuse_offset_ch4_trim(OFFSET_CH4);
	mt6325_upmu_set_efuse_gain_ch7_trim(GAIN_CH7);
	mt6325_upmu_set_efuse_offset_ch7_trim(OFFSET_CH7);
	mt6325_upmu_set_efuse_gain_ch0_trim(GAIN_CH0);
	mt6325_upmu_set_efuse_offset_ch0_trim(OFFSET_CH0);
	mt6325_upmu_set_rg_sw_gain_trim(SW_GAIN);
	mt6325_upmu_set_rg_sw_offset_trim(SW_OFFSET);

	for (i= 0; i < MAX_CHANNEL_NUM; i++) {
		for (j = 0; j < 3; j++) {
			mt6325_upmu_set_rg_adc_trim_ch0_sel(j);
			mt6325_upmu_set_rg_adc_trim_ch2_sel(j);
			mt6325_upmu_set_rg_adc_trim_ch3_sel(j);
			mt6325_upmu_set_rg_adc_trim_ch4_sel(j);
			mt6325_upmu_set_rg_adc_trim_ch5_sel(j);
			mt6325_upmu_set_rg_adc_trim_ch6_sel(j);
			mt6325_upmu_set_rg_adc_trim_ch7_sel(j);
			
			if (i < 8)
				ret[i] = auxadc_request_one_channel(i);
			if (i < 7) 
				mt6325_upmu_set_rg_cic_out_raw(ret_raw[i]);
			else if (i == 7) {
				ret_raw[i] =  (mt6325_upmu_get_rg_adc_out_cic_raw_16_1()<< 1) + mt6325_upmu_get_rg_adc_out_cic_raw_0();
			}
			
			ret_trim[i] = pmic_get_adc_output(channel_num[i], MT6325_CHIP, user_num[i]);
			printk("ret[%d]: %d , ret_raw[%d]: %d ", i, ret[i], i, ret_raw[i]);
			if (j >=2) {
				if (j==2) {
					gain_value = GAIN_CH7;
					offset_value = OFFSET_CH7;
				} else if (j==3) {
					gain_value = SW_GAIN;
					offset_value = SW_OFFSET;
				}
					
				ret_cali[i] = ret_raw[i] * (100000 + ((gain_value*100000) / 131072))/100000 + offset_value;
				ret_cali[i] = ret_cali[i] + 0x8000;
			} else {
				if (j==0) {
					gain_value = GAIN_CH0 & 0x01FF;
					offset_value = OFFSET_CH0 & 0x00FF;
				} else if (j==1) {
					gain_value = GAIN_CH4;
					offset_value = GAIN_CH4;
				}	
				ret_cali[i] = ret_raw[i] * (1000 + ((gain_value*1000) / 4096))/1000 + offset_value*32;
				ret_cali[i] = ret_cali[i] + 0x8000;
			}
			
			adc_output_16bit = ret_cali[i] >> 1;
			adc_output_15bit = ret_cali[i] >> 2;
			adc_output_12bit = ret_cali[i] >> 5;
			
			if (i==7)
				adc_out = adc_output_16bit;
			else
				adc_out = adc_output_15bit;
				
			if (abs(adc_out - ret_trim[i]) < 2) 
				printk("[auxadc_trimm_one_channel_test] TEST PASS\n");
			else
				printk(" ret[%d]: %d , ret_raw[%d]: %d,  ret_cali[%d] : %d\n",
					i, ret[i], i, ret_raw[i], i, ret_cali[i]);	
		}	
	}
}

void auxadc_trimm_one_channel_test(int j)
{
	int  i;
	U32 ret[MAX_CHANNEL_NUM] = {0}, ret_raw[MAX_CHANNEL_NUM] = {0}, ret_trim[MAX_CHANNEL_NUM] = {0} , ret_cali[MAX_CHANNEL_NUM] = {0};
	int channel_num[MAX_CHANNEL_NUM];
	upmu_adc_user_list_enum user_num[MAX_CHANNEL_NUM];
	U32 temp_result[4] = {0.0};
	U32 gain_value, offset_value; 
	U32 adc_output_16bit=0, adc_output_15bit=0, adc_output_12bit=0, adc_out=0;
	printk("[auxadc_trimm_one_channel_test] start\n");
	
		

	temp_result[0] = 1015;
	temp_result[1] = 1030;
	temp_result[2] = 1045;
	temp_result[3] = 1060;
		
	mt6325_upmu_set_efuse_gain_ch4_trim(GAIN_CH4);
	mt6325_upmu_set_efuse_offset_ch4_trim(OFFSET_CH4);
	mt6325_upmu_set_efuse_gain_ch7_trim(GAIN_CH7);
	mt6325_upmu_set_efuse_offset_ch7_trim(OFFSET_CH7);
	mt6325_upmu_set_efuse_gain_ch0_trim(GAIN_CH0);
	mt6325_upmu_set_efuse_offset_ch0_trim(OFFSET_CH0);
	mt6325_upmu_set_rg_sw_gain_trim(SW_GAIN);
	mt6325_upmu_set_rg_sw_offset_trim(SW_OFFSET);
	
	for (i= 0; i < 8; i++) {
		channel_num[i] = PMIC_IMM_GetChannelNumber(eChannelEnumList[i]);
		user_num[i] = PMIC_IMM_GetUserNumber(eChannelEnumList[i]);
		printk("[auxadc_trimm_channel_test] channel_num = %d user_num = %d\n", channel_num[i], user_num[i]);
		mt6325_upmu_set_rg_adc_trim_ch0_sel(j);
		mt6325_upmu_set_rg_adc_trim_ch2_sel(j);
		mt6325_upmu_set_rg_adc_trim_ch3_sel(j);
		mt6325_upmu_set_rg_adc_trim_ch4_sel(j);
		mt6325_upmu_set_rg_adc_trim_ch5_sel(j);
		mt6325_upmu_set_rg_adc_trim_ch6_sel(j);
		mt6325_upmu_set_rg_adc_trim_ch7_sel(j);
		
		
		ret[i] = auxadc_request_one_channel(i);
		
		if (i < 7) {
			mt6325_upmu_set_rg_cic_out_raw(ret_raw[i]);
		} else if (i == 7) {
			ret_raw[i] = (mt6325_upmu_get_rg_adc_out_cic_raw_16_1()<< 1) + mt6325_upmu_get_rg_adc_out_cic_raw_0();
		}
		ret_trim[i] = pmic_get_adc_output(channel_num[i], MT6325_CHIP, user_num[i]);

		//                                                                                  
		printk("temp_result[%d] = %d\n", j, temp_result[j]);
		if (j >=2) {
			if (j==2) {
				gain_value = GAIN_CH7;
				offset_value = OFFSET_CH7;
			} else if (j==3) {
				gain_value = SW_GAIN;
				offset_value = SW_OFFSET;
			}
				
			ret_cali[i] = ret_raw[i] * (100000 + ((gain_value*100000) / 131072))/100000 + offset_value;
			ret_cali[i] = ret_cali[i] + 0x8000;
		} else {
			if (j==0) {
				gain_value = GAIN_CH0 & 0x01FF;
				offset_value = OFFSET_CH0 & 0x00FF;
			} else if (j==1) {
				gain_value = GAIN_CH4;
				offset_value = GAIN_CH4;
			}	
			ret_cali[i] = ret_raw[i] * (1000 + ((gain_value*1000) / 4096))/1000 + offset_value*32;
			ret_cali[i] = ret_cali[i] + 0x8000;
		}
		
		adc_output_16bit = ret_cali[i] >> 1;
		adc_output_15bit = ret_cali[i] >> 2;
		adc_output_12bit = ret_cali[i] >> 5;
		
		if (i==7)
			adc_out = adc_output_16bit;
		else
			adc_out = adc_output_15bit;
			
		if (abs(adc_out - ret_trim[i]) < 2)
			printk("[auxadc_trimm_one_channel_test] TEST PASS\n");
		else
			printk(" ret[%d]: %d , ret_raw[%d]: %d,  ret_cali[%d] : %d\n",
				i, ret[i], i, ret_raw[i], i, ret_cali[i]);	
	}
}
kal_int32 dvtloop=1000;

void do_auxadc_polling(int number, int *channel_num, upmu_adc_user_list_enum *user_num)
{

	kal_int32 i=0;
	kal_int32 ret_data[number];
	kal_int32 ret_order[number];
	kal_int16 order=1;
	kal_int16 count = 0;
	printk("[do_auxadc_polling] start\n");
	for (i=0; i < number; i++) {
		ret_data[i]=0;
		ret_order[i]=0;
	}
    
	/*
         
               
               
             
              
               
                
               
                 
                 
           
             

                
               
 */
	do {
		for (i=0; i < number; i++) {
			//                                                                                                                
				
			if (ret_order[i]==0 && pmic_is_auxadc_ready(channel_num[i], MT6325_CHIP, user_num[i]) == 1 )
			{
				printk("[do_auxadc_polling 2] channel_num = %d, user_num = %d\n", channel_num[i], user_num[i]);
				ret_data[i] = pmic_get_adc_output(channel_num[i], MT6325_CHIP, user_num[i]);	
				ret_order[i]=order;
				printk("[do_auxadc_polling] ret_order[%d] = %d, ret_data[%d] = %d\n", i, order, i , ret_data[i]);
				order++;
			}
		}
		count++;
		printk("count = %d\n", count);
		if (count>dvtloop)
			break;
		
		printk("\n");
	} while (count<=dvtloop);
#if 1
	printk("ch0:BATSNS		order[%2d]  [%4d]\n ", ret_order[0], ret_data[0]);   
	printk("ch1:ISENSE		order[%2d]  [%4d]\n ", ret_order[1], ret_data[1]); 
	printk("ch2:VCDT		order[%2d]  [%4d]\n ", ret_order[2], ret_data[2]); 
	printk("ch3:BATON1		order[%2d]  [%4d]\n ", ret_order[3], ret_data[3]); 
	printk("ch4:TSENSE1		order[%2d]  [%4d]\n ", ret_order[4], ret_data[4]); 
	printk("ch5:ACCDET		order[%2d]  [%4d]\n ", ret_order[5], ret_data[5]); 
	printk("ch6:VISMPS		order[%2d]  [%4d]\n ", ret_order[6], ret_data[6]); 
	printk("ch7:ADCVIN0_GPS		order[%2d]  [%4d]\n ", ret_order[7], ret_data[7]); 
	printk("ch8:ICLASSAB		order[%2d]  [%4d]\n ", ret_order[8], ret_data[8]); 
	printk("ch9:HPL			order[%2d]  [%4d]\n ", ret_order[9], ret_data[9]);
	printk("ch11:VBIF		order[%2d]  [%4d]\n ", ret_order[10], ret_data[10]);
	printk("ADCVIN0_MD:		order[%2d]  [%4d]\n ", ret_order[11], ret_data[11]); 
	printk("TSENSE_MD:		order[%2d]  [%4d]\n ", ret_order[12], ret_data[12]); 

	printk("[%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d]\n ", 
		mt6325_upmu_get_rg_adc_rdy_batsns(),
		mt6325_upmu_get_rg_adc_rdy_isense(),
		mt6325_upmu_get_rg_adc_rdy_vcdt(),
		mt6325_upmu_get_rg_adc_rdy_baton1(),
		mt6325_upmu_get_rg_adc_rdy_thr_sense1(),
		mt6325_upmu_get_rg_adc_rdy_ch5(),
		mt6325_upmu_get_rg_adc_rdy_ch6(),
		mt6325_upmu_get_rg_adc_rdy_gps(),
		mt6325_upmu_get_rg_adc_rdy_ch8(),
		mt6325_upmu_get_rg_adc_rdy_ch9(),
		mt6325_upmu_get_rg_adc_rdy_ch11(),
		mt6325_upmu_get_rg_adc_rdy_md(),
		mt6325_upmu_get_rg_adc_rdy_thr_md());
#endif
}
#if 0
void auxadc_set_avg_num_max_6331(void)
{
	//                               
	//                                       
	//                                        

	//                                      
	//                          
	mt6331_upmu_set_auxadc_avg_num_sel(0x0FFF);
	mt6331_upmu_set_auxadc_avg_num_sel_lbat(0x1);
	mt6331_upmu_set_auxadc_avg_num_sel_wakeup(0x1);
	//                                    
	mt6331_upmu_set_auxadc_avg_num_large(7);
}

void auxadc_set_avg_num_max_6332(void)
{
	//                               
	//                                       
	//                                        

	//                                      
	//                          
	mt6332_upmu_set_auxadc_avg_num_sel(0x0FFF);
	mt6332_upmu_set_auxadc_avg_num_sel_lbat(0x1);
	mt6332_upmu_set_auxadc_avg_num_sel_wakeup(0x1);
	//                                    
	mt6332_upmu_set_auxadc_avg_num_large(7);
}
#endif
void auxadc_clr_request(void)
{
	mt6325_upmu_set_rg_md_rqst(0);
        mt6325_upmu_set_rg_gps_rqst(0);
        mt6325_upmu_set_rg_ap_rqst_list(0);
        mt6325_upmu_set_rg_ap_rqst_list_rsv(0);
}
//                                            
void auxadc_priority_test_1(void)
{
	int i;
        int channel_num[MAX_CHANNEL_NUM];
        upmu_adc_user_list_enum user_num[MAX_CHANNEL_NUM];

        for (i=0; i < MAX_CHANNEL_NUM; i++) {
        	channel_num[i] = PMIC_IMM_GetChannelNumber(eChannelEnumList[i]);
		user_num[i] = PMIC_IMM_GetUserNumber(eChannelEnumList[i]);
        }
        
        auxadc_clr_request();
        mt6325_upmu_set_rg_md_rqst(1);
        mt6325_upmu_set_rg_gps_rqst(1);
        mt6325_upmu_set_rg_ap_rqst_list(0x1FE);
        mt6325_upmu_set_rg_ap_rqst_list_rsv(0xF8);
        
	do_auxadc_polling(MAX_CHANNEL_NUM, channel_num, user_num);
     	
}
//                                                             
void auxadc_priority_test_2(void)
{
	int i;
        int channel_num[MAX_CHANNEL_NUM];
        upmu_adc_user_list_enum user_num[MAX_CHANNEL_NUM];

        for (i=0; i < MAX_CHANNEL_NUM; i++) {
        	channel_num[i] = PMIC_IMM_GetChannelNumber(eChannelEnumList[i]);
		user_num[i] = PMIC_IMM_GetUserNumber(eChannelEnumList[i]);
        }
        
        auxadc_clr_request();
        mt6325_upmu_set_rg_md_rqst(1);
        mt6325_upmu_set_rg_gps_rqst(1);
        mt6325_upmu_set_rg_ap_rqst_list(0x1BE);
        mt6325_upmu_set_rg_ap_rqst_list(0x40);
        mt6325_upmu_set_rg_ap_rqst_list_rsv(0xF8);
        
	do_auxadc_polling(MAX_CHANNEL_NUM, channel_num, user_num);
     	
}
//                                                      
void auxadc_priority_test_3(void)
{
	int i;
        int channel_num[MAX_CHANNEL_NUM];
        upmu_adc_user_list_enum user_num[MAX_CHANNEL_NUM];

        for (i=0; i < MAX_CHANNEL_NUM; i++) {
        	channel_num[i] = PMIC_IMM_GetChannelNumber(eChannelEnumList[i]);
		user_num[i] = PMIC_IMM_GetUserNumber(eChannelEnumList[i]);
        }
        
        auxadc_clr_request();
        mt6325_upmu_set_rg_ap_rqst_list(0x1BE);
        mt6325_upmu_set_rg_ap_rqst_list_rsv(0xF8);
        mt6325_upmu_set_rg_md_rqst(1);
        mt6325_upmu_set_rg_gps_rqst(1);
        mt6325_upmu_set_rg_ap_rqst_list(0x40);
        
	do_auxadc_polling(MAX_CHANNEL_NUM, channel_num, user_num);
     	
}
//                                                   
void auxadc_priority_test_4(void)
{
	int i;
        int channel_num[MAX_CHANNEL_NUM];
        upmu_adc_user_list_enum user_num[MAX_CHANNEL_NUM];

        for (i=0; i < MAX_CHANNEL_NUM; i++) {
        	channel_num[i] = PMIC_IMM_GetChannelNumber(eChannelEnumList[i]);
		user_num[i] = PMIC_IMM_GetUserNumber(eChannelEnumList[i]);
        }
        
        auxadc_clr_request();
        mt6325_upmu_set_rg_gps_rqst(1);
        udelay(100);
        mt6325_upmu_set_rg_md_rqst(1);
        
	do_auxadc_polling(MAX_CHANNEL_NUM, channel_num, user_num);
     	
}
//                                                         
void auxadc_priority_test_5(void)
{
	int i, count=0;
        int channel_num[MAX_CHANNEL_NUM];
        upmu_adc_user_list_enum user_num[MAX_CHANNEL_NUM];
        kal_uint32 output_gps, output_md;

        for (i=0; i < MAX_CHANNEL_NUM; i++) {
        	channel_num[i] = PMIC_IMM_GetChannelNumber(eChannelEnumList[i]);
		user_num[i] = PMIC_IMM_GetUserNumber(eChannelEnumList[i]);
        }
        
        auxadc_clr_request();
        mt6325_upmu_set_rg_gps_rqst(1);

        while (pmic_is_auxadc_ready(PMIC_IMM_GetChannelNumber(AUX_ADCVIN0_GPS), MT6325_CHIP, GPS)==0) {
        	udelay(20);
        	if( (count++) > dvtloop)
		{
			printk("[auxadc_priority_test_5] AUX_ADCVIN0_GPS Time out!\n");
			break;
		}
        }
        output_gps = pmic_get_adc_output(PMIC_IMM_GetChannelNumber(AUX_ADCVIN0_GPS), MT6325_CHIP, GPS);
        if (count < dvtloop) {
        	mt6325_upmu_set_rg_md_rqst(1);
	        while (pmic_is_auxadc_ready(PMIC_IMM_GetChannelNumber(AUX_ADCVIN0_MD), MT6325_CHIP, MD)==0) {
	        	udelay(20);
	        	if( (count++) > dvtloop)
			{
				printk("[auxadc_priority_test_5] AUX_ADCVIN0_MD Time out!\n");
				break;
			}
	        }
	        
	        if (count < dvtloop) {
	        	printk("[auxadc_priority_test_5] Polling Time = %d us\n", count * 20);
	        	output_md = pmic_get_adc_output(PMIC_IMM_GetChannelNumber(AUX_ADCVIN0_MD), MT6325_CHIP, MD);
	        	
	        	printk("[auxadc_priority_test_5] output_md = %d output_gps = %d\n", output_md, output_gps);
	        	if (output_gps == output_md)
	        		printk("[auxadc_priority_test_5] Test Case Pass!\n");
	        }
        }
        	
        
     	
}

#define LBAT_TEST 0
#define HBAT_TEST 1

#define WAKEUP_MODE 0
#define SLEEP_MODE 1


kal_uint32 LBAT_VOLT_MAX=0x0A80; //                      
kal_uint32 LBAT_VOLT_MIN=0x0880; //                      

kal_uint32 LBAT_DET_PRD_19_16=0x0; 
kal_uint32 LBAT_DET_PRD_15_0=0x2710; //   


kal_uint32 LBAT_DEBT_MAX=1;
kal_uint32 LBAT_DEBT_MIN=1;

extern kal_uint32 mt6325_bat_l_int_handler(void);
extern kal_uint32 mt6325_bat_h_int_handler(void);


void auxadc_lbat_int_protection(U32 test_type, U32 test_mode)
{
	
	U32 lbat_debounce_count_max=0;
	U32 lbat_debounce_count_min=0;
	U32 timeout_count = 0;
	U32 adc_out_lbat = 0;
	U32 adc_result = 0;

    //    
	mt6325_upmu_set_rg_lbat_irq_en_max(0);
	mt6325_upmu_set_rg_lbat_irq_en_min(0);	
	mt6325_upmu_set_rg_lbat_en_max(0);
	mt6325_upmu_set_rg_lbat_en_min(0);

	//                                           
	mt6325_upmu_set_rg_lbat_volt_max(LBAT_VOLT_MAX);
	//                                           
	mt6325_upmu_set_rg_lbat_volt_min(LBAT_VOLT_MIN);
	//                         
	mt6325_upmu_set_rg_lbat_det_prd_19_16(LBAT_DET_PRD_19_16);
	mt6325_upmu_set_rg_lbat_det_prd_15_0(LBAT_DET_PRD_15_0);
	//                                 
	mt6325_upmu_set_rg_lbat_debt_max(LBAT_DEBT_MAX);
	mt6325_upmu_set_rg_lbat_debt_min(LBAT_DEBT_MIN);
	
    
	if (test_type == LBAT_TEST) {
		//              
		mt6325_upmu_set_rg_lbat_irq_en_max(0);
		mt6325_upmu_set_rg_lbat_irq_en_min(1);
	
		//                               
		mt6325_upmu_set_rg_lbat_en_max(0);
		mt6325_upmu_set_rg_lbat_en_min(1);
	}else if (test_type == HBAT_TEST) {
		//              
		mt6325_upmu_set_rg_lbat_irq_en_max(1);
		mt6325_upmu_set_rg_lbat_irq_en_min(0);
	
		//                               
		mt6325_upmu_set_rg_lbat_en_max(1);
		mt6325_upmu_set_rg_lbat_en_min(0);
	}
	//                          
	lbat_debounce_count_max = mt6325_upmu_get_rg_lbat_debounce_count_max();
	lbat_debounce_count_min = mt6325_upmu_get_rg_lbat_debounce_count_min();
	
	while (!mt6325_upmu_get_rg_adc_rdy_lbat()) {
		mdelay(1000);
		timeout_count++;
		printk("[auxadc_battery_int_protection_6325] wait for lbat ready! timeout_count = (%d)\n", timeout_count);
		if (timeout_count > 10000) break;	
	} 
	adc_out_lbat = mt6325_upmu_get_rg_adc_out_lbat();
	adc_result = (adc_out_lbat*4*1800)/4096;
	printk("raw = %d , adc_result = %d\n", adc_out_lbat, adc_result);
	//                                                                       
	//                                                                       
	printk("[auxadc_battery_int_protection_6325] setting .. done (adc_out_lbat=%d, lbat_debounce_count_max=%d, lbat_debounce_count_min=%d) \n", 
		adc_out_lbat, lbat_debounce_count_max, lbat_debounce_count_min);
		
#if 0		
	if (test_mode == SLEEP_MODE) {
		printk("[auxadc_battery_int_protection_6325] sleep mode\n");
		//                                       
		mt6325_upmu_set_strup_auxadc_start_sel(0);
		mt6325_upmu_set_strup_auxadc_rstb_sw(1);
		mt6325_upmu_set_strup_auxadc_rstb_sel(1);
		//                                            
		//            
    		//             
		set_srclken_sw_mode();
		set_srclken_0_val(0);
	}
#endif
}

void mt6325_bat_int_close(void)
{
	mt6325_upmu_set_rg_lbat_irq_en_max(0);
	mt6325_upmu_set_rg_lbat_irq_en_min(0);	
	mt6325_upmu_set_rg_lbat_en_max(0);
	mt6325_upmu_set_rg_lbat_en_min(0);
	
	printk("[mt6325_bat_int_close] done\n");
}

void auxadc_lbat2_int_protection(U32 test_type, U32 test_mode)
{
	
	U32 lbat2_debounce_count_max=0;
	U32 lbat2_debounce_count_min=0;
	U32 timeout_count = 0;
	U32 adc_out_lbat2 = 0;

	U32 adc_result = 0;
    //    
	mt6325_upmu_set_auxadc_lbat2_irq_en_max(0);
	mt6325_upmu_set_auxadc_lbat2_irq_en_min(0);	
	mt6325_upmu_set_auxadc_lbat2_en_max(0);
	mt6325_upmu_set_auxadc_lbat2_en_min(0);

	//                                           
	mt6325_upmu_set_auxadc_lbat2_volt_max(LBAT_VOLT_MAX);
	//                                           
	mt6325_upmu_set_auxadc_lbat2_volt_min(LBAT_VOLT_MIN);
	//                         
	mt6325_upmu_set_auxadc_lbat2_det_prd_19_16(LBAT_DET_PRD_19_16);
	mt6325_upmu_set_auxadc_lbat2_det_prd_15_0(LBAT_DET_PRD_15_0);
	//                                 
	mt6325_upmu_set_auxadc_lbat2_debt_max(LBAT_DEBT_MAX);
	mt6325_upmu_set_auxadc_lbat2_debt_min(LBAT_DEBT_MIN);
	
    
	if (test_type == LBAT_TEST) {
		//              
		mt6325_upmu_set_auxadc_lbat2_irq_en_max(0);
		mt6325_upmu_set_auxadc_lbat2_irq_en_min(1);
	
		//                               
		mt6325_upmu_set_auxadc_lbat2_en_max(0);
		mt6325_upmu_set_auxadc_lbat2_en_min(1);
	}else if (test_type == HBAT_TEST) {
		//              
		mt6325_upmu_set_auxadc_lbat2_irq_en_max(1);
		mt6325_upmu_set_auxadc_lbat2_irq_en_min(0);
	
		//                               
		mt6325_upmu_set_auxadc_lbat2_en_max(1);
		mt6325_upmu_set_auxadc_lbat2_en_min(0);
	}
	//                          
	lbat2_debounce_count_max = mt6325_upmu_get_auxadc_lbat2_debounce_count_max();
	lbat2_debounce_count_min = mt6325_upmu_get_auxadc_lbat2_debounce_count_min();
	
	while (!mt6325_upmu_get_rg_adc_rdy_lbat2()) {
		mdelay(1000);
		timeout_count++;
		printk("[auxadc_lbat2_int_protection_6325] wait for lbat ready! timeout_count = (%d)\n", timeout_count);
		if (timeout_count > 10000) break;	
	} 
	adc_out_lbat2 = mt6325_upmu_get_rg_adc_out_lbat2();
	adc_result = (adc_out_lbat2*4*1800)/4096;
	printk("raw = %d , adc_result = %d\n", adc_out_lbat2, adc_result);
	//                                                                       
	//                                                                       
	printk("[auxadc_lbat2_int_protection_6325] setting .. done (adc_out_lbat2=%d, lbat2_debounce_count_max=%d, lbat2_debounce_count_min=%d) \n", 
		adc_out_lbat2, lbat2_debounce_count_max, lbat2_debounce_count_min);
		
#if 0		
	if (test_mode == SLEEP_MODE) {
		printk("[auxadc_lbat2_int_protection_6325] sleep mode\n");
		//                                       
		mt6331_upmu_set_strup_auxadc_start_sel(0);
		mt6331_upmu_set_strup_auxadc_rstb_sw(1);
		mt6331_upmu_set_strup_auxadc_rstb_sel(1);
		//                                            
		//            
    		//             
		set_srclken_sw_mode();
		set_srclken_0_val(0);
	}
#endif
}

void mt6325_bat2_int_close(void)
{
	mt6325_upmu_set_auxadc_lbat2_irq_en_max(0);
	mt6325_upmu_set_auxadc_lbat2_irq_en_min(0);
	mt6325_upmu_set_auxadc_lbat2_en_max(0);
	mt6325_upmu_set_auxadc_lbat2_en_min(0);
			
	printk("[mt6325_bat2_int_close] done\n");
}
#if 1
//                                        
void auxadc_low_battery_int_wakeup(void) 
{
	auxadc_lbat_int_protection(LBAT_TEST, WAKEUP_MODE);
}
//                                                 
void auxadc_high_battery_int_wakeup(void) 
{
	auxadc_lbat_int_protection(HBAT_TEST, WAKEUP_MODE);
}
//                                                               
void auxadc_low_battery_int_sleep(void) 
{
	auxadc_lbat_int_protection(LBAT_TEST, SLEEP_MODE);
}
//                                                               
void auxadc_high_battery_int_sleep(void) 
{
	auxadc_lbat_int_protection(HBAT_TEST, SLEEP_MODE);
}
//                                        
void auxadc_low_battery2_int_wakeup(void) 
{
	auxadc_lbat2_int_protection(LBAT_TEST, WAKEUP_MODE);
}
//                                                 
void auxadc_high_battery2_int_wakeup(void) 
{
	auxadc_lbat2_int_protection(HBAT_TEST, WAKEUP_MODE);
}
//                                                               
void auxadc_low_battery2_int_sleep(void) 
{
	auxadc_lbat2_int_protection(LBAT_TEST, SLEEP_MODE);
}
//                                                               
void auxadc_high_battery2_int_sleep(void) 
{
	auxadc_lbat2_int_protection(HBAT_TEST, SLEEP_MODE);
}
#endif

//                                             
#define LTHR_TEST 0
#define HTHR_TEST 1

kal_uint32 THR_VOLT_MAX=0x0340; //                    
kal_uint32 THR_VOLT_MIN=0x03c0; //                     

kal_uint32 THR_DET_PRD_19_16=0x0; 
kal_uint32 THR_DET_PRD_15_0=0x2710; //    


kal_uint32 THR_DEBT_MAX=1;
kal_uint32 THR_DEBT_MIN=1;

extern kal_uint32 mt6325_thr_l_int_handler(void);
extern kal_uint32 mt6325_thr_h_int_handler(void);
void auxadc_thermal_int_protection_6325(U32 test_type, U32 test_mode)
{
	
	U32 thr_debounce_count_max=0;
	U32 thr_debounce_count_min=0;
	U32 timeout_count = 0;
	U32 adc_out_thr = 0;
	

	//                      
	mt6325_upmu_set_rg_int_en_thr_l(1);
	mt6325_upmu_set_rg_int_en_thr_h(1);
	//                                           
	mt6325_upmu_set_auxadc_thr_volt_max(THR_VOLT_MAX);
	//                                           
	mt6325_upmu_set_auxadc_thr_volt_min(THR_VOLT_MIN);
	//                         
	mt6325_upmu_set_auxadc_thr_det_prd_19_16(THR_DET_PRD_19_16);
	mt6325_upmu_set_auxadc_thr_det_prd_15_0(THR_DET_PRD_15_0);
	//                                 
	mt6325_upmu_set_auxadc_thr_debt_max(THR_DEBT_MAX);
	mt6325_upmu_set_auxadc_thr_debt_min(THR_DEBT_MIN);
	
	if (test_type == LTHR_TEST) {
		//              
		mt6325_upmu_set_auxadc_thr_irq_en_max(0);
		mt6325_upmu_set_auxadc_thr_irq_en_min(1);
	
		//                               
		mt6325_upmu_set_auxadc_thr_en_max(0);
		mt6325_upmu_set_auxadc_thr_en_min(1);
	}else if (test_type == HTHR_TEST) {
		//              
		mt6325_upmu_set_auxadc_thr_irq_en_max(1);
		mt6325_upmu_set_auxadc_thr_irq_en_min(0);
	
		//                                
		mt6325_upmu_set_auxadc_thr_en_max(1);
		mt6325_upmu_set_auxadc_thr_en_min(0);
	}
	//                          
	thr_debounce_count_max = mt6325_upmu_get_auxadc_thr_debounce_count_max();
	thr_debounce_count_min = mt6325_upmu_get_auxadc_thr_debounce_count_min();
	
	while (!mt6325_upmu_get_rg_adc_rdy_thr_hw()) {
		timeout_count++;
		printk("[auxadc_thermal_int_protection_6325] wait for lbat ready! timeout_count = (%d)\n", timeout_count);
		if (timeout_count > 10000) break;	
	} 
	adc_out_thr = mt6325_upmu_get_rg_adc_out_thr_hw();
	
	//                                                                       
	//                                                                       
	printk("[auxadc_thermal_int_protection_6325] done (adc_out_thr=%d, thr_debounce_count_max=%d, thr_debounce_count_min=%d) \n", 
		adc_out_thr, thr_debounce_count_max, thr_debounce_count_min);
#if 0		
	if (test_mode == SLEEP_MODE) {
		printk("[auxadc_thermal_int_protection_6325] sleep mode\n");
		//                                       
		mt6325_upmu_set_strup_auxadc_start_sel(0);
		mt6325_upmu_set_strup_auxadc_rstb_sw(1);
		mt6325_upmu_set_strup_auxadc_rstb_sel(1);
		//                                             
		
		//            
		//             
		set_srclken_sw_mode();
		set_srclken_0_val(0);	
	}
#endif
}

void mt6325_thr_int_close(void)
{
	mt6325_upmu_set_auxadc_thr_irq_en_max(0);
	mt6325_upmu_set_auxadc_thr_irq_en_min(0);
	mt6325_upmu_set_auxadc_thr_en_max(0);
	mt6325_upmu_set_auxadc_thr_en_min(0);
	mt6325_upmu_set_rg_int_en_thr_l(0);
	mt6325_upmu_set_rg_int_en_thr_h(0);
	
	printk("[mt6325_thr_int_close] done\n");
}

//                                        
void auxadc_low_thermal_int_test_wakeup(void) 
{
	auxadc_thermal_int_protection_6325(LTHR_TEST, WAKEUP_MODE);
}
//                                                 
void auxadc_high_thermal_int_test_wakeup(void) 
{
	auxadc_thermal_int_protection_6325(HTHR_TEST, WAKEUP_MODE);
}
//                                                               
void auxadc_low_thermal_int_test_sleep(void) 
{
	auxadc_thermal_int_protection_6325(LTHR_TEST, SLEEP_MODE);
}
//                                                               
void auxadc_high_thermal_int_test_sleep(void) 
{
	auxadc_thermal_int_protection_6325(HTHR_TEST, SLEEP_MODE);
}

//                                             
#define LVISMPS_TEST 0
#define HVISMPS_TEST 1

kal_uint32 VISMPS_VOLT_MAX=0x0340; //                    
kal_uint32 VISMPS_VOLT_MIN=0x03c0; //                     

kal_uint32 VISMPS_DET_PRD_19_16=0x0; 
kal_uint32 VISMPS_DET_PRD_15_0=0x2710; //    


kal_uint32 VISMPS_DEBT_MAX=1;
kal_uint32 VISMPS_DEBT_MIN=1;

extern kal_uint32 mt6325_vismp_l_int_handler(void);
extern kal_uint32 mt6325_vismp_h_int_handler(void);

void auxadc_vismps_int_protection_6325(U32 test_type, U32 test_mode)
{
	
	U32 vismps_debounce_count_max=0;
	U32 vismps_debounce_count_min=0;
	U32 timeout_count = 0;
	U32 adc_out_vismps = 0;
	U32 adc_result = 0;

	//                      
	mt6325_upmu_set_rg_int_en_vismps0_l(1);
	mt6325_upmu_set_rg_int_en_vismps0_h(1);
	//                                           
	mt6325_upmu_set_auxadc_vismps0_volt_max(VISMPS_VOLT_MAX);
	//                                           
	mt6325_upmu_set_auxadc_vismps0_volt_min(VISMPS_VOLT_MIN);
	//                         
	mt6325_upmu_set_auxadc_vismps0_det_prd_19_16(VISMPS_DET_PRD_19_16);
	mt6325_upmu_set_auxadc_vismps0_det_prd_15_0(VISMPS_DET_PRD_15_0);
	//                                 
	mt6325_upmu_set_auxadc_vismps0_debt_max(VISMPS_DEBT_MAX);
	mt6325_upmu_set_auxadc_vismps0_debt_min(VISMPS_DEBT_MIN);
	
	if (test_type == LVISMPS_TEST) {
		//              
		mt6325_upmu_set_auxadc_vismps0_irq_en_max(0);
		mt6325_upmu_set_auxadc_vismps0_irq_en_min(1);
	
		//                              
		mt6325_upmu_set_auxadc_vismps0_en_max(0);
		mt6325_upmu_set_auxadc_vismps0_en_min(1);
	}else if (test_type == HVISMPS_TEST) {
		//              
		mt6325_upmu_set_auxadc_vismps0_irq_en_max(0);
		mt6325_upmu_set_auxadc_vismps0_irq_en_min(1);
	
		//                               
		mt6325_upmu_set_auxadc_vismps0_en_max(0);
		mt6325_upmu_set_auxadc_vismps0_en_min(1);
	}
	//                          
	vismps_debounce_count_max = mt6325_upmu_get_auxadc_vismps0_debounce_count_max();
	vismps_debounce_count_min = mt6325_upmu_get_auxadc_vismps0_debounce_count_min();
	
	while (!mt6325_upmu_get_rg_adc_rdy_vismps0()) {
		timeout_count++;
		printk("[auxadc_vismps_int_protection_6325] wait for vismps ready! timeout_count = (%d)\n", timeout_count);
		if (timeout_count > 10000) break;	
	} 
	adc_out_vismps = mt6325_upmu_get_rg_adc_out_vismps0();
	adc_result = (adc_out_vismps*1800)/4096;
	printk("raw = %d , adc_result = %d\n", adc_out_vismps, adc_result);
	//                                                                          
	//                                                                          
	printk("[auxadc_vismps_int_protection_6325] done (adc_out_vismps=%d, vismps_debounce_count_max=%d, vismps_debounce_count_min=%d) \n", 
		adc_out_vismps, vismps_debounce_count_max, vismps_debounce_count_min);
#if 0
	if (test_mode == SLEEP_MODE) {
		printk("[auxadc_thermal_int_protection_6331] sleep mode\n");
		//                                       
		mt6331_upmu_set_strup_auxadc_start_sel(0);
		mt6331_upmu_set_strup_auxadc_rstb_sw(1);
		mt6331_upmu_set_strup_auxadc_rstb_sel(1);
		//                                             
		
		//            
		//             
		set_srclken_sw_mode();
		set_srclken_1_val(0);	
	}
#endif
}
		
void mt6325_vismps_int_close(void)
{
	mt6325_upmu_set_auxadc_vismps0_irq_en_max(0);
	mt6325_upmu_set_auxadc_vismps0_irq_en_min(0);
	mt6325_upmu_set_auxadc_vismps0_en_max(0);
	mt6325_upmu_set_auxadc_vismps0_en_min(0);
	mt6325_upmu_set_rg_int_en_vismps0_l(0);
	mt6325_upmu_set_rg_int_en_vismps0_h(0);
	
	printk("[mt6325_vismps_int_close] done\n");
}

//                                       
void auxadc_low_vismps_int_test_wakeup(void) 
{
	auxadc_vismps_int_protection_6325(LTHR_TEST, WAKEUP_MODE);
}
//                                                
void auxadc_high_vismps_int_test_wakeup(void) 
{
	auxadc_vismps_int_protection_6325(HTHR_TEST, WAKEUP_MODE);
}
//                                                              
void auxadc_low_vismps_int_test_sleep(void) 
{
	auxadc_vismps_int_protection_6325(LTHR_TEST, SLEEP_MODE);
}
//                                                              
void auxadc_high_vismps_int_test_sleep(void) 
{
	auxadc_vismps_int_protection_6325(HTHR_TEST, SLEEP_MODE);
}

//                                                   

#define POWER_ON_MODE 0
#define WAKE_UP_MODE 1
void auxadc_battery_wakeup_pchr_measure_6325(U32 test_mode)
{
	U32 count = 0;
	U32 adc_result = 0;
	U32 wakeup_volt = 0;
	
	//                                                 
	if (test_mode == WAKE_UP_MODE) {
		printk("[auxadc_battery_wakeup_pchr_measure_6325] WAKEUP mode setting\n");
		//                                       
		mt6325_upmu_set_strup_auxadc_start_sel(0);
		mt6325_upmu_set_strup_auxadc_rstb_sw(1);
		mt6325_upmu_set_strup_auxadc_rstb_sel(1);
		//            
    		//             
		set_srclken_sw_mode();
		set_srclken_0_val(0);
		set_srclken_0_val(1);
	}
	
	while (!mt6325_upmu_get_rg_adc_rdy_wakeup_pchr()){
		count++;
		if (count> 100) {
			printk("[auxadc_battery_wakeup_pchr_measure_6325] WHILE LOOP TIMEOUT!!\n");
			break;
		}
	}
	
	if (mt6325_upmu_get_rg_adc_rdy_wakeup_pchr()){
		wakeup_volt = mt6325_upmu_get_rg_adc_out_wakeup_pchr();
		printk("[auxadc_battery_wakeup_pchr_measure_6325] wakeup_volt = %d\n", wakeup_volt);	
		adc_result = mt6325_upmu_get_rg_adc_out_wakeup_pchr();
		wakeup_volt = (adc_result * 4 * 1800) / 32768;
		printk("[auxadc_battery_wakeup_pchr_measure_6325] adc_result = %d, wakeup_volt = %d\n", adc_result, wakeup_volt);	
	}	
}

void auxadc_battery_wakeup_swchr_measure_6325(U32 test_mode)
{
	U32 count = 0;
	U32 adc_result = 0;
	U32 wakeup_volt = 0;
	//                
	if (test_mode == WAKE_UP_MODE) {
		printk("[auxadc_battery_wakeup_swchr_measure_6325] WAKEUP mode setting\n");
		//                                       
		mt6325_upmu_set_strup_auxadc_start_sel(0);
		mt6325_upmu_set_strup_auxadc_rstb_sw(1);
		mt6325_upmu_set_strup_auxadc_rstb_sel(1);
		//            
    		//             
		set_srclken_sw_mode();
		set_srclken_0_val(0);
		set_srclken_0_val(1);
	}
	
	while (!mt6325_upmu_get_rg_adc_rdy_wakeup_swchr()){
		count++;
		if (count> 100) {
			printk("[auxadc_battery_wakeup_swchr_measure_6325] WHILE LOOP TIMEOUT!!\n");
			break;
		}
	}
	
	if (mt6325_upmu_get_rg_adc_rdy_wakeup_swchr()){
		wakeup_volt = mt6325_upmu_get_rg_adc_out_wakeup_swchr();
		printk("[auxadc_battery_wakeup_swchr_measure_6325] wakeup_volt = %d\n", wakeup_volt);	
		adc_result = mt6325_upmu_get_rg_adc_out_wakeup_swchr();
		wakeup_volt = (adc_result * 4 * 1800) / 32768;
		printk("[auxadc_battery_wakeup_swchr_measure_6325] adc_result = %d, wakeup_volt = %d\n", adc_result, wakeup_volt);
	}	
}

void auxadc_measure_poweron_test_pchr(void)
{
	auxadc_battery_wakeup_pchr_measure_6325(POWER_ON_MODE);
}

void auxadc_measure_wakeup_test_pchr(void)
{
	auxadc_battery_wakeup_pchr_measure_6325(WAKE_UP_MODE);
}

void auxadc_measure_poweron_test_swchr(void)
{
	auxadc_battery_wakeup_swchr_measure_6325(POWER_ON_MODE);
}

void auxadc_measure_wakeup_test_swchr(void)
{
	auxadc_battery_wakeup_swchr_measure_6325(WAKE_UP_MODE);
}

//                                                        
void auxadc_swctrl_measure_test_6325(void)
{
	kal_int32 data;
/*
               
               
             
              
               
                
               
                 
                 
           
             

                
                
*/
	//      

	mt6325_upmu_set_rg_adc_swctrl_en(1);

	mt6325_upmu_set_rg_adcin_vsen_en(0);
	mt6325_upmu_set_rg_adcin_vbat_en(1);
	mt6325_upmu_set_rg_adcin_vsen_mux_en(0);
	mt6325_upmu_set_rg_adcin_vsen_ext_baton_en(0);
	mt6325_upmu_set_rg_adcin_chr_en(0);
	mt6325_upmu_set_baton_tdet_en(0);
	
	mt6325_upmu_set_rg_baton_en(0);
	mt6325_upmu_set_rg_auxadc_chsel(PMIC_IMM_GetChannelNumber(AUX_BATSNS_AP));

	printk("[polling batsns]");
	data=PMIC_IMM_GetOneChannelValue(AUX_BATSNS_AP,1,1);
	printk("[batsns :%d \n",data);



	//      

	mt6325_upmu_set_rg_adc_swctrl_en(1);

	mt6325_upmu_set_rg_adcin_vsen_en(1);
	mt6325_upmu_set_rg_adcin_vbat_en(0);
	mt6325_upmu_set_rg_adcin_vsen_mux_en(1);
	mt6325_upmu_set_rg_adcin_vsen_ext_baton_en(0);
	mt6325_upmu_set_rg_adcin_chr_en(0);
	mt6325_upmu_set_baton_tdet_en(0);
	
	mt6325_upmu_set_rg_baton_en(0);
	mt6325_upmu_set_rg_auxadc_chsel(PMIC_IMM_GetChannelNumber(AUX_ISENSE_AP));

	printk("[polling isense]");
	data=PMIC_IMM_GetOneChannelValue(AUX_ISENSE_AP,1,1);
	printk("[isense :%d \n",data);

	//     

	mt6325_upmu_set_rg_adc_swctrl_en(1);

	mt6325_upmu_set_rg_adcin_vsen_en(0);
	mt6325_upmu_set_rg_adcin_vbat_en(0);
	mt6325_upmu_set_rg_adcin_vsen_mux_en(0);
	mt6325_upmu_set_rg_adcin_vsen_ext_baton_en(0);
	mt6325_upmu_set_rg_adcin_chr_en(1);
	mt6325_upmu_set_baton_tdet_en(0);
	
	mt6325_upmu_set_rg_baton_en(0);
	mt6325_upmu_set_rg_auxadc_chsel(PMIC_IMM_GetChannelNumber(AUX_VCDT_AP));

	printk("[polling vcdt]");
	data=PMIC_IMM_GetOneChannelValue(AUX_VCDT_AP,1,1);
	printk("[vcdt :%d \n",data);

	//      

	mt6325_upmu_set_rg_adc_swctrl_en(1);

	mt6325_upmu_set_rg_adcin_vsen_en(0);
	mt6325_upmu_set_rg_adcin_vbat_en(0);
	mt6325_upmu_set_rg_adcin_vsen_mux_en(0);
	mt6325_upmu_set_rg_adcin_vsen_ext_baton_en(0);
	mt6325_upmu_set_rg_adcin_chr_en(0);
	mt6325_upmu_set_baton_tdet_en(1);
	
	mt6325_upmu_set_rg_baton_en(0);
	mt6325_upmu_set_rg_auxadc_chsel(PMIC_IMM_GetChannelNumber(AUX_BATON_AP));

	printk("[polling baton1]");
	data=PMIC_IMM_GetOneChannelValue(AUX_BATON_AP,1,1);
	printk("[baton1 :%d \n",data);

	//          

	mt6325_upmu_set_rg_adc_swctrl_en(1);

	mt6325_upmu_set_baton_tdet_en(1);
	mt6325_upmu_set_rg_vbuf_calen(1);
	mt6325_upmu_set_rg_vbuf_en(1);
	
	mt6325_upmu_set_rg_auxadc_chsel(PMIC_IMM_GetChannelNumber(AUX_TSENSE_AP));
	
	printk("[polling THR_SENSE1]");
	data=PMIC_IMM_GetOneChannelValue(AUX_TSENSE_AP,1,1);
	printk("[THR_SENSE1 :%d \n",data);
	
	//          

	mt6325_upmu_set_rg_adc_swctrl_en(1);
	mt6325_upmu_set_rg_adc_swctrl_en(1);

	mt6325_upmu_set_rg_adcin_vsen_en(0);
	mt6325_upmu_set_rg_adcin_vbat_en(0);
	mt6325_upmu_set_rg_adcin_vsen_mux_en(0);
	mt6325_upmu_set_rg_adcin_vsen_ext_baton_en(1);
	mt6325_upmu_set_rg_adcin_chr_en(0);
	mt6325_upmu_set_baton_tdet_en(0);
	
	mt6325_upmu_set_rg_baton_en(0);
	mt6325_upmu_set_rg_vbuf_calen(1);
	mt6325_upmu_set_rg_auxadc_chsel(4);

	printk("[polling THR SENSE2]");
	data=PMIC_IMM_GetOneChannelValue(3,1,1);
	printk("[THR_SENSE2 :%d \n",data);
	
}

void auxadc_impendance_batsns_test(void)
{
	int count = 0;
	
	mt6325_upmu_set_auxadc_impedance_chsel(0);
	mt6325_upmu_set_auxadc_autorpt_en(1);
	mt6325_upmu_set_auxadc_impedance_cnt(4);//                              
	mt6325_upmu_set_auxadc_impedance_mode(1);
	
	while (mt6325_upmu_get_auxadc_impedance_irq_status() == 1) {
		count++;
		if (count > dvtloop)
			break;
		
	}
	if (count < dvtloop) {
		//                        
		mt6325_upmu_set_auxadc_clr_imp_cnt_stop(1);
		mt6325_upmu_set_auxadc_impedance_irq_clr(1);
		//                        
		mt6325_upmu_set_auxadc_clr_imp_cnt_stop(0);
		mt6325_upmu_set_auxadc_impedance_irq_clr(0);
		printk("[auxadc_impendance_batsns_test] Test case pass! \n");
	}
}

void auxadc_impendance_isense_test(void)
{
	int count = 0;
	
	mt6325_upmu_set_auxadc_impedance_chsel(1);
	mt6325_upmu_set_auxadc_autorpt_en(1);
	mt6325_upmu_set_auxadc_impedance_cnt(4);//                              
	mt6325_upmu_set_auxadc_impedance_mode(1);
	
	while (mt6325_upmu_get_auxadc_impedance_irq_status() != 1) {
		count++;
		if (count > dvtloop)
			break;
		
	}
	if (count < dvtloop) {
		//                        
		mt6325_upmu_set_auxadc_clr_imp_cnt_stop(1);
		mt6325_upmu_set_auxadc_impedance_irq_clr(1);
		//                        
		mt6325_upmu_set_auxadc_clr_imp_cnt_stop(0);
		mt6325_upmu_set_auxadc_impedance_irq_clr(0);
		printk("[auxadc_impendance_isense_test] Test case pass! \n");
	}
}

#endif
#ifdef RTC_DVT_TEST
extern struct rtc_time irqtm;
extern void RTC_XOSC32_ENB_Ball_ldvt(void);
extern void RTC_Tick_Interrupt_ldvt(void);
extern void RTC_Alarm_Interrupt_ldvt(void);
extern void RTC_Write_Trigger_ldvt(void);
extern void RTC_BBPU_bit_ldvt(void);
extern void RTC_One_Shot_Calibration_ldvt(void);
extern void RTC_Repeat_Calibration_ldvt(int i);
extern void RTC_CLRPKY_Bit_ldvt(int i);
extern void RTC_Alarm_ldvt(void);
extern void RTC_Spare_Register_ldvt(void);
extern void RTC_AutoReload_After_Reboot_ldvt(void);
extern void RTC_Tick_Check_ldvt(void);
extern void RTC_Auto_test_ldvt(void);
extern void RTC_Protect_ldvt(void);
extern void RTC_32K_Stop_ldvt(void);
extern void RTC_Vcore_Off_ldvt(void);
extern void RTC_Powerkey_Unmatch_ldvt(void);
extern void RTC_OSC32_Con_Read_Write_ldvt(void);
extern void RTC_EMBCK_SEL_MODE_ldvt(void);
extern void RTC_2SEC_MODE_TEST(int i);
extern void rtc_hw_init(bool writeif_lock_test);
extern void RTC_LPD_VCCmin_test(int i);
extern void RTC_read_time(struct rtc_time *tm);
extern void ts_rtc_Init(void);
#endif

extern void mt6311_int_test(void);

void pmic_dvt_entry(int test_id)
{
    printk("[pmic_dvt_entry] start : test_id=%d\n", test_id);

    pmic_config_interface(0x2BC,0x1,0xFF,0);
    pmic_config_interface(0x26C,0x2,0xFF,0);
    printk("[0x%x]=0x%x,[0x%x]=0x%x,[0x%x]=0x%x\n", 
        0x2BC,upmu_get_reg_value(0x2BC),
        0x268,upmu_get_reg_value(0x268),
        0x26C,upmu_get_reg_value(0x26C)
        );

    switch (test_id)
    {
        case 0: read_adc_value(ADC_ONLY); break;

        //             
        case 1: top_6325_read();        break;
        case 2: top_6325_write(0x5a5a); break;
        case 3: top_6325_write(0xa5a5); break;
        case 4: top_6311_read();        break;
        case 5: top_6311_write(0x5a);   break;
        case 6: top_6311_write(0xa5);   break;

        case 10:
            printk("set_srclken_sw_mode(); set_srclken_0_val(0); set_srclken_1_val(0);\n");
                    set_srclken_sw_mode(); set_srclken_0_val(0); set_srclken_1_val(0); break;
        case 11: 
            printk("set_srclken_sw_mode(); set_srclken_0_val(0); set_srclken_1_val(1);\n");
                    set_srclken_sw_mode(); set_srclken_0_val(0); set_srclken_1_val(1); break;
        case 12: 
            printk("set_srclken_sw_mode(); set_srclken_0_val(1); set_srclken_1_val(0);\n");
                    set_srclken_sw_mode(); set_srclken_0_val(1); set_srclken_1_val(0); break;
        case 13: 
            printk("set_srclken_sw_mode(); set_srclken_0_val(1); set_srclken_1_val(1);\n");
                    set_srclken_sw_mode(); set_srclken_0_val(1); set_srclken_1_val(1); break;
        
        //    
        case 1000: tc_bif_1000(); break;
        case 1001: tc_bif_1001(); break;
        case 1002: tc_bif_1002(); break;
        case 1003: tc_bif_1003(); break;
        case 1004: tc_bif_1004(); break;
        case 1005: tc_bif_1005(); break;
        case 1006: tc_bif_1006(); break;
        case 1007: tc_bif_1007(); break; //    
        case 1008: tc_bif_1008(); break; //    

        //    
        case 2000: PMIC_BUCK_ON_OFF(VCORE1_INDEX);  break;
        case 2001: PMIC_BUCK_ON_OFF(VCORE2_INDEX);  break;
        case 2002: PMIC_BUCK_ON_OFF(VDRAM_INDEX);   break;
        case 2003: PMIC_BUCK_ON_OFF(VGPU_INDEX);    break;
        case 2004: PMIC_BUCK_ON_OFF(VRF18_0_INDEX); break;
        case 2005: PMIC_BUCK_ON_OFF(VIO18_INDEX);   break;
        case 2006: PMIC_BUCK_ON_OFF(VDVFS11_INDEX); break;
        case 2007: PMIC_BUCK_ON_OFF(VDVFS12_INDEX); break;
        case 2008: PMIC_BUCK_ON_OFF(VPA_INDEX);     break;

        case 2010: PMIC_BUCK_VOSEL(VCORE1_INDEX);  break;
        case 2011: PMIC_BUCK_VOSEL(VCORE2_INDEX);  break;
        case 2012: PMIC_BUCK_VOSEL(VDRAM_INDEX);   break;
        case 2013: PMIC_BUCK_VOSEL(VGPU_INDEX);    break;
        case 2014: PMIC_BUCK_VOSEL(VRF18_0_INDEX); break;
        case 2015: PMIC_BUCK_VOSEL(VIO18_INDEX);   break;
        case 2016: PMIC_BUCK_VOSEL(VDVFS11_INDEX); break;
        case 2017: PMIC_BUCK_VOSEL(VDVFS12_INDEX); break;
        case 2018: PMIC_BUCK_VOSEL(VPA_INDEX);     break;

        case 2020: PMIC_BUCK_DLC(VCORE1_INDEX);  break;
        case 2021: PMIC_BUCK_DLC(VCORE2_INDEX);  break;
        case 2022: PMIC_BUCK_DLC(VDRAM_INDEX);   break;
        case 2023: PMIC_BUCK_DLC(VGPU_INDEX);    break;
        case 2024: PMIC_BUCK_DLC(VRF18_0_INDEX); break;
        case 2025: PMIC_BUCK_DLC(VIO18_INDEX);   break;
        case 2026: PMIC_BUCK_DLC(VDVFS11_INDEX); break;
        case 2027: PMIC_BUCK_DLC(VDVFS12_INDEX); break;
        case 2028: PMIC_BUCK_DLC(VPA_INDEX);     break;

        case 2030: PMIC_BUCK_BURST(VCORE1_INDEX);  break;
        case 2031: PMIC_BUCK_BURST(VCORE2_INDEX);  break;
        case 2032: PMIC_BUCK_BURST(VDRAM_INDEX);   break;
        case 2033: PMIC_BUCK_BURST(VGPU_INDEX);    break;
        case 2034: PMIC_BUCK_BURST(VRF18_0_INDEX); break;
        case 2035: PMIC_BUCK_BURST(VIO18_INDEX);   break;
        case 2036: PMIC_BUCK_BURST(VDVFS11_INDEX); break;
        case 2037: PMIC_BUCK_BURST(VDVFS12_INDEX); break;
        case 2038: PMIC_BUCK_BURST(VPA_INDEX);     break;

        //           
        case 2100: PMIC_BUCK_ON_OFF(MT6311_VDVFS11_INDEX);  break;
        case 2101: PMIC_BUCK_ON_OFF(MT6311_VDVFS12_INDEX);  break;

        case 2110: PMIC_BUCK_VOSEL(MT6311_VDVFS11_INDEX);  break;
        case 2111: PMIC_BUCK_VOSEL(MT6311_VDVFS12_INDEX);  break;

        //           
        case 3000: exec_6325_ldo_vtcxo0_en_test();      break;
        case 3001: exec_6325_ldo_vtcxo1_en_test();      break;
        case 3002: exec_6325_ldo_vaud28_en_test();      break;
        case 3003: exec_6325_ldo_vauxa28_en_test();     break;
        case 3004: exec_6325_ldo_vbif28_en_test();      break;
        case 3005: exec_6325_ldo_vcama_en_test();       break;
        case 3006: exec_6325_ldo_vcn28_en_test();       break;
        case 3007: exec_6325_ldo_vcn33_en_test();       break;
        case 3008: exec_6325_ldo_vrf18_1_en_test();     break;
        case 3009: exec_6325_ldo_vusb33_en_test();      break;
        case 3010: exec_6325_ldo_vmch_en_test();        break;
        case 3011: exec_6325_ldo_vmc_en_test();         break;
        case 3012: exec_6325_ldo_vemc33_en_test();      break;
        case 3013: exec_6325_ldo_vio28_en_test();       break;
        case 3014: exec_6325_ldo_vcam_af_en_test();     break;
        case 3015: exec_6325_ldo_vgp1_en_test();        break;
        case 3016: exec_6325_ldo_vefuse_en_test();      break;
        case 3017: exec_6325_ldo_vsim1_en_test();       break;
        case 3018: exec_6325_ldo_vsim2_en_test();       break;
        case 3019: exec_6325_ldo_vmipi_en_test();       break;
        case 3020: exec_6325_ldo_vcn18_en_test();       break;
        case 3021: exec_6325_ldo_vgp2_en_test();        break;
        case 3022: exec_6325_ldo_vcamd_en_test();       break;
        case 3023: exec_6325_ldo_vcam_io_en_test();     break;
        case 3024: exec_6325_ldo_vsram_dvfs1_en_test(); break;
        case 3025: exec_6325_ldo_vgp3_en_test();        break;
        case 3026: exec_6325_ldo_vbiasn_en_test();      break;
        case 3027: exec_6325_ldo_vrtc_en_test();        break;

        //                                                         
        //                                                         
        //                                                         
        //                                                         
        //                                                         
        case 3105: exec_6325_ldo_vcama_vosel_test();       break;
        //                                                         
        case 3107: exec_6325_ldo_vcn33_vosel_test();       break;
        case 3108: exec_6325_ldo_vrf18_1_vosel_test();     break;
        //                                                         
        case 3110: exec_6325_ldo_vmch_vosel_test();        break;
        case 3111: exec_6325_ldo_vmc_vosel_test();         break;
        case 3112: exec_6325_ldo_vemc33_vosel_test();      break;
        //                                                         
        case 3114: exec_6325_ldo_vcam_af_vosel_test();     break;
        case 3115: exec_6325_ldo_vgp1_vosel_test();        break;
        case 3116: exec_6325_ldo_vefuse_vosel_test();      break;
        case 3117: exec_6325_ldo_vsim1_vosel_test();       break;
        case 3118: exec_6325_ldo_vsim2_vosel_test();       break;
        case 3119: exec_6325_ldo_vmipi_vosel_test();       break;
        case 3120: exec_6325_ldo_vcn18_vosel_test();       break;
        case 3121: exec_6325_ldo_vgp2_vosel_test();        break;
        case 3122: exec_6325_ldo_vcamd_vosel_test();       break;
        case 3123: exec_6325_ldo_vcam_io_vosel_test();     break;
        case 3124: exec_6325_ldo_vsram_dvfs1_vosel_test(); break;
        case 3125: exec_6325_ldo_vgp3_vosel_test();        break;
        case 3126: exec_6325_ldo_vbiasn_vosel_test();      break;
        //                                                         

        case 3200: exec_6325_ldo_vtcxo0_cal_test();      break;
        case 3201: exec_6325_ldo_vtcxo1_cal_test();      break;
        case 3202: exec_6325_ldo_vaud28_cal_test();      break;
        case 3203: exec_6325_ldo_vauxa28_cal_test();     break;
        case 3204: exec_6325_ldo_vbif28_cal_test();      break;
        case 3205: exec_6325_ldo_vcama_cal_test();       break;
        case 3206: exec_6325_ldo_vcn28_cal_test();       break;
        case 3207: exec_6325_ldo_vcn33_cal_test();       break;
        case 3208: exec_6325_ldo_vrf18_1_cal_test();     break;
        case 3209: exec_6325_ldo_vusb33_cal_test();      break;
        case 3210: exec_6325_ldo_vmch_cal_test();        break;
        case 3211: exec_6325_ldo_vmc_cal_test();         break;
        case 3212: exec_6325_ldo_vemc33_cal_test();      break;
        case 3213: exec_6325_ldo_vio28_cal_test();       break;
        case 3214: exec_6325_ldo_vcam_af_cal_test();     break;
        case 3215: exec_6325_ldo_vgp1_cal_test();        break;
        case 3216: exec_6325_ldo_vefuse_cal_test();      break;
        case 3217: exec_6325_ldo_vsim1_cal_test();       break;
        case 3218: exec_6325_ldo_vsim2_cal_test();       break;
        case 3219: exec_6325_ldo_vmipi_cal_test();       break;
        case 3220: exec_6325_ldo_vcn18_cal_test();       break;
        case 3221: exec_6325_ldo_vgp2_cal_test();        break;
        case 3222: exec_6325_ldo_vcamd_cal_test();       break;
        case 3223: exec_6325_ldo_vcam_io_cal_test();     break;
        case 3224: exec_6325_ldo_vsram_dvfs1_cal_test(); break;
        case 3225: exec_6325_ldo_vgp3_cal_test();        break;
        case 3226: exec_6325_ldo_vbiasn_cal_test();      break;
        case 3227: exec_6325_ldo_vrtc_cal_test();        break;

        case 3300: exec_6325_ldo_vtcxo0_mode_test();       break;
        case 3301: exec_6325_ldo_vtcxo1_mode_test();       break;
        case 3302: exec_6325_ldo_vaud28_mode_test();       break;
        case 3303: exec_6325_ldo_vauxa28_mode_test_main(); break;
        case 3304: exec_6325_ldo_vbif28_mode_test();       break;
        //                                                         
        case 3306: exec_6325_ldo_vcn28_mode_test();        break;
        case 3307: exec_6325_ldo_vcn33_mode_test();        break;
        case 3308: exec_6325_ldo_vrf18_1_mode_test();      break;
        case 3309: exec_6325_ldo_vusb33_mode_test();       break;
        case 3310: exec_6325_ldo_vmch_mode_test();         break;
        case 3311: exec_6325_ldo_vmc_mode_test();          break;
        case 3312: exec_6325_ldo_vemc33_mode_test();       break;
        case 3313: exec_6325_ldo_vio28_mode_test();        break;
        case 3314: exec_6325_ldo_vcam_af_mode_test();      break;
        case 3315: exec_6325_ldo_vgp1_mode_test();         break;
        case 3316: exec_6325_ldo_vefuse_mode_test();       break;
        case 3317: exec_6325_ldo_vsim1_mode_test();        break;
        case 3318: exec_6325_ldo_vsim2_mode_test();        break;
        case 3319: exec_6325_ldo_vmipi_mode_test();        break;
        case 3320: exec_6325_ldo_vcn18_mode_test();        break;
        case 3321: exec_6325_ldo_vgp2_mode_test();         break;
        case 3322: exec_6325_ldo_vcamd_mode_test();        break;
        case 3323: exec_6325_ldo_vcam_io_mode_test();      break;
        case 3324: exec_6325_ldo_vsram_dvfs1_mode_test();  break;
        case 3325: exec_6325_ldo_vgp3_mode_test();         break;
        case 3326: exec_6325_ldo_vbiasn_mode_test();       break;
        //                                                         

        //      
        case 3500: exec_6311_ldo_vbiasn_en_test();       break;
        
        case 3600: exec_6311_ldo_vbiasn_vosel_test();    break;
        case 3601: exec_6311_ldo_vdig18_vosel_test();    break;
        
        case 3701: exec_6311_ldo_vdig18_cal_test();      break;        
        
        case 3800: exec_6311_ldo_vbiasn_mode_test();     break;

        //     
        case 4001: exec_6325_efuse_test(); break;        

        //         
                
        //      
        
        //                                      
        case 6000: auxadc_request_test();			break;
	//                   
	case 6001: auxadc_trimm_channel_test();			break;
	//                                                     
	case 6002: auxadc_trimm_one_channel_test(0);		break;
	//                                                     
	case 6003: auxadc_trimm_one_channel_test(1);		break;
	//                                                     
	case 6004: auxadc_trimm_one_channel_test(2);		break;
	//                                                     
	case 6005: auxadc_trimm_one_channel_test(3);		break;
	//                     
	case 6006: auxadc_priority_test_1();			break;
	case 6007: auxadc_priority_test_2();			break;
	case 6008: auxadc_priority_test_3();			break;
	case 6009: auxadc_priority_test_4();			break;
	case 6010: auxadc_priority_test_5();			break;
					
	//                                                     
	case 6011: auxadc_low_battery_int_wakeup();		break;
	case 6012: auxadc_high_battery_int_wakeup();		break;
	case 6013: auxadc_low_battery_int_sleep();		break;
	case 6014: auxadc_high_battery_int_sleep();		break;
	case 6015: auxadc_low_battery2_int_wakeup();		break;
	case 6016: auxadc_high_battery2_int_wakeup();		break;
	case 6017: auxadc_low_battery2_int_sleep();		break;
	case 6018: auxadc_high_battery2_int_sleep();		break;
	case 6019: auxadc_low_vismps_int_test_wakeup();		break;
	case 6020: auxadc_high_vismps_int_test_wakeup();	break;
	case 6021: auxadc_low_vismps_int_test_sleep();		break;
	case 6022: auxadc_high_vismps_int_test_sleep();		break;
	//                                                 
	case 6023: auxadc_measure_poweron_test_pchr();		break;
	case 6024: auxadc_measure_wakeup_test_pchr();		break;
	case 6025: auxadc_measure_poweron_test_swchr();		break;
	case 6026: auxadc_measure_wakeup_test_swchr();		break;
	//                        
	case 6027: auxadc_swctrl_measure_test_6325();		break;
	//                           
	case 6028: auxadc_low_thermal_int_test_wakeup();	break;
	case 6029: auxadc_high_thermal_int_test_wakeup();	break;
	case 6030: auxadc_low_thermal_int_test_sleep();		break;
	case 6031: auxadc_high_thermal_int_test_sleep();	break;
		
	//                         
	case 6032: auxadc_impendance_batsns_test();		break;
	case 6033: auxadc_impendance_isense_test();		break;

	//                                  
	case 6040: auxadc_request_one_channel(0);		break;
	case 6041: auxadc_request_one_channel(1);		break;
	case 6042: auxadc_request_one_channel(2);		break;
	case 6043: auxadc_request_one_channel(3);		break;
	case 6044: auxadc_request_one_channel(4);		break;
	case 6045: auxadc_request_one_channel(5);		break;
	case 6046: auxadc_request_one_channel(6);		break;
	case 6047: auxadc_request_one_channel(7);		break;
	case 6048: auxadc_request_one_channel(8);		break;
	case 6049: auxadc_request_one_channel(9);		break;
	case 6050: auxadc_request_one_channel(10);		break;
	case 6051: auxadc_request_one_channel(11);		break;
	case 6052: auxadc_request_one_channel(12);		break;

        case 6100: mt6311_int_test();       break;
		
#ifdef RTC_DVT_TEST	
	//   
	case 7000:
		printk("\r\n******** rtc_hw_init ********\r\n");	
		rtc_hw_init(true);
		break;
	case 7001: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_XOSC32_ENB_Ball ********\r\n");	
		RTC_XOSC32_ENB_Ball_ldvt();			
		break;
	
	case 7002: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Tick_Interrupt ********\r\n");
		RTC_Tick_Interrupt_ldvt();	
		break;
	case 7003: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Alarm_Interrupt ********\r\n");	
		RTC_Alarm_Interrupt_ldvt();		
		break;
	case 7004: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Write_Trigger ********\r\n");			
		RTC_Write_Trigger_ldvt();	
		break;
	case 7005: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_BBPU_bit ********\r\n");				
		RTC_BBPU_bit_ldvt();	
		break;
	case 7006: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_One_Shot_Calibration ********\r\n");					
       		RTC_One_Shot_Calibration_ldvt();	
		break;
	case 7007: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Repeat_Shot_Calibration ********\r\n");					
       		RTC_Repeat_Calibration_ldvt(63);	
		break;
	case 7037: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Repeat_Shot_Calibration ********\r\n");					
       		RTC_Repeat_Calibration_ldvt(-64);	
		break;
	case 7047: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Repeat_Calibration ********\r\n");	
       		RTC_Repeat_Calibration_ldvt(0);
		break;
	case 7008: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_CLRPKY_Bit ********\r\n");		
       		RTC_CLRPKY_Bit_ldvt(0);
		break;
	case 7009: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_CLRPKY_Bit ********\r\n");		
       		RTC_CLRPKY_Bit_ldvt(1);
		break;
	case 7010: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_CLRPKY_Bit ********\r\n");		
       		RTC_CLRPKY_Bit_ldvt(2);
		break;
	case 7011: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Alarm ********\r\n");
       		RTC_Alarm_ldvt();		
		break;
	case 7012:
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Spare_Register ********\r\n");		
       		RTC_Spare_Register_ldvt();	
		break;
	case 7013: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_AutoReload_After_Reboot ********\r\n");
       		RTC_AutoReload_After_Reboot_ldvt();		
		break;
	case 7014: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Tick_Check ********\r\n");
       		RTC_Tick_Check_ldvt();		
       		break;
	case 7015: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Auto_test ********\r\n");
       		RTC_Auto_test_ldvt();		
		break;
	case 7016: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Protect ********\r\n");		
       		RTC_Protect_ldvt();	
		break;
	case 7017: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_32K_Stop ********\r\n");		
       		RTC_32K_Stop_ldvt();		
		break;
	case 7018: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Vcore_Off ********\r\n");		
       		RTC_Vcore_Off_ldvt();	
		break;
	case 7019: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_Powerkey_Unmatch ********\r\n");		
       		RTC_Powerkey_Unmatch_ldvt();	
		break;
	case 7020: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_OSC32_Con_Read_Write ********\r\n");		
       		RTC_OSC32_Con_Read_Write_ldvt();
		break;
	case 7021: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_EMBCK_SEL_MODE ********\r\n");		
       		RTC_EMBCK_SEL_MODE_ldvt();
		break;
	case 7022: 
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_LPD_VCCmin_test eosc ********\r\n");		
       		RTC_LPD_VCCmin_test(1);
		break;		
	case 7023:
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_LPD_VCCmin_test xosc ********\r\n");		
       		RTC_LPD_VCCmin_test(0);
		break;
	case 7024:
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_2SEC_MODE_TEST 0.5s ********\r\n");	
		RTC_2SEC_MODE_TEST(0); 	
		break;
	case 7025:
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_2SEC_MODE_TEST 1s ********\r\n");	
		RTC_2SEC_MODE_TEST(1); 	
		break;
	case 7026:
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_2SEC_MODE_TEST 2s ********\r\n");	
		RTC_2SEC_MODE_TEST(2); 	
		break;
	case 7027:
		printk("\r\n******** uvvp_rt_ioctl UVVP_RTC_read_time ********\r\n");	
		RTC_read_time(&irqtm);
		break;
	case 7028:
		ts_rtc_Init();
		break;
#endif
        default:
            printk("[pmic_dvt_entry] test_id=%d\n", test_id);
            break;
    }

    printk("[pmic_dvt_entry] end\n");
}
