/*
 * syscalls.h - Linux syscall interfaces (arch-specific)
 *
 * Copyright (c) 2008 Jaswinder Singh Rajput
 *
 * This file is released under the GPLv2.
 * See the file COPYING for more details.
 */

#ifndef _ASM_X86_SYSCALLS_H
#define _ASM_X86_SYSCALLS_H

#include <linux/compiler.h>
#include <linux/linkage.h>
#include <linux/signal.h>
#include <linux/types.h>

/*                             */
/*                 */
asmlinkage long sys_ioperm(unsigned long, unsigned long, int);
asmlinkage long sys_iopl(unsigned int);

/*              */
asmlinkage int sys_modify_ldt(int, void __user *, unsigned long);

/*                 */
long sys_rt_sigreturn(void);

/*              */
asmlinkage long sys_set_thread_area(struct user_desc __user *);
asmlinkage long sys_get_thread_area(struct user_desc __user *);

/*             */
#ifdef CONFIG_X86_32

/*                 */
unsigned long sys_sigreturn(void);

/*                  */
asmlinkage long sys_vm86old(struct vm86_struct __user *);
asmlinkage long sys_vm86(unsigned long, unsigned long);

#else /*               */

/*             */
/*                     */
long sys_arch_prctl(int, unsigned long);

/*                     */
asmlinkage long sys_mmap(unsigned long, unsigned long, unsigned long,
			 unsigned long, unsigned long, unsigned long);

#endif /*               */
#endif /*                     */
