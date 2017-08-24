/*
 * Copyright (C) 2012 Samsung Electronics.
 * Kyungmin Park <kyungmin.park@samsung.com>
 * Tomasz Figa <t.figa@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_ARM_FIRMWARE_H
#define __ASM_ARM_FIRMWARE_H

#include <linux/bug.h>

/*
                      
  
                                                        
  
                                                                        
 */
struct firmware_ops {
	/*
                        
  */
	int (*do_idle)(void);
	/*
                                               
  */
	int (*set_cpu_boot_addr)(int cpu, unsigned long boot_addr);
	/*
                                
  */
	int (*cpu_boot)(int cpu);
	/*
                        
  */
	int (*l2x0_init)(void);
};

/*                                                                   */
extern const struct firmware_ops *firmware_ops;

/*
                            
  
                                                        
                            
 */
#define call_firmware_op(op, ...)					\
	((firmware_ops->op) ? firmware_ops->op(__VA_ARGS__) : (-ENOSYS))

/*
                             
  
                                                       
 */
static inline void register_firmware_ops(const struct firmware_ops *ops)
{
	BUG_ON(!ops);

	firmware_ops = ops;
}

#endif
