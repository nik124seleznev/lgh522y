/*  linux/arch/sparc/kernel/signal.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 *  Copyright (C) 1996 Miguel de Icaza (miguel@nuclecu.unam.mx)
 *  Copyright (C) 1997 Eddie C. Dost   (ecd@skynet.be)
 */

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/ptrace.h>
#include <linux/unistd.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/smp.h>
#include <linux/binfmts.h>	/*            */
#include <linux/bitops.h>
#include <linux/tracehook.h>

#include <asm/uaccess.h>
#include <asm/ptrace.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>
#include <asm/cacheflush.h>	/*                 */
#include <asm/switch_to.h>

#include "sigutil.h"

extern void fpsave(unsigned long *fpregs, unsigned long *fsr,
		   void *fpqueue, unsigned long *fpqdepth);
extern void fpload(unsigned long *fpregs, unsigned long *fsr);

struct signal_frame {
	struct sparc_stackf	ss;
	__siginfo32_t		info;
	__siginfo_fpu_t __user	*fpu_save;
	unsigned long		insns[2] __attribute__ ((aligned (8)));
	unsigned int		extramask[_NSIG_WORDS - 1];
	unsigned int		extra_size; /*             */
	__siginfo_rwin_t __user	*rwin_save;
} __attribute__((aligned(8)));

struct rt_signal_frame {
	struct sparc_stackf	ss;
	siginfo_t		info;
	struct pt_regs		regs;
	sigset_t		mask;
	__siginfo_fpu_t __user	*fpu_save;
	unsigned int		insns[2];
	stack_t			stack;
	unsigned int		extra_size; /*             */
	__siginfo_rwin_t __user	*rwin_save;
} __attribute__((aligned(8)));

/*              */
#define SF_ALIGNEDSZ  (((sizeof(struct signal_frame) + 7) & (~7)))
#define RT_ALIGNEDSZ  (((sizeof(struct rt_signal_frame) + 7) & (~7)))

asmlinkage void do_sigreturn(struct pt_regs *regs)
{
	struct signal_frame __user *sf;
	unsigned long up_psr, pc, npc;
	sigset_t set;
	__siginfo_fpu_t __user *fpu_save;
	__siginfo_rwin_t __user *rwin_save;
	int err;

	/*                                                              */
	current_thread_info()->restart_block.fn = do_no_restart_syscall;

	synchronize_user_stack();

	sf = (struct signal_frame __user *) regs->u_regs[UREG_FP];

	/*                                                       */
	if (!access_ok(VERIFY_READ, sf, sizeof(*sf)))
		goto segv_and_exit;

	if (((unsigned long) sf) & 3)
		goto segv_and_exit;

	err = __get_user(pc,  &sf->info.si_regs.pc);
	err |= __get_user(npc, &sf->info.si_regs.npc);

	if ((pc | npc) & 3)
		goto segv_and_exit;

	/*                      */
	up_psr = regs->psr;
	err |= __copy_from_user(regs, &sf->info.si_regs, sizeof(struct pt_regs));

	/*                                                                */
	regs->psr = (up_psr & ~(PSR_ICC | PSR_EF))
		  | (regs->psr & (PSR_ICC | PSR_EF));

	/*                           */
	pt_regs_clear_syscall(regs);

	err |= __get_user(fpu_save, &sf->fpu_save);
	if (fpu_save)
		err |= restore_fpu_state(regs, fpu_save);
	err |= __get_user(rwin_save, &sf->rwin_save);
	if (rwin_save)
		err |= restore_rwin_state(rwin_save);

	/*                                                            
                                  
  */
	err |= __get_user(set.sig[0], &sf->info.si_mask);
	err |= __copy_from_user(&set.sig[1], &sf->extramask,
			        (_NSIG_WORDS-1) * sizeof(unsigned int));
			   
	if (err)
		goto segv_and_exit;

	set_current_blocked(&set);
	return;

segv_and_exit:
	force_sig(SIGSEGV, current);
}

