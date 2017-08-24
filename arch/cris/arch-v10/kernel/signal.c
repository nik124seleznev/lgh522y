/*
 *  linux/arch/cris/kernel/signal.c
 *
 *  Based on arch/i386/kernel/signal.c by
 *     Copyright (C) 1991, 1992  Linus Torvalds
 *     1997-11-28  Modified for POSIX.1b signals by Richard Henderson *
 *
 *  Ideas also taken from arch/arm.
 *
 *  Copyright (C) 2000-2007 Axis Communications AB
 *
 *  Authors:  Bjorn Wesen (bjornw@axis.com)
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

#include <asm/processor.h>
#include <asm/ucontext.h>
#include <asm/uaccess.h>
#include <arch/system.h>

#define DEBUG_SIG 0

/*                                                                    */
/*                                                             */

/*                                                                      
                                                                        
 */
#define RESTART_CRIS_SYS(regs) regs->r10 = regs->orig_r10; regs->irp -= 2;

void do_signal(int canrestart, struct pt_regs *regs);

/*
                                             
 */

struct sigframe {
	struct sigcontext sc;
	unsigned long extramask[_NSIG_WORDS-1];
	unsigned char retcode[8];  /*                 */
};

struct rt_sigframe {
	struct siginfo *pinfo;
	void *puc;
	struct siginfo info;
	struct ucontext uc;
	unsigned char retcode[8];  /*                 */
};


static int
restore_sigcontext(struct pt_regs *regs, struct sigcontext __user *sc)
{
	unsigned int err = 0;
	unsigned long old_usp;

        /*                                                              */
	current_thread_info()->restart_block.fn = do_no_restart_syscall;

	/*                                                                  
                                                                 
                                         
  */

	if (__copy_from_user(regs, sc, sizeof(struct pt_regs)))
                goto badframe;

	/*                                                         */

	regs->dccr |= 1 << 8;

	/*                                                            
                                                                  
                            
  */

	err |= __get_user(old_usp, &sc->usp);

	wrusp(old_usp);

	/*                                                                  
                                                                       
                
  */

	return err;

badframe:
	return 1;
}

asmlinkage int sys_sigreturn(void)
{
	struct pt_regs *regs = current_pt_regs();
	struct sigframe __user *frame = (struct sigframe *)rdusp();
	sigset_t set;

        /*
                                                           
                                                            
                                                        
         */
        if (((long)frame) & 3)
                goto badframe;

	if (!access_ok(VERIFY_READ, frame, sizeof(*frame)))
		goto badframe;
	if (__get_user(set.sig[0], &frame->sc.oldmask)
	    || (_NSIG_WORDS > 1
		&& __copy_from_user(&set.sig[1], frame->extramask,
				    sizeof(frame->extramask))))
		goto badframe;

	set_current_blocked(&set);

	if (restore_sigcontext(regs, &frame->sc))
		goto badframe;

	/*                                                */

	return regs->r10;

badframe:
	force_sig(SIGSEGV, current);
	return 0;
}

asmlinkage int sys_rt_sigreturn(void)
{
	struct pt_regs *regs = current_pt_regs();
	struct rt_sigframe __user *frame = (struct rt_sigframe *)rdusp();
	sigset_t set;

        /*
                                                           
                                                            
                                                        
         */
        if (((long)frame) & 3)
                goto badframe;

	if (!access_ok(VERIFY_READ, frame, sizeof(*frame)))
		goto badframe;
	if (__copy_from_user(&set, &frame->uc.uc_sigmask, sizeof(set)))
		goto badframe;

	set_current_blocked(&set);

	if (restore_sigcontext(regs, &frame->uc.uc_mcontext))
		goto badframe;

	if (restore_altstack(&frame->uc.uc_stack))
		goto badframe;

	return regs->r10;

badframe:
	force_sig(SIGSEGV, current);
	return 0;
}

/*
                         
 */

static int setup_sigcontext(struct sigcontext __user *sc,
	struct pt_regs *regs, unsigned long mask)
{
	int err = 0;
	unsigned long usp = rdusp();

	/*                                                               */

	err |= __copy_to_user(sc, regs, sizeof(struct pt_regs));

        /*                                                            
                                                                             
                                        */
        regs->frametype = CRIS_FRAME_NORMAL;

	/*                       */

	err |= __put_user(mask, &sc->oldmask);

	err |= __put_user(usp, &sc->usp);

	return err;
}

/*                                                     
                           */

static inline void __user *
get_sigframe(struct k_sigaction *ka, struct pt_regs *regs, size_t frame_size)
{
	unsigned long sp = rdusp();

	/*                                                        */
	if (ka->sa.sa_flags & SA_ONSTACK) {
		if (! on_sig_stack(sp))
			sp = current->sas_ss_sp + current->sas_ss_size;
	}

	/*                                      */

	sp &= ~3;

	return (void __user*)(sp - frame_size);
}

/*                               
  
                                                              
                                                           
                                                                 
                        
 */

