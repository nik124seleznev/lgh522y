/*******************************************************************************
* mt_pwm.c PWM Drvier                                                     
*                                                                                             
* Copyright (c) 2012, Media Teck.inc                                           
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
* Author : Changlei Gao (changlei.gao@mediatek.com)                              
********************************************************************************
*/

#include <linux/kernel.h>
#include <generated/autoconf.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <asm/atomic.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <mach/mt_pwm_hal_pub.h>
#include <mach/mt_pwm_hal.h>
#include <mach/mt_pwm_prv.h>

enum {                                                                                     
	PWM_CON,
	PWM_HDURATION,
	PWM_LDURATION,
	PWM_GDURATION,
	PWM_BUF0_BASE_ADDR,
	PWM_BUF0_SIZE,
	PWM_BUF1_BASE_ADDR,
	PWM_BUF1_SIZE,
	PWM_SEND_DATA0,
	PWM_SEND_DATA1,
	PWM_WAVE_NUM,
	PWM_DATA_WIDTH,		//                    
	PWM_THRESH,				//                 
	PWM_SEND_WAVENUM,	//             
	PWM_VALID
}PWM_REG_OFF;

unsigned long PWM_register[PWM_NUM]={
	(PWM_BASE+0x0010),     //                                  
	(PWM_BASE+0x0050),     //                                  
	(PWM_BASE+0x0090),     //                                  
	(PWM_BASE+0x00d0),     //                                  
	(PWM_BASE+0x0110),     //                                  
};


static int pwm_power_id[] = {
	MT_CG_PERI_PWM1,
	MT_CG_PERI_PWM2,
	MT_CG_PERI_PWM3,
	MT_CG_PERI_PWM4,
	MT_CG_PERI_PWM5,
	MT_CG_PERI_PWM6,
	MT_CG_PERI_PWM7,
	MT_CG_PERI_PWM
};

//                           
#define PWM_CG 7
void mt_pwm_power_on_hal(U32 pwm_no, BOOL pmic_pad, unsigned long* power_flag)
{
	if(0 == (*power_flag)){
		PWMDBG("enable_clock: main\n");//
		enable_clock(pwm_power_id[PWM_CG], "PWM");
		set_bit(PWM_CG, power_flag);
	}
	if (!test_bit(pwm_no, power_flag)) {
		PWMDBG("enable_clock: %d\n", pwm_no);//

			PWMDBG("ap pad\n");//
			enable_clock(pwm_power_id[pwm_no], "PWM");  //             

		set_bit(pwm_no,power_flag);
		PWMDBG("enable_clock PWM%d\n", pwm_no+1);
	}
}

void mt_pwm_power_off_hal(U32 pwm_no, BOOL pmic_pad, unsigned long* power_flag)
{
	if (test_bit(pwm_no, power_flag)) {
		PWMDBG("disable_clock: %d\n", pwm_no);//

			PWMDBG("ap pad\n");//
			disable_clock(pwm_power_id[pwm_no], "PWM");  //              

		clear_bit(pwm_no,power_flag);
		PWMDBG("disable_clock PWM%d\n", pwm_no+1);
	}
	if(BIT(PWM_CG) == (*power_flag)){
		PWMDBG("disable_clock: main\n");//
		disable_clock(pwm_power_id[PWM_CG], "PWM");
		clear_bit(PWM_CG, power_flag);
	}
}
void mt_pwm_init_power_flag(unsigned long* power_flag)
{/*
                        
                                                                
     
                                          
         
                                    
         
            
         
                                       
         
     
                                                         */
}
S32 mt_pwm_sel_pmic_hal(U32 pwm_no)
{
	PWMDBG("mt_pwm_sel_pmic\n");//
	return EINVALID;
}

S32 mt_pwm_sel_ap_hal(U32 pwm_no)
{
	PWMDBG("mt_pwm_sel_ap\n");//
	return EINVALID;
}