asmlinkage void do_rt_sigreturn(struct pt_regs *regs)
{
	struct rt_signal_frame __user *sf;
	unsigned int psr, pc, npc;
	__siginfo_fpu_t __user *fpu_save;
	__siginfo_rwin_t __user *rwin_save;
	sigset_t set;
	int err;

	synchronize_user_stack();
	sf = (struct rt_signal_frame __user *) regs->u_regs[UREG_FP];
	if (!access_ok(VERIFY_READ, sf, sizeof(*sf)) ||
	    (((unsigned long) sf) & 0x03))
		goto segv;

	err = __get_user(pc, &sf->regs.pc);
	err |= __get_user(npc, &sf->regs.npc);
	err |= ((pc | npc) & 0x03);

	err |= __get_user(regs->y, &sf->regs.y);
	err |= __get_user(psr, &sf->regs.psr);

	err |= __copy_from_user(&regs->u_regs[UREG_G1],
				&sf->regs.u_regs[UREG_G1], 15 * sizeof(u32));

	regs->psr = (regs->psr & ~PSR_ICC) | (psr & PSR_ICC);

	/*                           */
	pt_regs_clear_syscall(regs);

	err |= __get_user(fpu_save, &sf->fpu_save);
	if (!err && fpu_save)
		err |= restore_fpu_state(regs, fpu_save);
	err |= __copy_from_user(&set, &sf->mask, sizeof(sigset_t));
	err |= restore_altstack(&sf->stack);
	
	if (err)
		goto segv;
		
	regs->pc = pc;
	regs->npc = npc;
	
	err |= __get_user(rwin_save, &sf->rwin_save);
	if (!err && rwin_save) {
		if (restore_rwin_state(rwin_save))
			goto segv;
	}

	set_current_blocked(&set);
	return;
segv:
	force_sig(SIGSEGV, current);
}

/*                           */
static inline int invalid_frame_pointer(void __user *fp, int fplen)
{
	if ((((unsigned long) fp) & 7) || !__access_ok((unsigned long)fp, fplen))
		return 1;

	return 0;
}

static inline void __user *get_sigframe(struct ksignal *ksig, struct pt_regs *regs, unsigned long framesize)
{
	unsigned long sp = regs->u_regs[UREG_FP];

	/*
                                                                         
                                                                       
  */
	if (on_sig_stack(sp) && !likely(on_sig_stack(sp - framesize)))
		return (void __user *) -1L;

	/*                                                        */
	sp = sigsp(sp, ksig) - framesize;

	/*                                                               
                                                              
                                                                
                                                                 
                    
  */
	sp &= ~15UL;

	return (void __user *) sp;
}

static int setup_frame(struct ksignal *ksig, struct pt_regs *regs,
		       sigset_t *oldset)
{
	struct signal_frame __user *sf;
	int sigframe_size, err, wsaved;
	void __user *tail;

	/*                                  */
	synchronize_user_stack();

	wsaved = current_thread_info()->w_saved;

	sigframe_size = sizeof(*sf);
	if (used_math())
		sigframe_size += sizeof(__siginfo_fpu_t);
	if (wsaved)
		sigframe_size += sizeof(__siginfo_rwin_t);

	sf = (struct signal_frame __user *)
		get_sigframe(ksig, regs, sigframe_size);

	if (invalid_frame_pointer(sf, sigframe_size)) {
		do_exit(SIGILL);
		return -EINVAL;
	}

	tail = sf + 1;

	/*                                   */
	err = __copy_to_user(&sf->info.si_regs, regs, sizeof(struct pt_regs));
	
	err |= __put_user(0, &sf->extra_size);

	if (used_math()) {
		__siginfo_fpu_t __user *fp = tail;
		tail += sizeof(*fp);
		err |= save_fpu_state(regs, fp);
		err |= __put_user(fp, &sf->fpu_save);
	} else {
		err |= __put_user(0, &sf->fpu_save);
	}
	if (wsaved) {
		__siginfo_rwin_t __user *rwp = tail;
		tail += sizeof(*rwp);
		err |= save_rwin_state(wsaved, rwp);
		err |= __put_user(rwp, &sf->rwin_save);
	} else {
		err |= __put_user(0, &sf->rwin_save);
	}

	err |= __put_user(oldset->sig[0], &sf->info.si_mask);
	err |= __copy_to_user(sf->extramask, &oldset->sig[1],
			      (_NSIG_WORDS - 1) * sizeof(unsigned int));
	if (!wsaved) {
		err |= __copy_to_user(sf, (char *) regs->u_regs[UREG_FP],
				      sizeof(struct reg_window32));
	} else {
		struct reg_window32 *rp;

		rp = &current_thread_info()->reg_window[wsaved - 1];
		err |= __copy_to_user(sf, rp, sizeof(struct reg_window32));
	}
	if (err)
		return err;
	
	/*                                                  */
	regs->u_regs[UREG_FP] = (unsigned long) sf;
	regs->u_regs[UREG_I0] = ksig->sig;
	regs->u_regs[UREG_I1] = (unsigned long) &sf->info;
	regs->u_regs[UREG_I2] = (unsigned long) &sf->info;

	/*                   */
	regs->pc = (unsigned long) ksig->ka.sa.sa_handler;
	regs->npc = (regs->pc + 4);

	/*                                  */
	if (ksig->ka.ka_restorer)
		regs->u_regs[UREG_I7] = (unsigned long)ksig->ka.ka_restorer;
	else {
		regs->u_regs[UREG_I7] = (unsigned long)(&(sf->insns[0]) - 2);

		/*                         */
		err |= __put_user(0x821020d8, &sf->insns[0]);

		/*        */
		err |= __put_user(0x91d02010, &sf->insns[1]);
		if (err)
			return err;

		/*                          */
		flush_sig_insns(current->mm, (unsigned long) &(sf->insns[0]));
	}
	return 0;
}

