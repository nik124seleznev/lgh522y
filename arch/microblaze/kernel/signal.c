/*
 * Signal handling
 *
 * Copyright (C) 2008-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2008-2009 PetaLogix
 * Copyright (C) 2003,2004 John Williams <jwilliams@itee.uq.edu.au>
 * Copyright (C) 2001 NEC Corporation
 * Copyright (C) 2001 Miles Bader <miles@gnu.org>
 * Copyright (C) 1999,2000 Niibe Yutaka & Kaz Kojima
 * Copyright (C) 1991,1992 Linus Torvalds
 *
 * 1997-11-28 Modified for POSIX.1b signals by Richard Henderson
 *
 * This file was was derived from the sh version, arch/sh/kernel/signal.c
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License. See the file COPYING in the main directory of this
 * archive for more details.
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
#include <linux/percpu.h>
#include <linux/linkage.h>
#include <linux/tracehook.h>
#include <asm/entry.h>
#include <asm/ucontext.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <linux/syscalls.h>
#include <asm/cacheflush.h>
#include <asm/syscalls.h>

/*
                                             
 */
struct sigframe {
	struct sigcontext sc;
	unsigned long extramask[_NSIG_WORDS-1];
	unsigned long tramp[2];	/*                   */
};

struct rt_sigframe {
	struct siginfo info;
	struct ucontext uc;
	unsigned long tramp[2];	/*                   */
};

static int restore_sigcontext(struct pt_regs *regs,
				struct sigcontext __user *sc, int *rval_p)
{
	unsigned int err = 0;

#define COPY(x)		{err |= __get_user(regs->x, &sc->regs.x); }
	COPY(r0);
	COPY(r1);
	COPY(r2);	COPY(r3);	COPY(r4);	COPY(r5);
	COPY(r6);	COPY(r7);	COPY(r8);	COPY(r9);
	COPY(r10);	COPY(r11);	COPY(r12);	COPY(r13);
	COPY(r14);	COPY(r15);	COPY(r16);	COPY(r17);
	COPY(r18);	COPY(r19);	COPY(r20);	COPY(r21);
	COPY(r22);	COPY(r23);	COPY(r24);	COPY(r25);
	COPY(r26);	COPY(r27);	COPY(r28);	COPY(r29);
	COPY(r30);	COPY(r31);
	COPY(pc);	COPY(ear);	COPY(esr);	COPY(fsr);
#undef COPY

	*rval_p = regs->r3;

	return err;
}

asmlinkage long sys_rt_sigreturn(struct pt_regs *regs)
{
	struct rt_sigframe __user *frame =
		(struct rt_sigframe __user *)(regs->r1);

	sigset_t set;
	int rval;

	/*                                                              */
	current_thread_info()->restart_block.fn = do_no_restart_syscall;

	if (!access_ok(VERIFY_READ, frame, sizeof(*frame)))
		goto badframe;

	if (__copy_from_user(&set, &frame->uc.uc_sigmask, sizeof(set)))
		goto badframe;

	set_current_blocked(&set);

	if (restore_sigcontext(regs, &frame->uc.uc_mcontext, &rval))
		goto badframe;

	if (restore_altstack(&frame->uc.uc_stack))
		goto badframe;

	return rval;

badframe:
	force_sig(SIGSEGV, current);
	return 0;
}

/*
                         
 */

static int
setup_sigcontext(struct sigcontext __user *sc, struct pt_regs *regs,
		unsigned long mask)
{
	int err = 0;

#define COPY(x)		{err |= __put_user(regs->x, &sc->regs.x); }
	COPY(r0);
	COPY(r1);
	COPY(r2);	COPY(r3);	COPY(r4);	COPY(r5);
	COPY(r6);	COPY(r7);	COPY(r8);	COPY(r9);
	COPY(r10);	COPY(r11);	COPY(r12);	COPY(r13);
	COPY(r14);	COPY(r15);	COPY(r16);	COPY(r17);
	COPY(r18);	COPY(r19);	COPY(r20);	COPY(r21);
	COPY(r22);	COPY(r23);	COPY(r24);	COPY(r25);
	COPY(r26);	COPY(r27);	COPY(r28);	COPY(r29);
	COPY(r30);	COPY(r31);
	COPY(pc);	COPY(ear);	COPY(esr);	COPY(fsr);
#undef COPY

	err |= __put_user(mask, &sc->oldmask);

	return err;
}

/*
                                 
 */
static inline void __user *
get_sigframe(struct k_sigaction *ka, struct pt_regs *regs, size_t frame_size)
{
	/*                               */
	unsigned long sp = regs->r1;

	if ((ka->sa.sa_flags & SA_ONSTACK) != 0 && !on_sig_stack(sp))
		sp = current->sas_ss_sp + current->sas_ss_size;

	return (void __user *)((sp - frame_size) & -8UL);
}