void mt_set_pwm_enable_hal(U32 pwm_no)
{
	SETREG32(PWM_ENABLE, 1 << pwm_no);
}
void mt_set_pwm_disable_hal(U32 pwm_no)
{
	CLRREG32 ( PWM_ENABLE, 1 << pwm_no );
}
void mt_set_pwm_enable_seqmode_hal(void)
{
	SETREG32 ( PWM_ENABLE, 1 << PWM_ENABLE_SEQ_OFFSET );
}
void mt_set_pwm_disable_seqmode_hal(void)
{
	CLRREG32 ( PWM_ENABLE,1 << PWM_ENABLE_SEQ_OFFSET );
}
S32 mt_set_pwm_test_sel_hal(U32 val)
{
	if (val == TEST_SEL_TRUE)
		SETREG32 ( PWM_ENABLE, 1 << PWM_ENABLE_TEST_SEL_OFFSET );
	else if ( val == TEST_SEL_FALSE )
		CLRREG32 ( PWM_ENABLE, 1 << PWM_ENABLE_TEST_SEL_OFFSET );
	else
		return 1;
	return 0;
}
void mt_set_pwm_clk_hal (U32 pwm_no, U32 clksrc, U32 div )
{
	unsigned long reg_con;

	reg_con = PWM_register [pwm_no] + 4* PWM_CON;
	MASKREG32 ( reg_con, PWM_CON_CLKDIV_MASK, div );
	if ( (clksrc&0x80000000)!=0 ) {
		clksrc &= ~(0x80000000);		
		if (clksrc == CLK_BLOCK_BY_1625_OR_32K) {	//                           
			CLRREG32 ( reg_con, 1 << PWM_CON_CLKSEL_OLD_OFFSET );	//         
			SETREG32 ( reg_con, 1 << PWM_CON_CLKSEL_OFFSET );	//          
		}
		else {	 //                 
			SETREG32 ( reg_con, 1 << PWM_CON_CLKSEL_OLD_OFFSET ); 	
			SETREG32 ( reg_con, 1 << PWM_CON_CLKSEL_OFFSET );	
		} 
	} 
	else {		
		CLRREG32 ( reg_con, 1 << PWM_CON_CLKSEL_OLD_OFFSET );		
		if (clksrc == CLK_BLOCK) {			
		CLRREG32 ( reg_con, 1 << PWM_CON_CLKSEL_OFFSET );
		} 
		else if (clksrc == CLK_BLOCK_BY_1625_OR_32K) {			
		SETREG32 ( reg_con, 1 << PWM_CON_CLKSEL_OFFSET );
        }
	}
}
S32 mt_get_pwm_clk_hal(U32 pwm_no)
{
	S32 clk, clksrc, clkdiv;
	unsigned long reg_con, reg_val,reg_en;

	reg_con = PWM_register[pwm_no] + 4*PWM_CON;

	reg_val = INREG32 (reg_con);
	reg_en = INREG32 (PWM_ENABLE);

	if ( ( ( reg_val & PWM_CON_CLKSEL_MASK ) >> PWM_CON_CLKSEL_OFFSET ) == 1 )
		if ( ((reg_en &PWM_CON_OLD_MODE_MASK) >> PWM_CON_OLD_MODE_OFFSET ) == 1)
			clksrc = 32*1024;
		else clksrc = BLOCK_CLK;
	else
		clksrc = BLOCK_CLK/1625;

	clkdiv = 2 << ( reg_val & PWM_CON_CLKDIV_MASK);
	if ( clkdiv <= 0 ) {
		PWMDBG ( "clkdiv less zero, not valid \n" );
		return -ERROR;
	}

	clk = clksrc/clkdiv;
	PWMDBG ( "CLK is :%d\n", clk );
	return clk;
}
S32 mt_set_pwm_con_datasrc_hal ( U32 pwm_no, U32 val )
{
	unsigned long reg_con;
	
	reg_con = PWM_register[pwm_no] + 4*PWM_CON;
	if ( val == PWM_FIFO )
		CLRREG32 ( reg_con, 1 << PWM_CON_SRCSEL_OFFSET );
	else if ( val == MEMORY )
		SETREG32 ( reg_con, 1 << PWM_CON_SRCSEL_OFFSET );
	else 
		return 1;
	return 0;
}
S32 mt_set_pwm_con_mode_hal( U32 pwm_no, U32 val )
{
	unsigned long reg_con;
	
	reg_con = PWM_register[pwm_no] + 4*PWM_CON;
	if ( val == PERIOD )
		CLRREG32 ( reg_con, 1 << PWM_CON_MODE_OFFSET );
	else if (val == RAND)
		SETREG32 ( reg_con, 1 << PWM_CON_MODE_OFFSET );
	else
		return 1;
	return 0;
}
S32 mt_set_pwm_con_idleval_hal(U32 pwm_no, U16 val)
{
	unsigned long reg_con;
	
	reg_con = PWM_register[pwm_no] + 4*PWM_CON;
	if ( val == IDLE_TRUE )
		SETREG32 ( reg_con,1 << PWM_CON_IDLE_VALUE_OFFSET );
	else if ( val == IDLE_FALSE )
		CLRREG32 ( reg_con, 1 << PWM_CON_IDLE_VALUE_OFFSET );
	else 
		return 1;
	return 0;
}
S32 mt_set_pwm_con_guardval_hal(U32 pwm_no, U16 val)
{
	unsigned long reg_con;
	
	reg_con = PWM_register[pwm_no] + 4*PWM_CON;
	if ( val == GUARD_TRUE )
		SETREG32 ( reg_con, 1 << PWM_CON_GUARD_VALUE_OFFSET );
	else if ( val == GUARD_FALSE )
		CLRREG32 ( reg_con, 1 << PWM_CON_GUARD_VALUE_OFFSET );
	else 
		return 1;
	return 0;
}
void mt_set_pwm_con_stpbit_hal(U32 pwm_no, U32 stpbit, U32 srcsel )
{
	unsigned long reg_con;
	
	reg_con = PWM_register[pwm_no] + 4*PWM_CON;
	if ( srcsel == PWM_FIFO )
		MASKREG32 ( reg_con, PWM_CON_STOP_BITS_MASK, stpbit << PWM_CON_STOP_BITS_OFFSET);
	if ( srcsel == MEMORY )
		MASKREG32 ( reg_con, PWM_CON_STOP_BITS_MASK & (0x1f << PWM_CON_STOP_BITS_OFFSET), stpbit << PWM_CON_STOP_BITS_OFFSET);
}
S32 mt_set_pwm_con_oldmode_hal ( U32 pwm_no, U32 val )
{
	unsigned long reg_con;
	
	reg_con = PWM_register[pwm_no] + 4*PWM_CON;
	if ( val == OLDMODE_DISABLE )
		CLRREG32 ( reg_con, 1 << PWM_CON_OLD_MODE_OFFSET );
	else if ( val == OLDMODE_ENABLE )
		SETREG32 ( reg_con, 1 << PWM_CON_OLD_MODE_OFFSET );
	else 
		return 1;
	return 0;
}
void mt_set_pwm_HiDur_hal(U32 pwm_no, U16 DurVal)//                          
{
	unsigned long reg_HiDur;
	
	reg_HiDur = PWM_register[pwm_no]+4*PWM_HDURATION;
	OUTREG32 ( reg_HiDur, DurVal);	
}
void mt_set_pwm_LowDur_hal (U32 pwm_no, U16 DurVal)
{
	unsigned long reg_LowDur;
	reg_LowDur = PWM_register[pwm_no] + 4*PWM_LDURATION;
	OUTREG32 ( reg_LowDur, DurVal );
}
void mt_set_pwm_GuardDur_hal (U32 pwm_no, U16 DurVal)
{
	unsigned long reg_GuardDur;
	reg_GuardDur = PWM_register[pwm_no] + 4*PWM_GDURATION;
	OUTREG32 ( reg_GuardDur, DurVal );
}
void mt_set_pwm_send_data0_hal ( U32 pwm_no, U32 data )
{
	unsigned long reg_data0;
	reg_data0 = PWM_register[pwm_no] + 4 * PWM_SEND_DATA0;
	OUTREG32 ( reg_data0, data );
}
void mt_set_pwm_send_data1_hal ( U32 pwm_no, U32 data )
{
	unsigned long reg_data1;
	reg_data1 = PWM_register[pwm_no] + 4 * PWM_SEND_DATA1;
	OUTREG32 ( reg_data1, data );
}
void mt_set_pwm_wave_num_hal ( U32 pwm_no, U16 num )
{
	unsigned long reg_wave_num;
	reg_wave_num = PWM_register[pwm_no] + 4 * PWM_WAVE_NUM;
	OUTREG32 ( reg_wave_num, num );
}
void mt_set_pwm_data_width_hal ( U32 pwm_no, U16 width )
{
	unsigned long reg_data_width;
	if (pwm_no == PWM5 || pwm_no == PWM4) {
		reg_data_width = PWM_register[pwm_no] + 4 * PWM_THRESH;
	} else {
		reg_data_width = PWM_register[pwm_no] + 4 * PWM_DATA_WIDTH;
	}
	OUTREG32 ( reg_data_width, width );
}
void mt_set_pwm_thresh_hal ( U32 pwm_no, U16 thresh )
{
	unsigned long reg_thresh;

	if (pwm_no == PWM5 || pwm_no == PWM4) {
		reg_thresh = PWM_register[pwm_no] + 4 * PWM_SEND_WAVENUM;
	} else {
		reg_thresh = PWM_register[pwm_no] + 4 * PWM_THRESH;
	}
	OUTREG32 ( reg_thresh, thresh );
}
S32 mt_get_pwm_send_wavenum_hal ( U32 pwm_no )
{
	unsigned long reg_send_wavenum;

	if ( pwm_no <=PWM3 )
		reg_send_wavenum = PWM_register[pwm_no] + 4 * PWM_SEND_WAVENUM;
	else
		reg_send_wavenum = PWM_register[pwm_no] + 4 * (PWM_SEND_WAVENUM - 2);  //                                                    
	return INREG32 ( reg_send_wavenum );
}

