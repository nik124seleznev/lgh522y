/*
 * include/asm-xtensa/ptrace.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2001 - 2005 Tensilica Inc.
 */
#ifndef _XTENSA_PTRACE_H
#define _XTENSA_PTRACE_H

#include <uapi/asm/ptrace.h>


#ifndef __ASSEMBLY__

#include <asm/coprocessor.h>

/*
                                                              
                                                           
 */
struct pt_regs {
	unsigned long pc;		/*     */
	unsigned long ps;		/*     */
	unsigned long depc;		/*     */
	unsigned long exccause;		/*     */
	unsigned long excvaddr;		/*     */
	unsigned long debugcause;	/*     */
	unsigned long wmask;		/*     */
	unsigned long lbeg;		/*     */
	unsigned long lend;		/*     */
	unsigned long lcount;		/*     */
	unsigned long sar;		/*     */
	unsigned long windowbase;	/*     */
	unsigned long windowstart;	/*     */
	unsigned long syscall;		/*     */
	unsigned long icountlevel;	/*     */
	unsigned long scompare1;	/*     */
	unsigned long threadptr;	/*     */

	/*                                                                  */
	xtregs_opt_t xtregs_opt;

	/*                                               */
	int align[0] __attribute__ ((aligned(16)));

	/*                        
                                                                
  */
	unsigned long areg[16];
};

#include <variant/core.h>

# define arch_has_single_step()	(1)
# define task_pt_regs(tsk) ((struct pt_regs*) \
	(task_stack_page(tsk) + KERNEL_STACK_SIZE - (XCHAL_NUM_AREGS-16)*4) - 1)
# define user_mode(regs) (((regs)->ps & 0x00000020)!=0)
# define instruction_pointer(regs) ((regs)->pc)

# ifndef CONFIG_SMP
#  define profile_pc(regs) instruction_pointer(regs)
# endif

#define user_stack_pointer(regs) ((regs)->areg[1])

#else	/*              */

# include <asm/asm-offsets.h>
#define PT_REGS_OFFSET	  (KERNEL_STACK_SIZE - PT_USER_SIZE)

#endif	/*               */

#endif	/*                  */
