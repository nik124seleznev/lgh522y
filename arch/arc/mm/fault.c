/* Page Fault Handling for ARC (TLB Miss / ProtV)
 *
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/signal.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/ptrace.h>
#include <linux/uaccess.h>
#include <linux/kdebug.h>
#include <asm/pgalloc.h>

static int handle_vmalloc_fault(unsigned long address)
{
	/*
                                                
                                    
  */
	pgd_t *pgd, *pgd_k;
	pud_t *pud, *pud_k;
	pmd_t *pmd, *pmd_k;

	pgd = pgd_offset_fast(current->active_mm, address);
	pgd_k = pgd_offset_k(address);

	if (!pgd_present(*pgd_k))
		goto bad_area;

	pud = pud_offset(pgd, address);
	pud_k = pud_offset(pgd_k, address);
	if (!pud_present(*pud_k))
		goto bad_area;

	pmd = pmd_offset(pud, address);
	pmd_k = pmd_offset(pud_k, address);
	if (!pmd_present(*pmd_k))
		goto bad_area;

	set_pmd(pmd, *pmd_k);

	/*                                */
	return 0;

bad_area:
	return 1;
}

void do_page_fault(struct pt_regs *regs, int write, unsigned long address,
		   unsigned long cause_code)
{
	struct vm_area_struct *vma = NULL;
	struct task_struct *tsk = current;
	struct mm_struct *mm = tsk->mm;
	siginfo_t info;
	int fault, ret;
	unsigned int flags = FAULT_FLAG_ALLOW_RETRY | FAULT_FLAG_KILLABLE |
				(write ? FAULT_FLAG_WRITE : 0);

	/*
                                                          
                                          
   
                                                          
                                                       
                                                         
                 
  */
	if (address >= VMALLOC_START && address <= VMALLOC_END) {
		ret = handle_vmalloc_fault(address);
		if (unlikely(ret))
			goto bad_area_nosemaphore;
		else
			return;
	}

	info.si_code = SEGV_MAPERR;

	/*
                                            
                                         
  */
	if (in_atomic() || !mm)
		goto no_context;

retry:
	down_read(&mm->mmap_sem);
	vma = find_vma(mm, address);
	if (!vma)
		goto bad_area;
	if (vma->vm_start <= address)
		goto good_area;
	if (!(vma->vm_flags & VM_GROWSDOWN))
		goto bad_area;
	if (expand_stack(vma, address))
		goto bad_area;

	/*
                                                         
                      
  */
good_area:
	info.si_code = SEGV_ACCERR;

	/*                                                       */

	if (cause_code == ((ECR_V_PROTV << 16) | ECR_C_PROTV_INST_FETCH))
		goto bad_area;

	if (write) {
		if (!(vma->vm_flags & VM_WRITE))
			goto bad_area;
	} else {
		if (!(vma->vm_flags & (VM_READ | VM_EXEC)))
			goto bad_area;
	}

survive:
	/*
                                                          
                                                           
              
  */
	fault = handle_mm_fault(mm, vma, address, flags);

	/*                                                                  */
	if (unlikely(fatal_signal_pending(current))) {
		if ((fault & VM_FAULT_ERROR) && !(fault & VM_FAULT_RETRY))
			up_read(&mm->mmap_sem);
		if (user_mode(regs))
			return;
	}

	if (likely(!(fault & VM_FAULT_ERROR))) {
		if (flags & FAULT_FLAG_ALLOW_RETRY) {
			/*                                              */
			if (fault & VM_FAULT_MAJOR)
				tsk->maj_flt++;
			else
				tsk->min_flt++;

			if (fault & VM_FAULT_RETRY) {
				flags &= ~FAULT_FLAG_ALLOW_RETRY;
				flags |= FAULT_FLAG_TRIED;
				goto retry;
			}
		}

		/*                          */
		up_read(&mm->mmap_sem);
		return;
	}

	/*                                          */
	if (fault & VM_FAULT_OOM)
		goto out_of_memory;
	else if (fault & VM_FAULT_SIGBUS)
		goto do_sigbus;

	/*               */
	BUG();

	/*
                                                                   
                                                    
  */
bad_area:
	up_read(&mm->mmap_sem);

bad_area_nosemaphore:
	/*                                         */
	if (user_mode(regs)) {
		tsk->thread.fault_address = address;
		tsk->thread.cause_code = cause_code;
		info.si_signo = SIGSEGV;
		info.si_errno = 0;
		/*                                 */
		info.si_addr = (void __user *)address;
		force_sig_info(SIGSEGV, &info, tsk);
		return;
	}

no_context:
	/*                                             
   
                                                        
                                                      
                                                         
                                                       
          
  */
	if (fixup_exception(regs))
		return;

	die("Oops", regs, address, cause_code);

out_of_memory:
	if (is_global_init(tsk)) {
		yield();
		goto survive;
	}
	up_read(&mm->mmap_sem);

	if (user_mode(regs))
		do_group_exit(SIGKILL);	/*                        */

	goto no_context;

do_sigbus:
	up_read(&mm->mmap_sem);

	if (!user_mode(regs))
		goto no_context;

	tsk->thread.fault_address = address;
	tsk->thread.cause_code = cause_code;
	info.si_signo = SIGBUS;
	info.si_errno = 0;
	info.si_code = BUS_ADRERR;
	info.si_addr = (void __user *)address;
	force_sig_info(SIGBUS, &info, tsk);
}