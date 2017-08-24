/*
 * Copyright (C) 2012 Synopsys, Inc. (www.synopsys.com)
 *
 * based on METAG mach/arch.h (which in turn was based on ARM)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _ASM_ARC_MACH_DESC_H_
#define _ASM_ARC_MACH_DESC_H_

/* 
                                                                              
                                                                     
                                                                 
                                                                         
                                                                       
                                                  
  
                         
                                                         
                                                 
                                                               
                                                                      
                                            
                                                    
                                                                     
                               
                                                                    
                                           
                                            
  
 */
struct machine_desc {
	const char		*name;
	const char		**dt_compat;

	void			(*init_early)(void);
	void			(*init_irq)(void);
#ifdef CONFIG_SMP
	void			(*init_smp)(unsigned int);
#endif
	void			(*init_time)(void);
	void			(*init_machine)(void);
	void			(*init_late)(void);

};

/*
                                                 
 */
extern struct machine_desc *machine_desc;

/*
                                                        
 */
extern struct machine_desc __arch_info_begin[], __arch_info_end[];
#define for_each_machine_desc(p)			\
	for (p = __arch_info_begin; p < __arch_info_end; p++)

static inline struct machine_desc *default_machine_desc(void)
{
	/*                                               */
	if (__arch_info_end - 1 < __arch_info_begin)
		return NULL;
	return __arch_info_end - 1;
}

/*
                                                 
                                            
 */
#define MACHINE_START(_type, _name)			\
static const struct machine_desc __mach_desc_##_type	\
__used							\
__attribute__((__section__(".arch.info.init"))) = {	\
	.name		= _name,

#define MACHINE_END				\
};

extern struct machine_desc *setup_machine_fdt(void *dt);
extern void __init copy_devtree(void);

#endif
