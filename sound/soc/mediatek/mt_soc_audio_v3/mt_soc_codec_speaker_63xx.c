/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*                                                                              
  
            
            
                           
  
           
           
  
  
               
               
                          
  
          
          
                
  
                                                                                
                  
             
         
  
  
                                                                               */


/*                                                                            
                                                  
                                                                             */


/*                                                                            
                                                       
                                                                             */

#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>

#include "AudDrv_Common.h"
#include "AudDrv_Def.h"
#include "AudDrv_Afe.h"
#include "AudDrv_Ana.h"
#include "AudDrv_Clk.h"
#include "mt_soc_analog_type.h"
#include <mach/pmic_mt6325_sw.h>
#include "mt_soc_pcm_common.h"

//                                        
//                                     
//                               
//                                       

extern kal_uint32 mt6332_upmu_get_swcid(void);

void Speaker_ClassD_Open(void)
{
    kal_uint32 i, SPKTrimReg = 0;
    printk("%s\n", __func__);
    //         
    Ana_Set_Reg(SPK_CON7, 0x4531, 0xffff); //                                                     
    Ana_Set_Reg(SPK_CON0, 0x3008, 0xffff);//                                                                  
    Ana_Set_Reg(SPK_CON13, 0x1800, 0xffff);//                   
    Ana_Set_Reg(SPK_CON2, 0x04A4, 0xffff); //                                           
    Ana_Set_Reg(SPK_ANA_CON0, 0x5000, 0xffff); //                 
    Ana_Set_Reg(SPK_CON9, 0x2000, 0xffff); //                 
    Ana_Set_Reg(SPK_CON0, 0x3009, 0xffff); //               
    Ana_Set_Reg(SPK_CON9, 0x0A00, 0xffff); //                      
    for (i = 0; i < 10; i++)
    {
        udelay(1000); //                      
    }
    Ana_Set_Reg(SPK_CON0, 0x3001, 0xffff); //                                   
    Ana_Set_Reg(SPK_CON13, 0x0000, 0xffff); //                                       

    SPKTrimReg = Ana_Get_Reg(SPK_CON1);
    if ((SPKTrimReg & 0x8000) == 0)
    {
        printk("spk trim fail!\n");
    }
    else
    {
        printk("spk trim offset=%d\n", (SPKTrimReg & 0x1f));
    }

    //                          
    Ana_Set_Reg(SPK_CON0, 0x3001, 0xffff); //                      
}

void Speaker_ClassD_close(void)
{
    printk("%s\n", __func__);
    Ana_Set_Reg(SPK_CON9, 0x2A00, 0xffff); //                      
    Ana_Set_Reg(SPK_ANA_CON0, 0x5000, 0xffff); //                                      
    Ana_Set_Reg(SPK_CON0, 0x3001, 0xffff); //                                     
    Ana_Set_Reg(SPK_CON0, 0x3000, 0xffff); //                
}


void Speaker_ClassAB_Open(void)
{
    kal_uint32 i, SPKTrimReg = 0;
    printk("%s\n", __func__);
    //         
    Ana_Set_Reg(SPK_CON7, 0x4531, 0xffff); //                                                     
    Ana_Set_Reg(SPK_CON0, 0x1008, 0xffff);//                                                                   
    Ana_Set_Reg(SPK_CON13, 0x1800, 0xffff);//                   
    Ana_Set_Reg(SPK_CON2, 0x02A4, 0xffff); //                                           
    Ana_Set_Reg(SPK_ANA_CON0, 0x0800, 0xffff); //                
    Ana_Set_Reg(SPK_CON9, 0x2000, 0xffff); //                 
    Ana_Set_Reg(SPK_CON0, 0x1009, 0xffff); //               
    Ana_Set_Reg(SPK_CON9, 0x0100, 0xffff); //                      
    for (i = 0; i < 10; i++)
    {
        udelay(1000); //                      
    }
    Ana_Set_Reg(SPK_CON0, 0x1001, 0xffff); //             
    Ana_Set_Reg(SPK_CON0, 0x1000, 0xffff); //               
    Ana_Set_Reg(SPK_CON0, 0x1004, 0xffff); //                     
    Ana_Set_Reg(SPK_CON0, 0x1005, 0xffff); //               
    Ana_Set_Reg(SPK_CON13, 0x0000, 0xffff); //                                       

    SPKTrimReg = Ana_Get_Reg(SPK_CON1);
    if ((SPKTrimReg & 0x8000) == 0)
    {
        printk("spk trim fail!\n");
    }
    else
    {
        printk("spk trim offset=%d\n", (SPKTrimReg & 0x1f));
    }

    //                          
    Ana_Set_Reg(SPK_CON0, 0x3005, 0xffff); //                      
}

void Speaker_ClassAB_close(void)
{
    printk("%s\n", __func__);
    Ana_Set_Reg(SPK_CON9, 0x2100, 0xffff); //                      
    Ana_Set_Reg(SPK_ANA_CON0, 0x0800, 0xffff); //                                     
    Ana_Set_Reg(SPK_CON0, 0x1005, 0xffff); //                                      
    Ana_Set_Reg(SPK_CON0, 0x1004, 0xffff); //                
}

void Speaker_ReveiverMode_Open(void)
{
    printk("%s\n", __func__);
    Ana_Set_Reg(SPK_CON0, 0x1304, 0xffff); //                                                                          
    Ana_Set_Reg(SPK_CON2, 0x02A4, 0xffff); //                                            
//                                                                                    
    Ana_Set_Reg(SPK_ANA_CON0, 0x2000, 0xffff); //                
    Ana_Set_Reg(SPK_CON0, 0x1305, 0xffff); //               
    udelay(2000);
//                                                                                        
}

void Speaker_ReveiverMode_close(void)
{
//                                                                                                  
    Ana_Set_Reg(SPK_CON2, 0x0014, 0xffff); //                    
    Ana_Set_Reg(SPK_ANA_CON0, 0x0, 0xffff); //                 
    Ana_Set_Reg(SPK_CON0, 0x0, 0xffff); //                                   
}

bool GetSpeakerOcFlag(void)
{
    unsigned int OCregister = 0;
    unsigned int bitmask = 1;
    bool DmodeFlag = false;
    bool ABmodeFlag = false;
    Ana_Set_Reg(TOP_CKPDN_CON2_CLR, 0x3, 0xffff);
    OCregister = Ana_Get_Reg(SPK_CON6);
    DmodeFlag = OCregister & (bitmask << 14); //                              
    ABmodeFlag = OCregister & (bitmask << 15); //                               
    printk("OCregister = %d \n", OCregister);
    return (DmodeFlag | ABmodeFlag);
}


