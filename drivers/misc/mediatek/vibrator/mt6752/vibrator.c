/******************************************************************************
 * mt6575_vibrator.c - MT6575 Android Linux Vibrator Device Driver
 * 
 * Copyright 2009-2010 MediaTek Co.,Ltd.
 * 
 * DESCRIPTION:
 *     This file provid the other drivers vibrator relative functions
 *
 ******************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/device.h>
#include <mach/mt_typedefs.h>
#include <cust_vibrator.h>
#include <mach/upmu_common_sw.h>
#include <mach/upmu_hw.h>
#include <linux/delay.h>
static int vibe_mode = 0;

extern S32 pwrap_read( U32  adr, U32 *rdata );
extern S32 pwrap_write( U32  adr, U32  wdata );

void vibr_Enable_HW(void)
{
	
	//                                   

	mt6325_upmu_set_rg_vibr_sw_mode(0);//                                   
	//                                                                             
	mt6325_upmu_set_rg_vibr_fr_ori(1);  //                                                            
	//                                                                                

	mt6325_upmu_set_rg_vibr_en(1);     //                           
}

void vibr_Disable_HW(void)
{
	//                                    

	switch(vibe_mode)
    {
        case 1:
            mt6325_upmu_set_rg_vibr_fr_ori(2);  //                                                           
            mt6325_upmu_set_rg_vibr_fr_ori(3);  //                                                            

            msleep(30); //          
            mt6325_upmu_set_rg_vibr_fr_ori(2);  //                                                            
            mt6325_upmu_set_rg_vibr_en(0);     //                           
        case 0:
        default:
            mt6325_upmu_set_rg_vibr_en(0);     //                           
            break;
    }
    vibe_mode = 0;
}


/*                                         
                                         
                                  
              
              
              
              
              
              
              
              
                                          */

void vibr_power_set(void)
{
#ifdef CUST_VIBR_VOL
	struct vibrator_hw* hw = get_cust_vibrator_hw();	
	printk("[vibrator]vibr_init: vibrator set voltage = %d\n", hw->vib_vol);
	mt6325_upmu_set_rg_vibr_vosel(hw->vib_vol);
#endif
}

struct vibrator_hw* mt_get_cust_vibrator_hw(void)
{
	return get_cust_vibrator_hw();
}
