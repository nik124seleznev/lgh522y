/* linux/include/asm-arm/arch-msm/hsusb.h
 *
 * Copyright (C) 2008 Google, Inc.
 * Author: Brian Swetland <swetland@google.com>
 * Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __ASM_ARCH_MSM_HSUSB_H
#define __ASM_ARCH_MSM_HSUSB_H

#include <linux/types.h>
#include <linux/usb/otg.h>

/* 
                      
  
                                                          
                                                    
                                              
  
 */
enum usb_mode_type {
	USB_NONE = 0,
	USB_PERIPHERAL,
	USB_HOST,
	USB_OTG,
};

/* 
              
  
                                                                    
                                           
                                                            
                                                                   
                                                                   
  
 */
enum otg_control_type {
	OTG_NO_CONTROL = 0,
	OTG_PHY_CONTROL,
	OTG_PMIC_CONTROL,
	OTG_USER_CONTROL,
};

/* 
              
  
                                
                                                      
                                                        
  
 */
enum msm_usb_phy_type {
	INVALID_PHY = 0,
	CI_45NM_INTEGRATED_PHY,
	SNPS_28NM_INTEGRATED_PHY,
};

#define IDEV_CHG_MAX	1500
#define IUNIT		100

/* 
                                                      
  
                                                                    
                                                           
                                                            
                                                       
                                                                     
                                                         
                                                                         
                                                     
                                                         
  
 */
enum usb_chg_state {
	USB_CHG_STATE_UNDEFINED = 0,
	USB_CHG_STATE_WAIT_FOR_DCD,
	USB_CHG_STATE_DCD_DONE,
	USB_CHG_STATE_PRIMARY_DONE,
	USB_CHG_STATE_SECONDARY_DONE,
	USB_CHG_STATE_DETECTED,
};

/* 
                    
  
                                           
                                                                        
                                                     
                                                                     
                                                                       
                                                                            
  
 */
enum usb_chg_type {
	USB_INVALID_CHARGER = 0,
	USB_SDP_CHARGER,
	USB_DCP_CHARGER,
	USB_CDP_CHARGER,
};

/* 
                                                      
                                   
                                                            
                                 
                                          
                                                                       
                                               
                                                     
                                                              
                                                 
                                                                           
                                                         
 */
struct msm_otg_platform_data {
	int *phy_init_seq;
	void (*vbus_power)(bool on);
	unsigned power_budget;
	enum usb_mode_type mode;
	enum otg_control_type otg_control;
	enum usb_mode_type default_mode;
	enum msm_usb_phy_type phy_type;
	void (*setup_gpio)(enum usb_otg_state state);
	char *pclk_src_name;
};

/* 
                                                          
                                       
                                    
                                                  
                                    
                                      
                                     
                                               
                                              
                                           
                                                        
                                    
                                                 
                                       
                                                               
                                     
                                                      
                                           
                                                           
                                   
 */
struct msm_otg {
	struct usb_phy phy;
	struct msm_otg_platform_data *pdata;
	int irq;
	struct clk *clk;
	struct clk *pclk;
	struct clk *pclk_src;
	struct clk *phy_reset_clk;
	struct clk *core_clk;
	void __iomem *regs;
#define ID		0
#define B_SESS_VLD	1
	unsigned long inputs;
	struct work_struct sm_work;
	atomic_t in_lpm;
	int async_int;
	unsigned cur_power;
	struct delayed_work chg_work;
	enum usb_chg_state chg_state;
	enum usb_chg_type chg_type;
	u8 dcd_retries;
};

#endif
