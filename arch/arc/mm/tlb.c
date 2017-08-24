/*
 * TLB Management (flush/create/diagnostics) for ARC700
 *
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * vineetg: Aug 2011
 *  -Reintroduce duplicate PD fixup - some customer chips still have the issue
 *
 * vineetg: May 2011
 *  -No need to flush_cache_page( ) for each call to update_mmu_cache()
 *   some of the LMBench tests improved amazingly
 *      = page-fault thrice as fast (75 usec to 28 usec)
 *      = mmap twice as fast (9.6 msec to 4.6 msec),
 *      = fork (5.3 msec to 3.7 msec)
 *
 * vineetg: April 2011 :
 *  -MMU v3: PD{0,1} bits layout changed: They don't overlap anymore,
 *      helps avoid a shift when preparing PD0 from PTE
 *
 * vineetg: April 2011 : Preparing for MMU V3
 *  -MMU v2/v3 BCRs decoded differently
 *  -Remove TLB_SIZE hardcoding as it's variable now: 256 or 512
 *  -tlb_entry_erase( ) can be void
 *  -local_flush_tlb_range( ):
 *      = need not "ceil" @end
 *      = walks MMU only if range spans < 32 entries, as opposed to 256
 *
 * Vineetg: Sept 10th 2008
 *  -Changes related to MMU v2 (Rel 4.8)
 *
 * Vineetg: Aug 29th 2008
 *  -In TLB Flush operations (Metal Fix MMU) there is a explict command to
 *    flush Micro-TLBS. If TLB Index Reg is invalid prior to TLBIVUTLB cmd,
 *    it fails. Thus need to load it with ANY valid value before invoking
 *    TLBIVUTLB cmd
 *
 * Vineetg: Aug 21th 2008:
 *  -Reduced the duration of IRQ lockouts in TLB Flush routines
 *  -Multiple copies of TLB erase code seperated into a "single" function
 *  -In TLB Flush routines, interrupt disabling moved UP to retrieve ASID
 *       in interrupt-safe region.
 *
 * Vineetg: April 23rd Bug #93131
 *    Problem: tlb_flush_kernel_range() doesnt do anything if the range to
 *              flush is more than the size of TLB itself.
 *
 * Rahul Trivedi : Codito Technologies 2004
 */

#include <linux/module.h>
#include <asm/arcregs.h>
#include <asm/setup.h>
#include <asm/mmu_context.h>
#include <asm/tlb.h>

/*                      
  
                                                                          
                                                                             
                                                                             
             
  
                                                                              
                                                                   
                                                                          
                                                                            
                                                                                
  
                                                                              
                                                                      
                                                                          
                                                                          
  
                                         
                                                                  
                                                 
  
                                                                           
                                                                              
                                                                            
                                                                               
                                                                                
       
  
                                                                      
                                      
  
                                                                               
                                                                          
                                                                          
                                                                           
                                                                         
                                    
 */

/*                                                           */
int asid_cache = FIRST_ASID;

/*                                                                    
                                                                       
                                                  
 */
struct mm_struct *asid_mm_map[NUM_ASID + 1];

/*
                                         
                                                                   
                                             
 */

static void __tlb_entry_erase(void)
{
	write_aux_reg(ARC_REG_TLBPD1, 0);
	write_aux_reg(ARC_REG_TLBPD0, 0);
	write_aux_reg(ARC_REG_TLBCOMMAND, TLBWrite);
}

static void tlb_entry_erase(unsigned int vaddr_n_asid)
{
	unsigned int idx;

	/*                                            */
	write_aux_reg(ARC_REG_TLBPD0, vaddr_n_asid);
	write_aux_reg(ARC_REG_TLBCOMMAND, TLBProbe);
	idx = read_aux_reg(ARC_REG_TLBINDEX);

	/*                                         */
	if (likely(!(idx & TLB_LKUP_ERR))) {
		__tlb_entry_erase();
	} else {		/*                    */

		/*                       */
		if (idx & 0x1) {
			/*                                      */
			pr_emerg("unhandled Duplicate flush for %x\n",
			       vaddr_n_asid);
		}
		/*                                       */
	}
}

/*                                                                           
                                                                       
  
                                                          
  
                      
                                    
                                                          
                                                              
                                                                           */

