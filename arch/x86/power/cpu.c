/*
 * Suspend support specific for i386/x86-64.
 *
 * Distribute under GPLv2
 *
 * Copyright (c) 2007 Rafael J. Wysocki <rjw@sisk.pl>
 * Copyright (c) 2002 Pavel Machek <pavel@ucw.cz>
 * Copyright (c) 2001 Patrick Mochel <mochel@osdl.org>
 */

#include <linux/suspend.h>
#include <linux/export.h>
#include <linux/smp.h>
#include <linux/perf_event.h>

#include <asm/pgtable.h>
#include <asm/proto.h>
#include <asm/mtrr.h>
#include <asm/page.h>
#include <asm/mce.h>
#include <asm/xcr.h>
#include <asm/suspend.h>
#include <asm/debugreg.h>
#include <asm/fpu-internal.h> /*             */
#include <asm/cpu.h>

#ifdef CONFIG_X86_32
unsigned long saved_context_ebx;
unsigned long saved_context_esp, saved_context_ebp;
unsigned long saved_context_esi, saved_context_edi;
unsigned long saved_context_eflags;
#endif
struct saved_context saved_context;

/* 
                                                                
                                                                   
                                                       
  
                                                                    
                                                                      
                                                                         
                                                                           
                                                                     
                                                                      
                                                                       
                                                                        
                                                    
 */
static void __save_processor_state(struct saved_context *ctxt)
{
#ifdef CONFIG_X86_32
	mtrr_save_fixed_ranges(NULL);
#endif
	kernel_fpu_begin();

	/*
                     
  */
#ifdef CONFIG_X86_32
	store_idt(&ctxt->idt);
#else
/*               */
	store_idt((struct desc_ptr *)&ctxt->idt_limit);
#endif
	/*
                                                               
                                                                     
                                                                 
                                
  */
	ctxt->gdt_desc.size = GDT_SIZE - 1;
	ctxt->gdt_desc.address = (unsigned long)get_cpu_gdt_table(smp_processor_id());

	store_tr(ctxt->tr);

	/*                                                      */
	/*
                     
  */
#ifdef CONFIG_X86_32
	savesegment(es, ctxt->es);
	savesegment(fs, ctxt->fs);
	savesegment(gs, ctxt->gs);
	savesegment(ss, ctxt->ss);
#else
/*               */
	asm volatile ("movw %%ds, %0" : "=m" (ctxt->ds));
	asm volatile ("movw %%es, %0" : "=m" (ctxt->es));
	asm volatile ("movw %%fs, %0" : "=m" (ctxt->fs));
	asm volatile ("movw %%gs, %0" : "=m" (ctxt->gs));
	asm volatile ("movw %%ss, %0" : "=m" (ctxt->ss));

	rdmsrl(MSR_FS_BASE, ctxt->fs_base);
	rdmsrl(MSR_GS_BASE, ctxt->gs_base);
	rdmsrl(MSR_KERNEL_GS_BASE, ctxt->gs_kernel_base);
	mtrr_save_fixed_ranges(NULL);

	rdmsrl(MSR_EFER, ctxt->efer);
#endif

	/*
                     
  */
	ctxt->cr0 = read_cr0();
	ctxt->cr2 = read_cr2();
	ctxt->cr3 = read_cr3();
#ifdef CONFIG_X86_32
	ctxt->cr4 = read_cr4_safe();
#else
/*               */
	ctxt->cr4 = read_cr4();
	ctxt->cr8 = read_cr8();
#endif
	ctxt->misc_enable_saved = !rdmsrl_safe(MSR_IA32_MISC_ENABLE,
					       &ctxt->misc_enable);
}

/*                 */
void save_processor_state(void)
{
	__save_processor_state(&saved_context);
	x86_platform.save_sched_clock_state();
}
EXPORT_SYMBOL(save_processor_state);

static void do_fpu_end(void)
{
	/*
                                  
  */
	kernel_fpu_end();
}

static void fix_processor_context(void)
{
	int cpu = smp_processor_id();
	struct tss_struct *t = &per_cpu(init_tss, cpu);
#ifdef CONFIG_X86_64
	struct desc_struct *desc = get_cpu_gdt_table(cpu);
	tss_desc tss;
#endif
	set_tss_desc(cpu, t);	/*
                                               
                                                   
                                                   
                         
     */

#ifdef CONFIG_X86_64
	memcpy(&tss, &desc[GDT_ENTRY_TSS], sizeof(tss_desc));
	tss.type = 0x9; /*                                                */
	write_gdt_entry(desc, GDT_ENTRY_TSS, &tss, DESC_TSS);

	syscall_init();				/*                                 */
#endif
	load_TR_desc();				/*               */
	load_LDT(&current->active_mm->context);	/*                */
}

