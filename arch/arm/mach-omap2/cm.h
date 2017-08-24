/*
 * OMAP2+ Clock Management prototypes
 *
 * Copyright (C) 2007-2009, 2012 Texas Instruments, Inc.
 * Copyright (C) 2007-2009 Nokia Corporation
 *
 * Written by Paul Walmsley
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ARCH_ASM_MACH_OMAP2_CM_H
#define __ARCH_ASM_MACH_OMAP2_CM_H

/*
                                                                      
                                                                       
             
                                                                        
                                                                
 */
#define MAX_MODULE_READY_TIME		2000

# ifndef __ASSEMBLER__
extern void __iomem *cm_base;
extern void __iomem *cm2_base;
extern void omap2_set_globals_cm(void __iomem *cm, void __iomem *cm2);
# endif

/*
                                                                    
                                                                    
                                                                    
                                                                  
                      
                                                              
               
 */
#define MAX_MODULE_DISABLE_TIME		5000

# ifndef __ASSEMBLER__

/* 
                                                                     
                                                                      
                                                                        
 */
struct cm_ll_data {
	int (*split_idlest_reg)(void __iomem *idlest_reg, s16 *prcm_inst,
				u8 *idlest_reg_id);
	int (*wait_module_ready)(s16 prcm_mod, u8 idlest_id, u8 idlest_shift);
};

extern int cm_split_idlest_reg(void __iomem *idlest_reg, s16 *prcm_inst,
			       u8 *idlest_reg_id);
extern int cm_wait_module_ready(s16 prcm_mod, u8 idlest_id, u8 idlest_shift);

extern int cm_register(struct cm_ll_data *cld);
extern int cm_unregister(struct cm_ll_data *cld);

# endif

#endif
