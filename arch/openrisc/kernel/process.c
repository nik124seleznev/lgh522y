/*
 * OpenRISC process.c
 *
 * Linux architectural port borrowing liberally from similar works of
 * others.  All original copyrights apply as per the original source
 * declaration.
 *
 * Modifications for the OpenRISC architecture:
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 *
 * This file handles the architecture-dependent parts of process handling...
 */

#define __KERNEL_SYSCALLS__
#include <stdarg.h>

#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/stddef.h>
#include <linux/unistd.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/elfcore.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/init_task.h>
#include <linux/mqueue.h>
#include <linux/fs.h>

#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <asm/processor.h>
#include <asm/spr_defs.h>

#include <linux/smp.h>

/*
                                            
  
                                            
 */
struct thread_info *current_thread_info_set[NR_CPUS] = { &init_thread_info, };

void machine_restart(void)
{
	printk(KERN_INFO "*** MACHINE RESTART ***\n");
	__asm__("l.nop 1");
}

/*
                                                                    
                                                                    
                                                             
 */
void machine_halt(void)
{
	printk(KERN_INFO "*** MACHINE HALT ***\n");
	__asm__("l.nop 1");
}

/*                                                               */
void machine_power_off(void)
{
	printk(KERN_INFO "*** MACHINE POWER OFF ***\n");
	__asm__("l.nop 1");
}

void (*pm_power_off) (void) = machine_power_off;

/*
                                                                  
                                                                 
                                                                     
 */
void flush_thread(void)
{
}

void show_regs(struct pt_regs *regs)
{
	extern void show_registers(struct pt_regs *regs);

	show_regs_print_info(KERN_DEFAULT);
	/*                           */
	show_registers(regs);
}

unsigned long thread_saved_pc(struct task_struct *t)
{
	return (unsigned long)user_regs(t->stack)->pc;
}

void release_thread(struct task_struct *dead_task)
{
}

/*
                                                                 
                           
 */
extern asmlinkage void ret_from_fork(void);

/*
              
                      
                                                   
                                                                      
                             
                                                                           
  
                                                                        
                                                                           
                                                       
  
                                                                             
                                                                        
                                                                             
                                                                              
                
  
                                                                  
                                                                         
                                                                           
                                                                         
             
  
                                                                         
                                                                          
                                                                        
                                                                     
                                                                
 */

int
copy_thread(unsigned long clone_flags, unsigned long usp,
	    unsigned long arg, struct task_struct *p)
{
	struct pt_regs *userregs;
	struct pt_regs *kregs;
	unsigned long sp = (unsigned long)task_stack_page(p) + THREAD_SIZE;
	unsigned long top_of_kernel_stack;

	top_of_kernel_stack = sp;

	p->set_child_tid = p->clear_child_tid = NULL;

	/*                                      */
	sp -= STACK_FRAME_OVERHEAD;	/*         */
	sp -= sizeof(struct pt_regs);
	userregs = (struct pt_regs *) sp;

	/*                       */
	sp -= STACK_FRAME_OVERHEAD;	/*         */
	sp -= sizeof(struct pt_regs);
	kregs = (struct pt_regs *)sp;

	if (unlikely(p->flags & PF_KTHREAD)) {
		memset(kregs, 0, sizeof(struct pt_regs));
		kregs->gpr[20] = usp; /*                   */
		kregs->gpr[22] = arg;
	} else {
		*userregs = *current_pt_regs();

		if (usp)
			userregs->sp = usp;
		userregs->gpr[11] = 0;	/*                    */

		kregs->gpr[20] = 0;	/*                  */
	}

	/*
                                                                 
                                                                  
  */
	kregs->sp = top_of_kernel_stack;
	kregs->gpr[9] = (unsigned long)ret_from_fork;

	task_thread_info(p)->ksp = (unsigned long)kregs;

	return 0;
}

/*
                                              
 */
void start_thread(struct pt_regs *regs, unsigned long pc, unsigned long sp)
{
	unsigned long sr = mfspr(SPR_SR) & ~SPR_SR_SM;

	set_fs(USER_DS);
	memset(regs, 0, sizeof(struct pt_regs));

	regs->pc = pc;
	regs->sr = sr;
	regs->sp = sp;
}

/*                                             */
int dump_fpu(struct pt_regs *regs, elf_fpregset_t * fpu)
{
	/*      */
	return 0;
}

extern struct thread_info *_switch(struct thread_info *old_ti,
				   struct thread_info *new_ti);

struct task_struct *__switch_to(struct task_struct *old,
				struct task_struct *new)
{
	struct task_struct *last;
	struct thread_info *new_ti, *old_ti;
	unsigned long flags;

	local_irq_save(flags);

	/*                                                  
                                                                
                                                 
  */
	new_ti = new->stack;
	old_ti = old->stack;

	current_thread_info_set[smp_processor_id()] = new_ti;
	last = (_switch(old_ti, new_ti))->task;

	local_irq_restore(flags);

	return last;
}

/*
                                                             
                          
 */
void dump_elf_thread(elf_greg_t *dest, struct pt_regs* regs)
{
	dest[0] = 0; /*    */
	memcpy(dest+1, regs->gpr+1, 31*sizeof(unsigned long));
	dest[32] = regs->pc;
	dest[33] = regs->sr;
	dest[34] = 0;
	dest[35] = 0;
}

unsigned long get_wchan(struct task_struct *p)
{
	/*      */

	return 0;
}