/*
 * ptrace for 32-bit processes running on a 64-bit kernel.
 *
 *  PowerPC version
 *    Copyright (C) 1995-1996 Gary Thomas (gdt@linuxppc.org)
 *
 *  Derived from "arch/m68k/kernel/ptrace.c"
 *  Copyright (C) 1994 by Hamish Macdonald
 *  Taken from linux/kernel/ptrace.c and modified for M680x0.
 *  linux/kernel/ptrace.c is by Ross Biro 1/23/92, edited by Linus Torvalds
 *
 * Modified by Cort Dougan (cort@hq.fsmlabs.com)
 * and Paul Mackerras (paulus@samba.org).
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file COPYING in the main directory of
 * this archive for more details.
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/errno.h>
#include <linux/ptrace.h>
#include <linux/regset.h>
#include <linux/user.h>
#include <linux/security.h>
#include <linux/signal.h>
#include <linux/compat.h>

#include <asm/uaccess.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/switch_to.h>

/*
                                                       
                            
 */

/*                                                                      */
#define FPRNUMBER(i) (((i) - PT_FPR0) >> 1)
#define FPRHALF(i) (((i) - PT_FPR0) & 1)
#define FPRINDEX(i) TS_FPRWIDTH * FPRNUMBER(i) * 2 + FPRHALF(i)
#define FPRINDEX_3264(i) (TS_FPRWIDTH * ((i) - PT_FPR0))

