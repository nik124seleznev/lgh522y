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
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <generated/autoconf.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <asm/atomic.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#endif

#include <mach/mt_typedefs.h>
#include <mach/mt_pwm.h>  
#include <mach/mt_pwm_hal_pub.h>

#ifdef CONFIG_OF
void __iomem *pwm_base;
//                       
#endif	

struct pwm_device {
	const char      *name;
	atomic_t        ref;
	dev_t           devno;
	spinlock_t      lock;
	unsigned long 	power_flag;//                                      
	struct device   dev;
};

static struct pwm_device pwm_dat = {
	.name = PWM_DEVICE,
	.ref = ATOMIC_INIT(0),
	.power_flag = 0,
	.lock = __SPIN_LOCK_UNLOCKED(pwm_dat.lock)
};

static struct pwm_device *pwm_dev = &pwm_dat;

static void mt_pwm_power_on(U32 pwm_no, BOOL pmic_pad)
{
	mt_pwm_power_on_hal(pwm_no, pmic_pad, &(pwm_dev->power_flag));
}

static void mt_pwm_power_off (U32 pwm_no, BOOL pmic_pad)
{
	mt_pwm_power_off_hal (pwm_no, pmic_pad, &(pwm_dev->power_flag));
}

static S32 mt_pwm_sel_pmic(U32 pwm_no)
{
	unsigned long flags;
	S32 ret;
	struct pwm_device *dev = pwm_dev;

	spin_lock_irqsave ( &dev->lock,flags );
	ret = mt_pwm_sel_pmic_hal(pwm_no);
	spin_unlock_irqrestore ( &dev->lock, flags );
	
	if(ret == (-EEXCESSPWMNO))
		PWMDBG ( "PWM1~PWM4 not support pmic_pad\n" );

	return ret;
}

static S32 mt_pwm_sel_ap(U32 pwm_no)
{
	unsigned long flags;
	S32 ret;
	struct pwm_device *dev = pwm_dev;

	spin_lock_irqsave ( &dev->lock,flags );
	ret = mt_pwm_sel_ap_hal(pwm_no);
	spin_unlock_irqrestore ( &dev->lock, flags );
	
	if(ret == (-EEXCESSPWMNO))
		PWMDBG ( "PWM1~PWM4 not support pmic_pad\n" );

	return ret;
}

/*                                                      
                                                   
 
                                                       */
static S32 mt_set_pwm_enable(U32 pwm_no) 
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;

	if ( !dev ) {
		PWMDBG("dev is not valid!\n");
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ( "pwm number is not between PWM1~PWM7\n" );
		return -EEXCESSPWMNO;
	} 

	spin_lock_irqsave ( &dev->lock,flags );
	mt_set_pwm_enable_hal(pwm_no);
	spin_unlock_irqrestore ( &dev->lock, flags );

	return RSUCCESS;
}


