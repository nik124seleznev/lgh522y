/*
 * Process creation support for Hexagon
 *
 * Copyright (c) 2010-2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <linux/sched.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/tick.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/tracehook.h>

/*
                                                                   
                                                                      
                                  
  
                                                                       
                                                                          
                                                         
 */
void start_thread(struct pt_regs *regs, unsigned long pc, unsigned long sp)
{
	/*                                             */
	set_fs(USER_DS);
	/*                                                                  */
	memset(regs, 0, sizeof(*regs));
	/*                                                         */
	pt_set_usermode(regs);
	pt_set_elr(regs, pc);
	pt_set_rte_sp(regs, sp);
}

/*
                                                               
                                                                   
                 
 */
void arch_cpu_idle(void)
{
	__vmwait();
	/*                                                     */
	local_irq_enable();
}

/*
                                       
 */
unsigned long thread_saved_pc(struct task_struct *tsk)
{
	return 0;
}

/*
                                          
 */
int copy_thread(unsigned long clone_flags, unsigned long usp,
		unsigned long arg, struct task_struct *p)
{
	struct thread_info *ti = task_thread_info(p);
	struct hexagon_switch_stack *ss;
	struct pt_regs *childregs;
	asmlinkage void ret_from_fork(void);

	childregs = (struct pt_regs *) (((unsigned long) ti + THREAD_SIZE) -
					sizeof(*childregs));

	ti->regs = childregs;

	/*
                                                                
                                                            
                                                                 
                                                           
  */
	ss = (struct hexagon_switch_stack *) ((unsigned long) childregs -
						    sizeof(*ss));
	ss->lr = (unsigned long)ret_from_fork;
	p->thread.switch_sp = ss;
	if (unlikely(p->flags & PF_KTHREAD)) {
		memset(childregs, 0, sizeof(struct pt_regs));
		/*                       */
		ss->r24 = usp;
		ss->r25 = arg;
		pt_set_kmode(childregs);
		return 0;
	}
	memcpy(childregs, current_pt_regs(), sizeof(*childregs));
	ss->r2524 = 0;

	if (usp)
		pt_set_rte_sp(childregs, usp);

	/*                              */
	childregs->r00 = 0;

	/*
                                          
                                  
                                     
                                    
                                   
                                               
                                       
  */
	if (clone_flags & CLONE_SETTLS)
		childregs->ugp = childregs->r04;

	/*
                                                              
                                  
                                                     
  */

	return 0;
}

/*
                                                               
 */
void release_thread(struct task_struct *dead_task)
{
}

/*
                                                              
 */
void exit_thread(void)
{
}

/*
                                           
 */
void flush_thread(void)
{
}

/*
                                                              
                                                           
                                   
 */
unsigned long get_wchan(struct task_struct *p)
{
	unsigned long fp, pc;
	unsigned long stack_page;
	int count = 0;
	if (!p || p == current || p->state == TASK_RUNNING)
		return 0;

	stack_page = (unsigned long)task_stack_page(p);
	fp = ((struct hexagon_switch_stack *)p->thread.switch_sp)->fp;
	do {
		if (fp < (stack_page + sizeof(struct thread_info)) ||
			fp >= (THREAD_SIZE - 8 + stack_page))
			return 0;
		pc = ((unsigned long *)fp)[1];
		if (!in_sched_functions(pc))
			return pc;
		fp = *(unsigned long *) fp;
	} while (count++ < 16);

	return 0;
}

/*
                        
 */
int dump_fpu(struct pt_regs *regs, elf_fpregset_t *fpu)
{
	return 0;
}


/*
                                                         
  
                                       
  
                                                          
 */

int do_work_pending(struct pt_regs *regs, u32 thread_info_flags)
{
	if (!(thread_info_flags & _TIF_WORK_MASK)) {
		return 0;
	}  /*                                */

	local_irq_enable();

	if (thread_info_flags & _TIF_NEED_RESCHED) {
		schedule();
		return 1;
	}

	if (thread_info_flags & _TIF_SIGPENDING) {
		do_signal(regs);
		return 1;
	}

	if (thread_info_flags & _TIF_NOTIFY_RESUME) {
		clear_thread_flag(TIF_NOTIFY_RESUME);
		tracehook_notify_resume(regs);
		return 1;
	}

	/*                            */
	panic("%s: bad thread_info flags 0x%08x\n", __func__,
		thread_info_flags);
}
