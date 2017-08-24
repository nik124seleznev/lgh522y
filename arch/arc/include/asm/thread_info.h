/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Vineetg: Oct 2009
 *  No need for ARC specific thread_info allocator (kmalloc/free). This is
 *  anyways one page allocation, thus slab alloc can be short-circuited and
 *  the generic version (get_free_page) would be loads better.
 *
 * Sameer Dhavale: Codito Technologies 2004
 */

#ifndef _ASM_THREAD_INFO_H
#define _ASM_THREAD_INFO_H

#ifdef __KERNEL__

#include <asm/page.h>

#ifdef CONFIG_16KSTACKS
#define THREAD_SIZE_ORDER 1
#else
#define THREAD_SIZE_ORDER 0
#endif

#define THREAD_SIZE     (PAGE_SIZE << THREAD_SIZE_ORDER)

#ifndef __ASSEMBLY__

#include <linux/thread_info.h>
#include <asm/segment.h>

/*
                                                             
                                                             
                                                  
                                                                          
                         
 */
struct thread_info {
	unsigned long flags;		/*                 */
	int preempt_count;		/*                             */
	struct task_struct *task;	/*                     */
	mm_segment_t addr_limit;	/*                      */
	struct exec_domain *exec_domain;/*                  */
	__u32 cpu;			/*             */
	unsigned long thr_ptr;		/*         */
	struct restart_block restart_block;
};

/*
                                                                          
  
                                                                            
 */
#define INIT_THREAD_INFO(tsk)			\
{						\
	.task       = &tsk,			\
	.exec_domain    = &default_exec_domain,	\
	.flags      = 0,			\
	.cpu        = 0,			\
	.preempt_count  = INIT_PREEMPT_COUNT,	\
	.addr_limit = KERNEL_DS,		\
	.restart_block  = {			\
		.fn = do_no_restart_syscall,	\
	},					\
}

#define init_thread_info    (init_thread_union.thread_info)
#define init_stack          (init_thread_union.stack)

static inline __attribute_const__ struct thread_info *current_thread_info(void)
{
	register unsigned long sp asm("sp");
	return (struct thread_info *)(sp & ~(THREAD_SIZE - 1));
}

#endif /*               */

#define PREEMPT_ACTIVE      0x10000000

/*
                           
                                                                          
           
                                             
                       
 */
#define TIF_RESTORE_SIGMASK	0	/*                                 */
#define TIF_NOTIFY_RESUME	1	/*                                   */
#define TIF_SIGPENDING		2	/*                */
#define TIF_NEED_RESCHED	3	/*                        */
#define TIF_SYSCALL_AUDIT	4	/*                         */
#define TIF_SYSCALL_TRACE	15	/*                      */

/*                                                 */
#define TIF_MEMDIE		16

#define _TIF_SYSCALL_TRACE	(1<<TIF_SYSCALL_TRACE)
#define _TIF_NOTIFY_RESUME	(1<<TIF_NOTIFY_RESUME)
#define _TIF_SIGPENDING		(1<<TIF_SIGPENDING)
#define _TIF_NEED_RESCHED	(1<<TIF_NEED_RESCHED)
#define _TIF_SYSCALL_AUDIT	(1<<TIF_SYSCALL_AUDIT)
#define _TIF_MEMDIE		(1<<TIF_MEMDIE)

/*                                          */
#define _TIF_WORK_MASK		(_TIF_NEED_RESCHED | _TIF_SIGPENDING | \
				 _TIF_NOTIFY_RESUME)

/*
                                                                  
                                                                          
                                                              
 */

#endif /*            */

#endif /*                    */
