/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * vineetg: March 2009
 *  -Implemented task_pt_regs( )
 *
 * Amit Bhor, Sameer Dhavale, Ashwin Chaugule: Codito Technologies 2004
 */

#ifndef __ASM_ARC_PROCESSOR_H
#define __ASM_ARC_PROCESSOR_H

#ifdef __KERNEL__

#ifndef __ASSEMBLY__

#include <asm/arcregs.h>	/*                           */

/*                                                      
                                                                    
                     
 */
struct thread_struct {
	unsigned long ksp;	/*                           */
	unsigned long callee_reg;	/*                        */
	unsigned long fault_address;	/*                              */
	unsigned long cause_code;	/*                            */
#ifdef CONFIG_ARC_CURR_IN_REG
	unsigned long user_r25;
#endif
#ifdef CONFIG_ARC_FPU_SAVE_RESTORE
	struct arc_fpu fpu;
#endif
};

#define INIT_THREAD  {                          \
	.ksp = sizeof(init_stack) + (unsigned long) init_stack, \
}

/*                                        */
struct task_struct;

/*
                                       
 */
unsigned long thread_saved_pc(struct task_struct *t);

#define task_pt_regs(p) \
	((struct pt_regs *)(THREAD_SIZE - 4 + (void *)task_stack_page(p)) - 1)

/*                                      */
#define release_thread(thread) do { } while (0)

/*                                                       */
#define prepare_to_copy(tsk)    do { } while (0)

/*
                                                                               
                            
 */
#ifdef CONFIG_SMP
#define cpu_relax()	__asm__ __volatile__ ("" : : : "memory")
#else
#define cpu_relax()	do { } while (0)
#endif

#define copy_segments(tsk, mm)      do { } while (0)
#define release_segments(mm)        do { } while (0)

#define KSTK_EIP(tsk)   (task_pt_regs(tsk)->ret)

/*
                                                                             
                                                                              
 */
#define KSTK_ESP(tsk)   (tsk->thread.ksp)
#define KSTK_BLINK(tsk) (*((unsigned int *)((KSTK_ESP(tsk)) + (13+1+1)*4)))
#define KSTK_FP(tsk)    (*((unsigned int *)((KSTK_ESP(tsk)) + (13+1)*4)))

/*
                                                          
  
                                                    
                                         
                                                                           
                                         
 */
#define start_thread(_regs, _pc, _usp)				\
do {								\
	set_fs(USER_DS); /*                       */		\
	(_regs)->ret = (_pc);					\
	/*                                 */			\
	(_regs)->status32 = STATUS_U_MASK | STATUS_L_MASK	\
		| STATUS_E1_MASK | STATUS_E2_MASK;		\
	(_regs)->sp = (_usp);					\
	/*                                 */			\
	(_regs)->lp_start = 0x10;				\
	(_regs)->lp_end = 0x80;					\
} while (0)

extern unsigned int get_wchan(struct task_struct *p);

/*
                                                       
                                           
                                                                       
                                                       
 */
#define current_text_addr() ({ __label__ _l; _l: &&_l; })

#endif /*               */

/*                             
                                                                      
                                                                          
                                                                     
                                                             
 */
#define VMALLOC_SIZE	(0x10000000)	/*      */
#define VMALLOC_START	(PAGE_OFFSET - VMALLOC_SIZE)
#define VMALLOC_END	(PAGE_OFFSET)

/*                                                                          
                                                                     
 */
#define USER_KERNEL_GUTTER    0x10000000

/*                    
                                                                            
                                                                             
                                                                              
                                                                      
                                                 
 */
#define TASK_SIZE	(PAGE_OFFSET - VMALLOC_SIZE - USER_KERNEL_GUTTER)

#define STACK_TOP       TASK_SIZE
#define STACK_TOP_MAX   STACK_TOP

/*                                                                 
                       
 */
#define TASK_UNMAPPED_BASE      (TASK_SIZE / 3)

#endif /*            */

#endif /*                       */