static void utlb_invalidate(void)
{
#if (CONFIG_ARC_MMU_VER >= 2)

#if (CONFIG_ARC_MMU_VER < 3)
	/*                                          
                                                                     
                                                                    
                                                           
                                                                      
                                                             
  */
	unsigned int idx;

	/*                              */
	idx = read_aux_reg(ARC_REG_TLBINDEX);

	/*                             */
	if (unlikely(idx & TLB_LKUP_ERR))
		write_aux_reg(ARC_REG_TLBINDEX, 0xa);
#endif

	write_aux_reg(ARC_REG_TLBCOMMAND, TLBIVUTLB);
#endif

}

/*
                                                                  
 */

noinline void local_flush_tlb_all(void)
{
	unsigned long flags;
	unsigned int entry;
	struct cpuinfo_arc_mmu *mmu = &cpuinfo_arc700[smp_processor_id()].mmu;

	local_irq_save(flags);

	/*                                                  */
	write_aux_reg(ARC_REG_TLBPD1, 0);
	write_aux_reg(ARC_REG_TLBPD0, 0);

	for (entry = 0; entry < mmu->num_tlb; entry++) {
		/*                             */
		write_aux_reg(ARC_REG_TLBINDEX, entry);
		write_aux_reg(ARC_REG_TLBCOMMAND, TLBWrite);
	}

	utlb_invalidate();

	local_irq_restore(flags);
}

/*
                                                                            
 */
noinline void local_flush_tlb_mm(struct mm_struct *mm)
{
	/*
                                    
                                                                        
                                                                 
                                               
  */
	if (atomic_read(&mm->mm_users) == 0)
		return;

	/*
                                    
                                                                    
                                                          
  */
	if (current->mm != mm)
		destroy_context(mm);
	else
		get_new_mmu_context(mm);
}

/*
                                             
                                               
                                                    
                                                                         
                                            
                                                                 
 */
void local_flush_tlb_range(struct vm_area_struct *vma, unsigned long start,
			   unsigned long end)
{
	unsigned long flags;
	unsigned int asid;

	/*                                                          
                                                              
                                                  
   
                                                                 
                                          
  */
	if (unlikely((end - start) >= PAGE_SIZE * 32)) {
		local_flush_tlb_mm(vma->vm_mm);
		return;
	}

	/*
                                                                
                                                               
                                            
  */
	start &= PAGE_MASK;

	local_irq_save(flags);
	asid = vma->vm_mm->context.asid;

	if (asid != NO_ASID) {
		while (start < end) {
			tlb_entry_erase(start | (asid & 0xff));
			start += PAGE_SIZE;
		}
	}

	utlb_invalidate();

	local_irq_restore(flags);
}

/*                                                                             
                                      
                                                                   
                                                                            
                                                                        
 */
void local_flush_tlb_kernel_range(unsigned long start, unsigned long end)
{
	unsigned long flags;

	/*                                                             */

	if (unlikely((end - start) >= PAGE_SIZE * 32)) {
		local_flush_tlb_all();
		return;
	}

	start &= PAGE_MASK;

	local_irq_save(flags);
	while (start < end) {
		tlb_entry_erase(start);
		start += PAGE_SIZE;
	}

	utlb_invalidate();

	local_irq_restore(flags);
}

/*
                                                         
                                                          
 */

void local_flush_tlb_page(struct vm_area_struct *vma, unsigned long page)
{
	unsigned long flags;

	/*                                                              
                                                      
  */
	local_irq_save(flags);

	if (vma->vm_mm->context.asid != NO_ASID) {
		tlb_entry_erase((page & PAGE_MASK) |
				(vma->vm_mm->context.asid & 0xff));
		utlb_invalidate();
	}

	local_irq_restore(flags);
}

/*
                                
 */