static int setup_rt_frame(int sig, struct k_sigaction *ka, siginfo_t *info,
			sigset_t *set, struct pt_regs *regs)
{
	struct rt_sigframe __user *frame;
	int err = 0;
	int signal;
	unsigned long address = 0;
#ifdef CONFIG_MMU
	pmd_t *pmdp;
	pte_t *ptep;
#endif

	frame = get_sigframe(ka, regs, sizeof(*frame));

	if (!access_ok(VERIFY_WRITE, frame, sizeof(*frame)))
		goto give_sigsegv;

	signal = current_thread_info()->exec_domain
		&& current_thread_info()->exec_domain->signal_invmap
		&& sig < 32
		? current_thread_info()->exec_domain->signal_invmap[sig]
		: sig;

	if (info)
		err |= copy_siginfo_to_user(&frame->info, info);

	/*                      */
	err |= __put_user(0, &frame->uc.uc_flags);
	err |= __put_user(NULL, &frame->uc.uc_link);
	err |= __save_altstack(&frame->uc.uc_stack, regs->r1);
	err |= setup_sigcontext(&frame->uc.uc_mcontext,
			regs, set->sig[0]);
	err |= __copy_to_user(&frame->uc.uc_sigmask, set, sizeof(*set));

	/*                                                         
                        */
	/*                                             */
	/*                              */
	err |= __put_user(0x31800000 | __NR_rt_sigreturn ,
			frame->tramp + 0);
	/*               */
	err |= __put_user(0xb9cc0008, frame->tramp + 1);

	/*                                               
                                                  */
	regs->r15 = ((unsigned long)frame->tramp)-8;

	address = ((unsigned long)frame->tramp);
#ifdef CONFIG_MMU
	pmdp = pmd_offset(pud_offset(
			pgd_offset(current->mm, address),
					address), address);

	preempt_disable();
	ptep = pte_offset_map(pmdp, address);
	if (pte_present(*ptep)) {
		address = (unsigned long) page_address(pte_page(*ptep));
		/*                               */
		address += ((unsigned long)frame->tramp) & ~PAGE_MASK;
		/*                       */
		address = virt_to_phys(address);
		invalidate_icache_range(address, address + 8);
		flush_dcache_range(address, address + 8);
	}
	pte_unmap(ptep);
	preempt_enable();
#else
	flush_icache_range(address, address + 8);
	flush_dcache_range(address, address + 8);
#endif
	if (err)
		goto give_sigsegv;

	/*                                     */
	regs->r1 = (unsigned long) frame;

	/*                      */
	regs->r5 = signal; /*               */
	regs->r6 = (unsigned long) &frame->info; /*                */
	regs->r7 = (unsigned long) &frame->uc; /*                */
	/*                                         */
	regs->pc = (unsigned long)ka->sa.sa_handler;

	set_fs(USER_DS);

#ifdef DEBUG_SIG
	pr_info("SIG deliver (%s:%d): sp=%p pc=%08lx\n",
		current->comm, current->pid, frame, regs->pc);
#endif

	return 0;

give_sigsegv:
	force_sigsegv(sig, current);
	return -EFAULT;
}

/*                                */
static inline void
handle_restart(struct pt_regs *regs, struct k_sigaction *ka, int has_handler)
{
	switch (regs->r3) {
	case -ERESTART_RESTARTBLOCK:
	case -ERESTARTNOHAND:
		if (!has_handler)
			goto do_restart;
		regs->r3 = -EINTR;
		break;
	case -ERESTARTSYS:
		if (has_handler && !(ka->sa.sa_flags & SA_RESTART)) {
			regs->r3 = -EINTR;
			break;
	}
	/*             */
	case -ERESTARTNOINTR:
do_restart:
		/*                                                         */
		regs->pc -= 4;
		break;
	}
}

/*
                               
 */

static void
handle_signal(unsigned long sig, struct k_sigaction *ka,
		siginfo_t *info, struct pt_regs *regs)
{
	sigset_t *oldset = sigmask_to_save();
	int ret;

	/*                        */
	if (ka->sa.sa_flags & SA_SIGINFO)
		ret = setup_rt_frame(sig, ka, info, oldset, regs);
	else
		ret = setup_rt_frame(sig, ka, NULL, oldset, regs);

	if (ret)
		return;

	signal_delivered(sig, info, ka, regs,
			test_thread_flag(TIF_SINGLESTEP));
}

/*
                                                                           
                                                                        
           
  
                                                                            
                                                                              
                                     
 */
static void do_signal(struct pt_regs *regs, int in_syscall)
{
	siginfo_t info;
	int signr;
	struct k_sigaction ka;
#ifdef DEBUG_SIG
	pr_info("do signal: %p %d\n", regs, in_syscall);
	pr_info("do signal2: %lx %lx %ld [%lx]\n", regs->pc, regs->r1,
			regs->r12, current_thread_info()->flags);
#endif

	signr = get_signal_to_deliver(&info, &ka, regs, NULL);
	if (signr > 0) {
		/*                                    */
		if (in_syscall)
			handle_restart(regs, &ka, 1);
		handle_signal(signr, &ka, &info, regs);
		return;
	}

	if (in_syscall)
		handle_restart(regs, NULL, 0);

	/*
                                                                  
         
  */
	restore_saved_sigmask();
}

asmlinkage void do_notify_resume(struct pt_regs *regs, int in_syscall)
{
	if (test_thread_flag(TIF_SIGPENDING))
		do_signal(regs, in_syscall);

	if (test_and_clear_thread_flag(TIF_NOTIFY_RESUME))
		tracehook_notify_resume(regs);
}
