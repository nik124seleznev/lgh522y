/*
 * Copyright 2004-2008 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */
#ifndef _BFIN_PTRACE_H
#define _BFIN_PTRACE_H

#include <uapi/asm/ptrace.h>

#ifndef __ASSEMBLY__

/*                                                                       
                             */
#define user_mode(regs) (!(((regs)->ipend & ~0x10) & (((regs)->ipend & ~0x10) - 1)))

#define arch_has_single_step()	(1)
/*                                   */
#define ptrace_disable(child) user_disable_single_step(child)
#define current_user_stack_pointer() rdusp()

extern int is_user_addr_valid(struct task_struct *child,
			      unsigned long start, unsigned long len);

/*
                                                              
                                                             
                  
  
                                                            
                                                            
                                  
 */
#define task_pt_regs(task) \
	(struct pt_regs *) \
	    ((unsigned long)task_stack_page(task) + \
	     (THREAD_SIZE - sizeof(struct pt_regs)))

#include <asm-generic/ptrace.h>

#endif				/*              */
#endif				/*                */
