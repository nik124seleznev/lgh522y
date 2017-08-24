/*
 *	stacktrace.c : stacktracing APIs needed by rest of kernel
 *			(wrappers over ARC dwarf based unwinder)
 *
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  vineetg: aug 2009
 *  -Implemented CONFIG_STACKTRACE APIs, primarily save_stack_trace_tsk( )
 *   for displaying task's kernel mode call stack in /proc/<pid>/stack
 *  -Iterator based approach to have single copy of unwinding core and APIs
 *   needing unwinding, implement the logic in iterator regarding:
 *      = which frame onwards to start capture
 *      = which frame to stop capturing (wchan)
 *      = specifics of data structs where trace is saved(CONFIG_STACKTRACE etc)
 *
 *  vineetg: March 2009
 *  -Implemented correct versions of thread_saved_pc() and get_wchan()
 *
 *  rajeshwarr: 2008
 *  -Initial implementation
 */

#include <linux/ptrace.h>
#include <linux/export.h>
#include <linux/stacktrace.h>
#include <linux/kallsyms.h>
#include <asm/arcregs.h>
#include <asm/unwind.h>
#include <asm/switch_to.h>

/*                                                                         
                                 
                                                                           
 */

#ifdef CONFIG_ARC_DW2_UNWIND

static void seed_unwind_frame_info(struct task_struct *tsk,
				   struct pt_regs *regs,
				   struct unwind_frame_info *frame_info)
{
	if (tsk == NULL && regs == NULL) {
		unsigned long fp, sp, blink, ret;
		frame_info->task = current;

		__asm__ __volatile__(
			"mov %0,r27\n\t"
			"mov %1,r28\n\t"
			"mov %2,r31\n\t"
			"mov %3,r63\n\t"
			: "=r"(fp), "=r"(sp), "=r"(blink), "=r"(ret)
		);

		frame_info->regs.r27 = fp;
		frame_info->regs.r28 = sp;
		frame_info->regs.r31 = blink;
		frame_info->regs.r63 = ret;
		frame_info->call_frame = 0;
	} else if (regs == NULL) {

		frame_info->task = tsk;

		frame_info->regs.r27 = KSTK_FP(tsk);
		frame_info->regs.r28 = KSTK_ESP(tsk);
		frame_info->regs.r31 = KSTK_BLINK(tsk);
		frame_info->regs.r63 = (unsigned int)__switch_to;

		/*                                                           
                                                               
                                                             
                             
                                                                
                                                            
                                                                  
                  
   */
		frame_info->regs.r27 = 0;
		frame_info->regs.r28 += 64;
		frame_info->call_frame = 0;

	} else {
		frame_info->task = tsk;

		frame_info->regs.r27 = regs->fp;
		frame_info->regs.r28 = regs->sp;
		frame_info->regs.r31 = regs->blink;
		frame_info->regs.r63 = regs->ret;
		frame_info->call_frame = 0;
	}
}

#endif

static noinline unsigned int
arc_unwind_core(struct task_struct *tsk, struct pt_regs *regs,
		int (*consumer_fn) (unsigned int, void *), void *arg)
{
#ifdef CONFIG_ARC_DW2_UNWIND
	int ret = 0;
	unsigned int address;
	struct unwind_frame_info frame_info;

	seed_unwind_frame_info(tsk, regs, &frame_info);

	while (1) {
		address = UNW_PC(&frame_info);

		if (address && __kernel_text_address(address)) {
			if (consumer_fn(address, arg) == -1)
				break;
		}

		ret = arc_unwind(&frame_info);

		if (ret == 0) {
			frame_info.regs.r63 = frame_info.regs.r31;
			continue;
		} else {
			break;
		}
	}

	return address;		/*                                */
#else
	/*                                                                 
                                                                      
                                                                      
              
  */
	pr_warn("CONFIG_ARC_DW2_UNWIND needs to be enabled\n");
	return 0;

#endif
}

/*                                                                         
                                                                 
  
                                                                             
                                          
                                                                           
 */

/*                                                               
                             
 */
static int __print_sym(unsigned int address, void *unused)
{
	__print_symbol("  %s\n", address);
	return 0;
}

#ifdef CONFIG_STACKTRACE

/*                                                         
                                               
 */
static int __collect_all(unsigned int address, void *arg)
{
	struct stack_trace *trace = arg;

	if (trace->skip > 0)
		trace->skip--;
	else
		trace->entries[trace->nr_entries++] = address;

	if (trace->nr_entries >= trace->max_entries)
		return -1;

	return 0;
}

static int __collect_all_but_sched(unsigned int address, void *arg)
{
	struct stack_trace *trace = arg;

	if (in_sched_functions(address))
		return 0;

	if (trace->skip > 0)
		trace->skip--;
	else
		trace->entries[trace->nr_entries++] = address;

	if (trace->nr_entries >= trace->max_entries)
		return -1;

	return 0;
}

#endif

static int __get_first_nonsched(unsigned int address, void *unused)
{
	if (in_sched_functions(address))
		return 0;

	return -1;
}

/*                                                                         
                                                           
                                                                           
 */

noinline void show_stacktrace(struct task_struct *tsk, struct pt_regs *regs)
{
	pr_info("\nStack Trace:\n");
	arc_unwind_core(tsk, regs, __print_sym, NULL);
}
EXPORT_SYMBOL(show_stacktrace);

/*                        */
void show_stack(struct task_struct *tsk, unsigned long *sp)
{
	show_stacktrace(tsk, NULL);
}

/*                                                                    
                                                                         
                                        
 */
unsigned int get_wchan(struct task_struct *tsk)
{
	return arc_unwind_core(tsk, NULL, __get_first_nonsched, NULL);
}

#ifdef CONFIG_STACKTRACE

/*
                                                        
                                                                 
 */
void save_stack_trace_tsk(struct task_struct *tsk, struct stack_trace *trace)
{
	arc_unwind_core(tsk, NULL, __collect_all_but_sched, trace);
}

void save_stack_trace(struct stack_trace *trace)
{
	arc_unwind_core(current, NULL, __collect_all, trace);
}
#endif
