#include <linux/mm.h>
#include <linux/suspend.h>
#include <asm/cacheflush.h>
#include <asm/system_misc.h>
#include <asm/idmap.h>
#include <asm/suspend.h>

extern void cpu_resume(void);
typedef void (*phys_reset_t)(unsigned long);
static int swsusp_saved = 0;


/*
                                               
                                                         
 */
static int notrace __swsusp_arch_save_image(unsigned long unused)
{
	extern int swsusp_save(void);
	int ret;

	ret = swsusp_save();
	swsusp_saved = (ret == 0) ? 1 : 0;
	return ret;
}

/*
                                                        
 */
int notrace swsusp_arch_suspend(void)
{
	int retval = 0;

	retval = cpu_suspend(0, __swsusp_arch_save_image);
	if (swsusp_saved)
		retval = 0;

	return retval;
}

/*
                                                                        
                                                                         
                
  
                                                               
                                                                
 */
static void notrace __swsusp_arch_restore_image(void *unused)
{
	extern void __disable_dcache__inner_flush_dcache_L1__inner_flush_dcache_L2(void);
	extern struct pbe *restore_pblist;
	phys_reset_t phys_reset;
	struct pbe *pbe;

	for (pbe = restore_pblist; pbe; pbe = pbe->next)
		copy_page(pbe->orig_address, pbe->address);

#if 0 //                                                                                     
	/*                             */
	flush_cache_all();

	/*                  */
	cpu_proc_fin();

	/*                                                            */
	flush_cache_all();
#else
	__disable_dcache__inner_flush_dcache_L1__inner_flush_dcache_L2();
#endif

	/*                                 */
	setup_mm_for_reboot();
	phys_reset = (phys_reset_t)(unsigned long)virt_to_phys(cpu_reset);
	/*                                             */
	phys_reset((unsigned long)virt_to_phys(cpu_resume));

	/*                        */
	BUG();
}

static u8 __swsusp_resume_stk[PAGE_SIZE/2] __nosavedata;

/*
                                     
                                                                          
                                                                       
                                                        
 */
int __naked swsusp_arch_resume(void)
{
	extern void call_with_stack(void (*fn)(void *), void *arg, void *sp);
	cpu_init();	/*                 */
	call_with_stack(__swsusp_arch_restore_image, 0,
		__swsusp_resume_stk + sizeof(__swsusp_resume_stk));
	return 0;
}
