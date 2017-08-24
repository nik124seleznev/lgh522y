/*******************************************************************************
* mt6575_pwm.h PWM Drvier
*
* Copyright (c) 2010, Media Teck.inc
*
* This program is free software; you can redistribute it and/or modify it
* under the terms and conditions of the GNU General Public Licence,
* version 2, as publish by the Free Software Foundation.
*
* This program is distributed and in hope it will be useful, but WITHOUT
* ANY WARRNTY; without even the implied warranty of MERCHANTABITLITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
*
********************************************************************************
* Author : Chagnlei Gao (changlei.gao@mediatek.com)
********************************************************************************
*/

#ifndef __MT_PWM_H__
#define __MT_PWM_H__

#include <mach/mt_typedefs.h>
#include <mach/mt_pwm_hal.h>

struct pwm_easy_config {
	U32 pwm_no;
	U32 duty;
	U32 clk_src;
	U32 clk_div;
	U16 duration;
	BOOL pmic_pad;
};
struct pwm_spec_config {
	U32 pwm_no;
	U32 mode;
	U32 clk_div;
	U32 clk_src;
	BOOL intr;
	BOOL pmic_pad;

	union {
		/*              */
		struct _PWM_OLDMODE_REGS {
			U16 IDLE_VALUE;
			U16 GUARD_VALUE;
			U16 GDURATION;
			U16 WAVE_NUM;
			U16 DATA_WIDTH;
			U16 THRESH;
		} PWM_MODE_OLD_REGS;

		/*               */
		struct _PWM_MODE_FIFO_REGS {
			U32 IDLE_VALUE;
			U32 GUARD_VALUE;
			U32 STOP_BITPOS_VALUE;
			U16 HDURATION;
			U16 LDURATION;
			U32 GDURATION;
			U32 SEND_DATA0;
			U32 SEND_DATA1;
			U32 WAVE_NUM;
		} PWM_MODE_FIFO_REGS;
/*
                   
                                
                  
                   
                         
                 
                 
                 
                         
                 
                
                        

                   
                                
                  
                   
                         
                 
                 
                 
                         
                 
                       
                 
                
             
                        

                
                               
                            
                      
                                                                            
                      
                      
                      
                      
                       
*/
	};
};

S32 pwm_set_easy_config(struct pwm_easy_config *conf);
S32 pwm_set_spec_config(struct pwm_spec_config *conf);

void mt_pwm_dump_regs(void);
void mt_pwm_disable(U32 pwm_no, BOOL pmic_pad);

/*                                  */
void mt_set_pwm_3dlcm_enable(BOOL enable);
/*
                                          
 */
void mt_set_pwm_3dlcm_inv(U32 pwm_no, BOOL inv);
//                                       

//                                       
#endif
