/*
 * arch/metag/include/asm/mach/arch.h
 *
 * Copyright (C) 2012 Imagination Technologies Ltd.
 *
 * based on the ARM version:
 *  Copyright (C) 2000 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _METAG_MACH_ARCH_H_
#define _METAG_MACH_ARCH_H_

#include <linux/stddef.h>

#include <asm/clock.h>

/* 
                                                                
                          
                                                          
                             
  
                                     
                                                    
  
                                     
                                                                
                                                            
                                   
  
                                                                              
                                                                              
                                                                               
                                                             
 */
struct machine_desc {
	const char		*name;
	const char		**dt_compat;
	struct meta_clock_desc	*clocks;

	unsigned int		nr_irqs;

	void			(*init_early)(void);
	void			(*init_irq)(void);
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

#endif /*                     */
