/*
 * Copyright 2010 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 *
 * Copied from i386: Ross Biro 1/23/92
 */

#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/kprobes.h>
#include <linux/compat.h>
#include <linux/uaccess.h>
#include <linux/regset.h>
#include <linux/elf.h>
#include <linux/tracehook.h>
#include <asm/traps.h>
#include <arch/chip.h>

#define CREATE_TRACE_POINTS
#include <trace/events/syscalls.h>

void user_enable_single_step(struct task_struct *child)
{
	set_tsk_thread_flag(child, TIF_SINGLESTEP);
}

void user_disable_single_step(struct task_struct *child)
{
	clear_tsk_thread_flag(child, TIF_SINGLESTEP);
}

/*
                                             
 */
void ptrace_disable(struct task_struct *child)
{
	clear_tsk_thread_flag(child, TIF_SINGLESTEP);

	/*
                                                                    
                                                             
  */
	clear_tsk_thread_flag(child, TIF_SYSCALL_TRACE);
}

/*
                                                              
                                                                     
                                                                 
                                                                 
                                                  
 */
static char *getregs(struct task_struct *child, struct pt_regs *uregs)
{
	*uregs = *task_pt_regs(child);

	/*                        */
	uregs->flags = 0;
#ifdef CONFIG_COMPAT
	if (task_thread_info(child)->status & TS_COMPAT)
		uregs->flags |= PT_FLAGS_COMPAT;
#endif

	return (char *)uregs;
}

/*                             */
static void putregs(struct task_struct *child, struct pt_regs *uregs)
{
	struct pt_regs *regs = task_pt_regs(child);

	/*                                                         */
	uregs->flags = regs->flags;

	/*                                                 */
	uregs->ex1 = PL_ICS_EX1(USER_PL, EX1_ICS(uregs->ex1));

	*regs = *uregs;
}

enum tile_regset {
	REGSET_GPR,
};

static int tile_gpr_get(struct task_struct *target,
			  const struct user_regset *regset,
			  unsigned int pos, unsigned int count,
			  void *kbuf, void __user *ubuf)
{
	struct pt_regs regs;

	getregs(target, &regs);

	return user_regset_copyout(&pos, &count, &kbuf, &ubuf, &regs, 0,
				   sizeof(regs));
}

static int tile_gpr_set(struct task_struct *target,
			  const struct user_regset *regset,
			  unsigned int pos, unsigned int count,
			  const void *kbuf, const void __user *ubuf)
{
	int ret;
	struct pt_regs regs;

	ret = user_regset_copyin(&pos, &count, &kbuf, &ubuf, &regs, 0,
				 sizeof(regs));
	if (ret)
		return ret;

	putregs(target, &regs);

	return 0;
}

static const struct user_regset tile_user_regset[] = {
	[REGSET_GPR] = {
		.core_note_type = NT_PRSTATUS,
		.n = ELF_NGREG,
		.size = sizeof(elf_greg_t),
		.align = sizeof(elf_greg_t),
		.get = tile_gpr_get,
		.set = tile_gpr_set,
	},
};

static const struct user_regset_view tile_user_regset_view = {
	.name = CHIP_ARCH_NAME,
	.e_machine = ELF_ARCH,
	.ei_osabi = ELF_OSABI,
	.regsets = tile_user_regset,
	.n = ARRAY_SIZE(tile_user_regset),
};

const struct user_regset_view *task_user_regset_view(struct task_struct *task)
{
	return &tile_user_regset_view;
}