static int setup_rt_frame(struct ksignal *ksig, struct pt_regs *regs,
			  sigset_t *oldset)
{
	struct rt_signal_frame __user *sf;
	int sigframe_size, wsaved;
	void __user *tail;
	unsigned int psr;
	int err;

	synchronize_user_stack();
	wsaved = current_thread_info()->w_saved;
	sigframe_size = sizeof(*sf);
	if (used_math())
		sigframe_size += sizeof(__siginfo_fpu_t);
	if (wsaved)
		sigframe_size += sizeof(__siginfo_rwin_t);
	sf = (struct rt_signal_frame __user *)
		get_sigframe(ksig, regs, sigframe_size);
	if (invalid_frame_pointer(sf, sigframe_size)) {
		do_exit(SIGILL);
		return -EINVAL;
	}

	tail = sf + 1;
	err  = __put_user(regs->pc, &sf->regs.pc);
	err |= __put_user(regs->npc, &sf->regs.npc);
	err |= __put_user(regs->y, &sf->regs.y);
	psr = regs->psr;
	if (used_math())
		psr |= PSR_EF;
	err |= __put_user(psr, &sf->regs.psr);
	err |= __copy_to_user(&sf->regs.u_regs, regs->u_regs, sizeof(regs->u_regs));
	err |= __put_user(0, &sf->extra_size);

	if (psr & PSR_EF) {
		__siginfo_fpu_t *fp = tail;
		tail += sizeof(*fp);
		err |= save_fpu_state(regs, fp);
		err |= __put_user(fp, &sf->fpu_save);
	} else {
		err |= __put_user(0, &sf->fpu_save);
	}
	if (wsaved) {
		__siginfo_rwin_t *rwp = tail;
		tail += sizeof(*rwp);
		err |= save_rwin_state(wsaved, rwp);
		err |= __put_user(rwp, &sf->rwin_save);
	} else {
		err |= __put_user(0, &sf->rwin_save);
	}
	err |= __copy_to_user(&sf->mask, &oldset->sig[0], sizeof(sigset_t));
	
	/*                   */
	err |= __save_altstack(&sf->stack, regs->u_regs[UREG_FP]);
	
	if (!wsaved) {
		err |= __copy_to_user(sf, (char *) regs->u_regs[UREG_FP],
				      sizeof(struct reg_window32));
	} else {
		struct reg_window32 *rp;

		rp = &current_thread_info()->reg_window[wsaved - 1];
		err |= __copy_to_user(sf, rp, sizeof(struct reg_window32));
	}

	err |= copy_siginfo_to_user(&sf->info, &ksig->info);

	if (err)
		return err;

	regs->u_regs[UREG_FP] = (unsigned long) sf;
	regs->u_regs[UREG_I0] = ksig->sig;
	regs->u_regs[UREG_I1] = (unsigned long) &sf->info;
	regs->u_regs[UREG_I2] = (unsigned long) &sf->regs;

	regs->pc = (unsigned long) ksig->ka.sa.sa_handler;
	regs->npc = (regs->pc + 4);

	if (ksig->ka.ka_restorer)
		regs->u_regs[UREG_I7] = (unsigned long)ksig->ka.ka_restorer;
	else {
		regs->u_regs[UREG_I7] = (unsigned long)(&(sf->insns[0]) - 2);

		/*                         */
		err |= __put_user(0x821020d8, &sf->insns[0]);

		/*        */
		err |= __put_user(0x91d02010, &sf->insns[1]);
		if (err)
			return err;

		/*                          */
		flush_sig_insns(current->mm, (unsigned long) &(sf->insns[0]));
	}
	return 0;
}

