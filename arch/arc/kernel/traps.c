/*
 * Traps/Non-MMU Exception handling for ARC
 *
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * vineetg: May 2011
 *  -user-space unaligned access emulation
 *
 * Rahul Trivedi: Codito Technologies 2004
 */

#include <linux/sched.h>
#include <linux/kdebug.h>
#include <linux/uaccess.h>
#include <linux/ptrace.h>
#include <linux/kprobes.h>
#include <linux/kgdb.h>
#include <asm/setup.h>
#include <asm/unaligned.h>
#include <asm/kprobes.h>

void __init trap_init(void)
{
	return;
}

void die(const char *str, struct pt_regs *regs, unsigned long address,
	 unsigned long cause_reg)
{
	show_kernel_fault_diag(str, regs, address, cause_reg);

	/*          */
	__asm__("flag 1");
}

/*
                                                                     
                                              
                                                                   
 */
static noinline int handle_exception(unsigned long cause, char *str,
				     struct pt_regs *regs, siginfo_t *info)
{
	if (user_mode(regs)) {
		struct task_struct *tsk = current;

		tsk->thread.fault_address = (__force unsigned int)info->si_addr;
		tsk->thread.cause_code = cause;

		force_sig_info(info->si_signo, info, tsk);

	} else {
		/*                                                  */
		if (fixup_exception(regs))
			return 0;

		die(str, regs, (unsigned long)info->si_addr, cause);
	}

	return 1;
}

#define DO_ERROR_INFO(signr, str, name, sicode) \
int name(unsigned long cause, unsigned long address, struct pt_regs *regs) \
{						\
	siginfo_t info = {			\
		.si_signo = signr,		\
		.si_errno = 0,			\
		.si_code  = sicode,		\
		.si_addr = (void __user *)address,	\
	};					\
	return handle_exception(cause, str, regs, &info);\
}

/*
                                                            
 */
DO_ERROR_INFO(SIGILL, "Priv Op/Disabled Extn", do_privilege_fault, ILL_PRVOPC)
DO_ERROR_INFO(SIGILL, "Invalid Extn Insn", do_extension_fault, ILL_ILLOPC)
DO_ERROR_INFO(SIGILL, "Illegal Insn (or Seq)", insterror_is_error, ILL_ILLOPC)
DO_ERROR_INFO(SIGBUS, "Invalid Mem Access", do_memory_error, BUS_ADRERR)
DO_ERROR_INFO(SIGTRAP, "Breakpoint Set", trap_is_brkpt, TRAP_BRKPT)
DO_ERROR_INFO(SIGBUS, "Misaligned Access", do_misaligned_error, BUS_ADRALN)

#ifdef CONFIG_ARC_MISALIGN_ACCESS
/*
                                                                              
 */
int do_misaligned_access(unsigned long cause, unsigned long address,
			 struct pt_regs *regs, struct callee_regs *cregs)
{
	if (misaligned_fixup(address, regs, cause, cregs) != 0)
		return do_misaligned_error(cause, address, regs);

	return 0;
}
#endif

/*
                                                          
                                                     
 */
void do_machine_check_fault(unsigned long cause, unsigned long address,
			    struct pt_regs *regs)
{
	die("Machine Check Exception", regs, address, cause);
}


/*
                                                             
                                                               
                                                                        
                                            
                                                   
                                            
                      
 */
void do_non_swi_trap(unsigned long cause, unsigned long address,
			struct pt_regs *regs)
{
	unsigned int param = cause & 0xff;

	switch (param) {
	case 1:
		trap_is_brkpt(cause, address, regs);
		break;

	case 2:
		trap_is_kprobe(param, address, regs);
		break;

	case 3:
	case 4:
		kgdb_trap(regs, param);
		break;

	default:
		break;
	}
}

/*
                                              
                                                                   
                                    
 */
void do_insterror_or_kprobe(unsigned long cause,
				       unsigned long address,
				       struct pt_regs *regs)
{
	/*                                              */
	if (notify_die(DIE_IERR, "kprobe_ierr", regs, address,
		       cause, SIGILL) == NOTIFY_STOP)
		return;

	insterror_is_error(cause, address, regs);
}
