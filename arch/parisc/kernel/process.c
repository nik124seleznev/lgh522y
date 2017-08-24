/*
 *    PARISC Architecture-dependent parts of process handling
 *    based on the work for i386
 *
 *    Copyright (C) 1999-2003 Matthew Wilcox <willy at parisc-linux.org>
 *    Copyright (C) 2000 Martin K Petersen <mkp at mkp.net>
 *    Copyright (C) 2000 John Marvin <jsm at parisc-linux.org>
 *    Copyright (C) 2000 David Huggins-Daines <dhd with pobox.org>
 *    Copyright (C) 2000-2003 Paul Bame <bame at parisc-linux.org>
 *    Copyright (C) 2000 Philipp Rumpf <prumpf with tux.org>
 *    Copyright (C) 2000 David Kennedy <dkennedy with linuxcare.com>
 *    Copyright (C) 2000 Richard Hirst <rhirst with parisc-linux.org>
 *    Copyright (C) 2000 Grant Grundler <grundler with parisc-linux.org>
 *    Copyright (C) 2001 Alan Modra <amodra at parisc-linux.org>
 *    Copyright (C) 2001-2002 Ryan Bradetich <rbrad at parisc-linux.org>
 *    Copyright (C) 2001-2007 Helge Deller <deller at parisc-linux.org>
 *    Copyright (C) 2002 Randolph Chung <tausq with parisc-linux.org>
 *
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdarg.h>

#include <linux/elf.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/stddef.h>
#include <linux/unistd.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/rcupdate.h>

#include <asm/io.h>
#include <asm/asm-offsets.h>
#include <asm/assembly.h>
#include <asm/pdc.h>
#include <asm/pdc_chassis.h>
#include <asm/pgalloc.h>
#include <asm/unwind.h>
#include <asm/sections.h>

#define COMMAND_GLOBAL  F_EXTEND(0xfffe0030)
#define CMD_RESET       5       /*                  */

/*
                                                           
                                                           
                                                           
                                                                 
                               
  
                                                                 
                                                                
                                                                   
                                                           
*/
void machine_restart(char *cmd)
{
#ifdef FASTBOOT_SELFTEST_SUPPORT
	/*
                                                       
                                                    
                                       
    
                                                        
    
                                                               
                                                       
                                             
  */
	if (ftc_bitmap) {
		pdc_do_firm_test_reset(ftc_bitmap);
	}
#endif
	/*                                                                  */
	pdc_chassis_send_status(PDC_CHASSIS_DIRECT_SHUTDOWN);
	
	/*                       */
	pdc_do_reset();

	/*                                                 */
	gsc_writel(CMD_RESET, COMMAND_GLOBAL);

	/*                                   */
	while (1) ;

}

void machine_halt(void)
{
	/*
                                                      
                                                  
 */
}

void (*chassis_power_off)(void);

/*
                                                                  
           
 */
void machine_power_off(void)
{
	/*                                                      */
	if (chassis_power_off)
		chassis_power_off();

	/*                                                       
                                                         
                                               */
	pdc_soft_power_button(0);
	
	pdc_chassis_send_status(PDC_CHASSIS_DIRECT_SHUTDOWN);
		
	/*                                                    
                                                        */

	printk(KERN_EMERG "System shut down completed.\n"
	       "Please power this system off now.");
}

void (*pm_power_off)(void) = machine_power_off;
EXPORT_SYMBOL(pm_power_off);

/*
                                            
 */
void exit_thread(void)
{
}

void flush_thread(void)
{
	/*                                                 
                                                         
 */
}

void release_thread(struct task_struct *dead_task)
{
}

/*
                                             
 */

int dump_fpu (struct pt_regs * regs, elf_fpregset_t *r)
{
	if (regs == NULL)
		return 0;

	memcpy(r, regs->fr, sizeof *r);
	return 1;
}

int dump_task_fpu (struct task_struct *tsk, elf_fpregset_t *r)
{
	memcpy(r, tsk->thread.regs.fr, sizeof(*r));
	return 1;
}

int
copy_thread(unsigned long clone_flags, unsigned long usp,
	    unsigned long arg, struct task_struct *p)
{
	struct pt_regs *cregs = &(p->thread.regs);
	void *stack = task_stack_page(p);
	
	/*                                                             
                                                                 
                                                             */
	extern void * const ret_from_kernel_thread;
	extern void * const child_return;
#ifdef CONFIG_HPUX
	extern void * const hpux_child_return;
#endif
	if (unlikely(p->flags & PF_KTHREAD)) {
		memset(cregs, 0, sizeof(struct pt_regs));
		if (!usp) /*             */
			return 0;

		/*               */
		/*                                              
                            
   */
		cregs->ksp = (unsigned long)stack + THREAD_SZ_ALGN + FRAME_SIZE;
		cregs->kpc = (unsigned long) &ret_from_kernel_thread;
		/*
                                                 
                            
   */
#ifdef CONFIG_64BIT
		cregs->gr[27] = ((unsigned long *)usp)[3];
		cregs->gr[26] = ((unsigned long *)usp)[2];
#else
		cregs->gr[26] = usp;
#endif
		cregs->gr[25] = arg;
	} else {
		/*             */
		/*                                                         
                                                              
                                 */
		if (usp) {
			usp = ALIGN(usp, 4);
			if (likely(usp))
				cregs->gr[30] = usp;
		}
		cregs->ksp = (unsigned long)stack + THREAD_SZ_ALGN + FRAME_SIZE;
		if (personality(p->personality) == PER_HPUX) {
#ifdef CONFIG_HPUX
			cregs->kpc = (unsigned long) &hpux_child_return;
#else
			BUG();
#endif
		} else {
			cregs->kpc = (unsigned long) &child_return;
		}
		/*                                                       */
		if (clone_flags & CLONE_SETTLS)
			cregs->cr27 = cregs->gr[23];
	}

	return 0;
}

unsigned long thread_saved_pc(struct task_struct *t)
{
	return t->thread.regs.kpc;
}

unsigned long
get_wchan(struct task_struct *p)
{
	struct unwind_frame_info info;
	unsigned long ip;
	int count = 0;

	if (!p || p == current || p->state == TASK_RUNNING)
		return 0;

	/*
                                          
  */

	unwind_frame_init_from_blocked_task(&info, p);
	do {
		if (unwind_once(&info) < 0)
			return 0;
		ip = info.ip;
		if (!in_sched_functions(ip))
			return ip;
	} while (count++ < 16);
	return 0;
}

#ifdef CONFIG_64BIT
void *dereference_function_descriptor(void *ptr)
{
	Elf64_Fdesc *desc = ptr;
	void *p;

	if (!probe_kernel_address(&desc->addr, p))
		ptr = p;
	return ptr;
}
#endif
