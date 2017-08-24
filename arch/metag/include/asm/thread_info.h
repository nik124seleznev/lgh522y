/* thread_info.h: Meta low-level thread information
 *
 * Copyright (C) 2002  David Howells (dhowells@redhat.com)
 * - Incorporating suggestions made by Linus Torvalds and Dave Miller
 *
 * Meta port by Imagination Technologies
 */

#ifndef _ASM_THREAD_INFO_H
#define _ASM_THREAD_INFO_H

#include <linux/compiler.h>
#include <asm/page.h>

#ifndef __ASSEMBLY__
#include <asm/processor.h>
#endif

/*
                                                             
                                                             
                                                  
                                                                               
                    
 */
#ifndef __ASSEMBLY__

/*                                                               */
struct thread_info {
	struct task_struct *task;	/*                     */
	struct exec_domain *exec_domain;	/*                  */
	unsigned long flags;	/*                 */
	unsigned long status;	/*                          */
	u32 cpu;		/*             */
	int preempt_count;	/*                             */

	mm_segment_t addr_limit;	/*                      */
	struct restart_block restart_block;

	u8 supervisor_stack[0];
};

#else /*               */

#include <generated/asm-offsets.h>

#endif

#define PREEMPT_ACTIVE		0x10000000

#ifdef CONFIG_4KSTACKS
#define THREAD_SHIFT		12
#else
#define THREAD_SHIFT		13
#endif

#if THREAD_SHIFT >= PAGE_SHIFT
#define THREAD_SIZE_ORDER	(THREAD_SHIFT - PAGE_SHIFT)
#else
#define THREAD_SIZE_ORDER	0
#endif

#define THREAD_SIZE		(PAGE_SIZE << THREAD_SIZE_ORDER)

#define STACK_WARN		(THREAD_SIZE/8)
/*
                                                                          
 */
#ifndef __ASSEMBLY__

#define INIT_THREAD_INFO(tsk)			\
{						\
	.task		= &tsk,			\
	.exec_domain	= &default_exec_domain,	\
	.flags		= 0,			\
	.cpu		= 0,			\
	.preempt_count	= INIT_PREEMPT_COUNT,	\
	.addr_limit	= KERNEL_DS,		\
	.restart_block = {			\
		.fn = do_no_restart_syscall,	\
	},					\
}

#define init_thread_info	(init_thread_union.thread_info)
#define init_stack		(init_thread_union.stack)

/*                                             */
register unsigned long current_stack_pointer asm("A0StP") __used;

/*                                                 */
static inline struct thread_info *current_thread_info(void)
{
	return (struct thread_info *)(current_stack_pointer &
				      ~(THREAD_SIZE - 1));
}

#define __HAVE_ARCH_KSTACK_END
static inline int kstack_end(void *addr)
{
	return addr == (void *) (((unsigned long) addr & ~(THREAD_SIZE - 1))
				 + sizeof(struct thread_info));
}

#endif

/*
                           
                                                                          
           
                                             
                       
 */
#define TIF_SYSCALL_TRACE	0	/*                      */
#define TIF_SIGPENDING		1	/*                */
#define TIF_NEED_RESCHED	2	/*                        */
#define TIF_SINGLESTEP		3	/*                                     
             */
#define TIF_SYSCALL_AUDIT	4	/*                         */
#define TIF_SECCOMP		5	/*                  */
#define TIF_RESTORE_SIGMASK	6	/*                                    */
#define TIF_NOTIFY_RESUME	7	/*                                   */
#define TIF_POLLING_NRFLAG      8	/*                               
                         */
#define TIF_MEMDIE		9	/*                                  */
#define TIF_SYSCALL_TRACEPOINT  10	/*                                    */


#define _TIF_SYSCALL_TRACE	(1<<TIF_SYSCALL_TRACE)
#define _TIF_SIGPENDING		(1<<TIF_SIGPENDING)
#define _TIF_NEED_RESCHED	(1<<TIF_NEED_RESCHED)
#define _TIF_SINGLESTEP		(1<<TIF_SINGLESTEP)
#define _TIF_SYSCALL_AUDIT	(1<<TIF_SYSCALL_AUDIT)
#define _TIF_SECCOMP		(1<<TIF_SECCOMP)
#define _TIF_NOTIFY_RESUME	(1<<TIF_NOTIFY_RESUME)
#define _TIF_RESTORE_SIGMASK	(1<<TIF_RESTORE_SIGMASK)
#define _TIF_SYSCALL_TRACEPOINT	(1<<TIF_SYSCALL_TRACEPOINT)

/*                             */
#define _TIF_WORK_SYSCALL_MASK	(_TIF_SYSCALL_TRACE | _TIF_SINGLESTEP | \
				 _TIF_SYSCALL_AUDIT | _TIF_SECCOMP | \
				 _TIF_SYSCALL_TRACEPOINT)

/*                                     */
#define _TIF_ALLWORK_MASK	(_TIF_SYSCALL_TRACE | _TIF_SIGPENDING      | \
				 _TIF_NEED_RESCHED  | _TIF_SYSCALL_AUDIT   | \
				 _TIF_SINGLESTEP    | _TIF_RESTORE_SIGMASK | \
				 _TIF_NOTIFY_RESUME)

/*                                          */
#define _TIF_WORK_MASK		(_TIF_ALLWORK_MASK & ~(_TIF_SYSCALL_TRACE | \
				 _TIF_SYSCALL_AUDIT | _TIF_SINGLESTEP))

#endif /*                    */
