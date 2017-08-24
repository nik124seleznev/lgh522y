#include <linux/init.h>

#include <asm/idmap.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>
#include <asm/memory.h>
#include <asm/suspend.h>
#include <asm/tlbflush.h>

extern int __cpu_suspend(unsigned long, int (*)(unsigned long));
extern void cpu_resume_mmu(void);

#define CA15L_TYPEID 0x410FC0D0
#define CA7_TYPEID 0x410FC070
#define CPU_TYPEID_MASK 0xfffffff0

//                                 
#define read_midr()							\
	({								\
		register unsigned int ret;				\
		__asm__ __volatile__ ("mrc   p15, 0, %0, c0, c0, 0 \n\t" \
				      :"=r"(ret));			\
		ret;							\
	})

#define is_cpu_type(type)						\
	({								\
		((read_midr() & CPU_TYPEID_MASK) == type) ? 1 : 0;	\
	})

/*
                                                                       
                                                                         
                                                                 
 */
void __cpu_suspend_save(u32 *ptr, u32 ptrsz, u32 sp, u32 *save_ptr)
{
	u32 *ctx = ptr;

	*save_ptr = virt_to_phys(ptr);

	/*                                                          */
	*ptr++ = virt_to_phys(idmap_pgd);
	*ptr++ = sp;
	*ptr++ = virt_to_phys(cpu_do_resume);

	cpu_do_suspend(ptr);

        /*                                   */
	if (is_cpu_type(CA15L_TYPEID)) {
                __asm__ __volatile__ (
                        "mov  r0, #0 \n\t"
                        "MCR p15, 1, r0, c15, c14, 0    @; DCCIALL L1 \n\t"
                        "dsb \n\t"
                        "isb \n\t"
                        :::"r0");
        }
        else {
                flush_cache_louis();
        }

	/*
                                             
                                                
                                                        
                                                
                                             
                                              
                                                 
 */
	__cpuc_flush_dcache_area(ctx, ptrsz);
	__cpuc_flush_dcache_area(save_ptr, sizeof(*save_ptr));

	outer_clean_range(*save_ptr, *save_ptr + ptrsz);
	outer_clean_range(virt_to_phys(save_ptr),
			  virt_to_phys(save_ptr) + sizeof(*save_ptr));
}

/*
                                                                              
                                                           
 */
int cpu_suspend(unsigned long arg, int (*fn)(unsigned long))
{
	struct mm_struct *mm = current->active_mm;
	int ret;

	if (!idmap_pgd)
		return -EINVAL;

	/*
                                                               
                                                              
                                                               
                                    
  */
	ret = __cpu_suspend(arg, fn);
	if (ret == 0) {
		cpu_switch_mm(mm->pgd, mm);
		local_flush_bp_all();
		local_flush_tlb_all();
	}

	return ret;
}