long arch_ptrace(struct task_struct *child, long request,
		 unsigned long addr, unsigned long data)
{
	unsigned long __user *datap = (long __user __force *)data;
	unsigned long tmp;
	long ret = -EIO;
	char *childreg;
	struct pt_regs copyregs;

	switch (request) {

	case PTRACE_PEEKUSR:  /*                             */
		if (addr >= PTREGS_SIZE)
			break;
		childreg = getregs(child, &copyregs) + addr;
#ifdef CONFIG_COMPAT
		if (is_compat_task()) {
			if (addr & (sizeof(compat_long_t)-1))
				break;
			ret = put_user(*(compat_long_t *)childreg,
				       (compat_long_t __user *)datap);
		} else
#endif
		{
			if (addr & (sizeof(long)-1))
				break;
			ret = put_user(*(long *)childreg, datap);
		}
		break;

	case PTRACE_POKEUSR:  /*                            */
		if (addr >= PTREGS_SIZE)
			break;
		childreg = getregs(child, &copyregs) + addr;
#ifdef CONFIG_COMPAT
		if (is_compat_task()) {
			if (addr & (sizeof(compat_long_t)-1))
				break;
			*(compat_long_t *)childreg = data;
		} else
#endif
		{
			if (addr & (sizeof(long)-1))
				break;
			*(long *)childreg = data;
		}
		putregs(child, &copyregs);
		ret = 0;
		break;

	case PTRACE_GETREGS:  /*                                   */
		ret = copy_regset_to_user(child, &tile_user_regset_view,
					  REGSET_GPR, 0,
					  sizeof(struct pt_regs), datap);
		break;

	case PTRACE_SETREGS:  /*                                 */
		ret = copy_regset_from_user(child, &tile_user_regset_view,
					    REGSET_GPR, 0,
					    sizeof(struct pt_regs), datap);
		break;

	case PTRACE_GETFPREGS:  /*                          */
	case PTRACE_SETFPREGS:  /*                          */
		break;

	case PTRACE_SETOPTIONS:
		/*                                       */
		BUILD_BUG_ON(PTRACE_O_MASK_TILE & PTRACE_O_MASK);
		tmp = data & PTRACE_O_MASK_TILE;
		data &= ~PTRACE_O_MASK_TILE;
		ret = ptrace_request(child, request, addr, data);
		if (ret == 0) {
			unsigned int flags = child->ptrace;
			flags &= ~(PTRACE_O_MASK_TILE << PT_OPT_FLAG_SHIFT);
			flags |= (tmp << PT_OPT_FLAG_SHIFT);
			child->ptrace = flags;
		}
		break;

	default:
#ifdef CONFIG_COMPAT
		if (task_thread_info(current)->status & TS_COMPAT) {
			ret = compat_ptrace_request(child, request,
						    addr, data);
			break;
		}
#endif
		ret = ptrace_request(child, request, addr, data);
		break;
	}

	return ret;
}

#ifdef CONFIG_COMPAT
/*                                                              */
long compat_arch_ptrace(struct task_struct *child, compat_long_t request,
			       compat_ulong_t addr, compat_ulong_t data)
{
	BUG();
}
#endif

int do_syscall_trace_enter(struct pt_regs *regs)
{
	if (test_thread_flag(TIF_SYSCALL_TRACE)) {
		if (tracehook_report_syscall_entry(regs))
			regs->regs[TREG_SYSCALL_NR] = -1;
	}

	if (test_thread_flag(TIF_SYSCALL_TRACEPOINT))
		trace_sys_enter(regs, regs->regs[TREG_SYSCALL_NR]);

	return regs->regs[TREG_SYSCALL_NR];
}

void do_syscall_trace_exit(struct pt_regs *regs)
{
	if (test_thread_flag(TIF_SYSCALL_TRACE))
		tracehook_report_syscall_exit(regs, 0);

	if (test_thread_flag(TIF_SYSCALL_TRACEPOINT))
		trace_sys_exit(regs, regs->regs[0]);
}

void send_sigtrap(struct task_struct *tsk, struct pt_regs *regs, int error_code)
{
	struct siginfo info;

	memset(&info, 0, sizeof(info));
	info.si_signo = SIGTRAP;
	info.si_code  = TRAP_BRKPT;
	info.si_addr  = (void __user *) regs->pc;

	/*                           */
	force_sig_info(SIGTRAP, &info, tsk);
}

/*                                                             */
void __kprobes do_breakpoint(struct pt_regs* regs, int fault_num)
{
	send_sigtrap(current, regs, fault_num);
}
