/*
 * kernel/power/tuxonice_atomic_copy.c
 *
 * Copyright 2004-2010 Nigel Cunningham (nigel at tuxonice net)
 *
 * Distributed under GPLv2.
 *
 * Routines for doing the atomic save/restore.
 */

#include <linux/suspend.h>
#include <linux/highmem.h>
#include <linux/cpu.h>
#include <linux/freezer.h>
#include <linux/console.h>
#include <linux/syscore_ops.h>
#include <linux/ftrace.h>
#include <asm/suspend.h>
#include "tuxonice.h"
#include "tuxonice_storage.h"
#include "tuxonice_power_off.h"
#include "tuxonice_ui.h"
#include "tuxonice_io.h"
#include "tuxonice_prepare_image.h"
#include "tuxonice_pageflags.h"
#include "tuxonice_checksum.h"
#include "tuxonice_builtin.h"
#include "tuxonice_atomic_copy.h"
#include "tuxonice_alloc.h"
#include "tuxonice_modules.h"

unsigned long extra_pd1_pages_used;

/* 
                                                                         
                       
                                            
  
                                                                           
                                                                            
  */
static void free_pbe_list(struct pbe **list, int highmem)
{
	while (*list) {
		int i;
		struct pbe *free_pbe, *next_page = NULL;
		struct page *page;

		if (highmem) {
			page = (struct page *)*list;
			free_pbe = (struct pbe *)kmap(page);
		} else {
			page = virt_to_page(*list);
			free_pbe = *list;
		}

		for (i = 0; i < PBES_PER_PAGE; i++) {
			if (!free_pbe)
				break;
			if (highmem)
				toi__free_page(29, free_pbe->address);
			else
				toi_free_page(29, (unsigned long)free_pbe->address);
			free_pbe = free_pbe->next;
		}

		if (highmem) {
			if (free_pbe)
				next_page = free_pbe;
			kunmap(page);
		} else {
			if (free_pbe)
				next_page = free_pbe;
		}

		toi__free_page(29, page);
		*list = (struct pbe *)next_page;
	};
}

/* 
                                              
  
                                                                   
                                                                      
                                                      
                           
                     
                                                      
                            
  */
void copyback_post(void)
{
	struct toi_boot_kernel_data *bkd = (struct toi_boot_kernel_data *)boot_kernel_data_buffer;

	if (toi_activate_storage(1))
		panic("Failed to reactivate our storage.");

	toi_post_atomic_restore_modules(bkd);

	toi_cond_pause(1, "About to reload secondary pagedir.");

	if (read_pageset2(0))
		panic("Unable to successfully reread the page cache.");

	/*
                                                                  
                                                               
                                                                  
                                                         
  */
	toi_check_resleep();
}

/* 
                                                     
  
                                                                            
                                                                           
                                                                       
                                       
  
                                                                            
                                                                             
                                                                          
                                                         
  */
void toi_copy_pageset1(void)
{
	int i;
	unsigned long source_index, dest_index;

	memory_bm_position_reset(pageset1_map);
	memory_bm_position_reset(pageset1_copy_map);

	source_index = memory_bm_next_pfn(pageset1_map);
	dest_index = memory_bm_next_pfn(pageset1_copy_map);

	for (i = 0; i < pagedir1.size; i++) {
		unsigned long *origvirt, *copyvirt;
		struct page *origpage, *copypage;
		int loop = (PAGE_SIZE / sizeof(unsigned long)) - 1, was_present1, was_present2;

#ifdef CONFIG_TOI_ENHANCE
		if (!pfn_valid(source_index) || !pfn_valid(dest_index)) {
			pr_emerg("[%s] (%d) dest_index:%lu, source_index:%lu\n", __func__, i,
				 dest_index, source_index);
			set_abort_result(TOI_ARCH_PREPARE_FAILED);
			return;
		}
#endif

		origpage = pfn_to_page(source_index);
		copypage = pfn_to_page(dest_index);

		origvirt = PageHighMem(origpage) ? kmap_atomic(origpage) : page_address(origpage);

		copyvirt = PageHighMem(copypage) ? kmap_atomic(copypage) : page_address(copypage);

		was_present1 = kernel_page_present(origpage);
		if (!was_present1)
			kernel_map_pages(origpage, 1, 1);

		was_present2 = kernel_page_present(copypage);
		if (!was_present2)
			kernel_map_pages(copypage, 1, 1);

		while (loop >= 0) {
			*(copyvirt + loop) = *(origvirt + loop);
			loop--;
		}

		if (!was_present1)
			kernel_map_pages(origpage, 1, 0);

		if (!was_present2)
			kernel_map_pages(copypage, 1, 0);

		if (PageHighMem(origpage))
			kunmap_atomic(origvirt);

		if (PageHighMem(copypage))
			kunmap_atomic(copyvirt);

		source_index = memory_bm_next_pfn(pageset1_map);
		dest_index = memory_bm_next_pfn(pageset1_copy_map);
	}
}