static int setup_frame(int sig, struct k_sigaction *ka,
		       sigset_t *set, struct pt_regs *regs)
{
	struct sigframe __user *frame;
	unsigned long return_ip;
	int err = 0;

	frame = get_sigframe(ka, regs, sizeof(*frame));

	if (!access_ok(VERIFY_WRITE, frame, sizeof(*frame)))
		goto give_sigsegv;

	err |= setup_sigcontext(&frame->sc, regs, set->sig[0]);
	if (err)
		goto give_sigsegv;

	if (_NSIG_WORDS > 1) {
		err |= __copy_to_user(frame->extramask, &set->sig[1],
				      sizeof(frame->extramask));
	}
	if (err)
		goto give_sigsegv;

	/*                                                          
                           */
	if (ka->sa.sa_flags & SA_RESTORER) {
		return_ip = (unsigned long)ka->sa.sa_restorer;
	} else {
		/*                                                          */
		return_ip = (unsigned long)&frame->retcode;
		/*                                              */
		err |= __put_user(0x9c5f,         (short __user*)(frame->retcode+0));
		err |= __put_user(__NR_sigreturn, (short __user*)(frame->retcode+2));
		err |= __put_user(0xe93d,         (short __user*)(frame->retcode+4));
	}

	if (err)
		goto give_sigsegv;

	/*                                     */

	regs->irp = (unsigned long) ka->sa.sa_handler;  /*                     */
	regs->srp = return_ip;                          /*                     */
	regs->r10 = sig;                                /*                         */

	/*                                                    */

	wrusp((unsigned long)frame);

	return 0;

give_sigsegv:
	force_sigsegv(sig, current);
	return -EFAULT;
}

static int setup_rt_frame(int sig, struct k_sigaction *ka, siginfo_t *info,
	sigset_t *set, struct pt_regs *regs)
{
	struct rt_sigframe __user *frame;
	unsigned long return_ip;
	int err = 0;

	frame = get_sigframe(ka, regs, sizeof(*frame));

	if (!access_ok(VERIFY_WRITE, frame, sizeof(*frame)))
		goto give_sigsegv;

	err |= __put_user(&frame->info, &frame->pinfo);
	err |= __put_user(&frame->uc, &frame->puc);
	err |= copy_siginfo_to_user(&frame->info, info);
	if (err)
		goto give_sigsegv;

	/*                                                   */
        err |= __clear_user(&frame->uc, offsetof(struct ucontext, uc_mcontext));

	err |= setup_sigcontext(&frame->uc.uc_mcontext, regs, set->sig[0]);

	err |= __copy_to_user(&frame->uc.uc_sigmask, set, sizeof(*set));

	err |= __save_altstack(&frame->uc.uc_stack, rdusp());

	if (err)
		goto give_sigsegv;

	/*                                                          
                           */
	if (ka->sa.sa_flags & SA_RESTORER) {
		return_ip = (unsigned long)ka->sa.sa_restorer;
	} else {
		/*                                                          */
		return_ip = (unsigned long)&frame->retcode;
		/*                                                 */
		err |= __put_user(0x9c5f, (short __user *)(frame->retcode+0));
		err |= __put_user(__NR_rt_sigreturn,
			(short __user *)(frame->retcode+2));
		err |= __put_user(0xe93d, (short __user *)(frame->retcode+4));
	}

	if (err)
		goto give_sigsegv;

	/*                                                          */

	/*                                     */

	/*                     */
	regs->irp = (unsigned long) ka->sa.sa_handler;
	/*                     */
	regs->srp = return_ip;
	/*                         */
	regs->r10 = sig;
	/*                                  */
	regs->r11 = (unsigned long)&frame->info;
	/*                          */
	regs->r12 = 0;

	/*                                                    */
	wrusp((unsigned long)frame);

	return 0;

give_sigsegv:
	force_sigsegv(sig, current);
	return -EFAULT;
}

/*
                               
 */

static inline void handle_signal(int canrestart, unsigned long sig,
	siginfo_t *info, struct k_sigaction *ka,
	struct pt_regs *regs)
{
	sigset_t *oldset = sigmask_to_save();
	int ret;

	/*                            */
	if (canrestart) {
		/*                                       */
		switch (regs->r10) {
		case -ERESTART_RESTARTBLOCK:
		case -ERESTARTNOHAND:
			/*                                             
                                                   
                                                     
                                     */
			regs->r10 = -EINTR;
			break;
		case -ERESTARTSYS:
			/*                                            
                                            
                                 */
			if (!(ka->sa.sa_flags & SA_RESTART)) {
				regs->r10 = -EINTR;
				break;
			}
		/*             */
		case -ERESTARTNOINTR:
			/*                                             
                                                        */
			RESTART_CRIS_SYS(regs);
		}
	}

	/*                        */
	if (ka->sa.sa_flags & SA_SIGINFO)
		ret = setup_rt_frame(sig, ka, info, oldset, regs);
	else
		ret = setup_frame(sig, ka, oldset, regs);

	if (ret == 0)
		signal_delivered(sig, info, ka, regs, 0);
}

/*
                                                                           
                                                                        
           
  
                                                                             
                                                                              
                                                                         
                                                                            
              
 */

void do_signal(int canrestart, struct pt_regs *regs)
{
	siginfo_t info;
	int signr;
        struct k_sigaction ka;

	/*
                                             
                                                
                                                   
          
  */
	if (!user_mode(regs))
		return;

	signr = get_signal_to_deliver(&info, &ka, regs, NULL);
	if (signr > 0) {
		/*                                      */
		handle_signal(canrestart, signr, &info, &ka, regs);
		return;
	}

	/*                                 */
	if (canrestart) {
		/*                                               */
		if (regs->r10 == -ERESTARTNOHAND ||
		    regs->r10 == -ERESTARTSYS ||
		    regs->r10 == -ERESTARTNOINTR) {
			RESTART_CRIS_SYS(regs);
		}
		if (regs->r10 == -ERESTART_RESTARTBLOCK) {
			regs->r9 = __NR_restart_syscall;
			regs->irp -= 2;
		}
	}

	/*                                                               
         */
	restore_saved_sigmask();
}
