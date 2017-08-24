/*
 *  Meta page fault handling.
 *
 *  Copyright (C) 2005-2012 Imagination Technologies Ltd.
 */

#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>

#include <asm/tlbflush.h>
#include <asm/mmu.h>
#include <asm/traps.h>

/*                                       */
static void clear_cbuf_entry(struct pt_regs *regs, unsigned long addr,
			     unsigned int trapno)
{
	PTBICTXEXTCB0 cbuf = regs->extcb0;

	switch (trapno) {
		/*                                                       */
	case TBIXXF_SIGNUM_IGF:
	case TBIXXF_SIGNUM_IPF:
		return;
	default:
		if (cbuf[0].CBAddr == addr) {
			cbuf[0].CBAddr = 0;
			cbuf[0].CBFlags &= ~TXCATCH0_FAULT_BITS;

			/*                                         
                                                  
    */
			regs->ctx.SaveMask &= ~(TBICTX_CBUF_BIT |
						TBICTX_XCBF_BIT);

			return;
		}
		pr_err("Failed to clear cbuf entry!\n");
	}
}

int show_unhandled_signals = 1;

int do_page_fault(struct pt_regs *regs, unsigned long address,
		  unsigned int write_access, unsigned int trapno)
{
	struct task_struct *tsk;
	struct mm_struct *mm;
	struct vm_area_struct *vma, *prev_vma;
	siginfo_t info;
	int fault;
	unsigned int flags = FAULT_FLAG_ALLOW_RETRY | FAULT_FLAG_KILLABLE |
				(write_access ? FAULT_FLAG_WRITE : 0);

	tsk = current;

	if ((address >= VMALLOC_START) && (address < VMALLOC_END)) {
		/*
                                                 
                                     
    
                                                
                                                  
   */
		int offset = pgd_index(address);
		pgd_t *pgd, *pgd_k;
		pud_t *pud, *pud_k;
		pmd_t *pmd, *pmd_k;
		pte_t *pte_k;

		pgd = ((pgd_t *)mmu_get_base()) + offset;
		pgd_k = swapper_pg_dir + offset;

		/*                                                    */
		if (!pgd_present(*pgd)) {
			if (!pgd_present(*pgd_k))
				goto bad_area_nosemaphore;
			set_pgd(pgd, *pgd_k);
			return 0;
		}

		pud = pud_offset(pgd, address);
		pud_k = pud_offset(pgd_k, address);
		if (!pud_present(*pud_k))
			goto bad_area_nosemaphore;
		set_pud(pud, *pud_k);

		pmd = pmd_offset(pud, address);
		pmd_k = pmd_offset(pud_k, address);
		if (!pmd_present(*pmd_k))
			goto bad_area_nosemaphore;
		set_pmd(pmd, *pmd_k);

		pte_k = pte_offset_kernel(pmd_k, address);
		if (!pte_present(*pte_k))
			goto bad_area_nosemaphore;

		/*                               */
		flush_tlb_all();
		return 0;
	}

	mm = tsk->mm;

	if (in_atomic() || !mm)
		goto no_context;

retry:
	down_read(&mm->mmap_sem);

	vma = find_vma_prev(mm, address, &prev_vma);

	if (!vma || address < vma->vm_start)
		goto check_expansion;

good_area:
	if (write_access) {
		if (!(vma->vm_flags & VM_WRITE))
			goto bad_area;
	} else {
		if (!(vma->vm_flags & (VM_READ | VM_EXEC | VM_WRITE)))
			goto bad_area;
	}

	/*
                                                          
                                                           
              
  */
	fault = handle_mm_fault(mm, vma, address, flags);

	if ((fault & VM_FAULT_RETRY) && fatal_signal_pending(current))
		return 0;

	if (unlikely(fault & VM_FAULT_ERROR)) {
		if (fault & VM_FAULT_OOM)
			goto out_of_memory;
		else if (fault & VM_FAULT_SIGBUS)
			goto do_sigbus;
		BUG();
	}
	if (flags & FAULT_FLAG_ALLOW_RETRY) {
		if (fault & VM_FAULT_MAJOR)
			tsk->maj_flt++;
		else
			tsk->min_flt++;
		if (fault & VM_FAULT_RETRY) {
			flags &= ~FAULT_FLAG_ALLOW_RETRY;
			flags |= FAULT_FLAG_TRIED;

			/*
                                                   
                                                      
                      
    */

			goto retry;
		}
	}

	up_read(&mm->mmap_sem);
	return 0;

check_expansion:
	vma = prev_vma;
	if (vma && (expand_stack(vma, address) == 0))
		goto good_area;

bad_area:
	up_read(&mm->mmap_sem);

bad_area_nosemaphore:
	if (user_mode(regs)) {
		info.si_signo = SIGSEGV;
		info.si_errno = 0;
		info.si_code = SEGV_MAPERR;
		info.si_addr = (__force void __user *)address;
		info.si_trapno = trapno;

		if (show_unhandled_signals && unhandled_signal(tsk, SIGSEGV) &&
		    printk_ratelimit()) {
			pr_info("%s%s[%d]: segfault at %lx pc %08x sp %08x write %d trap %#x (%s)",
			       task_pid_nr(tsk) > 1 ? KERN_INFO : KERN_EMERG,
			       tsk->comm, task_pid_nr(tsk), address,
			       regs->ctx.CurrPC, regs->ctx.AX[0].U0,
			       write_access, trapno, trap_name(trapno));
			print_vma_addr(" in ", regs->ctx.CurrPC);
			print_vma_addr(" rtp in ", regs->ctx.DX[4].U1);
			printk("\n");
			show_regs(regs);
		}
		force_sig_info(SIGSEGV, &info, tsk);
		return 1;
	}
	goto no_context;

do_sigbus:
	up_read(&mm->mmap_sem);

	/*
                                                          
                 
  */
	info.si_signo = SIGBUS;
	info.si_errno = 0;
	info.si_code = BUS_ADRERR;
	info.si_addr = (__force void __user *)address;
	info.si_trapno = trapno;
	force_sig_info(SIGBUS, &info, tsk);

	/*                                       */
	if (!user_mode(regs))
		goto no_context;

	return 1;

	/*
                                                                      
                                                  
  */
out_of_memory:
	up_read(&mm->mmap_sem);
	if (user_mode(regs))
		do_group_exit(SIGKILL);

no_context:
	/*                                               */
	if (fixup_exception(regs)) {
		clear_cbuf_entry(regs, address, trapno);
		return 1;
	}

	die("Oops", regs, (write_access << 15) | trapno, address);
	do_exit(SIGKILL);
}