/* 
                                                               
  
                                                            
               
  
                                                                   
  */
int __toi_post_context_save(void)
{
	unsigned long old_ps1_size = pagedir1.size;

	check_checksums();

	free_checksum_pages();

	toi_recalculate_image_contents(1);

	extra_pd1_pages_used = pagedir1.size > old_ps1_size ? pagedir1.size - old_ps1_size : 0;

	if (extra_pd1_pages_used > extra_pd1_pages_allowance) {
		printk(KERN_INFO "Pageset1 has grown by %lu pages. "
		       "extra_pages_allowance is currently only %lu.\n",
		       pagedir1.size - old_ps1_size, extra_pd1_pages_allowance);

		/*
                                                      
                                               
   */
		if (any_to_free(1)) {
			set_abort_result(TOI_EXTRA_PAGES_ALLOW_TOO_SMALL);
			return 1;
		}
		if (try_allocate_extra_memory()) {
			printk(KERN_INFO "Failed to allocate the extra memory"
			       " needed. Restarting the process.");
			set_abort_result(TOI_EXTRA_PAGES_ALLOW_TOO_SMALL);
			return 1;
		}
		printk(KERN_INFO "However it looks like there's enough"
		       " free ram and storage to handle this, so " " continuing anyway.");
		/*
                                                  
                                                          
                                                            
   */
		toi_recalculate_image_contents(1);
	}

	if (!test_action_state(TOI_TEST_FILTER_SPEED) && !test_action_state(TOI_TEST_BIO))
		toi_copy_pageset1();

	return 0;
}

/* 
                                                            
  
                                                                      
                                                        
                                                               
                                                                    
             
                                                                          
                                     
  */
int toi_hibernate(void)
{
	int error;

	toi_running = 1;	/*                               */

	error = toi_lowlevel_builtin();

	if (!error) {
		struct toi_boot_kernel_data *bkd =
		    (struct toi_boot_kernel_data *)boot_kernel_data_buffer;

		/*
                                                            
                                                        
                                                              
                          
   */

		memcpy(&toi_bkd, (char *)boot_kernel_data_buffer,
		       min_t(int, sizeof(struct toi_boot_kernel_data), bkd->size));
	}

	toi_running = 0;
	return error;
}

/* 
                                                        
  
                                                                      
                                                            
                                                                   
                                                       
  */
int toi_atomic_restore(void)
{
	int error;

	toi_running = 1;

	toi_prepare_status(DONT_CLEAR_BAR, "Atomic restore.");

	memcpy(&toi_bkd.toi_nosave_commandline, saved_command_line, strlen(saved_command_line));

	toi_pre_atomic_restore_modules(&toi_bkd);

	if (add_boot_kernel_data_pbe())
		goto Failed;

	toi_prepare_status(DONT_CLEAR_BAR, "Doing atomic copy/restore.");

	if (toi_go_atomic(PMSG_QUIESCE, 0))
		goto Failed;

	/*                                                                   */
	save_processor_state();

	error = swsusp_arch_resume();
	/*
                                                                 
                                                                      
   
                                                                        
                                       
  */
	BUG();

 Failed:
	free_pbe_list(&restore_pblist, 0);
#ifdef CONFIG_HIGHMEM
	pr_warn("[%s] 0x%p 0x%p 0x%p\n", __func__,
			restore_highmem_pblist->address, restore_highmem_pblist->orig_address, restore_highmem_pblist->next);
	if (restore_highmem_pblist->next != NULL)
		free_pbe_list(&restore_highmem_pblist, 1);
#endif
	toi_running = 0;
	return 1;
}

/* 
                                                    
                                                                        
                                                        
  */
