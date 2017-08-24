#include <linux/init.h>

#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/cpu.h>

#include <asm/tlbflush.h>
#include <asm/mmu_context.h>
#include <asm/cache.h>
#include <asm/apic.h>
#include <asm/uv/uv.h>
#include <linux/debugfs.h>

DEFINE_PER_CPU_SHARED_ALIGNED(struct tlb_state, cpu_tlbstate)
			= { &init_mm, 0, };

/*
                               
                       
  
                                                            
                                                                   
  
                                                          
  
                                       
  
                                                        
 */

struct flush_tlb_info {
	struct mm_struct *flush_mm;
	unsigned long flush_start;
	unsigned long flush_end;
};

/*
                                                               
                                  
 */
void leave_mm(int cpu)
{
	struct mm_struct *active_mm = this_cpu_read(cpu_tlbstate.active_mm);
	if (this_cpu_read(cpu_tlbstate.state) == TLBSTATE_OK)
		BUG();
	if (cpumask_test_cpu(cpu, mm_cpumask(active_mm))) {
		cpumask_clear_cpu(cpu, mm_cpumask(active_mm));
		load_cr3(swapper_pg_dir);
	}
}
EXPORT_SYMBOL_GPL(leave_mm);

/*
                                                                    
                                
                                   
                                      
                                       
                                                                   
                                
                            
                                               
                                          
                                               
                   
                                            
                                                                  
                                                                     
                                                                
                                      
                                                             
                                       
                                                
                                                                     
                    
                                                             
                              
  
                                             
                                                                          
                                                                      
                                                                        
           
  
                                                              
                                
 */

/*
                       
                                                                        
                                                  
 */
static void flush_tlb_func(void *info)
{
	struct flush_tlb_info *f = info;

	inc_irq_stat(irq_tlb_count);

	if (f->flush_mm != this_cpu_read(cpu_tlbstate.active_mm))
		return;

	if (this_cpu_read(cpu_tlbstate.state) == TLBSTATE_OK) {
		if (f->flush_end == TLB_FLUSH_ALL)
			local_flush_tlb();
		else if (!f->flush_end)
			__flush_tlb_single(f->flush_start);
		else {
			unsigned long addr;
			addr = f->flush_start;
			while (addr < f->flush_end) {
				__flush_tlb_single(addr);
				addr += PAGE_SIZE;
			}
		}
	} else
		leave_mm(smp_processor_id());

}

void native_flush_tlb_others(const struct cpumask *cpumask,
				 struct mm_struct *mm, unsigned long start,
				 unsigned long end)
{
	struct flush_tlb_info info;
	info.flush_mm = mm;
	info.flush_start = start;
	info.flush_end = end;

	if (is_uv_system()) {
		unsigned int cpu;

		cpu = smp_processor_id();
		cpumask = uv_flush_tlb_others(cpumask, mm, start, end, cpu);
		if (cpumask)
			smp_call_function_many(cpumask, flush_tlb_func,
								&info, 1);
		return;
	}
	smp_call_function_many(cpumask, flush_tlb_func, &info, 1);
}

void flush_tlb_current_task(void)
{
	struct mm_struct *mm = current->mm;

	preempt_disable();

	local_flush_tlb();
	if (cpumask_any_but(mm_cpumask(mm), smp_processor_id()) < nr_cpu_ids)
		flush_tlb_others(mm_cpumask(mm), mm, 0UL, TLB_FLUSH_ALL);
	preempt_enable();
}

/*
                                         
                                              
 */
static inline unsigned long has_large_page(struct mm_struct *mm,
				 unsigned long start, unsigned long end)
{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	unsigned long addr = ALIGN(start, HPAGE_SIZE);
	for (; addr < end; addr += HPAGE_SIZE) {
		pgd = pgd_offset(mm, addr);
		if (likely(!pgd_none(*pgd))) {
			pud = pud_offset(pgd, addr);
			if (likely(!pud_none(*pud))) {
				pmd = pmd_offset(pud, addr);
				if (likely(!pmd_none(*pmd)))
					if (pmd_large(*pmd))
						return addr;
			}
		}
	}
	return 0;
}