void create_tlb(struct vm_area_struct *vma, unsigned long address, pte_t *ptep)
{
	unsigned long flags;
	unsigned int idx, asid_or_sasid;
	unsigned long pd0_flags;

	/*
                                                           
                                                                        
                                                                     
   
                                    
                                                               
                                                                    
                                                                       
                                                         
   
                                                                       
                                                                 
                                                                     
                                                                        
                                                                     
                                                        
                                                     
   
                                                                     
                                                                  
                             
   
                                                            
  */
	if (current->active_mm != vma->vm_mm)
		return;

	local_irq_save(flags);

	tlb_paranoid_check(vma->vm_mm->context.asid, address);

	address &= PAGE_MASK;

	/*                             */
	pte_val(*ptep) |= (_PAGE_PRESENT | _PAGE_ACCESSED);

	/*                                                   */
#if (CONFIG_ARC_MMU_VER <= 2)
	pd0_flags = ((pte_val(*ptep) & PTE_BITS_IN_PD0) >> 1);
#else
	pd0_flags = ((pte_val(*ptep) & PTE_BITS_IN_PD0));
#endif

	/*                    */
	asid_or_sasid = read_aux_reg(ARC_REG_PID) & 0xff;

	write_aux_reg(ARC_REG_TLBPD0, address | pd0_flags | asid_or_sasid);

	/*                                                                 */
	write_aux_reg(ARC_REG_TLBPD1, (pte_val(*ptep) & PTE_BITS_IN_PD1));

	/*                                                          */
	write_aux_reg(ARC_REG_TLBCOMMAND, TLBProbe);
	idx = read_aux_reg(ARC_REG_TLBINDEX);

	/*
                                                    
                                                                   
                                                                   
                                       
  */
	if (likely(idx & TLB_LKUP_ERR))
		write_aux_reg(ARC_REG_TLBCOMMAND, TLBGetIndex);

	/*
                           
                                                       
                                                             
  */
	write_aux_reg(ARC_REG_TLBCOMMAND, TLBWrite);

	local_irq_restore(flags);
}

/*
                                                              
                                                     
                                                                        
                                          
  
                                                                        
                                                                     
 */
void update_mmu_cache(struct vm_area_struct *vma, unsigned long vaddr_unaligned,
		      pte_t *ptep)
{
	unsigned long vaddr = vaddr_unaligned & PAGE_MASK;
	unsigned long paddr = pte_val(*ptep) & PAGE_MASK;

	create_tlb(vma, vaddr, ptep);

	/*
                                                                  
                                                              
                                                                 
                                                         
                                                                    
                                            
                                                             
  */
	if ((vma->vm_flags & VM_EXEC) ||
	     addr_not_cache_congruent(paddr, vaddr)) {
		struct page *page = pfn_to_page(pte_pfn(*ptep));

		int dirty = test_and_clear_bit(PG_arch_1, &page->flags);
		if (dirty) {
			/*                          */
			__flush_dcache_page(paddr, paddr);

			/*                                      */
			if (vma->vm_flags & VM_EXEC)
				__inv_icache_page(paddr, vaddr);
		}
	}
}

/*                                                                      
                                       
                                                           
 */
void __cpuinit read_decode_mmu_bcr(void)
{
	unsigned int tmp;
	struct bcr_mmu_1_2 *mmu2;	/*                   */
	struct bcr_mmu_3 *mmu3;		/*                   */
	struct cpuinfo_arc_mmu *mmu = &cpuinfo_arc700[smp_processor_id()].mmu;

	tmp = read_aux_reg(ARC_REG_MMU_BCR);
	mmu->ver = (tmp >> 24);

	if (mmu->ver <= 2) {
		mmu2 = (struct bcr_mmu_1_2 *)&tmp;
		mmu->pg_sz = PAGE_SIZE;
		mmu->sets = 1 << mmu2->sets;
		mmu->ways = 1 << mmu2->ways;
		mmu->u_dtlb = mmu2->u_dtlb;
		mmu->u_itlb = mmu2->u_itlb;
	} else {
		mmu3 = (struct bcr_mmu_3 *)&tmp;
		mmu->pg_sz = 512 << mmu3->pg_sz;
		mmu->sets = 1 << mmu3->sets;
		mmu->ways = 1 << mmu3->ways;
		mmu->u_dtlb = mmu3->u_dtlb;
		mmu->u_itlb = mmu3->u_itlb;
	}

	mmu->num_tlb = mmu->sets * mmu->ways;
}

char *arc_mmu_mumbojumbo(int cpu_id, char *buf, int len)
{
	int n = 0;
	struct cpuinfo_arc_mmu *p_mmu = &cpuinfo_arc700[cpu_id].mmu;

	n += scnprintf(buf + n, len - n, "ARC700 MMU [v%x]\t: %dk PAGE, ",
		       p_mmu->ver, TO_KB(p_mmu->pg_sz));

	n += scnprintf(buf + n, len - n,
		       "J-TLB %d (%dx%d), uDTLB %d, uITLB %d, %s\n",
		       p_mmu->num_tlb, p_mmu->sets, p_mmu->ways,
		       p_mmu->u_dtlb, p_mmu->u_itlb,
		       __CONFIG_ARC_MMU_SASID_VAL ? "SASID" : "");

	return buf;
}

