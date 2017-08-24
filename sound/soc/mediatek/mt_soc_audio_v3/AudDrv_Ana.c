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

#include "AudDrv_Common.h"
#include "AudDrv_Ana.h"
#include "AudDrv_Clk.h"

//                                      
#define AUDIO_USING_WRAP_DRIVER
#ifdef AUDIO_USING_WRAP_DRIVER
#include <mach/mt_pmic_wrap.h>
#endif

/*                                                                            
                                              
                                                                             */

void Ana_Set_Reg(uint32 offset, uint32 value, uint32 mask)
{
    //                                               
    int ret = 0;
    uint32 Reg_Value;

    PRINTK_ANA_REG("Ana_Set_Reg offset= 0x%x , value = 0x%x mask = 0x%x\n", offset, value, mask);
#ifdef AUDIO_USING_WRAP_DRIVER
    Reg_Value = Ana_Get_Reg(offset);
    Reg_Value &= (~mask);
    Reg_Value |= (value & mask);
    ret = pwrap_write(offset, Reg_Value);
    Reg_Value = Ana_Get_Reg(offset);
    if ((Reg_Value & mask) != (value & mask))
    {
        //                                                                                                                               
    }
#endif
}

uint32 Ana_Get_Reg(uint32 offset)
{
    //                  
    int ret = 0;
    uint32 Rdata = 0;
#ifdef AUDIO_USING_WRAP_DRIVER
    ret = pwrap_read(offset, &Rdata);
#endif
    PRINTK_ANA_REG ("Ana_Get_Reg offset= 0x%x  Rdata = 0x%x ret = %d\n",offset,Rdata,ret);
    return Rdata;
}

void Ana_Log_Print(void)
{
    AudDrv_ANA_Clk_On();
    AudDrv_ANA_Clk_Off();
    printk("-Ana_Log_Print \n");
}


//                                      
EXPORT_SYMBOL(Ana_Log_Print);
EXPORT_SYMBOL(Ana_Set_Reg);
EXPORT_SYMBOL(Ana_Get_Reg);


