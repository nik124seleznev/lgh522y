#ifndef _ALPHA_THREAD_INFO_H
#define _ALPHA_THREAD_INFO_H

#ifdef __KERNEL__

#ifndef __ASSEMBLY__
#include <asm/processor.h>
#include <asm/types.h>
#include <asm/hwrpb.h>
#include <asm/sysinfo.h>
#endif

#ifndef __ASSEMBLY__
struct thread_info {
	struct pcb_struct	pcb;		/*               */

	struct task_struct	*task;		/*                     */
	unsigned int		flags;		/*                 */
	unsigned int		ieee_state;	/*           */

	struct exec_domain	*exec_domain;	/*                  */
	mm_segment_t		addr_limit;	/*                      */
	unsigned		cpu;		/*             */
	int			preempt_count; /*                             */
	unsigned int		status;		/*                          */

	int bpt_nsaved;
	unsigned long bpt_addr[2];		/*                      */
	unsigned int bpt_insn[2];

	struct restart_block	restart_block;
};

/*
                                                                           
 */
#define INIT_THREAD_INFO(tsk)			\
{						\
	.task		= &tsk,			\
	.exec_domain	= &default_exec_domain,	\
	.addr_limit	= KERNEL_DS,		\
	.preempt_count	= INIT_PREEMPT_COUNT,	\
	.restart_block = {			\
		.fn = do_no_restart_syscall,	\
	},					\
}

#define init_thread_info	(init_thread_union.thread_info)
#define init_stack		(init_thread_union.stack)

/*                                                   */
register struct thread_info *__current_thread_info __asm__("$8");
#define current_thread_info()  __current_thread_info

#endif /*              */

/*                                 */
#define THREAD_SIZE_ORDER 1
#define THREAD_SIZE (2*PAGE_SIZE)

#define PREEMPT_ACTIVE		0x40000000

/*
                            
                                                         
                                                                        
                                                        
  
                                               
 */
#define TIF_SYSCALL_TRACE	0	/*                      */
#define TIF_NOTIFY_RESUME	1	/*                                   */
#define TIF_SIGPENDING		2	/*                */
#define TIF_NEED_RESCHED	3	/*                        */
#define TIF_DIE_IF_KERNEL	9	/*                    */
#define TIF_MEMDIE		13	/*                                  */

#define _TIF_SYSCALL_TRACE	(1<<TIF_SYSCALL_TRACE)
#define _TIF_SIGPENDING		(1<<TIF_SIGPENDING)
#define _TIF_NEED_RESCHED	(1<<TIF_NEED_RESCHED)
#define _TIF_NOTIFY_RESUME	(1<<TIF_NOTIFY_RESUME)

/*                                            */
#define _TIF_WORK_MASK		(_TIF_SIGPENDING | _TIF_NEED_RESCHED | \
				 _TIF_NOTIFY_RESUME)

/*                                         */
#define _TIF_ALLWORK_MASK	(_TIF_WORK_MASK		\
				 | _TIF_SYSCALL_TRACE)

#define TS_UAC_NOPRINT		0x0001	/*                                */
#define TS_UAC_NOFIX		0x0002	/*                                */
#define TS_UAC_SIGBUS		0x0004	/*                                   */
#define TS_RESTORE_SIGMASK	0x0008	/*                                    */
#define TS_POLLING		0x0010	/*                                
                               */

#ifndef __ASSEMBLY__
#define HAVE_SET_RESTORE_SIGMASK	1
static inline void set_restore_sigmask(void)
{
	struct thread_info *ti = current_thread_info();
	ti->status |= TS_RESTORE_SIGMASK;
	WARN_ON(!test_bit(TIF_SIGPENDING, (unsigned long *)&ti->flags));
}
static inline void clear_restore_sigmask(void)
{
	current_thread_info()->status &= ~TS_RESTORE_SIGMASK;
}
static inline bool test_restore_sigmask(void)
{
	return current_thread_info()->status & TS_RESTORE_SIGMASK;
}
static inline bool test_and_clear_restore_sigmask(void)
{
	struct thread_info *ti = current_thread_info();
	if (!(ti->status & TS_RESTORE_SIGMASK))
		return false;
	ti->status &= ~TS_RESTORE_SIGMASK;
	return true;
}
#endif

#define SET_UNALIGN_CTL(task,value)	({				\
	__u32 status = task_thread_info(task)->status & ~UAC_BITMASK;	\
	if (value & PR_UNALIGN_NOPRINT)					\
		status |= TS_UAC_NOPRINT;				\
	if (value & PR_UNALIGN_SIGBUS)					\
		status |= TS_UAC_SIGBUS;				\
	if (value & 4)	/*                */				\
		status |= TS_UAC_NOFIX;					\
	task_thread_info(task)->status = status;			\
	0; })

#define GET_UNALIGN_CTL(task,value)	({				\
	__u32 status = task_thread_info(task)->status & ~UAC_BITMASK;	\
	__u32 res = 0;							\
	if (status & TS_UAC_NOPRINT)					\
		res |= PR_UNALIGN_NOPRINT;				\
	if (status & TS_UAC_SIGBUS)					\
		res |= PR_UNALIGN_SIGBUS;				\
	if (status & TS_UAC_NOFIX)					\
		res |= 4;						\
	put_user(res, (int __user *)(value));				\
	})

#endif /*            */
#endif /*                      */
