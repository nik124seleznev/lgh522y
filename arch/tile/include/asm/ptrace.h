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
 */
#ifndef _ASM_TILE_PTRACE_H
#define _ASM_TILE_PTRACE_H

#include <linux/compiler.h>

#ifndef __ASSEMBLY__
/*                                                     */
typedef unsigned long pt_reg_t;
#endif

#include <uapi/asm/ptrace.h>

#define PTRACE_O_MASK_TILE	(PTRACE_O_TRACEMIGRATE)
#define PT_TRACE_MIGRATE	PT_EVENT_FLAG(PTRACE_EVENT_MIGRATE)

/*                            */
#define PT_FLAGS_DISABLE_IRQ    1  /*                                   */
#define PT_FLAGS_CALLER_SAVES   2  /*                                 */
#define PT_FLAGS_RESTORE_REGS   4  /*                                    */

#ifndef __ASSEMBLY__

#define instruction_pointer(regs) ((regs)->pc)
#define profile_pc(regs) instruction_pointer(regs)
#define user_stack_pointer(regs) ((regs)->sp)

/*                                                       */
#define user_mode(regs) (EX1_PL((regs)->ex1) == USER_PL)

/*                                                             */
struct pt_regs *get_pt_regs(struct pt_regs *);

/*                            */
extern int do_syscall_trace_enter(struct pt_regs *regs);
extern void do_syscall_trace_exit(struct pt_regs *regs);

#define arch_has_single_step()	(1)

/*
                                            
  
                                                         
 */
struct single_step_state {
	/*                                                   */
	void __user *buffer;

	union {
		int flags;
		struct {
			unsigned long is_enabled:1, update:1, update_reg:6;
		};
	};

	unsigned long orig_pc;		/*                 */
	unsigned long next_pc;		/*                                 */
	unsigned long branch_next_pc;	/*                                 */
	unsigned long update_value;	/*                                   */
};

/*                                         */
extern void single_step_once(struct pt_regs *regs);

/*                          */
extern void single_step_execve(void);

struct task_struct;

extern void send_sigtrap(struct task_struct *tsk, struct pt_regs *regs,
			 int error_code);

#ifdef __tilegx__
/*                                                                           */
#define __ARCH_WANT_COMPAT_SYS_PTRACE
#endif

#endif /*               */

#define SINGLESTEP_STATE_MASK_IS_ENABLED      0x1
#define SINGLESTEP_STATE_MASK_UPDATE          0x2
#define SINGLESTEP_STATE_TARGET_LB              2
#define SINGLESTEP_STATE_TARGET_UB              7

#endif /*                    */