/*                                                     */
static S32 mt_set_pwm_disable ( U32 pwm_no )  
{
	unsigned long flags;
	struct pwm_device *dev = pwm_dev;

	if ( !dev ) {
		PWMDBG ( "dev is not valid\n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ( "pwm number is not between PWM1~PWM7\n" );
		return -EEXCESSPWMNO;
	}

	spin_lock_irqsave ( &dev->lock, flags );	
	mt_set_pwm_disable_hal(pwm_no);
	spin_unlock_irqrestore ( &dev->lock, flags );

	mdelay(1);

	return RSUCCESS;
}

void mt_pwm_disable(U32 pwm_no, BOOL pmic_pad)
{
	mt_set_pwm_disable(pwm_no);
	mt_pwm_power_off(pwm_no, pmic_pad);
}

void mt_set_pwm_enable_seqmode(void)
{
	unsigned long flags;
	struct pwm_device *dev = pwm_dev;

	if ( !dev ) {
		PWMDBG ( "dev is not valid \n" );
		return ;
    }

	spin_lock_irqsave ( &dev->lock,flags );
	mt_set_pwm_enable_seqmode_hal();
	spin_unlock_irqrestore ( &dev->lock, flags );
}

void mt_set_pwm_disable_seqmode(void)
{
	unsigned long flags;
	struct pwm_device *dev = pwm_dev;

	if ( !dev ) {
		PWMDBG ( "dev is not valid\n" );
		return ;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_disable_seqmode_hal();
	spin_unlock_irqrestore ( &dev->lock, flags );
}

S32 mt_set_pwm_test_sel(U32 val)  //             
{
	unsigned long flags;
	struct pwm_device *dev = pwm_dev;

	if ( !dev ) {
		PWMDBG ( "dev is not pwm_dev \n" );
		return -EINVALID;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	if(mt_set_pwm_test_sel_hal(val))
		goto err;	
	spin_unlock_irqrestore ( &dev->lock, flags );
	return RSUCCESS;

err:
	spin_unlock_irqrestore ( &dev->lock, flags );
	return -EPARMNOSUPPORT;
}

S32 mt_set_pwm_clk ( U32 pwm_no, U32 clksrc, U32 div )
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ( "dev is not valid\n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ( "pwm number excesses PWM_MAX \n" );
		return -EEXCESSPWMNO;
	}

	if ( div >= CLK_DIV_MAX ) {
		PWMDBG ("division excesses CLK_DIV_MAX\n");
		return -EPARMNOSUPPORT;
	}

	if ( (clksrc & 0x7FFFFFFF) > CLK_BLOCK_BY_1625_OR_32K) {
		PWMDBG("clksrc excesses CLK_BLOCK_BY_1625_OR_32K\n");
		return -EPARMNOSUPPORT;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_clk_hal (pwm_no, clksrc, div );
	spin_unlock_irqrestore ( &dev->lock, flags );

	return RSUCCESS;
}

S32 mt_get_pwm_clk ( U32 pwm_no )
{
	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ("dev is not valid \n");
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX) {
		PWMDBG ( "pwm number excesses PWM_MAX \n" );
		return -EEXCESSPWMNO;
	}

	return mt_get_pwm_clk_hal(pwm_no);
}

/*                                         
                                  
                        
                    
                        
                                          */

static S32 mt_set_pwm_con_datasrc ( U32 pwm_no, U32 val )
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG (" pwm deivce doesn't exist\n");
		return -EINVALID;
	}
		
	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ("pwm number excesses PWM_MAX \n");
		return -EEXCESSPWMNO;
	}
	
	spin_lock_irqsave ( &dev->lock, flags );
	if (mt_set_pwm_con_datasrc_hal(pwm_no, val))
		goto err;
	spin_unlock_irqrestore ( &dev->lock, flags );
	return RSUCCESS;

err:
	spin_unlock_irqrestore ( &dev->lock, flags );
	return -EPARMNOSUPPORT;
}


/*                                               
                           
                          
                       
                         
 
                                                  */
static S32 mt_set_pwm_con_mode( U32 pwm_no, U32 val )
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ("dev is not valid \n");
		return -EINVALID;
	}
	
	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ("pwm number excesses PWM_MAX\n");
		return -EEXCESSPWMNO;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	if(mt_set_pwm_con_mode_hal( pwm_no, val ))
		goto err;
	spin_unlock_irqrestore ( &dev->lock, flags );
	return RSUCCESS;

err:
	spin_unlock_irqrestore ( &dev->lock, flags );
	return -EPARMNOSUPPORT;
}

/*                                              
                                      
                                               
                                          
 
                    
                   
                                              */
static S32 mt_set_pwm_con_idleval(U32 pwm_no, U16 val)
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( ! dev ) {
		PWMDBG ( "dev is not valid \n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ( "pwm number excesses PWM_MAX \n" );
		return -EEXCESSPWMNO;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	if ( mt_set_pwm_con_idleval_hal(pwm_no, val))
		goto err;
	spin_unlock_irqrestore ( &dev->lock, flags );
	return RSUCCESS;

err:
	spin_unlock_irqrestore ( &dev->lock, flags );
	return -EPARMNOSUPPORT;
}

/*                                            
                                     
                                           
                                       
 
                   
                  
                                             */
static S32 mt_set_pwm_con_guardval(U32 pwm_no, U16 val)
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ( "dev is not valid\n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ("pwm number excesses PWM_MAX \n");
		return -EEXCESSPWMNO;
	}
	
	spin_lock_irqsave ( &dev->lock, flags );
	if(mt_set_pwm_con_guardval_hal(pwm_no, val))
		goto err;
	spin_unlock_irqrestore ( &dev->lock, flags );
	return RSUCCESS;

err:
	spin_unlock_irqrestore ( &dev->lock, flags );
	return -EPARMNOSUPPORT;
}

/*                                                
                               
                                   
 
                                                 */
