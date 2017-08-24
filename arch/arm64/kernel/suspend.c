#include <linux/percpu.h>
#include <linux/slab.h>
#include <asm/cacheflush.h>
#include <asm/cpu_ops.h>
#include <asm/debug-monitors.h>
#include <asm/pgtable.h>
#include <asm/memory.h>
#include <asm/smp_plat.h>
#include <asm/suspend.h>
#include <asm/tlbflush.h>

extern phys_addr_t sleep_aee_rec_cpu_dormant_va;

#define pclog()                                                         \
        do {                                                            \
                __asm__ __volatile__  (                                 \
                        "mrs	x7, mpidr_el1 \n\t"                     \
                        "ubfx   x8, x7, #0, #8 \n\t"                    \
                        "ubfx   x9, x7, #8, #8 \n\t"                    \
                        "add    x8, x8, x9, lsl #2 \n\t"                \
                        "ldr    x9, =sleep_aee_rec_cpu_dormant_va \n\t" \
                        "ldr    x9, [x9] \n\t"                          \
                        "cbz    x9, 1f \n\t"                            \
                        "adr    x10, 1f \n\t"                           \
                        "str    x10, [ x9, x8, lsl #3 ] \n\t"           \
                        "1: \n\t"                                       \
                        ::: "x7", "x8", "x9", "x10");                   \
        } while(0)

extern int __cpu_suspend(unsigned long);
/*
                                                                       
                                                                         
                                                                 
  
                                               
                                    
                                                                        
                           
 */
int __cpu_suspend_finisher(unsigned long arg, struct cpu_suspend_ctx *ptr,
			   phys_addr_t *save_ptr)
{
	int cpu = smp_processor_id();

	*save_ptr = virt_to_phys(ptr);

	cpu_do_suspend(ptr);
	/*
                                                              
                                                         
                                              
  */
	__flush_dcache_area(ptr, sizeof(*ptr));
	__flush_dcache_area(save_ptr, sizeof(*save_ptr));

	return cpu_ops[cpu]->cpu_suspend(arg);
}

/*
                                                                
                                                                         
                                                                           
                                                                           
                                                                   
 */
void (*hw_breakpoint_restore)(void *);
void __init cpu_suspend_set_dbg_restorer(void (*hw_bp_restore)(void *))
{
	/*                                               */
	if (WARN_ON(hw_breakpoint_restore))
		return;
	hw_breakpoint_restore = hw_bp_restore;
}

/* 
              
  
                                                  
 */
int cpu_suspend(unsigned long arg)
{
	struct mm_struct *mm = current->active_mm;
	int ret, cpu = smp_processor_id();
	unsigned long flags;

	/*
                                                  
                                                           
  */
	if (!cpu_ops[cpu] || !cpu_ops[cpu]->cpu_suspend)
		return -EOPNOTSUPP;

	/*
                                                            
                                                            
                                                     
  */
	local_dbg_save(flags);

	/*
                                                           
                                                          
                                                             
                              
  */
	ret = __cpu_suspend(arg);
        pclog();

	if (ret == 0) {
		cpu_switch_mm(mm->pgd, mm);
		flush_tlb_all();

		/*
                                             
                                                 
   */
		set_my_cpu_offset(per_cpu_offset(cpu));

		/*
                                                   
                                                   
                               
   */
		if (hw_breakpoint_restore)
			hw_breakpoint_restore(NULL);
	}

	/*
                                                             
                                                            
                                                          
  */
	local_dbg_restore(flags);

	return ret;
}

extern struct sleep_save_sp sleep_save_sp;
extern phys_addr_t sleep_idmap_phys;

static int cpu_suspend_init(void)
{
	void *ctx_ptr;

	/*                                           */
	ctx_ptr = kcalloc(mpidr_hash_size(), sizeof(phys_addr_t), GFP_KERNEL);

	if (WARN_ON(!ctx_ptr))
		return -ENOMEM;

	sleep_save_sp.save_ptr_stash = ctx_ptr;
	sleep_save_sp.save_ptr_stash_phys = virt_to_phys(ctx_ptr);
	sleep_idmap_phys = virt_to_phys(idmap_pg_dir);
	__flush_dcache_area(&sleep_save_sp, sizeof(struct sleep_save_sp));
	__flush_dcache_area(&sleep_idmap_phys, sizeof(sleep_idmap_phys));

	return 0;
}
early_initcall(cpu_suspend_init);