void flush_tlb_mm_range(struct mm_struct *mm, unsigned long start,
				unsigned long end, unsigned long vmflag)
{
	unsigned long addr;
	unsigned act_entries, tlb_entries = 0;

	preempt_disable();
	if (current->active_mm != mm)
		goto flush_all;

	if (!current->mm) {
		leave_mm(smp_processor_id());
		goto flush_all;
	}

	if (end == TLB_FLUSH_ALL || tlb_flushall_shift == -1
					|| vmflag & VM_HUGETLB) {
		local_flush_tlb();
		goto flush_all;
	}

	/*                                                      */
	if (vmflag & VM_EXEC)
		tlb_entries = tlb_lli_4k[ENTRIES];
	else
		tlb_entries = tlb_lld_4k[ENTRIES];
	/*                                                     */
	act_entries = mm->total_vm > tlb_entries ? tlb_entries : mm->total_vm;

	/*                                                               */
	if ((end - start) >> PAGE_SHIFT > act_entries >> tlb_flushall_shift)
		local_flush_tlb();
	else {
		if (has_large_page(mm, start, end)) {
			local_flush_tlb();
			goto flush_all;
		}
		/*                                    */
		for (addr = start; addr < end;	addr += PAGE_SIZE)
			__flush_tlb_single(addr);

		if (cpumask_any_but(mm_cpumask(mm),
				smp_processor_id()) < nr_cpu_ids)
			flush_tlb_others(mm_cpumask(mm), mm, start, end);
		preempt_enable();
		return;
	}

flush_all:
	if (cpumask_any_but(mm_cpumask(mm), smp_processor_id()) < nr_cpu_ids)
		flush_tlb_others(mm_cpumask(mm), mm, 0UL, TLB_FLUSH_ALL);
	preempt_enable();
}

void flush_tlb_page(struct vm_area_struct *vma, unsigned long start)
{
	struct mm_struct *mm = vma->vm_mm;

	preempt_disable();

	if (current->active_mm == mm) {
		if (current->mm)
			__flush_tlb_one(start);
		else
			leave_mm(smp_processor_id());
	}

	if (cpumask_any_but(mm_cpumask(mm), smp_processor_id()) < nr_cpu_ids)
		flush_tlb_others(mm_cpumask(mm), mm, start, 0UL);

	preempt_enable();
}

static void do_flush_tlb_all(void *info)
{
	__flush_tlb_all();
	if (this_cpu_read(cpu_tlbstate.state) == TLBSTATE_LAZY)
		leave_mm(smp_processor_id());
}

void flush_tlb_all(void)
{
	on_each_cpu(do_flush_tlb_all, NULL, 1);
}

static void do_kernel_range_flush(void *info)
{
	struct flush_tlb_info *f = info;
	unsigned long addr;

	/*                                    */
	for (addr = f->flush_start; addr < f->flush_end; addr += PAGE_SIZE)
		__flush_tlb_single(addr);
}

void flush_tlb_kernel_range(unsigned long start, unsigned long end)
{
	unsigned act_entries;
	struct flush_tlb_info info;

	/*                                                      */
	act_entries = tlb_lld_4k[ENTRIES];

	/*                                                        */
	if (end == TLB_FLUSH_ALL || tlb_flushall_shift == -1 ||
		(end - start) >> PAGE_SHIFT > act_entries >> tlb_flushall_shift)

		on_each_cpu(do_flush_tlb_all, NULL, 1);
	else {
		info.flush_start = start;
		info.flush_end = end;
		on_each_cpu(do_kernel_range_flush, &info, 1);
	}
}

#ifdef CONFIG_DEBUG_TLBFLUSH
static ssize_t tlbflush_read_file(struct file *file, char __user *user_buf,
			     size_t count, loff_t *ppos)
{
	char buf[32];
	unsigned int len;

	len = sprintf(buf, "%hd\n", tlb_flushall_shift);
	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t tlbflush_write_file(struct file *file,
		 const char __user *user_buf, size_t count, loff_t *ppos)
{
	char buf[32];
	ssize_t len;
	s8 shift;

	len = min(count, sizeof(buf) - 1);
	if (copy_from_user(buf, user_buf, len))
		return -EFAULT;

	buf[len] = '\0';
	if (kstrtos8(buf, 0, &shift))
		return -EINVAL;

	if (shift < -1 || shift >= BITS_PER_LONG)
		return -EINVAL;

	tlb_flushall_shift = shift;
	return count;
}

static const struct file_operations fops_tlbflush = {
	.read = tlbflush_read_file,
	.write = tlbflush_write_file,
	.llseek = default_llseek,
};

static int __init create_tlb_flushall_shift(void)
{
	debugfs_create_file("tlb_flushall_shift", S_IRUSR | S_IWUSR,
			    arch_debugfs_dir, NULL, &fops_tlbflush);
	return 0;
}
late_initcall(create_tlb_flushall_shift);
#endif
