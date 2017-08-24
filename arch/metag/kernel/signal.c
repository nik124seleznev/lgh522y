/*
 *  Copyright (C) 1991,1992  Linus Torvalds
 *  Copyright (C) 2005-2012  Imagination Technologies Ltd.
 *
 *  1997-11-28  Modified for POSIX.1b signals by Richard Henderson
 *
 */

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/ptrace.h>
#include <linux/unistd.h>
#include <linux/stddef.h>
#include <linux/personality.h>
#include <linux/uaccess.h>
#include <linux/tracehook.h>

#include <asm/ucontext.h>
#include <asm/cacheflush.h>
#include <asm/switch.h>
#include <asm/syscall.h>
#include <asm/syscalls.h>

#define REG_FLAGS	ctx.SaveMask
#define REG_RETVAL	ctx.DX[0].U0
#define REG_SYSCALL	ctx.DX[0].U1
#define REG_SP		ctx.AX[0].U0
#define REG_ARG1	ctx.DX[3].U1
#define REG_ARG2	ctx.DX[3].U0
#define REG_ARG3	ctx.DX[2].U1
#define REG_PC		ctx.CurrPC
#define REG_RTP		ctx.DX[4].U1

struct rt_sigframe {
	struct siginfo info;
	struct ucontext uc;
	unsigned long retcode[2];
};

static int restore_sigcontext(struct pt_regs *regs,
			      struct sigcontext __user *sc)
{
	int err;

	/*                                                              */
	current_thread_info()->restart_block.fn = do_no_restart_syscall;

	err = metag_gp_regs_copyin(regs, 0, sizeof(struct user_gp_regs), NULL,
				   &sc->regs);
	if (!err)
		err = metag_cb_regs_copyin(regs, 0,
					   sizeof(struct user_cb_regs), NULL,
					   &sc->cb);
	if (!err)
		err = metag_rp_state_copyin(regs, 0,
					    sizeof(struct user_rp_state), NULL,
					    &sc->rp);

	/*                              */
	regs->REG_FLAGS |= TBICTX_PRIV_BIT;

	return err;
}

long sys_rt_sigreturn(void)
{
	/*                                                            */
	struct pt_regs *regs = current_pt_regs();
	struct rt_sigframe __user *frame;
	sigset_t set;

	frame = (__force struct rt_sigframe __user *)(regs->REG_SP -
						      sizeof(*frame));

	if (!access_ok(VERIFY_READ, frame, sizeof(*frame)))
		goto badframe;

	if (__copy_from_user(&set, &frame->uc.uc_sigmask, sizeof(set)))
		goto badframe;

	set_current_blocked(&set);

	if (restore_sigcontext(regs, &frame->uc.uc_mcontext))
		goto badframe;

	if (restore_altstack(&frame->uc.uc_stack))
		goto badframe;

	return regs->REG_RETVAL;

badframe:
	force_sig(SIGSEGV, current);

	return 0;
}

static int setup_sigcontext(struct sigcontext __user *sc, struct pt_regs *regs,
			    unsigned long mask)
{
	int err;

	err = metag_gp_regs_copyout(regs, 0, sizeof(struct user_gp_regs), NULL,
				    &sc->regs);

	if (!err)
		err = metag_cb_regs_copyout(regs, 0,
					    sizeof(struct user_cb_regs), NULL,
					    &sc->cb);
	if (!err)
		err = metag_rp_state_copyout(regs, 0,
					     sizeof(struct user_rp_state), NULL,
					     &sc->rp);

	/*                                                           
                                                               
                                                          
                            
                                                           
                                                          
          
  */
	regs->REG_FLAGS &= ~(TBICTX_XCBF_BIT | TBICTX_CBUF_BIT |
			     TBICTX_CBRP_BIT);

	/*                                                            
                  
  */
	regs->ctx.Flags &= ~TXSTATUS_LSM_STEP_BITS;

	err |= __put_user(mask, &sc->oldmask);

	return err;
}

/*
                                 
 */
static void __user *get_sigframe(struct k_sigaction *ka, unsigned long sp,
				 size_t frame_size)
{
	/*                           */
	if ((ka->sa.sa_flags & SA_ONSTACK) && (sas_ss_flags(sp) == 0))
		sp = current->sas_ss_sp;

	sp = (sp + 7) & ~7;			/*                   */

	return (void __user *)sp;
}