long compat_arch_ptrace(struct task_struct *child, compat_long_t request,
			compat_ulong_t caddr, compat_ulong_t cdata)
{
	unsigned long addr = caddr;
	unsigned long data = cdata;
	int ret;

	switch (request) {
	/*
                                              
                                                          
                       
                                                                    
                                                                  
                                                                 
                                                                 
  */
	case PPC_PTRACE_PEEKTEXT_3264:
	case PPC_PTRACE_PEEKDATA_3264: {
		u32 tmp;
		int copied;
		u32 __user * addrOthers;

		ret = -EIO;

		/*                                                        */
		if (get_user(addrOthers, (u32 __user * __user *)addr) != 0)
			break;

		copied = access_process_vm(child, (u64)addrOthers, &tmp,
				sizeof(tmp), 0);
		if (copied != sizeof(tmp))
			break;
		ret = put_user(tmp, (u32 __user *)data);
		break;
	}

	/*                                                            */
	case PTRACE_PEEKUSR: {
		int index;
		unsigned long tmp;

		ret = -EIO;
		/*                            */
		index = (unsigned long) addr >> 2;
		if ((addr & 3) || (index > PT_FPSCR32))
			break;

		CHECK_FULL_REGS(child->thread.regs);
		if (index < PT_FPR0) {
			ret = ptrace_get_reg(child, index, &tmp);
			if (ret)
				break;
		} else {
			flush_fp_to_thread(child);
			/*
                                                      
                                                    
                                                  
    */
			tmp = ((unsigned int *)child->thread.fpr)
				[FPRINDEX(index)];
		}
		ret = put_user((unsigned int)tmp, (u32 __user *)data);
		break;
	}
  
	/*
                                                       
                                                          
                       
                                                                 
                      
                                                                 
  */
	case PPC_PTRACE_PEEKUSR_3264: {
		u32 index;
		u32 reg32bits;
		u64 tmp;
		u32 numReg;
		u32 part;

		ret = -EIO;
		/*                                         */
		index = (u64)addr >> 2;
		numReg = index / 2;
		/*                                                     */
		if (index % 2)
			part = 1;  /*                                                 */
		else
			part = 0;  /*                                                */

		/*                                                                      
                                       
   */
		if ((addr & 3) || numReg > PT_FPSCR)
			break;

		CHECK_FULL_REGS(child->thread.regs);
		if (numReg >= PT_FPR0) {
			flush_fp_to_thread(child);
			/*                */
			tmp = ((u64 *)child->thread.fpr)
				[FPRINDEX_3264(numReg)];
		} else { /*                                */
			unsigned long tmp2;
			ret = ptrace_get_reg(child, numReg, &tmp2);
			if (ret)
				break;
			tmp = tmp2;
		} 
		reg32bits = ((u32*)&tmp)[part];
		ret = put_user(reg32bits, (u32 __user *)data);
		break;
	}

	/*
                                                 
                                                    
                                                             
                                                         
                         
                                                                 
                                                                 
  */
	case PPC_PTRACE_POKETEXT_3264:
	case PPC_PTRACE_POKEDATA_3264: {
		u32 tmp = data;
		u32 __user * addrOthers;

		/*                                                              */
		ret = -EIO;
		if (get_user(addrOthers, (u32 __user * __user *)addr) != 0)
			break;
		ret = 0;
		if (access_process_vm(child, (u64)addrOthers, &tmp,
					sizeof(tmp), 1) == sizeof(tmp))
			break;
		ret = -EIO;
		break;
	}

	/*                                                  */
	case PTRACE_POKEUSR: {
		unsigned long index;

		ret = -EIO;
		/*                            */
		index = (unsigned long) addr >> 2;
		if ((addr & 3) || (index > PT_FPSCR32))
			break;

		CHECK_FULL_REGS(child->thread.regs);
		if (index < PT_FPR0) {
			ret = ptrace_put_reg(child, index, data);
		} else {
			flush_fp_to_thread(child);
			/*
                                                      
                                                    
                                                  
    */
			((unsigned int *)child->thread.fpr)
				[FPRINDEX(index)] = data;
			ret = 0;
		}
		break;
	}

	/*
                                                      
                                                    
                                                                 
                              
                                                                 
  */
	case PPC_PTRACE_POKEUSR_3264: {
		u32 index;
		u32 numReg;

		ret = -EIO;
		/*                                         */
		index = (u64)addr >> 2;
		numReg = index / 2;

		/*
                                                           
                                                      
   */
		if ((addr & 3) || (numReg > PT_FPSCR))
			break;
		CHECK_FULL_REGS(child->thread.regs);
		if (numReg < PT_FPR0) {
			unsigned long freg;
			ret = ptrace_get_reg(child, numReg, &freg);
			if (ret)
				break;
			if (index % 2)
				freg = (freg & ~0xfffffffful) | (data & 0xfffffffful);
			else
				freg = (freg & 0xfffffffful) | (data << 32);
			ret = ptrace_put_reg(child, numReg, freg);
		} else {
			u64 *tmp;
			flush_fp_to_thread(child);
			/*                    */
			tmp = &(((u64 *)child->thread.fpr)
				[FPRINDEX_3264(numReg)]);
			/*                                   */
			((u32 *)tmp)[index % 2] = data;
			ret = 0;
		}
		break;
	}

	case PTRACE_GET_DEBUGREG: {
#ifndef CONFIG_PPC_ADV_DEBUG_REGS
		unsigned long dabr_fake;
#endif
		ret = -EINVAL;
		/*                                                     */
		if (addr > 0)
			break;
#ifdef CONFIG_PPC_ADV_DEBUG_REGS
		ret = put_user(child->thread.dac1, (u32 __user *)data);
#else
		dabr_fake = (
			(child->thread.hw_brk.address & (~HW_BRK_TYPE_DABR)) |
			(child->thread.hw_brk.type & HW_BRK_TYPE_DABR));
		ret = put_user(dabr_fake, (u32 __user *)data);
#endif
		break;
	}

	case PTRACE_GETREGS:	/*                                 */
		return copy_regset_to_user(
			child, task_user_regset_view(current), 0,
			0, PT_REGS_COUNT * sizeof(compat_long_t),
			compat_ptr(data));

	case PTRACE_SETREGS:	/*                               */
		return copy_regset_from_user(
			child, task_user_regset_view(current), 0,
			0, PT_REGS_COUNT * sizeof(compat_long_t),
			compat_ptr(data));

	case PTRACE_GETFPREGS:
	case PTRACE_SETFPREGS:
	case PTRACE_GETVRREGS:
	case PTRACE_SETVRREGS:
	case PTRACE_GETVSRREGS:
	case PTRACE_SETVSRREGS:
	case PTRACE_GETREGS64:
	case PTRACE_SETREGS64:
	case PTRACE_KILL:
	case PTRACE_SINGLESTEP:
	case PTRACE_DETACH:
	case PTRACE_SET_DEBUGREG:
	case PTRACE_SYSCALL:
	case PTRACE_CONT:
	case PPC_PTRACE_GETHWDBGINFO:
	case PPC_PTRACE_SETHWDEBUG:
	case PPC_PTRACE_DELHWDEBUG:
		ret = arch_ptrace(child, request, addr, data);
		break;

	default:
		ret = compat_ptrace_request(child, request, addr, data);
		break;
	}

	return ret;
}