void mt_set_intr_enable_hal(U32 pwm_intr_enable_bit)
{
	SETREG32 ( PWM_INT_ENABLE, 1 << pwm_intr_enable_bit );
}
S32 mt_get_intr_status_hal(U32 pwm_intr_status_bit)
{
	int ret;
	ret = INREG32 ( PWM_INT_STATUS );
	ret = ( ret >> pwm_intr_status_bit ) & 0x01;
	return ret;
}
void mt_set_intr_ack_hal ( U32 pwm_intr_ack_bit )
{
	SETREG32 ( PWM_INT_ACK, 1 << pwm_intr_ack_bit );
}

void mt_pwm_dump_regs_hal(void)
{
	int i;
	unsigned long reg_val;
	reg_val = INREG32(PWM_ENABLE);
	PWMMSG("\r\n[PWM_ENABLE is:%lx]\n\r ", reg_val );
	printk("<0>""peri pdn0 clock: 0x%x\n", INREG32(PERI_PDN0_STA)); 

	for ( i = PWM1;  i < PWM_MAX; i ++ ) 
	{
		reg_val = INREG32(PWM_register[i] + 4* PWM_CON);
		PWMMSG("\r\n[PWM%d_CON is:%lx]\r\n", i+1, reg_val);
		reg_val=INREG32(PWM_register[i]+4* PWM_HDURATION);
		PWMMSG("[PWM%d_HDURATION is:%lx]\r\n",i+1,reg_val);
		reg_val = INREG32(PWM_register[i] + 4* PWM_LDURATION);
		PWMMSG("[PWM%d_LDURATION is:%lx]\r\n", i+1, reg_val);
		reg_val=INREG32(PWM_register[i]+4* PWM_GDURATION);
		PWMMSG("[PWM%d_GDURATION is:%lx]\r\n",i+1,reg_val);
/*
                                                             
                                                              
                                                    
                                                       
                                                             
                                                              
                                                    
                                                         
*/
		reg_val = INREG32(PWM_register[i] + 4* PWM_SEND_DATA0);
		PWMMSG("[PWM%d_SEND_DATA0 is:%lx]\r\n", i+1, reg_val);
		reg_val=INREG32(PWM_register[i]+4* PWM_SEND_DATA1);
		PWMMSG("[PWM%d_PWM_SEND_DATA1 is:%lx]\r\n",i+1,reg_val);
		reg_val = INREG32(PWM_register[i] + 4* PWM_WAVE_NUM);
		PWMMSG("[PWM%d_WAVE_NUM is:%lx]\r\n", i+1, reg_val);
		if(i==PWM5||i==PWM4){
			reg_val=INREG32(PWM_register[i]+4* PWM_THRESH);
		}else{
			reg_val=INREG32(PWM_register[i]+4* PWM_DATA_WIDTH);
		}
		PWMMSG("[PWM%d_WIDTH is:%lx]\r\n",i+1,reg_val);
		if(i==PWM5||i==PWM4){
			reg_val=INREG32(PWM_register[i]+4* PWM_SEND_WAVENUM);
		}else{
			reg_val=INREG32(PWM_register[i]+4* PWM_THRESH);
		}
		PWMMSG("[PWM%d_THRESH is:%lx]\r\n",i+1,reg_val);
		if(i==PWM5||i==PWM4){
			reg_val=INREG32(PWM_register[i]+4* PWM_DATA_WIDTH);
		}else{
			reg_val=INREG32(PWM_register[i]+4* PWM_SEND_WAVENUM);
		}
		PWMMSG("[PWM%d_SEND_WAVENUM is:%lx]\r\n",i+1,reg_val);
/*
                                                
                                                          
*/
	}		
}
/*                            */
static void mt_pmic_pwm1_test(void)
{/*
                             
                                                                

                                        
                    
                           
                         
                                       
                     
                       
                                                 
                                                   
                                                
                                       
                                       
                                       
                                                 
                                                 
                                       
                                                                                                             
                            
 
                                       
                    
                                                
                            */
}