static S32 mt_set_pwm_con_stpbit(U32 pwm_no, U32 stpbit, U32 srcsel )
{
	unsigned long flags;
	
	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ( "dev is not valid \n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ( "pwm number excesses PWM_MAX \n" );
		return -EEXCESSPWMNO;
	}


	if (srcsel == PWM_FIFO) {
		if ( stpbit > 0x3f ) {
			PWMDBG ( "stpbit execesses the most of 0x3f in fifo mode\n" );
			return -EPARMNOSUPPORT;
		}
	}else if (srcsel == MEMORY){
		if ( stpbit > 0x1f) {
			PWMDBG ("stpbit excesses the most of 0x1f in memory mode\n");
			return -EPARMNOSUPPORT;
		}
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_con_stpbit_hal(pwm_no, stpbit, srcsel);
	spin_unlock_irqrestore ( &dev->lock, flags );

	return RSUCCESS;
}

/*                                                    
                                 
                              
                               
 
                         
                        
                                                     */

static S32 mt_set_pwm_con_oldmode ( U32 pwm_no, U32 val )
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ( "dev is not valid \n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ("pwm number excesses PWM_MAX \n");
		return -EEXCESSPWMNO;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	if(mt_set_pwm_con_oldmode_hal ( pwm_no, val))
		goto err;
	
	spin_unlock_irqrestore ( &dev->lock, flags );
	return RSUCCESS;

err:
	spin_unlock_irqrestore ( &dev->lock, flags );
	return -EPARMNOSUPPORT;
}

/*                                                          
                             
 
                                                            */

static S32 mt_set_pwm_HiDur(U32 pwm_no, U16 DurVal)  
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ( "dev is not valid \n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ( "pwm number excesses PWM_MAX\n" );
		return -EEXCESSPWMNO;
	}
	
	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_HiDur_hal(pwm_no, DurVal);
	spin_unlock_irqrestore ( &dev->lock, flags );

	return RSUCCESS;
}

/*                                               
                               
                                                */
static S32 mt_set_pwm_LowDur (U32 pwm_no, U16 DurVal)
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ( "dev is not valid \n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ("pwm number excesses PWM_MAX\n");
		return -EEXCESSPWMNO;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_LowDur_hal (pwm_no, DurVal);
	spin_unlock_irqrestore ( &dev->lock, flags );

	return RSUCCESS;
}

/*                                                  
                                
                        
                                       
                                                   */
static S32 mt_set_pwm_GuardDur ( U32 pwm_no, U16 DurVal )
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ( "dev is not valid\n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ("pwm number excesses PWM_MAX\n");
		return -EEXCESSPWMNO;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_GuardDur_hal (pwm_no, DurVal);
	spin_unlock_irqrestore ( &dev->lock, flags );

	return RSUCCESS;
}

/*                                                    
                            
                         
                    
                                                     
                                                
 
                     
                    

                                  
              
                                  
                   
  

                           
                                              
                       
  

                                                                

                                         
                                   
                                              

                 
 */

/*                                                    
                            
                         
                    
                                                     
                                                
 
                     
                    
 
                                  
              
                                  
                   
  

                           
                                              
                       
  

                                                          

                                         
                                   
                                              

                 

 */

/*                                                    
                            
                         
                    
                                                     
                                                
 
                     
                    
 
                                  
              
                                   
                   
  

                           
                                              
                       
  

                                                                

                                         
                                   
                                              

                 
 */

/*                                                    
                            
                         
                    
                                                     
                                                
 
                     
                    

                                  
              
                                  
                   
  

                           
                                              
                       
  

                                                          

                                         
                                   
                                              

                 

 */

/*                                                    
                             
                         
                                
                                                     */
static S32 mt_set_pwm_send_data0 ( U32 pwm_no, U32 data )
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ( "dev is not valid\n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ( "pwm number excesses PWM_MAX \n" );
		return -EEXCESSPWMNO;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_send_data0_hal ( pwm_no, data );
	spin_unlock_irqrestore ( &dev->lock, flags );

	return RSUCCESS;
}

/*                                                    
                             
                         
                                
                                                     */