static inline void
handle_signal(struct ksignal *ksig, struct pt_regs *regs)
{
	sigset_t *oldset = sigmask_to_save();
	int err;

	if (ksig->ka.sa.sa_flags & SA_SIGINFO)
		err = setup_rt_frame(ksig, regs, oldset);
	else
		err = setup_frame(ksig, regs, oldset);
	signal_setup_done(err, ksig, 0);
}

static inline void syscall_restart(unsigned long orig_i0, struct pt_regs *regs,
				   struct sigaction *sa)
{
	switch(regs->u_regs[UREG_I0]) {
	case ERESTART_RESTARTBLOCK:
	case ERESTARTNOHAND:
	no_system_call_restart:
		regs->u_regs[UREG_I0] = EINTR;
		regs->psr |= PSR_C;
		break;
	case ERESTARTSYS:
		if (!(sa->sa_flags & SA_RESTART))
			goto no_system_call_restart;
		/*             */
	case ERESTARTNOINTR:
		regs->u_regs[UREG_I0] = orig_i0;
		regs->pc -= 4;
		regs->npc -= 4;
	}
}

/*                                                                         
                                                                        
           
 */
static void do_signal(struct pt_regs *regs, unsigned long orig_i0)
{
	struct ksignal ksig;
	int restart_syscall;
	bool has_handler;

	/*                                                           
                                                        
                                                        
   
                                                          
                                                              
                                                              
                                                                
   
                                                              
                                                            
                  
   
                                                              
                                                            
                                                             
                                                                
  */
	if (pt_regs_is_syscall(regs) && (regs->psr & PSR_C))
		regs->u_regs[UREG_G6] = orig_i0;

	has_handler = get_signal(&ksig);

	/*                                                           
                                                              
                      
  */
	restart_syscall = 0;
	if (pt_regs_is_syscall(regs) && (regs->psr & PSR_C)) {
		restart_syscall = 1;
		orig_i0 = regs->u_regs[UREG_G6];
	}

	if (has_handler) {
		if (restart_syscall)
			syscall_restart(orig_i0, regs, &ksig.ka.sa);
		handle_signal(&ksig, regs);
	} else {
		if (restart_syscall) {
			switch (regs->u_regs[UREG_I0]) {
			case ERESTARTNOHAND:
	     		case ERESTARTSYS:
			case ERESTARTNOINTR:
				/*                                         */
				regs->u_regs[UREG_I0] = orig_i0;
				regs->pc -= 4;
				regs->npc -= 4;
				pt_regs_clear_syscall(regs);
			case ERESTART_RESTARTBLOCK:
				regs->u_regs[UREG_G1] = __NR_restart_syscall;
				regs->pc -= 4;
				regs->npc -= 4;
				pt_regs_clear_syscall(regs);
			}
		}
		restore_saved_sigmask();
	}
}

void do_notify_resume(struct pt_regs *regs, unsigned long orig_i0,
		      unsigned long thread_info_flags)
{
	if (thread_info_flags & _TIF_SIGPENDING)
		do_signal(regs, orig_i0);
	if (thread_info_flags & _TIF_NOTIFY_RESUME) {
		clear_thread_flag(TIF_NOTIFY_RESUME);
		tracehook_notify_resume(regs);
	}
}

asmlinkage int
do_sys_sigstack(struct sigstack __user *ssptr, struct sigstack __user *ossptr,
		unsigned long sp)
{
	int ret = -EFAULT;

	/*                                   */
	if (ossptr) {
		if (put_user(current->sas_ss_sp + current->sas_ss_size,
			     &ossptr->the_stack) ||
		    __put_user(on_sig_stack(sp), &ossptr->cur_status))
			goto out;
	}

	/*                                             */
	if (ssptr) {
		char *ss_sp;

		if (get_user(ss_sp, &ssptr->the_stack))
			goto out;
		/*                                                     
                                      */
		ret = -EPERM;
		if (current->sas_ss_sp && on_sig_stack(sp))
			goto out;

		/*                                                          
                                                         
                                                       */
		current->sas_ss_sp = (unsigned long)ss_sp - SIGSTKSZ;
		current->sas_ss_size = SIGSTKSZ;
	}
	ret = 0;
out:
	return ret;
}