static void mt_pmic_pwm6_test(void)
{
	//                                                                      
	
	//                            
	//                           
}

void pwm_debug_store_hal()
{
	//                 
	printk("<0>""peri pdn0 clock: 0x%x\n", INREG32(PERI_PDN0_STA)); 
	mt_pmic_pwm1_test();
}

void pwm_debug_show_hal()
{
	mt_pwm_dump_regs_hal();
	mt_pmic_pwm6_test();
}

/*                                  */
/*
                                                             
 */
void mt_set_pwm_3dlcm_enable_hal(BOOL enable)
{
	SETREG32 ( PWM_3DLCM, 1 << PWM_3DLCM_ENABLE_OFFSET );
}

/*
                                        
                              
 */
void mt_set_pwm_3dlcm_inv_hal(U32 pwm_no, BOOL inv)
{
	if (inv){
		if (pwm_no == PWM3){
			SETREG32 ( PWM_3DLCM, 1 << PWM_3DLCM_0_INV );
		}else if (pwm_no == PWM4){
			SETREG32 ( PWM_3DLCM, 1 << PWM_3DLCM_1_INV );
		}else if (pwm_no == PWM5){
			SETREG32 ( PWM_3DLCM, 1 << PWM_3DLCM_2_INV );
		}
	}else{
		if (pwm_no == PWM3){
			CLRREG32 ( PWM_3DLCM, 1 << PWM_3DLCM_0_INV );
		}else if (pwm_no == PWM4){
			CLRREG32 ( PWM_3DLCM, 1 << PWM_3DLCM_1_INV );
		}else if (pwm_no == PWM5){
			CLRREG32 ( PWM_3DLCM, 1 << PWM_3DLCM_2_INV );
		}
	}
}