int toi_go_atomic(pm_message_t state, int suspend_time)
{
	if (suspend_time) {
		if (platform_begin(1)) {
			set_abort_result(TOI_PLATFORM_PREP_FAILED);
			toi_end_atomic(ATOMIC_STEP_PLATFORM_END, suspend_time, 3);
			hib_log("FAILED @line:%d suspend(%d) pm_state(%d)\n", __LINE__,
				suspend_time, state.event);
			return 1;
		}

		if (dpm_prepare(PMSG_FREEZE)) {
			set_abort_result(TOI_DPM_PREPARE_FAILED);
			dpm_complete(PMSG_RECOVER);
			toi_end_atomic(ATOMIC_STEP_PLATFORM_END, suspend_time, 3);
			hib_log("FAILED @line:%d suspend(%d) pm_state(%d)\n", __LINE__,
				suspend_time, state.event);
			return 1;
		}
	}

	suspend_console();
	ftrace_stop();
	pm_restrict_gfp_mask();

	if (suspend_time) {
#if 0				/*                                                                                                         */
		if (events_check_enabled) {
			hib_log("play trick here set events_check_enabled(%d) = false!!\n",
				events_check_enabled);
			events_check_enabled = false;
		}
#endif
		if (dpm_suspend(state)) {
			set_abort_result(TOI_DPM_SUSPEND_FAILED);
			toi_end_atomic(ATOMIC_STEP_DEVICE_RESUME, suspend_time, 3);
			hib_log("FAILED @line:%d suspend(%d) pm_state(%d) toi_result(0x%#lx)\n",
				__LINE__, suspend_time, state.event, toi_result);
			return 1;
		}
	} else {
		if (dpm_suspend_start(state)) {
			set_abort_result(TOI_DPM_SUSPEND_FAILED);
			toi_end_atomic(ATOMIC_STEP_DEVICE_RESUME, suspend_time, 3);
			hib_log("FAILED @line:%d suspend(%d) pm_state(%d) toi_result(0x%#lx)\n",
				__LINE__, suspend_time, state.event, toi_result);
			return 1;
		}
	}

	/*                                                              
                                                           
                                                                    
                                                               
                                              
  */

	if (dpm_suspend_end(state)) {
		set_abort_result(TOI_DEVICE_REFUSED);
		toi_end_atomic(ATOMIC_STEP_DEVICE_RESUME, suspend_time, 1);
		hib_log("FAILED @line:%d suspend(%d) pm_state(%d) toi_result(0x%#lx)\n", __LINE__,
			suspend_time, state.event, toi_result);
		return 1;
	}

	if (suspend_time) {
		if (platform_pre_snapshot(1))
			set_abort_result(TOI_PRE_SNAPSHOT_FAILED);
	} else {
		if (platform_pre_restore(1))
			set_abort_result(TOI_PRE_RESTORE_FAILED);
	}

	if (test_result_state(TOI_ABORTED)) {
		toi_end_atomic(ATOMIC_STEP_PLATFORM_FINISH, suspend_time, 1);
		hib_log("FAILED @line:%d suspend(%d) pm_state(%d) toi_result(0x%#lx)\n", __LINE__,
			suspend_time, state.event, toi_result);
		return 1;
	}

	if (test_action_state(TOI_LATE_CPU_HOTPLUG)) {
		if (disable_nonboot_cpus()) {
			set_abort_result(TOI_CPU_HOTPLUG_FAILED);
			toi_end_atomic(ATOMIC_STEP_CPU_HOTPLUG, suspend_time, 1);
			hib_log("FAILED @line:%d suspend(%d) pm_state(%d) toi_result(0x%#lx)\n",
				__LINE__, suspend_time, state.event, toi_result);
			return 1;
		}
	}

	local_irq_disable();

	if (syscore_suspend()) {
		set_abort_result(TOI_SYSCORE_REFUSED);
		toi_end_atomic(ATOMIC_STEP_IRQS, suspend_time, 1);
		hib_log("FAILED @line:%d suspend(%d) pm_state(%d) toi_result(0x%#lx)\n", __LINE__,
			suspend_time, state.event, toi_result);
		return 1;
	}

	if (suspend_time && pm_wakeup_pending()) {
		set_abort_result(TOI_WAKEUP_EVENT);
		toi_end_atomic(ATOMIC_STEP_SYSCORE_RESUME, suspend_time, 1);
		hib_log("FAILED @line:%d suspend(%d) pm_state(%d) toi_result(0x%#lx)\n", __LINE__,
			suspend_time, state.event, toi_result);
		return 1;
	}
	hib_log("SUCCEEDED @line:%d suspend(%d) pm_state(%d)\n", __LINE__, suspend_time,
		state.event);
	return 0;
}

/* 
                                                     
                                  
                                                       
                                                   
  */
void toi_end_atomic(int stage, int suspend_time, int error)
{
	pm_message_t msg = suspend_time ? (error ? PMSG_RECOVER : PMSG_THAW) : PMSG_RESTORE;

	switch (stage) {
	case ATOMIC_ALL_STEPS:
		if (!suspend_time) {
			events_check_enabled = false;
			platform_leave(1);
		}
	case ATOMIC_STEP_SYSCORE_RESUME:
		syscore_resume();
	case ATOMIC_STEP_IRQS:
		local_irq_enable();
	case ATOMIC_STEP_CPU_HOTPLUG:
		if (test_action_state(TOI_LATE_CPU_HOTPLUG))
			enable_nonboot_cpus();
	case ATOMIC_STEP_PLATFORM_FINISH:
		if (!suspend_time && error & 2)
			platform_restore_cleanup(1);
		else
			platform_finish(1);
		dpm_resume_start(msg);
	case ATOMIC_STEP_DEVICE_RESUME:
		if (suspend_time && (error & 2))
			platform_recover(1);
		dpm_resume(msg);
		if (error || !toi_in_suspend())
			pm_restore_gfp_mask();
		ftrace_start();
		resume_console();
	case ATOMIC_STEP_DPM_COMPLETE:
		dpm_complete(msg);
	case ATOMIC_STEP_PLATFORM_END:
		platform_end(1);

		toi_prepare_status(DONT_CLEAR_BAR, "Post atomic.");
	}
}
