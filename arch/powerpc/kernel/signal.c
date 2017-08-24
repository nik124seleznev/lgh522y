/*
 * Common signal handling code for both 32 and 64 bits
 *
 *    Copyright (c) 2007 Benjamin Herrenschmidt, IBM Coproration
 *    Extracted from signal_32.c and signal_64.c
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file README.legal in the main directory of
 * this archive for more details.
 */

#include <linux/tracehook.h>
#include <linux/signal.h>
#include <linux/uprobes.h>
#include <linux/key.h>
#include <linux/context_tracking.h>
#include <asm/hw_breakpoint.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/debug.h>
#include <asm/tm.h>

#include "signal.h"

/*                                                                       
                                        
 */

int show_unhandled_signals = 1;

/*
                                      
 */
void __user * get_sigframe(struct k_sigaction *ka, unsigned long sp,
			   size_t frame_size, int is_32)
{
        unsigned long oldsp, newsp;

        /*                               */
        oldsp = get_clean_sp(sp, is_32);

	/*                     */
	if ((ka->sa.sa_flags & SA_ONSTACK) &&
	    current->sas_ss_size && !on_sig_stack(oldsp))
		oldsp = (current->sas_ss_sp + current->sas_ss_size);

	/*                   */
	newsp = (oldsp - frame_size) & ~0xFUL;

	/*              */
	if (!access_ok(VERIFY_WRITE, (void __user *)newsp, oldsp - newsp))
		return NULL;

        return (void __user *)newsp;
}

static void check_syscall_restart(struct pt_regs *regs, struct k_sigaction *ka,
				  int has_handler)
{
	unsigned long ret = regs->gpr[3];
	int restart = 1;

	/*           */
	if (TRAP(regs) != 0x0C00)
		return;

	/*                   */
	if (!(regs->ccr & 0x10000000))
		return;

	switch (ret) {
	case ERESTART_RESTARTBLOCK:
	case ERESTARTNOHAND:
		/*                                                     
                                                                
                                                             
   */
		restart = !has_handler;
		break;
	case ERESTARTSYS:
		/*                                                        
                                                          
   */
		restart = !has_handler || (ka->sa.sa_flags & SA_RESTART) != 0;
		break;
	case ERESTARTNOINTR:
		/*                                                
                                                   
   */
		break;
	default:
		return;
	}
	if (restart) {
		if (ret == ERESTART_RESTARTBLOCK)
			regs->gpr[0] = __NR_restart_syscall;
		else
			regs->gpr[3] = regs->orig_gpr3;
		regs->nip -= 4;
		regs->result = 0;
	} else {
		regs->result = -EINTR;
		regs->gpr[3] = EINTR;
		regs->ccr |= 0x10000000;
	}
}

static int do_signal(struct pt_regs *regs)
{
	sigset_t *oldset = sigmask_to_save();
	siginfo_t info;
	int signr;
	struct k_sigaction ka;
	int ret;
	int is32 = is_32bit_task();

	signr = get_signal_to_deliver(&info, &ka, regs, NULL);

	/*                                              */
	check_syscall_restart(regs, &ka, signr > 0);

	if (signr <= 0) {
		/*                                                    */
		restore_saved_sigmask();
		regs->trap = 0;
		return 0;               /*                      */
	}

#ifndef CONFIG_PPC_ADV_DEBUG_REGS
        /*
                                                     
                                                     
                                
  */
	if (current->thread.hw_brk.address &&
		current->thread.hw_brk.type)
		set_breakpoint(&current->thread.hw_brk);
#endif
	/*                                                */
	thread_change_pc(current, regs);

	if (is32) {
        	if (ka.sa.sa_flags & SA_SIGINFO)
			ret = handle_rt_signal32(signr, &ka, &info, oldset,
					regs);
		else
			ret = handle_signal32(signr, &ka, &info, oldset,
					regs);
	} else {
		ret = handle_rt_signal64(signr, &ka, &info, oldset, regs);
	}

	regs->trap = 0;
	if (ret) {
		signal_delivered(signr, &info, &ka, regs,
					 test_thread_flag(TIF_SINGLESTEP));
	}

	return ret;
}

void do_notify_resume(struct pt_regs *regs, unsigned long thread_info_flags)
{
	user_exit();

	if (thread_info_flags & _TIF_UPROBE)
		uprobe_notify_resume(regs);

	if (thread_info_flags & _TIF_SIGPENDING)
		do_signal(regs);

	if (thread_info_flags & _TIF_NOTIFY_RESUME) {
		clear_thread_flag(TIF_NOTIFY_RESUME);
		tracehook_notify_resume(regs);
	}

	user_enter();
}

unsigned long get_tm_stackpointer(struct pt_regs *regs)
{
	/*                                                                 
                                                                        
                                                                     
                                                                       
                                                                     
                                                                      
                                                                       
                                                                       
                  
   
                                                                    
                                                                     
                                                                    
                                                                       
                                                                      
                                                                   
                       
   
                                                             
                                          
  */

#ifdef CONFIG_PPC_TRANSACTIONAL_MEM
	if (MSR_TM_ACTIVE(regs->msr)) {
		tm_enable();
		tm_reclaim(&current->thread, regs->msr, TM_CAUSE_SIGNAL);
		if (MSR_TM_TRANSACTIONAL(regs->msr))
			return current->thread.ckpt_regs.gpr[1];
	}
#endif
	return regs->gpr[1];
}
