#ifndef _METAG_PTRACE_H
#define _METAG_PTRACE_H

#include <linux/compiler.h>
#include <uapi/asm/ptrace.h>
#include <asm/tbx.h>

#ifndef __ASSEMBLY__

/*                                                            
                               */

struct pt_regs {
	TBICTX ctx;
	TBICTXEXTCB0 extcb0[5];
};

#define user_mode(regs) (((regs)->ctx.SaveMask & TBICTX_PRIV_BIT) > 0)

#define instruction_pointer(regs) ((unsigned long)(regs)->ctx.CurrPC)
#define profile_pc(regs) instruction_pointer(regs)

#define task_pt_regs(task) \
	((struct pt_regs *)(task_stack_page(task) + \
			    sizeof(struct thread_info)))

#define current_pt_regs() \
	((struct pt_regs *)((char *)current_thread_info() + \
			    sizeof(struct thread_info)))

int syscall_trace_enter(struct pt_regs *regs);
void syscall_trace_leave(struct pt_regs *regs);

/*                                        */
int metag_gp_regs_copyout(const struct pt_regs *regs,
			  unsigned int pos, unsigned int count,
			  void *kbuf, void __user *ubuf);
/*                                         */
int metag_gp_regs_copyin(struct pt_regs *regs,
			 unsigned int pos, unsigned int count,
			 const void *kbuf, const void __user *ubuf);
/*                                        */
int metag_cb_regs_copyout(const struct pt_regs *regs,
			  unsigned int pos, unsigned int count,
			  void *kbuf, void __user *ubuf);
/*                                         */
int metag_cb_regs_copyin(struct pt_regs *regs,
			 unsigned int pos, unsigned int count,
			 const void *kbuf, const void __user *ubuf);
/*                                         */
int metag_rp_state_copyout(const struct pt_regs *regs,
			   unsigned int pos, unsigned int count,
			   void *kbuf, void __user *ubuf);
/*                                          */
int metag_rp_state_copyin(struct pt_regs *regs,
			  unsigned int pos, unsigned int count,
			  const void *kbuf, const void __user *ubuf);

#endif /*              */
#endif /*                 */
