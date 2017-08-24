/*
 * The SH64 TLB miss.
 *
 * Original code from fault.c
 * Copyright (C) 2000, 2001  Paolo Alberelli
 *
 * Fast PTE->TLB refill path
 * Copyright (C) 2003 Richard.Curnow@superh.com
 *
 * IMPORTANT NOTES :
 * The do_fast_page_fault function is called from a context in entry.S
 * where very few registers have been saved.  In particular, the code in
 * this file must be compiled not to use ANY caller-save registers that
 * are not part of the restricted save set.  Also, it means that code in
 * this file must not make calls to functions elsewhere in the kernel, or
 * else the excepting context will see corruption in its caller-save
 * registers.  Plus, the entry.S save area is non-reentrant, so this code
 * has to run with SR.BL==1, i.e. no interrupts taken inside it and panic
 * on any exception.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <linux/kprobes.h>
#include <asm/tlb.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/pgalloc.h>
#include <asm/mmu_context.h>

static int handle_tlbmiss(unsigned long long protection_flags,
			  unsigned long address)
{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	pte_t entry;

	if (is_vmalloc_addr((void *)address)) {
		pgd = pgd_offset_k(address);
	} else {
		if (unlikely(address >= TASK_SIZE || !current->mm))
			return 1;

		pgd = pgd_offset(current->mm, address);
	}

	pud = pud_offset(pgd, address);
	if (pud_none(*pud) || !pud_present(*pud))
		return 1;

	pmd = pmd_offset(pud, address);
	if (pmd_none(*pmd) || !pmd_present(*pmd))
		return 1;

	pte = pte_offset_kernel(pmd, address);
	entry = *pte;
	if (pte_none(entry) || !pte_present(entry))
		return 1;

	/*
                                                              
                                                             
                                                              
            
  */
	if ((pte_val(entry) & protection_flags) != protection_flags)
		return 1;

	update_mmu_cache(NULL, address, pte);

	return 0;
}

/*
                                                                         
                                                                         
                                                                    
 */
struct expevt_lookup {
	unsigned short protection_flags[8];
	unsigned char  is_text_access[8];
	unsigned char  is_write_access[8];
};

#define PRU (1<<9)
#define PRW (1<<8)
#define PRX (1<<7)
#define PRR (1<<6)

/*                                                                             
                                                       */
static struct expevt_lookup expevt_lookup_table = {
	.protection_flags = {PRX, PRX, 0, 0, PRR, PRR, PRW, PRW},
	.is_text_access   = {1,   1,   0, 0, 0,   0,   0,   0}
};

static inline unsigned int
expevt_to_fault_code(unsigned long expevt)
{
	if (expevt == 0xa40)
		return FAULT_CODE_ITLB;
	else if (expevt == 0x060)
		return FAULT_CODE_WRITE;

	return 0;
}

/*
                                                                            
                                                                             
                                                                         
                                                                              
                                                                         
                                                              
 */
asmlinkage int __kprobes
do_fast_page_fault(unsigned long long ssr_md, unsigned long long expevt,
		   unsigned long address)
{
	unsigned long long protection_flags;
	unsigned long long index;
	unsigned long long expevt4;
	unsigned int fault_code;

	/*                                                            
                                                            
                                                  
   
         
                              
                              
                              
  */
	expevt4 = (expevt >> 4);
	/*                                                              
                                         */
	index = expevt4 ^ (expevt4 >> 5);
	index &= 7;

	fault_code = expevt_to_fault_code(expevt);

	protection_flags = expevt_lookup_table.protection_flags[index];

	if (expevt_lookup_table.is_text_access[index])
		fault_code |= FAULT_CODE_ITLB;
	if (!ssr_md)
		fault_code |= FAULT_CODE_USER;

	set_thread_fault_code(fault_code);

	return handle_tlbmiss(protection_flags, address);
}
