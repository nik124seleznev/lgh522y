/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */



#include <linux/bitops.h>

#include <mali_kbase.h>
#include <mali_kbase_mem.h>
#include <mali_kbase_mmu_hw.h>
#include <mali_kbase_mmu_hw_direct.h>

#if KBASE_MMU_HW_BACKEND

static inline u64 lock_region(struct kbase_device *kbdev, u64 pfn,
		u32 num_pages)
{
	u64 region;

	/*                               */
	KBASE_DEBUG_ASSERT(num_pages);

	region = pfn << PAGE_SHIFT;
	/*
                                         
           
   
                       
                                   
  */

	/*                                        */
	if (0 == num_pages) {
		region |= 11;
	} else {
		u8 region_width;

		region_width = 10 + fls(num_pages);
		if (num_pages != (1ul << (region_width - 11))) {
			/*                                          */
			region_width += 1;
		}
		KBASE_DEBUG_ASSERT(region_width <= KBASE_LOCK_REGION_MAX_SIZE);
		KBASE_DEBUG_ASSERT(region_width >= KBASE_LOCK_REGION_MIN_SIZE);
		region |= region_width;
	}

	return region;
}

static int wait_ready(struct kbase_device *kbdev,
		unsigned int as_nr, struct kbase_context *kctx)
{
	unsigned int max_loops = KBASE_AS_INACTIVE_MAX_LOOPS;

	/*                                                                 */
	while (--max_loops && kbase_reg_read(kbdev,
			MMU_AS_REG(as_nr, AS_STATUS),
			kctx) & AS_STATUS_AS_ACTIVE) {
		;
	}

	if (max_loops == 0) {
		dev_err(kbdev->dev, "AS_ACTIVE bit stuck\n");
		return -1;
	}

	return 0;
}

static int write_cmd(struct kbase_device *kbdev, int as_nr, u32 cmd,
		struct kbase_context *kctx)
{
	int status;

	/*                                                              */
	status = wait_ready(kbdev, as_nr, kctx);
	if (status == 0)
		kbase_reg_write(kbdev, MMU_AS_REG(as_nr, AS_COMMAND), cmd, kctx);

	return status;
}

void kbase_mmu_interrupt(struct kbase_device *kbdev, u32 irq_stat)
{
	const int num_as = 16;
	const int busfault_shift = MMU_REGS_PAGE_FAULT_FLAGS;
	const int pf_shift = 0;
	const unsigned long as_bit_mask = (1UL << num_as) - 1;
	unsigned long flags;
	u32 new_mask;
	u32 tmp;

	/*            */
	u32 bf_bits = (irq_stat >> busfault_shift) & as_bit_mask;
	/*                                                     */
	u32 pf_bits = ((irq_stat >> pf_shift) & as_bit_mask) & ~bf_bits;

	KBASE_DEBUG_ASSERT(NULL != kbdev);

	/*                       */
	spin_lock_irqsave(&kbdev->mmu_mask_change, flags);
	new_mask = kbase_reg_read(kbdev, MMU_REG(MMU_IRQ_MASK), NULL);
	/*                         */
	kbase_reg_write(kbdev, MMU_REG(MMU_IRQ_MASK), 0, NULL);
	spin_unlock_irqrestore(&kbdev->mmu_mask_change, flags);

	while (bf_bits | pf_bits) {
		struct kbase_as *as;
		int as_no;
		struct kbase_context *kctx;

		/*
                                                                
                       
   */
		as_no = ffs(bf_bits | pf_bits) - 1;
		as = &kbdev->as[as_no];

		/*
                                                                  
                                                                 
                                                                  
                       
   */
		kctx = kbasep_js_runpool_lookup_ctx(kbdev, as_no);

		/*                       */
		as->fault_addr = kbase_reg_read(kbdev,
						MMU_AS_REG(as_no, AS_FAULTADDRESS_HI),
						kctx);
		as->fault_addr <<= 32;
		as->fault_addr |= kbase_reg_read(kbdev,
						MMU_AS_REG(as_no, AS_FAULTADDRESS_LO),
						kctx);

		/*                         */
		as->fault_status = kbase_reg_read(kbdev,
						  MMU_AS_REG(as_no, AS_FAULTSTATUS),
						  kctx);

		/*                     */
		as->fault_type = (bf_bits & (1 << as_no)) ?
				  KBASE_MMU_FAULT_TYPE_BUS : KBASE_MMU_FAULT_TYPE_PAGE;


		if (kbase_as_has_bus_fault(as)) {
			/*
                                                          
                       
     
           
                                                        
                                                           
                                                         
                                    
    */
			kbase_reg_write(kbdev, MMU_REG(MMU_IRQ_CLEAR),
					(1UL << MMU_REGS_BUS_ERROR_FLAG(as_no)) |
					(1UL << MMU_REGS_PAGE_FAULT_FLAG(as_no)), kctx);

			/*                                                    */
			bf_bits &= ~(1UL << (as_no));

			/*                                               */
			new_mask &= ~((1UL << MMU_REGS_BUS_ERROR_FLAG(as_no)) |
				      (1UL << MMU_REGS_PAGE_FAULT_FLAG(as_no)));
		} else {
			/*
                                                          
                       
    */
			kbase_reg_write(kbdev, MMU_REG(MMU_IRQ_CLEAR),
					1UL << MMU_REGS_PAGE_FAULT_FLAG(as_no),
					kctx);

			/*                 */
			pf_bits &= ~(1UL << as_no);

			/*                                     */
			new_mask &= ~(1UL << MMU_REGS_PAGE_FAULT_FLAG(as_no));
		}

		/*                                              */
		kbase_mmu_interrupt_process(kbdev, kctx, as);
	}

	/*                     */
	spin_lock_irqsave(&kbdev->mmu_mask_change, flags);
	tmp = kbase_reg_read(kbdev, MMU_REG(MMU_IRQ_MASK), NULL);
	new_mask |= tmp;
	kbase_reg_write(kbdev, MMU_REG(MMU_IRQ_MASK), new_mask, NULL);
	spin_unlock_irqrestore(&kbdev->mmu_mask_change, flags);
}

