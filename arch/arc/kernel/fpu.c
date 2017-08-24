/*
 * fpu.c - save/restore of Floating Point Unit Registers on task switch
 *
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/sched.h>
#include <asm/switch_to.h>

/*
                                                                   
                                                                            
                                                            
  
                                                     
                                                      
  
                                                                
                   
                                                               
                                                                    
                                                        
  
                                                                              
                                                                           
                             
 */

void fpu_save_restore(struct task_struct *prev, struct task_struct *next)
{
	unsigned int *saveto = &prev->thread.fpu.aux_dpfp[0].l;
	unsigned int *readfrom = &next->thread.fpu.aux_dpfp[0].l;

	const unsigned int zero = 0;

	__asm__ __volatile__(
		"daddh11  %0, %2, %2\n"
		"dexcl1   %1, %3, %4\n"
		: "=&r" (*(saveto + 1)), /*                         */
		  "=&r" (*(saveto))
		: "r" (zero), "r" (*(readfrom + 1)), "r" (*(readfrom))
	);

	__asm__ __volatile__(
		"daddh22  %0, %2, %2\n"
		"dexcl2   %1, %3, %4\n"
		: "=&r"(*(saveto + 3)),	/*                         */
		  "=&r"(*(saveto + 2))
		: "r" (zero), "r" (*(readfrom + 3)), "r" (*(readfrom + 2))
	);
}