/* 
                                                                          
                               
                                                        
 */
static void __restore_processor_state(struct saved_context *ctxt)
{
	if (ctxt->misc_enable_saved)
		wrmsrl(MSR_IA32_MISC_ENABLE, ctxt->misc_enable);
	/*
                     
  */
	/*                                       */
#ifdef CONFIG_X86_32
	if (ctxt->cr4)
		write_cr4(ctxt->cr4);
#else
/*               */
	wrmsrl(MSR_EFER, ctxt->efer);
	write_cr8(ctxt->cr8);
	write_cr4(ctxt->cr4);
#endif
	write_cr3(ctxt->cr3);
	write_cr2(ctxt->cr2);
	write_cr0(ctxt->cr0);

	/*
                                                            
                                          
  */
#ifdef CONFIG_X86_32
	load_idt(&ctxt->idt);
#else
/*               */
	load_idt((const struct desc_ptr *)&ctxt->idt_limit);
#endif

	/*
                     
  */
#ifdef CONFIG_X86_32
	loadsegment(es, ctxt->es);
	loadsegment(fs, ctxt->fs);
	loadsegment(gs, ctxt->gs);
	loadsegment(ss, ctxt->ss);

	/*
                 
  */
	if (boot_cpu_has(X86_FEATURE_SEP))
		enable_sep_cpu();
#else
/*               */
	asm volatile ("movw %0, %%ds" :: "r" (ctxt->ds));
	asm volatile ("movw %0, %%es" :: "r" (ctxt->es));
	asm volatile ("movw %0, %%fs" :: "r" (ctxt->fs));
	load_gs_index(ctxt->gs);
	asm volatile ("movw %0, %%ss" :: "r" (ctxt->ss));

	wrmsrl(MSR_FS_BASE, ctxt->fs_base);
	wrmsrl(MSR_GS_BASE, ctxt->gs_base);
	wrmsrl(MSR_KERNEL_GS_BASE, ctxt->gs_kernel_base);
#endif

	/*
                                         
  */
	if (cpu_has_xsave)
		xsetbv(XCR_XFEATURE_ENABLED_MASK, pcntxt_mask);

	fix_processor_context();

	do_fpu_end();
	x86_platform.restore_sched_clock_state();
	mtrr_bp_restore();
	perf_restore_debug_store();
}

/*                 */
void restore_processor_state(void)
{
	__restore_processor_state(&saved_context);
}
#ifdef CONFIG_X86_32
EXPORT_SYMBOL(restore_processor_state);
#endif

/*
                                                                   
                                                                          
                                 
 */
static int bsp_check(void)
{
	if (cpumask_first(cpu_online_mask) != 0) {
		pr_warn("CPU0 is offline.\n");
		return -ENODEV;
	}

	return 0;
}

static int bsp_pm_callback(struct notifier_block *nb, unsigned long action,
			   void *ptr)
{
	int ret = 0;

	switch (action) {
	case PM_SUSPEND_PREPARE:
	case PM_HIBERNATION_PREPARE:
		ret = bsp_check();
		break;
#ifdef CONFIG_DEBUG_HOTPLUG_CPU0
	case PM_RESTORE_PREPARE:
		/*
                                                              
                                    
                                             
   */
		if (!cpu_online(0))
			_debug_hotplug_cpu(0, 1);
		break;
	case PM_POST_RESTORE:
		/*
                                                             
    
                                                                  
                                                              
                                                                  
                                   
    
                                                              
                                                         
                                           
    
                                                             
                                                                
                                                        
                                                               
                                                              
                                      
    
                                                              
                                                                 
                                                   
   */
		_debug_hotplug_cpu(0, 0);
		break;
#endif
	default:
		break;
	}
	return notifier_from_errno(ret);
}

static int __init bsp_pm_check_init(void)
{
	/*
                                                   
                                                                      
                                                           
  */
	pm_notifier(bsp_pm_callback, -INT_MAX);
	return 0;
}

core_initcall(bsp_pm_check_init);