static S32 mt_set_pwm_send_data1 ( U32 pwm_no, U32 data )
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev; 
	if ( !dev ) {
		PWMDBG ( "dev is not valid \n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ( "pwm number excesses PWM_MAX \n" );
		return -EEXCESSPWMNO;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_send_data1_hal ( pwm_no, data );
	spin_unlock_irqrestore ( &dev->lock, flags );

	return RSUCCESS;
}

/*                                                    
                           
                         
                     
                                                     */
static S32 mt_set_pwm_wave_num ( U32 pwm_no, U16 num )
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ("dev is not valid\n");
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ( "pwm number excesses PWM_MAX\n" );
		return -EEXCESSPWMNO;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_wave_num_hal ( pwm_no, num );
	spin_unlock_irqrestore ( &dev->lock, flags );

	return RSUCCESS;
}

/*                                                    
                               
                           
                         
                                            
                                                     */
static S32 mt_set_pwm_data_width ( U32 pwm_no, U16 width )
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ( "dev is not valid\n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ( "pwm number excesses PWM_MAX\n" );
		return -EEXCESSPWMNO;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_data_width_hal ( pwm_no, width );
	spin_unlock_irqrestore ( &dev->lock, flags );

	return RSUCCESS;
}

/*                                                    
                         
                         
                                 
                                                     */
static S32 mt_set_pwm_thresh ( U32 pwm_no, U16 thresh )
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev; 
	if ( !dev ) {
		PWMDBG ( "dev is not valid \n" );
		return -EINVALID;
	}

	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ( " pwm number excesses PWM_MAX \n");
		return -EEXCESSPWMNO;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_thresh_hal ( pwm_no, thresh );
	spin_unlock_irqrestore ( &dev->lock, flags );

	return RSUCCESS;
}

/*                                                    
                               
                         
 
                                                     */
S32 mt_get_pwm_send_wavenum ( U32 pwm_no )
{
	struct pwm_device *dev = pwm_dev;
	if ( ! dev ) {
		PWMDBG ( "dev is not valid\n" );
		return -EBADADDR;
	}
	
	if ( pwm_no >= PWM_MAX ) {
		PWMDBG ( "pwm number excesses PWM_MAX\n" );
		return -EEXCESSPWMNO;
	}

	return mt_get_pwm_send_wavenum_hal ( pwm_no );
}

/*                                                    
                             
                         
                 
                                           
                                          
                                                     
                                                                                                    
 
                     
               

                                  
              
                                
                   
  

                           
                                             
                       
  

                                    
                              
                         
  

                                         
                                                   
     
                                                       

                                         
                                            
                                              

                 
 */

/*                                                
                                     
 
                                                 
                                                         
 
                     
                                     

               
                                     
                   
  

                                             
                                                           
                                                 

                 
 */

/*                                                      
                     
  
 
                                                       
                                                          
 
                     
                                     
                  
                                      
                   
  

                                            
                                                        
                                               

                 
 */

/*                                          
                          
                                     
 
                                            */
S32 mt_set_intr_enable(U32 pwm_intr_enable_bit)
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ( "dev is not valid\n" );
		return -EINVALID;
	}
	
	if (pwm_intr_enable_bit >= PWM_INT_ENABLE_BITS_MAX) {
		PWMDBG (" pwm inter enable bit is not right.\n"); 
		return -EEXCESSBITS; 
	}
	
	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_intr_enable_hal(pwm_intr_enable_bit);
	spin_unlock_irqrestore (&dev->lock, flags );

	return RSUCCESS;
}

/*                                                    
                          
                         
                     
                                                     */
S32 mt_get_intr_status(U32 pwm_intr_status_bit)
{
	unsigned long flags;
	int ret;

	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ( "dev is not valid\n" );
		return -EINVALID;
	}

	if ( pwm_intr_status_bit >= PWM_INT_STATUS_BITS_MAX ) {
		PWMDBG ( "status bit excesses PWM_INT_STATUS_BITS_MAX\n" );
		return -EEXCESSBITS;
	}
	
	spin_lock_irqsave ( &dev->lock, flags );
	ret = mt_get_intr_status_hal(pwm_intr_status_bit);
	spin_unlock_irqrestore ( &dev->lock, flags );

	return ret;
}

/*                                                    
                       
                         
                  
                                                     */
