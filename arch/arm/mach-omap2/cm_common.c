/*
 * OMAP2+ common Clock Management (CM) IP block functions
 *
 * Copyright (C) 2012 Texas Instruments, Inc.
 * Paul Walmsley
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * XXX This code should eventually be moved to a CM driver.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>

#include "cm2xxx.h"
#include "cm3xxx.h"
#include "cm44xx.h"
#include "common.h"

/*
                                                                   
                      
 */
static struct cm_ll_data null_cm_ll_data;
static struct cm_ll_data *cm_ll_data = &null_cm_ll_data;

/*                                                  */
void __iomem *cm_base;

/*                                                                    */
void __iomem *cm2_base;

/* 
                                                                       
                               
                                                                
  
                                                              
 */
void __init omap2_set_globals_cm(void __iomem *cm, void __iomem *cm2)
{
	cm_base = cm;
	cm2_base = cm2;
}

/* 
                                                                     
                                          
                                                                   
                                                                       
  
                                                                   
                                                                     
                                                                      
                                                                      
                                                                   
 */
int cm_split_idlest_reg(void __iomem *idlest_reg, s16 *prcm_inst,
			u8 *idlest_reg_id)
{
	if (!cm_ll_data->split_idlest_reg) {
		WARN_ONCE(1, "cm: %s: no low-level function defined\n",
			  __func__);
		return -EINVAL;
	}

	return cm_ll_data->split_idlest_reg(idlest_reg, prcm_inst,
					   idlest_reg_id);
}

/* 
                                                                    
                                
                                                         
                                                                      
  
                                                              
                                                                    
                                                                      
                                                                      
                                                   
 */
int cm_wait_module_ready(s16 prcm_mod, u8 idlest_id, u8 idlest_shift)
{
	if (!cm_ll_data->wait_module_ready) {
		WARN_ONCE(1, "cm: %s: no low-level function defined\n",
			  __func__);
		return -EINVAL;
	}

	return cm_ll_data->wait_module_ready(prcm_mod, idlest_id, idlest_shift);
}

/* 
                                                            
                                                                       
  
                                                                     
                                                               
                                                              
                                                                    
                                                               
                                          
 */
int cm_register(struct cm_ll_data *cld)
{
	if (!cld)
		return -EINVAL;

	if (cm_ll_data != &null_cm_ll_data)
		return -EEXIST;

	cm_ll_data = cld;

	return 0;
}

/* 
                                                                        
                                                                         
  
                                                                  
                                                           
                                                                  
                                                                  
                                                               
                                                       
 */
int cm_unregister(struct cm_ll_data *cld)
{
	if (!cld || cm_ll_data != cld)
		return -EINVAL;

	cm_ll_data = &null_cm_ll_data;

	return 0;
}