static int setup_rt_frame(int sig, struct k_sigaction *ka, siginfo_t *info,
			  sigset_t *set, struct pt_regs *regs)
{
	struct rt_sigframe __user *frame;
	int err = -EFAULT;
	unsigned long code;

	frame = get_sigframe(ka, regs->REG_SP, sizeof(*frame));
	if (!access_ok(VERIFY_WRITE, frame, sizeof(*frame)))
		goto out;

	err = copy_siginfo_to_user(&frame->info, info);

	/*                       */
	err |= __put_user(0, &frame->uc.uc_flags);
	err |= __put_user(0, (unsigned long __user *)&frame->uc.uc_link);
	err |= __save_altstack(&frame->uc.uc_stack, regs->REG_SP);
	err |= setup_sigcontext(&frame->uc.uc_mcontext,
				regs, set->sig[0]);
	err |= __copy_to_user(&frame->uc.uc_sigmask, set, sizeof(*set));

	if (err)
		goto out;

	/*                                   */

	/*                                      */
	code = 0x03000004 | (__NR_rt_sigreturn << 3);
	err |= __put_user(code, (unsigned long __user *)(&frame->retcode[0]));

	/*                        */
	code = __METAG_SW_ENCODING(SYS);
	err |= __put_user(code, (unsigned long __user *)(&frame->retcode[1]));

	if (err)
		goto out;

	/*                                     */
	regs->REG_RTP = (unsigned long) frame->retcode;
	regs->REG_SP = (unsigned long) frame + sizeof(*frame);
	regs->REG_ARG1 = sig;
	regs->REG_ARG2 = (unsigned long) &frame->info;
	regs->REG_ARG3 = (unsigned long) &frame->uc;
	regs->REG_PC = (unsigned long) ka->sa.sa_handler;

	pr_debug("SIG deliver (%s:%d): sp=%p pc=%08x pr=%08x\n",
		 current->comm, current->pid, frame, regs->REG_PC,
		 regs->REG_RTP);

	/*                                                              
                                                              
  */
	flush_cache_sigtramp(regs->REG_RTP, sizeof(frame->retcode));
out:
	if (err) {
		force_sigsegv(sig, current);
		return -EFAULT;
	}
	return 0;
}

static void handle_signal(unsigned long sig, siginfo_t *info,
			  struct k_sigaction *ka, struct pt_regs *regs)
{
	sigset_t *oldset = sigmask_to_save();

	/*                        */
	if (setup_rt_frame(sig, ka, info, oldset, regs))
		return;

	signal_delivered(sig, info, ka, regs, test_thread_flag(TIF_SINGLESTEP));
}

 /*
                   
                                                                              
                                                         
                                                                               
                        
  */
static int do_signal(struct pt_regs *regs, int syscall)
{
	unsigned int retval = 0, continue_addr = 0, restart_addr = 0;
	struct k_sigaction ka;
	siginfo_t info;
	int signr;
	int restart = 0;

	/*
                                                                       
                                                                     
                                                                       
                                      
  */
	if (syscall == __NR_rt_sigreturn)
		syscall = -1;

	/*                                 */
	if (syscall >= 0) {
		continue_addr = regs->REG_PC;
		restart_addr = continue_addr - 4;
		retval = regs->REG_RETVAL;

		/*
                                                               
                                              
   */
		switch (retval) {
		case -ERESTART_RESTARTBLOCK:
			restart = -2;
		case -ERESTARTNOHAND:
		case -ERESTARTSYS:
		case -ERESTARTNOINTR:
			++restart;
			regs->REG_PC = restart_addr;
			break;
		}
	}

	/*
                                                                       
                                                 
  */
	signr = get_signal_to_deliver(&info, &ka, regs, NULL);
	/*
                                                                       
                                                                         
                              
  */
	if (regs->REG_PC != restart_addr)
		restart = 0;
	if (signr > 0) {
		if (unlikely(restart)) {
			if (retval == -ERESTARTNOHAND
			    || retval == -ERESTART_RESTARTBLOCK
			    || (retval == -ERESTARTSYS
				&& !(ka.sa.sa_flags & SA_RESTART))) {
				regs->REG_RETVAL = -EINTR;
				regs->REG_PC = continue_addr;
			}
		}

		/*                                     */
		handle_signal(signr, &info, &ka, regs);
		return 0;
	}

	/*                                                                  */
	if (unlikely(restart < 0))
		regs->REG_SYSCALL = __NR_restart_syscall;

	/*
                                                                        
  */
	restore_saved_sigmask();

	return restart;
}

int do_work_pending(struct pt_regs *regs, unsigned int thread_flags,
		    int syscall)
{
	do {
		if (likely(thread_flags & _TIF_NEED_RESCHED)) {
			schedule();
		} else {
			if (unlikely(!user_mode(regs)))
				return 0;
			local_irq_enable();
			if (thread_flags & _TIF_SIGPENDING) {
				int restart = do_signal(regs, syscall);
				if (unlikely(restart)) {
					/*
                                 
                                    
                         
      */
					return restart;
				}
				syscall = -1;
			} else {
				clear_thread_flag(TIF_NOTIFY_RESUME);
				tracehook_notify_resume(regs);
			}
		}
		local_irq_disable();
		thread_flags = current_thread_info()->flags;
	} while (thread_flags & _TIF_WORK_MASK);
	return 0;
}