S32 mt_set_intr_ack ( U32 pwm_intr_ack_bit )
{
	unsigned long flags;
	struct pwm_device *dev = pwm_dev;
	if ( !dev ) {
		PWMDBG ("dev is not valid\n");
		return -EINVALID;
	}

	if ( pwm_intr_ack_bit >= PWM_INT_ACK_BITS_MAX ) {
		PWMDBG ( "ack bit excesses PWM_INT_ACK_BITS_MAX\n" ); 
		return -EEXCESSBITS;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_intr_ack_hal ( pwm_intr_ack_bit );
	spin_unlock_irqrestore ( &dev->lock, flags );

	return RSUCCESS;
}

/*                                  */
/*
                                                             
 */
void mt_set_pwm_3dlcm_enable(BOOL enable)
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev; 
	if ( !dev ) {
		PWMDBG ( "dev is not valid \n" );
		return;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_3dlcm_enable_hal(enable);
	spin_unlock_irqrestore ( &dev->lock, flags );
	
	return;
}

/*
                                          
 */
void mt_set_pwm_3dlcm_inv(U32 pwm_no, BOOL inv)
{
	unsigned long flags;

	struct pwm_device *dev = pwm_dev; 
	if ( !dev ) {
		PWMDBG ( "dev is not valid \n" );
		return;
	}

	spin_lock_irqsave ( &dev->lock, flags );
	mt_set_pwm_3dlcm_inv_hal(pwm_no, inv);
	spin_unlock_irqrestore ( &dev->lock, flags );
	return;
}
/*
                                      
 
                     

                                   
              
                                   
         
  

                                         
                                   
                                              
        
 
*/
/*
                                      
 
                     

                                   
              
                                   
         
  

                                         
                                   
                                              
        
 
*/
S32 pwm_set_easy_config ( struct pwm_easy_config *conf)
{

	U32 duty = 0;
	U16 duration = 0;
	U32 data_AllH=0xffffffff;
	U32 data0 = 0;
	U32 data1 = 0;
	
	if ( conf->pwm_no >= PWM_MAX || conf->pwm_no < PWM_MIN ) {
		PWMDBG("pwm number excess PWM_MAX\n");
		return -EEXCESSPWMNO;
	}

	if ((conf->clk_div >= CLK_DIV_MAX) || (conf->clk_div < CLK_DIV_MIN )) {
		PWMDBG ( "PWM clock division invalid\n" );
		return -EINVALID;
	}
	
	if ( ( conf ->clk_src >= PWM_CLK_SRC_INVALID) || (conf->clk_src < PWM_CLK_SRC_MIN) ) {
		PWMDBG ("PWM clock source invalid\n");
		return -EINVALID;
	}

	if  ( conf->duty < 0 ) {
		PWMDBG("duty parameter is invalid\n");
		return -EINVALID;
	}

	PWMDBG("pwm_set_easy_config\n");

	if ( conf->duty == 0 ) {
		mt_set_pwm_disable (conf->pwm_no);
		mt_pwm_power_off(conf->pwm_no, conf->pmic_pad);
		return RSUCCESS;
	}
	
	duty = conf->duty;
	duration = conf->duration;
	
	switch ( conf->clk_src ) {
		case PWM_CLK_OLD_MODE_BLOCK:
		case PWM_CLK_OLD_MODE_32K:
			if ( duration > 8191 || duration < 0 ) {
				PWMDBG ( "duration invalid parameter\n" );
				return -EPARMNOSUPPORT;
			}
			if ( duration < 10 ) 
				duration = 10;
			break;
			
		case PWM_CLK_NEW_MODE_BLOCK:
		case PWM_CLK_NEW_MODE_BLOCK_DIV_BY_1625:
			if ( duration > 65535 || duration < 0 ){
				PWMDBG ("invalid paramters\n");
				return -EPARMNOSUPPORT;
			}
			break;
		default:
			PWMDBG("invalid clock source\n");
			return -EPARMNOSUPPORT;
	}
	
	if ( duty > 100 ) 
		duty = 100;

	if ( duty > 50 ){
		data0 = data_AllH;
		data1 = data_AllH >> ((PWM_NEW_MODE_DUTY_TOTAL_BITS * (100 - duty ))/100 );
	}else {
		data0 = data_AllH >> ((PWM_NEW_MODE_DUTY_TOTAL_BITS * (50 - duty))/100);
		PWMDBG("DATA0 :0x%x\n",data0);
		data1 = 0;
	}

	mt_pwm_power_on(conf->pwm_no, conf->pmic_pad);
	if(conf->pmic_pad){
		mt_pwm_sel_pmic(conf->pwm_no);
	} else {
		mt_pwm_sel_ap(conf->pwm_no);
	}
	mt_set_pwm_con_guardval(conf->pwm_no, GUARD_TRUE);

	switch ( conf->clk_src ) {
		case PWM_CLK_OLD_MODE_32K:
			mt_set_pwm_con_oldmode(conf->pwm_no, OLDMODE_ENABLE);
			mt_set_pwm_clk ( conf->pwm_no, CLK_BLOCK_BY_1625_OR_32K, conf->clk_div);
			break;

		case PWM_CLK_OLD_MODE_BLOCK:
			mt_set_pwm_con_oldmode (conf->pwm_no, OLDMODE_ENABLE );
			mt_set_pwm_clk ( conf->pwm_no, CLK_BLOCK, conf->clk_div );
			break;

		case PWM_CLK_NEW_MODE_BLOCK:
			mt_set_pwm_con_oldmode (conf->pwm_no, OLDMODE_DISABLE );
			mt_set_pwm_clk ( conf->pwm_no, CLK_BLOCK , conf->clk_div );
			mt_set_pwm_con_datasrc( conf->pwm_no, PWM_FIFO);
			mt_set_pwm_con_stpbit ( conf->pwm_no, 0x3f, PWM_FIFO );
			break;

		case PWM_CLK_NEW_MODE_BLOCK_DIV_BY_1625:
			mt_set_pwm_con_oldmode (conf->pwm_no,  OLDMODE_DISABLE );
			mt_set_pwm_clk ( conf->pwm_no, CLK_BLOCK_BY_1625_OR_32K, conf->clk_div );
			mt_set_pwm_con_datasrc( conf->pwm_no, PWM_FIFO);
			mt_set_pwm_con_stpbit ( conf->pwm_no, 0x3f, PWM_FIFO );
			break;

		default:
			break;
		}
	PWMDBG("The duration is:%x\n", duration);
	PWMDBG("The data0 is:%x\n",data0);
	PWMDBG("The data1 is:%x\n",data1);
	mt_set_pwm_HiDur ( conf->pwm_no, duration );
	mt_set_pwm_LowDur (conf->pwm_no, duration );
	mt_set_pwm_GuardDur (conf->pwm_no, 0 );
//                                         
//                                         
//                                         
//                                         
	mt_set_pwm_send_data0 (conf->pwm_no, data0 );
	mt_set_pwm_send_data1 (conf->pwm_no, data1 );
	mt_set_pwm_wave_num (conf->pwm_no, 0 );

//                                                   
//  
	mt_set_pwm_data_width (conf->pwm_no, duration );
	mt_set_pwm_thresh ( conf->pwm_no, (( duration * conf->duty)/100));
//                                                  
//                                                   
		
//  

	mb();
	mt_set_pwm_enable ( conf->pwm_no );
	PWMDBG("mt_set_pwm_enable\n");

	return RSUCCESS;
	
}

EXPORT_SYMBOL(pwm_set_easy_config);
	
S32 pwm_set_spec_config(struct pwm_spec_config *conf)
{

	if ( conf->pwm_no >= PWM_MAX ) {
		PWMDBG("pwm number excess PWM_MAX\n");
		return -EEXCESSPWMNO;
	}

       if ( ( conf->mode >= PWM_MODE_INVALID )||(conf->mode < PWM_MODE_MIN )) {
	   	PWMDBG ( "PWM mode invalid \n" );
		return -EINVALID;
       }

	if ( ( conf ->clk_src >= PWM_CLK_SRC_INVALID) || (conf->clk_src < PWM_CLK_SRC_MIN) ) {
		PWMDBG ("PWM clock source invalid\n");
		return -EINVALID;
	}

	if ((conf->clk_div >= CLK_DIV_MAX) || (conf->clk_div < CLK_DIV_MIN )) {
		PWMDBG ( "PWM clock division invalid\n" );
		return -EINVALID;
	}

	if ( (conf->mode == PWM_MODE_OLD &&
			(conf->clk_src == PWM_CLK_NEW_MODE_BLOCK)) 
		||(conf->mode != PWM_MODE_OLD &&
			(conf->clk_src == PWM_CLK_OLD_MODE_32K || conf->clk_src == PWM_CLK_OLD_MODE_BLOCK)) ) {

		PWMDBG ( "parameters match error\n" );
		return -ERROR;
	}

	mt_pwm_power_on(conf->pwm_no, conf->pmic_pad);
	if(conf->pmic_pad){
		mt_pwm_sel_pmic(conf->pwm_no);
	}

	switch (conf->mode ) {
		case PWM_MODE_OLD:
			PWMDBG("PWM_MODE_OLD\n");
			mt_set_pwm_con_oldmode(conf->pwm_no, OLDMODE_ENABLE);
			mt_set_pwm_con_idleval(conf->pwm_no, conf->PWM_MODE_OLD_REGS.IDLE_VALUE);
			mt_set_pwm_con_guardval (conf->pwm_no, conf->PWM_MODE_OLD_REGS.GUARD_VALUE);
			mt_set_pwm_GuardDur (conf->pwm_no, conf->PWM_MODE_OLD_REGS.GDURATION);
			mt_set_pwm_wave_num(conf->pwm_no, conf->PWM_MODE_OLD_REGS.WAVE_NUM);
			mt_set_pwm_data_width(conf->pwm_no, conf->PWM_MODE_OLD_REGS.DATA_WIDTH);
			mt_set_pwm_thresh(conf->pwm_no, conf->PWM_MODE_OLD_REGS.THRESH);
			PWMDBG ("PWM set old mode finish\n");
			break;
		case PWM_MODE_FIFO:
			PWMDBG("PWM_MODE_FIFO\n");
			mt_set_pwm_con_oldmode(conf->pwm_no, OLDMODE_DISABLE);
			mt_set_pwm_con_datasrc(conf->pwm_no, PWM_FIFO);
			mt_set_pwm_con_mode (conf->pwm_no, PERIOD);
			mt_set_pwm_con_idleval(conf->pwm_no, conf->PWM_MODE_FIFO_REGS.IDLE_VALUE);
			mt_set_pwm_con_guardval (conf->pwm_no, conf->PWM_MODE_FIFO_REGS.GUARD_VALUE);
			mt_set_pwm_HiDur (conf->pwm_no, conf->PWM_MODE_FIFO_REGS.HDURATION);
			mt_set_pwm_LowDur (conf->pwm_no, conf->PWM_MODE_FIFO_REGS.LDURATION);
			mt_set_pwm_GuardDur (conf->pwm_no, conf->PWM_MODE_FIFO_REGS.GDURATION);
			mt_set_pwm_send_data0 (conf->pwm_no, conf->PWM_MODE_FIFO_REGS.SEND_DATA0);
			mt_set_pwm_send_data1 (conf->pwm_no, conf->PWM_MODE_FIFO_REGS.SEND_DATA1);
			mt_set_pwm_wave_num(conf->pwm_no, conf->PWM_MODE_FIFO_REGS.WAVE_NUM);
			mt_set_pwm_con_stpbit(conf->pwm_no, conf->PWM_MODE_FIFO_REGS.STOP_BITPOS_VALUE,PWM_FIFO);
			break;
/*                       
                               
                                                         
                                                
                                              
                                                                               
                                                                                  
                                                                         
                                                                          
                                                                            
                                                                                      
                                                                             
                                                                          
                                                                                            
   
         
                       
                               
                                    
                                                         
                                                
                                            
                                                                               
                                                                                  
                                                                         
                                                                          
                                                                            
                                                                                       
                                                                             
                                                                                       
                                                                             
                                                                          
                                                                                             
                                                 
                                                 
         

                      
                              
                                                         
                               
                            
                            
                            
                            
                                                                                                            
                                                                                                         
                                                                                                         
                                    
                     
    
                                                                                    
                                                                                
                                                                                   
                                                                                
                                                                                   
                                                                                
   
                           
                           
                           
                           
         
*/			
		default:
			break;
		}

	switch (conf->clk_src) {
		case PWM_CLK_OLD_MODE_BLOCK:
			mt_set_pwm_clk (conf->pwm_no, CLK_BLOCK, conf->clk_div);
			PWMDBG("Enable oldmode and set clock block\n");
			break;
		case PWM_CLK_OLD_MODE_32K:
			mt_set_pwm_clk (conf->pwm_no, 0x80000000|CLK_BLOCK_BY_1625_OR_32K, conf->clk_div);
			PWMDBG("Enable oldmode and set clock 32K\n");
			break;
		case PWM_CLK_NEW_MODE_BLOCK:
			mt_set_pwm_clk (conf->pwm_no, CLK_BLOCK, conf->clk_div);
			PWMDBG("Enable newmode and set clock block\n");
			break;
		case PWM_CLK_NEW_MODE_BLOCK_DIV_BY_1625:      
			mt_set_pwm_clk (conf->pwm_no, CLK_BLOCK_BY_1625_OR_32K, conf->clk_div);
			PWMDBG("Enable newmode and set clock 32K\n");
			break;
		default:
			break;
	} 

	mb();
	mt_set_pwm_enable(conf->pwm_no); 
	PWMDBG("mt_set_pwm_enable\n");

	return RSUCCESS;
	
}	

EXPORT_SYMBOL(pwm_set_spec_config);

void mt_pwm_dump_regs()
{
	mt_pwm_dump_regs_hal();
	printk("<0>""pwm power_flag: 0x%x\n", (unsigned int)pwm_dev->power_flag); 
}
EXPORT_SYMBOL(mt_pwm_dump_regs);

struct platform_device pwm_plat_dev={
	.name = "mt-pwm",
};

/*  */
static ssize_t pwm_debug_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	printk("<0>""pwm power_flag: 0x%x\n", (unsigned int)pwm_dev->power_flag); 
	pwm_debug_store_hal();
	return count;
}