void kbase_mmu_hw_configure(struct kbase_device *kbdev, struct kbase_as *as,
		struct kbase_context *kctx)
{
	struct kbase_mmu_setup *current_setup = &as->current_setup;

	kbase_reg_write(kbdev, MMU_AS_REG(as->number, AS_TRANSTAB_LO),
			current_setup->transtab & 0xFFFFFFFFUL, kctx);
	kbase_reg_write(kbdev, MMU_AS_REG(as->number, AS_TRANSTAB_HI),
			(current_setup->transtab >> 32) & 0xFFFFFFFFUL, kctx);

	kbase_reg_write(kbdev, MMU_AS_REG(as->number, AS_MEMATTR_LO),
			current_setup->memattr & 0xFFFFFFFFUL, kctx);
	kbase_reg_write(kbdev, MMU_AS_REG(as->number, AS_MEMATTR_HI),
			(current_setup->memattr >> 32) & 0xFFFFFFFFUL, kctx);
	write_cmd(kbdev, as->number, AS_COMMAND_UPDATE, kctx);
}

int kbase_mmu_hw_do_operation(struct kbase_device *kbdev, struct kbase_as *as,
		struct kbase_context *kctx, u64 vpfn, u32 nr, u32 op,
		unsigned int handling_irq)
{
	int ret;

	if (op == AS_COMMAND_UNLOCK) {
		/*                                     */
		ret = write_cmd(kbdev, as->number, AS_COMMAND_UNLOCK, kctx);
	} else {
		u64 lock_addr = lock_region(kbdev, vpfn, nr);

		/*                                          */
		kbase_reg_write(kbdev, MMU_AS_REG(as->number, AS_LOCKADDR_LO),
				lock_addr & 0xFFFFFFFFUL, kctx);
		kbase_reg_write(kbdev, MMU_AS_REG(as->number, AS_LOCKADDR_HI),
				(lock_addr >> 32) & 0xFFFFFFFFUL, kctx);
		write_cmd(kbdev, as->number, AS_COMMAND_LOCK, kctx);

		if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_T76X_3285) &&
				handling_irq) {
			kbase_reg_write(kbdev, MMU_REG(MMU_IRQ_CLEAR),
					(1UL << as->number), NULL);
			write_cmd(kbdev, as->number, AS_COMMAND_LOCK, kctx);
		}

		/*                       */
		write_cmd(kbdev, as->number, op, kctx);

		/*                                */
		ret = wait_ready(kbdev, as->number, kctx);

		if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_9630)) {
			/*                                                  
                                                    
                                                         
                                                          
                                                        
                                                      
                                                         
                                                         
                                                         
                                                          
                                                      
                                              
                     
    */
			write_cmd(kbdev, as->number, AS_COMMAND_UNLOCK, kctx);
			write_cmd(kbdev, as->number, AS_COMMAND_UNLOCK, kctx);
		}
	}

	return ret;
}

void kbase_mmu_hw_clear_fault(struct kbase_device *kbdev, struct kbase_as *as,
		struct kbase_context *kctx, enum kbase_mmu_fault_type type)
{
	unsigned long flags;
	u32 mask;

	spin_lock_irqsave(&kbdev->mmu_mask_change, flags);
	mask = kbase_reg_read(kbdev, MMU_REG(MMU_IRQ_MASK), kctx);

	mask |= (1UL << MMU_REGS_PAGE_FAULT_FLAG(as->number));
	if (type == KBASE_MMU_FAULT_TYPE_BUS)
		mask |= (1UL << MMU_REGS_BUS_ERROR_FLAG(as->number));

	kbase_reg_write(kbdev, MMU_REG(MMU_IRQ_MASK), mask, kctx);
	spin_unlock_irqrestore(&kbdev->mmu_mask_change, flags);
}
#endif