void __cpuinit arc_mmu_init(void)
{
	char str[256];
	struct cpuinfo_arc_mmu *mmu = &cpuinfo_arc700[smp_processor_id()].mmu;

	printk(arc_mmu_mumbojumbo(0, str, sizeof(str)));

	/*                                                                
                                                  
                                                                       
                                                                
                                                                  
                                                        
                                                                   
  */
	if (mmu->ver != CONFIG_ARC_MMU_VER) {
		panic("MMU ver %d doesn't match kernel built for %d...\n",
		      mmu->ver, CONFIG_ARC_MMU_VER);
	}

	if (mmu->pg_sz != PAGE_SIZE)
		panic("MMU pg size != PAGE_SIZE (%luk)\n", TO_KB(PAGE_SIZE));

	/*
                                                   
                                                         
  */

	local_flush_tlb_all();

	/*                */
	write_aux_reg(ARC_REG_PID, MMU_ENABLE);

	/*                                                */
#ifndef CONFIG_SMP
	/*                                                           */
	write_aux_reg(ARC_REG_SCRATCH_DATA0, swapper_pg_dir);
#endif
}

/*
                                                                              
                               
                                     
                                     
                                     
                                           
                                           
                  
                                             
                                     
                                                                      
                                              
                                                          
 */
#define SET_WAY_TO_IDX(mmu, set, way)  ((set) * mmu->ways + (way))

/*                                             
                                                                  
                                                                           
                                              
                                                                               
                          
                                                             
 */
volatile int dup_pd_verbose = 1;/*                                        */

void do_tlb_overlap_fault(unsigned long cause, unsigned long address,
			  struct pt_regs *regs)
{
	int set, way, n;
	unsigned int pd0[4], pd1[4];	/*                   */
	unsigned long flags, is_valid;
	struct cpuinfo_arc_mmu *mmu = &cpuinfo_arc700[smp_processor_id()].mmu;

	local_irq_save(flags);

	/*                   */
	write_aux_reg(ARC_REG_PID, MMU_ENABLE | read_aux_reg(ARC_REG_PID));

	/*                           */
	for (set = 0; set < mmu->sets; set++) {

		/*                                      */
		for (way = 0, is_valid = 0; way < mmu->ways; way++) {
			write_aux_reg(ARC_REG_TLBINDEX,
					  SET_WAY_TO_IDX(mmu, set, way));
			write_aux_reg(ARC_REG_TLBCOMMAND, TLBRead);
			pd0[way] = read_aux_reg(ARC_REG_TLBPD0);
			pd1[way] = read_aux_reg(ARC_REG_TLBPD1);
			is_valid |= pd0[way] & _PAGE_PRESENT;
		}

		/*                                                    */
		if (!is_valid)
			continue;

		/*                                                      */
		for (way = 0; way < mmu->ways; way++) {
			if (!pd0[way])
				continue;

			for (n = way + 1; n < mmu->ways; n++) {
				if ((pd0[way] & PAGE_MASK) ==
				    (pd0[n] & PAGE_MASK)) {

					if (dup_pd_verbose) {
						pr_info("Duplicate PD's @"
							"[%d:%d]/[%d:%d]\n",
						     set, way, set, n);
						pr_info("TLBPD0[%u]: %08x\n",
						     way, pd0[way]);
					}

					/*
                                            
                                      
      */
					pd0[way] = pd1[way] = 0;
					write_aux_reg(ARC_REG_TLBINDEX,
						SET_WAY_TO_IDX(mmu, set, way));
					__tlb_entry_erase();
				}
			}
		}
	}

	local_irq_restore(flags);
}

/*                                                                      
                      
                                                                 
                                                                      */

#ifdef CONFIG_ARC_DBG_TLB_PARANOIA

/*
                                                                        
              
 */
void print_asid_mismatch(int is_fast_path)
{
	int pid_sw, pid_hw;
	pid_sw = current->active_mm->context.asid;
	pid_hw = read_aux_reg(ARC_REG_PID) & 0xff;

	pr_emerg("ASID Mismatch in %s Path Handler: sw-pid=0x%x hw-pid=0x%x\n",
	       is_fast_path ? "Fast" : "Slow", pid_sw, pid_hw);

	__asm__ __volatile__("flag 1");
}

void tlb_paranoid_check(unsigned int pid_sw, unsigned long addr)
{
	unsigned int pid_hw;

	pid_hw = read_aux_reg(ARC_REG_PID) & 0xff;

	if (addr < 0x70000000 && ((pid_hw != pid_sw) || (pid_sw == NO_ASID)))
		print_asid_mismatch(0);
}
#endif