static ssize_t pwm_debug_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	pwm_debug_show_hal();
	return sprintf(buf, "%s\n",buf);
}

static DEVICE_ATTR(pwm_debug, 0644, pwm_debug_show, pwm_debug_store);

static int mt_pwm_probe ( struct platform_device *pdev)
{
	int ret;

	
#ifdef CONFIG_OF
		pwm_base = of_iomap(pdev->dev.of_node, 0);
		if (!pwm_base) {
			PWMDBG("PWM iomap failed\n");
			return -ENODEV;
		};
	
#if 0
		pwm_irqnr = irq_of_parse_and_map(pdev->dev.of_node, 0);
		if (!pwm_irqnr) {
			PWMDBG("PWM get irqnr failed\n");
			return -ENODEV;
		}
		PWMDBG("pwm base: 0x%p	pwm irq: %d\n", pwm_base, pwm_irqnr);
#endif
PWMDBG("pwm base: 0x%p\n", pwm_base);

#endif

	platform_set_drvdata (pdev, pwm_dev);
	
	ret = device_create_file(&pdev->dev, &dev_attr_pwm_debug);
	if (ret)
		PWMDBG("error creating sysfs files: pwm_debug\n");

#ifdef CONFIG_OF
//                                                                            
#else
//                                                                   
#endif

	return RSUCCESS;
}

