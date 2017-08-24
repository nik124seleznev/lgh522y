/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Amit Bhor, Kanika Nema: Codito Technologies 2004
 */

#include <linux/errno.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/unistd.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/elf.h>
#include <linux/tick.h>

SYSCALL_DEFINE1(arc_settls, void *, user_tls_data_ptr)
{
	task_thread_info(current)->thr_ptr = (unsigned int)user_tls_data_ptr;
	return 0;
}

/*
                                                                
                                     
                                                                 
                       
                                                                    
                                          
                                                                     
             
 */
SYSCALL_DEFINE0(arc_gettls)
{
	return task_thread_info(current)->thr_ptr;
}

void arch_cpu_idle(void)
{
	/*                                                    */
	__asm__("sleep 0x3");
}

asmlinkage void ret_from_fork(void);

/*                                                                          
  
                     
                     
                     
                     
                                                      
                     
                     
                     
                     
                                                  
                     
                     
                     
                                      
                     
                     
                     
                     
                     
                     
                     
                     
                                                   
                     
                     
                     
                                         
 */
int copy_thread(unsigned long clone_flags,
		unsigned long usp, unsigned long arg,
		struct task_struct *p)
{
	struct pt_regs *c_regs;        /*                 */
	unsigned long *childksp;       /*                                */
	struct callee_regs *c_callee;  /*                     */
	struct callee_regs *parent_callee;  /*                */
	struct pt_regs *regs = current_pt_regs();

	/*                                                         */
	c_regs = task_pt_regs(p);
	childksp = (unsigned long *)c_regs - 2;  /*                      */
	c_callee = ((struct callee_regs *)childksp) - 1;

	/*
                                                          
                                                               
                                                       
                                                               
                                                                  
                                                     
  */
	p->thread.ksp = (unsigned long)c_callee;	/*            */

	/*                                                      */
	childksp[0] = 0;			/*    */
	childksp[1] = (unsigned long)ret_from_fork; /*       */

	if (unlikely(p->flags & PF_KTHREAD)) {
		memset(c_regs, 0, sizeof(struct pt_regs));

		c_callee->r13 = arg; /*                           */
		c_callee->r14 = usp;  /*          */

		return 0;
	}

	/*                                       */

	/*                                                               */
	childksp[0] = 0;				/*            */
	childksp[1] = (unsigned long)ret_from_fork;	/*               */

	/*                                                   */
	*c_regs = *regs;

	if (usp)
		c_regs->sp = usp;

	c_regs->r0 = 0;		/*                         */

	parent_callee = ((struct callee_regs *)regs) - 1;
	*c_callee = *parent_callee;

	if (unlikely(clone_flags & CLONE_SETTLS)) {
		/*
                                                  
                                                  
   */
		task_thread_info(p)->thr_ptr = regs->r3;
	} else {
		/*                                                 */
		task_thread_info(p)->thr_ptr =
		task_thread_info(current)->thr_ptr;
	}

	return 0;
}

/*
                                           
 */
void flush_thread(void)
{
}

/*
                                                              
 */
void exit_thread(void)
{
}

int dump_fpu(struct pt_regs *regs, elf_fpregset_t *fpu)
{
	return 0;
}

/*
                                                                        
                                                                           
                                
 */
unsigned long thread_saved_pc(struct task_struct *t)
{
	struct pt_regs *regs = task_pt_regs(t);
	unsigned long blink = 0;

	/*
                                                                       
                                                                      
                                                               
                                                                      
                                            
  */
	if (t != current && t->state != TASK_RUNNING)
		blink = *((unsigned int *)regs - 1);

	return blink;
}

int elf_check_arch(const struct elf32_hdr *x)
{
	unsigned int eflags;

	if (x->e_machine != EM_ARCOMPACT)
		return 0;

	eflags = x->e_flags;
	if ((eflags & EF_ARC_OSABI_MSK) < EF_ARC_OSABI_CURRENT) {
		pr_err("ABI mismatch - you need newer toolchain\n");
		force_sigsegv(SIGSEGV, current);
		return 0;
	}

	return 1;
}
EXPORT_SYMBOL(elf_check_arch);