static int  mt_pwm_remove(struct platform_device *pdev)
{
	if ( ! pdev ) {
		PWMDBG ("The plaform device is not exist\n");
		return -EBADADDR;
	}
	device_remove_file(&pdev->dev, &dev_attr_pwm_debug);

	PWMDBG ( "mt_pwm_remove\n" );
	return RSUCCESS;
}

static void mt_pwm_shutdown(struct platform_device *pdev)
{
	printk("mt_pwm_shutdown\n");
	return;
}

#ifdef CONFIG_OF
static const struct of_device_id pwm_of_match[] = {
	{ .compatible = "mediatek,PWM", },
	{},
};
#endif

struct platform_driver pwm_plat_driver={
	.probe = mt_pwm_probe,
	.remove = mt_pwm_remove,
	.shutdown = mt_pwm_shutdown,
	.driver = {
		.name="mt-pwm",
#ifdef CONFIG_OF
		.of_match_table = pwm_of_match,
#endif			
	},
};

static int __init mt_pwm_init(void)
{
	int ret;
#ifndef CONFIG_OF	
	ret = platform_device_register ( &pwm_plat_dev );
	if (ret < 0 ){
		PWMDBG ("platform_device_register error\n");
		goto out;
	}
#endif	
	ret = platform_driver_register ( &pwm_plat_driver );
	if ( ret < 0 ) {
		PWMDBG ("platform_driver_register error\n");
		goto out;
	}

out:
    mt_pwm_init_power_flag(&(pwm_dev->power_flag));
	return ret;
}

static void __exit mt_pwm_exit(void)
{
#ifndef CONFIG_OF
	platform_device_unregister ( &pwm_plat_dev );
#endif
	platform_driver_unregister ( &pwm_plat_driver );
}

module_init(mt_pwm_init);
module_exit(mt_pwm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("How wang <How.wang@mediatek.com>");
MODULE_DESCRIPTION(" This module is for mtk chip of mediatek");

