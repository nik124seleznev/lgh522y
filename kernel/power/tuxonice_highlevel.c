/*
                                    
 */
/** \mainpage TuxOnIce.
 *
 * TuxOnIce provides support for saving and restoring an image of
 * system memory to an arbitrary storage device, either on the local computer,
 * or across some network. The support is entirely OS based, so TuxOnIce
 * works without requiring BIOS, APM or ACPI support. The vast majority of the
 * code is also architecture independant, so it should be very easy to port
 * the code to new architectures. TuxOnIce includes support for SMP, 4G HighMem
 * and preemption. Initramfses and initrds are also supported.
 *
 * TuxOnIce uses a modular design, in which the method of storing the image is
 * completely abstracted from the core code, as are transformations on the data
 * such as compression and/or encryption (multiple 'modules' can be used to
 * provide arbitrary combinations of functionality). The user interface is also
 * modular, so that arbitrarily simple or complex interfaces can be used to
 * provide anything from debugging information through to eye candy.
 *
 * \section Copyright
 *
 * TuxOnIce is released under the GPLv2.
 *
 * Copyright (C) 1998-2001 Gabor Kuti <seasons@fornax.hu><BR>
 * Copyright (C) 1998,2001,2002 Pavel Machek <pavel@suse.cz><BR>
 * Copyright (C) 2002-2003 Florent Chabaud <fchabaud@free.fr><BR>
 * Copyright (C) 2002-2010 Nigel Cunningham (nigel at tuxonice net)<BR>
 *
 * \section Credits
 *
 * Nigel would like to thank the following people for their work:
 *
 * Bernard Blackham <bernard@blackham.com.au><BR>
 * Web page & Wiki administration, some coding. A person without whom
 * TuxOnIce would not be where it is.
 *
 * Michael Frank <mhf@linuxmail.org><BR>
 * Extensive testing and help with improving stability. I was constantly
 * amazed by the quality and quantity of Michael's help.
 *
 * Pavel Machek <pavel@ucw.cz><BR>
 * Modifications, defectiveness pointing, being with Gabor at the very
 * beginning, suspend to swap space, stop all tasks. Port to 2.4.18-ac and
 * 2.5.17. Even though Pavel and I disagree on the direction suspend to
 * disk should take, I appreciate the valuable work he did in helping Gabor
 * get the concept working.
 *
 * ..and of course the myriads of TuxOnIce users who have helped diagnose
 * and fix bugs, made suggestions on how to improve the code, proofread
 * documentation, and donated time and money.
 *
 * Thanks also to corporate sponsors:
 *
 * <B>Redhat.</B>Sometime employer from May 2006 (my fault, not Redhat's!).
 *
 * <B>Cyclades.com.</B> Nigel's employers from Dec 2004 until May 2006, who
 * allowed him to work on TuxOnIce and PM related issues on company time.
 *
 * <B>LinuxFund.org.</B> Sponsored Nigel's work on TuxOnIce for four months Oct
 * 2003 to Jan 2004.
 *
 * <B>LAC Linux.</B> Donated P4 hardware that enabled development and ongoing
 * maintenance of SMP and Highmem support.
 *
 * <B>OSDL.</B> Provided access to various hardware configurations, make
 * occasional small donations to the project.
 */

#include <linux/suspend.h>
#include <linux/freezer.h>
#include <generated/utsrelease.h>
#include <linux/cpu.h>
#include <linux/console.h>
#include <linux/writeback.h>
#include <linux/uaccess.h>	/*                                    */
#include <linux/bio.h>
#include <linux/kgdb.h>

#include "tuxonice.h"
#include "tuxonice_modules.h"
#include "tuxonice_sysfs.h"
#include "tuxonice_prepare_image.h"
#include "tuxonice_io.h"
#include "tuxonice_ui.h"
#include "tuxonice_power_off.h"
#include "tuxonice_storage.h"
#include "tuxonice_checksum.h"
#include "tuxonice_builtin.h"
#include "tuxonice_atomic_copy.h"
#include "tuxonice_alloc.h"
#include "tuxonice_cluster.h"

/*                    */
struct pagedir pagedir2 = { 2 };

EXPORT_SYMBOL_GPL(pagedir2);

static mm_segment_t oldfs;
static DEFINE_MUTEX(tuxonice_in_use);
static int block_dump_save;

/*                                         */
char tuxonice_signature[9] = "\xed\xc3\x02\xe9\x98\x56\xe5\x0c";
EXPORT_SYMBOL_GPL(tuxonice_signature);

unsigned long boot_kernel_data_buffer;

#if 0				/*                */
static char *result_strings[] = {
	"Hibernation was aborted",
	"The user requested that we cancel the hibernation",
	"No storage was available",
	"Insufficient storage was available",
	"Freezing filesystems and/or tasks failed",
	"A pre-existing image was used",
	"We would free memory, but image size limit doesn't allow this",
	"Unable to free enough memory to hibernate",
	"Unable to obtain the Power Management Semaphore",
	"A device suspend/resume returned an error",
	"A system device suspend/resume returned an error",
	"The extra pages allowance is too small",
	"We were unable to successfully prepare an image",
	"TuxOnIce module initialisation failed",
	"TuxOnIce module cleanup failed",
	"I/O errors were encountered",
	"Ran out of memory",
	"An error was encountered while reading the image",
	"Platform preparation failed",
	"CPU Hotplugging failed",
	"Architecture specific preparation failed",
	"Pages needed resaving, but we were told to abort if this happens",
	"We can't hibernate at the moment (invalid resume= or filewriter " "target?)",
	"A hibernation preparation notifier chain member cancelled the " "hibernation",
	"Pre-snapshot preparation failed",
	"Pre-restore preparation failed",
	"Failed to disable usermode helpers",
	"Can't resume from alternate image",
	"Header reservation too small",
	"Device Power Management Preparation failed",
};
#endif

/* 
                                                     
                                                                
               
  
                                                                        
                                                                           
                                                                            
  */
void toi_finish_anything(int hibernate_or_resume)
{
	hib_log("hibernate_or_resume(%d)\n", hibernate_or_resume);
	toi_cleanup_modules(hibernate_or_resume);
	toi_put_modules();
	if (hibernate_or_resume) {
		block_dump = block_dump_save;
		set_cpus_allowed_ptr(current, cpu_all_mask);
		toi_alloc_print_debug_stats();
		atomic_inc(&snapshot_device_available);
		unlock_system_sleep();
	}

	set_fs(oldfs);
	mutex_unlock(&tuxonice_in_use);
}

/* 
                                                         
                                                                   
  
                                                                        
                                                                            
                                                                       
  */
int toi_start_anything(int hibernate_or_resume)
{
	mutex_lock(&tuxonice_in_use);

	oldfs = get_fs();
	set_fs(KERNEL_DS);

	if (hibernate_or_resume) {
		lock_system_sleep();

		if (!atomic_add_unless(&snapshot_device_available, -1, 0))
			goto snapshotdevice_unavailable;
	}

	if (hibernate_or_resume == SYSFS_HIBERNATE)
		toi_print_modules();

	if (toi_get_modules()) {
		printk(KERN_INFO "TuxOnIce: Get modules failed!\n");
		goto prehibernate_err;
	}

	if (hibernate_or_resume) {
		block_dump_save = block_dump;
		block_dump = 0;
		set_cpus_allowed_ptr(current, cpumask_of(cpumask_first(cpu_online_mask)));
	}

	if (toi_initialise_modules_early(hibernate_or_resume))
		goto early_init_err;

	if (!toiActiveAllocator) {
		hib_log("hibernate_or_resume(0x%08x), resume_file=\"%s\"\n", hibernate_or_resume,
			resume_file);
		toi_attempt_to_parse_resume_device(!hibernate_or_resume);
	}

	if (!toi_initialise_modules_late(hibernate_or_resume))
		return 0;

	toi_cleanup_modules(hibernate_or_resume);
 early_init_err:
	if (hibernate_or_resume) {
		block_dump_save = block_dump;
		set_cpus_allowed_ptr(current, cpu_all_mask);
	}
	toi_put_modules();
 prehibernate_err:
	if (hibernate_or_resume)
		atomic_inc(&snapshot_device_available);
 snapshotdevice_unavailable:
	if (hibernate_or_resume)
		mutex_unlock(&pm_mutex);
	set_fs(oldfs);
	mutex_unlock(&tuxonice_in_use);
	return -EBUSY;
}

/*
                        
  
                                                                              
                    
 */

/* 
                                               
  
                                                                         
                                
  */
static void mark_nosave_pages(void)
{
	struct nosave_region *region;

	list_for_each_entry(region, &nosave_regions, list) {
		unsigned long pfn;

		for (pfn = region->start_pfn; pfn < region->end_pfn; pfn++)
			if (pfn_valid(pfn))
				SetPageNosave(pfn_to_page(pfn));
	}
}

static int toi_alloc_bitmap(struct memory_bitmap **bm)
{
	int result = 0;

	*bm = kzalloc(sizeof(struct memory_bitmap), GFP_KERNEL);
	if (!*bm) {
		printk(KERN_ERR "Failed to kzalloc memory for a bitmap.\n");
		return -ENOMEM;
	}

	result = memory_bm_create(*bm, GFP_KERNEL, 0);

	if (result) {
		printk(KERN_ERR "Failed to create a bitmap.\n");
		kfree(*bm);
		*bm = NULL;
	}

	return result;
}

/* 
                                                                 
  
                                                                     
               
  */
static int allocate_bitmaps(void)
{
	if (toi_alloc_bitmap(&pageset1_map) ||
	    toi_alloc_bitmap(&pageset1_copy_map) ||
	    toi_alloc_bitmap(&pageset2_map) ||
	    toi_alloc_bitmap(&io_map) ||
	    toi_alloc_bitmap(&nosave_map) ||
	    toi_alloc_bitmap(&free_map) || toi_alloc_bitmap(&page_resave_map))
		return 1;

	return 0;
}

static void toi_free_bitmap(struct memory_bitmap **bm)
{
	if (!*bm)
		return;

	memory_bm_free(*bm, 0);
	kfree(*bm);
	*bm = NULL;
}

/* 
                                                             
  
                                                               
                                                             
  */
static void free_bitmaps(void)
{
	toi_free_bitmap(&pageset1_map);
	toi_free_bitmap(&pageset1_copy_map);
	toi_free_bitmap(&pageset2_map);
	toi_free_bitmap(&io_map);
	toi_free_bitmap(&nosave_map);
	toi_free_bitmap(&free_map);
	toi_free_bitmap(&page_resave_map);
}

/* 
                                                               
                                                         
  
                                                                          
  */
static int io_MB_per_second(int write)
{
	return (toi_bkd.toi_io_time[write][1]) ?
	    MB((unsigned long)toi_bkd.toi_io_time[write][0]) * HZ /
	    toi_bkd.toi_io_time[write][1] : 0;
}

#define SNPRINTF(a...)	do { len += scnprintf(((char *) buffer) + len, \
		count - len - 1, ## a); } while (0)

/* 
                                                            
                                    
                                            
  
                                                                          
                                     
  */
static int get_toi_debug_info(const char *buffer, int count)
{
	int len = 0, i, first_result = 1;

	SNPRINTF("TuxOnIce debugging info:\n");
	SNPRINTF("- TuxOnIce core  : " TOI_CORE_VERSION "\n");
	SNPRINTF("- Kernel Version : " UTS_RELEASE "\n");
	SNPRINTF("- Compiler vers. : %d.%d\n", __GNUC__, __GNUC_MINOR__);
	SNPRINTF("- Attempt number : %d\n", nr_hibernates);
	SNPRINTF("- Parameters     : %ld %ld %ld %d %ld %ld\n",
		 toi_result,
		 toi_bkd.toi_action,
		 toi_bkd.toi_debug_state,
		 toi_bkd.toi_default_console_level, image_size_limit, toi_poweroff_method);
	SNPRINTF("- Overall expected compression percentage: %d.\n",
		 100 - toi_expected_compression_ratio());
	len += toi_print_module_debug_info(((char *)buffer) + len, count - len - 1);
	if (toi_bkd.toi_io_time[0][1]) {
		if ((io_MB_per_second(0) < 5) || (io_MB_per_second(1) < 5)) {
			SNPRINTF("- I/O speed: Write %ld KB/s",
				 (KB((unsigned long)toi_bkd.toi_io_time[0][0]) * HZ /
				  toi_bkd.toi_io_time[0][1]));
			if (toi_bkd.toi_io_time[1][1])
				SNPRINTF(", Read %ld KB/s", (KB((unsigned long)
								toi_bkd.toi_io_time[1][0]) * HZ /
							     toi_bkd.toi_io_time[1][1]));
		} else {
			SNPRINTF("- I/O speed: Write %ld MB/s",
				 (MB((unsigned long)toi_bkd.toi_io_time[0][0]) * HZ /
				  toi_bkd.toi_io_time[0][1]));
			if (toi_bkd.toi_io_time[1][1])
				SNPRINTF(", Read %ld MB/s", (MB((unsigned long)
								toi_bkd.toi_io_time[1][0]) * HZ /
							     toi_bkd.toi_io_time[1][1]));
		}
		SNPRINTF(".\n");
	} else
		SNPRINTF("- No I/O speed stats available.\n");
	SNPRINTF("- Extra pages    : %lu used/%lu.\n",
		 extra_pd1_pages_used, extra_pd1_pages_allowance);

	for (i = 0; i < TOI_NUM_RESULT_STATES; i++)
		if (test_result_state(i)) {
#if 0				/*                */
			SNPRINTF("%s: %s.\n", first_result ?
				 "- Result         " : "                 ", result_strings[i]);
#endif
			first_result = 0;
		}
	if (first_result) {
#if 0				/*                */
		SNPRINTF("- Result         : %s.\n", nr_hibernates ?
			 "Succeeded" : "No hibernation attempts so far");
#endif
	}
	return len;
}

/* 
                                                               
                                                                  
  
                                                                    
                              
  */
static void do_cleanup(int get_debug_info, int restarting)
{
	int i = 0;
	char *buffer = NULL;

	trap_non_toi_io = 0;

	if (get_debug_info)
		toi_prepare_status(DONT_CLEAR_BAR, "Cleaning up...");
#if !defined(HIB_TOI_DEBUG)	/*                                     */
	hib_warn("Turn off debug info\n");
	get_debug_info = 0;
#endif

#ifdef CONFIG_TOI_ENHANCE
	toi_actual_compression_ratio(); /*                                                */
#endif

	free_checksum_pages();

	if (get_debug_info)
		buffer = (char *)toi_get_zeroed_page(20, TOI_ATOMIC_GFP);

	if (buffer)
		i = get_toi_debug_info(buffer, PAGE_SIZE);

	toi_free_extra_pagedir_memory();

	pagedir1.size = 0;
	pagedir2.size = 0;
	set_highmem_size(pagedir1, 0);
	set_highmem_size(pagedir2, 0);

	if (boot_kernel_data_buffer) {
		if (!test_toi_state(TOI_BOOT_KERNEL))
			toi_free_page(37, boot_kernel_data_buffer);
		boot_kernel_data_buffer = 0;
	}

	clear_toi_state(TOI_BOOT_KERNEL);
	thaw_processes();

	if (!restarting)
		toi_stop_other_threads();

	if (test_action_state(TOI_KEEP_IMAGE) && !test_result_state(TOI_ABORTED)) {
		toi_message(TOI_ANY_SECTION, TOI_LOW, 1,
			    "TuxOnIce: Not invalidating the image due "
			    "to Keep Image being enabled.");
		set_result_state(TOI_KEPT_IMAGE);
	} else if (toiActiveAllocator)
		toiActiveAllocator->remove_image();

	free_bitmaps();
	usermodehelper_enable();

	if (test_toi_state(TOI_NOTIFIERS_PREPARE)) {
		pm_notifier_call_chain(PM_POST_HIBERNATION);
		clear_toi_state(TOI_NOTIFIERS_PREPARE);
	}

	if (buffer && i) {
		/*                                             
                         */
		for (i = 0; i < 3; i++)
			printk(KERN_ERR "%s", buffer + (1023 * i));
		toi_free_page(20, (unsigned long)buffer);
	}

	if (!test_action_state(TOI_LATE_CPU_HOTPLUG))
		enable_nonboot_cpus();

	if (!restarting)
		toi_cleanup_console();

	free_attention_list();

	if (!restarting)
		toi_deactivate_storage(0);

	clear_toi_state(TOI_IGNORE_LOGLEVEL);
	clear_toi_state(TOI_TRYING_TO_RESUME);
	clear_toi_state(TOI_NOW_RESUMING);
}

/* 
                                                                           
  
                                                                              
                                                                          
                                                                          
                                                                            
                
  
                                                                         
                                                         
  */
static int check_still_keeping_image(void)
{
	if (test_action_state(TOI_KEEP_IMAGE)) {
		printk(KERN_INFO "Image already stored: powering down " "immediately.");
		do_toi_step(STEP_HIBERNATE_POWERDOWN);
		return 1;	/*                             */
	}

	printk(KERN_INFO "Invalidating previous image.\n");
	toiActiveAllocator->remove_image();

	return 0;
}

/* 
                                          
  
                                                             
                       
  */
static int toi_init(int restarting)
{
	int result, i, j;

#ifdef CONFIG_TOI_FIXUP
	if (test_result_state(TOI_ABORTED))
		return 1;
#endif

	toi_result = 0;

	printk(KERN_INFO "Initiating a hibernation cycle.\n");

	nr_hibernates++;

	for (i = 0; i < 2; i++)
		for (j = 0; j < 2; j++)
			toi_bkd.toi_io_time[i][j] = 0;

	if (!test_toi_state(TOI_CAN_HIBERNATE) || allocate_bitmaps())
		return 1;

	mark_nosave_pages();

	if (!restarting)
		toi_prepare_console();

	result = pm_notifier_call_chain(PM_HIBERNATION_PREPARE);
	if (result) {
		set_result_state(TOI_NOTIFIERS_PREPARE_FAILED);
		return 1;
	}
	set_toi_state(TOI_NOTIFIERS_PREPARE);

	if (!restarting) {
		int num_threaded;
		num_threaded = toi_start_other_threads();
		printk(KERN_ERR "Starting other threads (%d).", num_threaded);
	}

	result = usermodehelper_disable();
	if (result) {
		printk(KERN_ERR "TuxOnIce: Failed to disable usermode " "helpers\n");
		set_result_state(TOI_USERMODE_HELPERS_ERR);
		return 1;
	}

	boot_kernel_data_buffer = toi_get_zeroed_page(37, TOI_ATOMIC_GFP);
	if (!boot_kernel_data_buffer) {
		printk(KERN_ERR "TuxOnIce: Failed to allocate " "boot_kernel_data_buffer.\n");
		set_result_state(TOI_OUT_OF_MEMORY);
		return 1;
	}

	if (!test_action_state(TOI_LATE_CPU_HOTPLUG) && disable_nonboot_cpus()) {
		set_abort_result(TOI_CPU_HOTPLUG_FAILED);
		return 1;
	}

	return 0;
}

/* 
                                                          
  
                                                                             
                                                                            
                     
  */
static int can_hibernate(void)
{
	if (!test_toi_state(TOI_CAN_HIBERNATE))
		toi_attempt_to_parse_resume_device(0);

	if (!test_toi_state(TOI_CAN_HIBERNATE)) {
		printk(KERN_INFO "TuxOnIce: Hibernation is disabled.\n"
		       "This may be because you haven't put something along "
		       "the lines of\n\nresume=swap:/dev/hda1\n\n"
		       "in lilo.conf or equivalent. (Where /dev/hda1 is your "
		       "swap partition).\n");
		set_abort_result(TOI_CANT_SUSPEND);
		return 0;
	}

	if (strlen(alt_resume_param)) {
		attempt_to_parse_alt_resume_param();

		if (!strlen(alt_resume_param)) {
			printk(KERN_INFO "Alternate resume parameter now " "invalid. Aborting.\n");
			set_abort_result(TOI_CANT_USE_ALT_RESUME);
			return 0;
		}
	}

	hib_log("passed!\n");

	return 1;
}

/* 
                                                                            
  
                                                                          
                                                                        
                                     
  */
static int do_post_image_write(void)
{
	/*                                                */
	if (alt_resume_param[0])
		do_toi_step(STEP_RESUME_ALT_IMAGE);

	toi_power_down();

	barrier();
	mb();
	return 0;
}

/* 
                                                      
  
                                                                           
                                                                          
  
                                                                           
                                                                        
        
  */
static int __save_image(void)
{
	int temp_result, did_copy = 0;

	toi_prepare_status(DONT_CLEAR_BAR, "Starting to save the image..");

	toi_message(TOI_ANY_SECTION, TOI_LOW, 1,
		    " - Final values: %d and %d.", pagedir1.size, pagedir2.size);

	toi_cond_pause(1, "About to write pagedir2.");

	temp_result = write_pageset(&pagedir2);

	if (temp_result == -1 || test_result_state(TOI_ABORTED))
		return 1;

	toi_cond_pause(1, "About to copy pageset 1.");

	if (test_result_state(TOI_ABORTED))
		return 1;

	toi_deactivate_storage(1);

	toi_prepare_status(DONT_CLEAR_BAR, "Doing atomic copy/restore.");

	toi_in_hibernate = 1;

	if (toi_go_atomic(PMSG_FREEZE, 1))
		goto Failed;

	hib_log("calling toi_hibernate()\n");
	temp_result = toi_hibernate();

#ifdef CONFIG_KGDB
	if (test_action_state(TOI_POST_RESUME_BREAKPOINT))
		kgdb_breakpoint();
#endif

	if (!temp_result)
		did_copy = 1;

	hib_log("calling toi_end_atomic() toi_in_hibernate(%d) temp_result(%d)\n", toi_in_hibernate,
		temp_result);
	/*                                    */
	toi_end_atomic(ATOMIC_ALL_STEPS, toi_in_hibernate, temp_result);

 Failed:
	if (toi_activate_storage(1))
		panic("Failed to reactivate our storage.");

	/*              */
	if (!toi_in_hibernate) {
		hib_log("last resume here ...\n");
		copyback_post();
		return 0;
	}

	/*                                                        */

	if (temp_result || test_result_state(TOI_ABORTED)) {
		if (did_copy)
			goto abort_reloading_pagedir_two;
		else
			return 1;
	}

	hib_log("@line:%d\n", __LINE__);
	toi_update_status(pagedir2.size, pagedir1.size + pagedir2.size, NULL);

	if (test_result_state(TOI_ABORTED))
		goto abort_reloading_pagedir_two;

	toi_cond_pause(1, "About to write pageset1.");

	toi_message(TOI_ANY_SECTION, TOI_LOW, 1, "-- Writing pageset1");

	temp_result = write_pageset(&pagedir1);

	/*                                                                */
	if (test_action_state(TOI_TEST_FILTER_SPEED) || test_action_state(TOI_TEST_BIO))
		return 1;

	if (temp_result == 1 || test_result_state(TOI_ABORTED))
		goto abort_reloading_pagedir_two;

	toi_cond_pause(1, "About to write header.");

	if (test_result_state(TOI_ABORTED))
		goto abort_reloading_pagedir_two;

	temp_result = write_image_header();

	if (!temp_result && !test_result_state(TOI_ABORTED))
		return 0;

 abort_reloading_pagedir_two:
	temp_result = read_pageset2(1);

	/*                                    */
	if (temp_result)
		panic("Attempt to reload pagedir 2 while aborting " "a hibernate failed.");

	return 1;
}

static void map_ps2_pages(int enable)
{
	unsigned long pfn = 0;

	pfn = memory_bm_next_pfn(pageset2_map);

	while (pfn != BM_END_OF_MAP) {
		struct page *page = pfn_to_page(pfn);
		kernel_map_pages(page, 1, enable);
		pfn = memory_bm_next_pfn(pageset2_map);
	}
}

/* 
                                                       
  
                                                                     
                                    
  */
static int do_save_image(void)
{
	int result;
	map_ps2_pages(0);
	result = __save_image();
	map_ps2_pages(1);
	return result;
}

/* 
                                             
  
                                                                   
           
  */
static int do_prepare_image(void)
{
	int restarting = test_result_state(TOI_EXTRA_PAGES_ALLOW_TOO_SMALL);

	if (!restarting && toi_activate_storage(0))
		return 1;

	hib_log("step 1 @line:%d\n", __LINE__);
	/*
                                                                         
                                                                       
                                                                       
  */

	if (!can_hibernate() || (test_result_state(TOI_KEPT_IMAGE) && check_still_keeping_image()))
		return 1;

	hib_log("step 2 @line:%d\n", __LINE__);

	if (toi_init(restarting) || toi_prepare_image() || test_result_state(TOI_ABORTED))
		return 1;

	hib_log("step 3 @line:%d\n", __LINE__);

	trap_non_toi_io = 1;

	return 0;
}

/* 
                                                                  
  
                                                               
                                                             
                                                    
  */
int do_check_can_resume(void)
{
	int result = -1;

	if (toi_activate_storage(0))
		return -1;

	if (!test_toi_state(TOI_RESUME_DEVICE_OK))
		toi_attempt_to_parse_resume_device(1);

	if (toiActiveAllocator)
		result = toiActiveAllocator->image_exists(1);

	toi_deactivate_storage(0);
	return result;
}
EXPORT_SYMBOL_GPL(do_check_can_resume);

/* 
                                                                      
  
                                                                    
                                                                 
                                                                      
                  
  
                                                                            
  */
static int do_load_atomic_copy(void)
{
	int read_image_result = 0;

	if (sizeof(swp_entry_t) != sizeof(long)) {
		printk(KERN_WARNING "TuxOnIce: The size of swp_entry_t != size"
		       " of long. Please report this!\n");
		return 1;
	}

	if (!resume_file[0])
		printk(KERN_WARNING "TuxOnIce: "
		       "You need to use a resume= command line parameter to "
		       "tell TuxOnIce where to look for an image.\n");

	toi_activate_storage(0);

	if (!(test_toi_state(TOI_RESUME_DEVICE_OK)) && !toi_attempt_to_parse_resume_device(0)) {
		/*
                                                        
                              
   */

		if (!toiNumAllocators)
			printk(KERN_ALERT "TuxOnIce: "
			       "No storage allocators have been registered.\n");
		else
			printk(KERN_ALERT "TuxOnIce: "
			       "Missing or invalid storage location "
			       "(resume= parameter). Please correct and "
			       "rerun lilo (or equivalent) before " "hibernating.\n");
		toi_deactivate_storage(0);
		return 1;
	}

	if (allocate_bitmaps())
		return 1;

	read_image_result = read_pageset1();	/*                         */

	if (test_toi_state(TOI_NORESUME_SPECIFIED))
		clear_toi_state(TOI_NORESUME_SPECIFIED);

	toi_deactivate_storage(0);

	if (read_image_result)
		return 1;

	return 0;
}

/* 
                                                                      
  
                                                                       
  */
static void prepare_restore_load_alt_image(int prepare)
{
	static struct memory_bitmap *pageset1_map_save, *pageset1_copy_map_save;

	if (prepare) {
		pageset1_map_save = pageset1_map;
		pageset1_map = NULL;
		pageset1_copy_map_save = pageset1_copy_map;
		pageset1_copy_map = NULL;
		set_toi_state(TOI_LOADING_ALT_IMAGE);
		toi_reset_alt_image_pageset2_pfn();
	} else {
		memory_bm_free(pageset1_map, 0);
		pageset1_map = pageset1_map_save;
		memory_bm_free(pageset1_copy_map, 0);
		pageset1_copy_map = pageset1_copy_map_save;
		clear_toi_state(TOI_NOW_RESUMING);
		clear_toi_state(TOI_LOADING_ALT_IMAGE);
	}
}

/* 
                                                          
  
                                                                       
                                                                            
                                                            
  */
int do_toi_step(int step)
{
	switch (step) {
	case STEP_HIBERNATE_PREPARE_IMAGE:
		return do_prepare_image();
	case STEP_HIBERNATE_SAVE_IMAGE:
		return do_save_image();
	case STEP_HIBERNATE_POWERDOWN:
		return do_post_image_write();
	case STEP_RESUME_CAN_RESUME:
		return do_check_can_resume();
	case STEP_RESUME_LOAD_PS1:
		return do_load_atomic_copy();
	case STEP_RESUME_DO_RESTORE:
		/*
                                        
                                                   
                       
   */
		return toi_atomic_restore();
	case STEP_RESUME_ALT_IMAGE:
		printk(KERN_INFO "Trying to resume alternate image.\n");
		toi_in_hibernate = 0;
		save_restore_alt_param(SAVE, NOQUIET);
		prepare_restore_load_alt_image(1);
		if (!do_check_can_resume()) {
			printk(KERN_INFO "Nothing to resume from.\n");
			goto out;
		}
		if (!do_load_atomic_copy())
			toi_atomic_restore();

		printk(KERN_INFO "Failed to load image.\n");
 out:
		prepare_restore_load_alt_image(0);
		save_restore_alt_param(RESTORE, NOQUIET);
		break;
	case STEP_CLEANUP:
		do_cleanup(1, 0);
		break;
	case STEP_QUIET_CLEANUP:
		do_cleanup(0, 0);
		break;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(do_toi_step);

/*                                                         */

/* 
                                                   
  
                                                                      
             
  */
void toi_try_resume(void)
{
	int num_threaded;

	hib_log("entering...\n");
	set_toi_state(TOI_TRYING_TO_RESUME);
	resume_attempted = 1;

	current->flags |= PF_MEMALLOC;

	get_online_cpus();	/*                                         */
	num_threaded = toi_start_other_threads();
	printk(KERN_ERR "[resume] Starting other threads (%d).", num_threaded);

	if (do_toi_step(STEP_RESUME_CAN_RESUME) && !do_toi_step(STEP_RESUME_LOAD_PS1)) {
		put_online_cpus();	/*                                         */
		do_toi_step(STEP_RESUME_DO_RESTORE);
	} else {
		put_online_cpus();	/*                                         */
	}

	toi_stop_other_threads();
	do_cleanup(0, 0);

	current->flags &= ~PF_MEMALLOC;

	clear_toi_state(TOI_IGNORE_LOGLEVEL);
	clear_toi_state(TOI_TRYING_TO_RESUME);
	clear_toi_state(TOI_NOW_RESUMING);
}

/* 
                                                                 
  
                                                                       
                                                         
  */
static void toi_sys_power_disk_try_resume(void)
{
	resume_attempted = 1;

	/*
                                                           
                                                          
                                                              
                
  */
	lockdep_off();

	if (toi_start_anything(SYSFS_RESUMING))
		goto out;

	toi_try_resume();

	/*
                                                 
                               
  */
	clear_toi_state(TOI_BOOT_TIME);

	toi_finish_anything(SYSFS_RESUMING);
 out:
	lockdep_on();
}

/* 
                                                       
  
                                                   
                                        
  
     
  
                               
  
                                                             
                             
  */
int toi_try_hibernate(void)
{
	int result = 0, sys_power_disk = 0, retries = 0;

	if (!mutex_is_locked(&tuxonice_in_use)) {
		/*                             */
		if (toi_start_anything(SYSFS_HIBERNATING))
			return -EBUSY;
		sys_power_disk = 1;
	}

	current->flags |= PF_MEMALLOC;

	if (test_toi_state(TOI_CLUSTER_MODE)) {
		toi_initiate_cluster_hibernate();
		goto out;
	}

 prepare:
	result = do_toi_step(STEP_HIBERNATE_PREPARE_IMAGE);
	hib_log("after calling do_toi_step(STEP_HIBERNATE_PREPARE_IMAGE), result(%d)\n", result);

	if (result)
		goto out;

	if (test_action_state(TOI_FREEZER_TEST))
		goto out_restore_gfp_mask;

	result = do_toi_step(STEP_HIBERNATE_SAVE_IMAGE);

	if (test_result_state(TOI_EXTRA_PAGES_ALLOW_TOO_SMALL)) {
		if (retries < 2) {
			hib_log("failed and calling do_cleanup(0, 1)\n");
			do_cleanup(0, 1);
			retries++;
			clear_result_state(TOI_ABORTED);
			extra_pd1_pages_allowance = extra_pd1_pages_used + 500;
			printk(KERN_INFO "Automatically adjusting the extra"
			       " pages allowance to %ld and restarting.\n",
			       extra_pd1_pages_allowance);
			pm_restore_gfp_mask();
			goto prepare;
		}

		printk(KERN_INFO "Adjusted extra pages allowance twice and "
		       "still couldn't hibernate successfully. Giving up.");
	}

	/*                                    */
	if (!result && toi_in_hibernate)
		result = do_toi_step(STEP_HIBERNATE_POWERDOWN);

 out_restore_gfp_mask:
	pm_restore_gfp_mask();
 out:
	do_cleanup(1, 0);
	current->flags &= ~PF_MEMALLOC;

	if (sys_power_disk)
		toi_finish_anything(SYSFS_HIBERNATING);

	return result;
}

/*
                                                                
 */
int toi_launch_userspace_program(char *command, int channel_no, int wait, int debug)
{
	int retval;
	static char *envp[] = {
		"HOME=/",
		"TERM=linux",
		"PATH=/sbin:/usr/sbin:/bin:/usr/bin",
		NULL
	};
	static char *argv[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
	};
	char *channel = NULL;
	int arg = 0, size;
	char test_read[255];
	char *orig_posn = command;

	if (!strlen(orig_posn))
		return 1;

	if (channel_no) {
		channel = toi_kzalloc(4, 6, GFP_KERNEL);
		if (!channel) {
			printk(KERN_INFO "Failed to allocate memory in "
			       "preparing to launch userspace program.\n");
			return 1;
		}
	}

	/*                        */
	while (arg < 6) {
		sscanf(orig_posn, "%254s", test_read);
		size = strlen(test_read);
		if (!(size))
			break;
		argv[arg] = toi_kzalloc(5, size + 1, TOI_ATOMIC_GFP);
		strcpy(argv[arg], test_read);
		orig_posn += size + 1;
		*test_read = 0;
		arg++;
	}

	if (channel_no) {
		sprintf(channel, "-c%d", channel_no);
		argv[arg] = channel;
	} else
		arg--;

	if (debug) {
		argv[++arg] = toi_kzalloc(5, 8, TOI_ATOMIC_GFP);
		strcpy(argv[arg], "--debug");
	}

	retval = call_usermodehelper(argv[0], argv, envp, wait);

	/*
                                                                 
                    
  */
	if (retval && retval != 256)
		printk(KERN_ERR "Failed to launch userspace program '%s': "
		       "Error %d\n", command, retval);

	{
		int i;
		for (i = 0; i < arg; i++)
			if (argv[i] && argv[i] != channel)
				toi_kfree(5, argv[i], sizeof(*argv[i]));
	}

	toi_kfree(4, channel, sizeof(*channel));

	return retval;
}

#ifdef CONFIG_TOI_ENHANCE
int toi_abort_hibernate(void)
{
	if (test_result_state(TOI_ABORTED))
		return 0;

	set_result_state(TOI_ABORTED);

	return 0;
}
EXPORT_SYMBOL_GPL(toi_abort_hibernate);

int toi_hibernate_fatalerror(void)
{
	if (test_result_state(TOI_ARCH_PREPARE_FAILED))
		return 1;
	else
		return 0;
}
EXPORT_SYMBOL_GPL(toi_hibernate_fatalerror);
#endif

/*
                                                                   
                                                                            
 */
static struct toi_sysfs_data sysfs_params[] = {
	SYSFS_LONG("extra_pages_allowance", SYSFS_RW,
		   &extra_pd1_pages_allowance, 0, LONG_MAX, 0),
	SYSFS_CUSTOM("image_exists", SYSFS_RW, image_exists_read,
		     image_exists_write, SYSFS_NEEDS_SM_FOR_BOTH, NULL),
	SYSFS_STRING("resume", SYSFS_RW, resume_file, 255,
		     SYSFS_NEEDS_SM_FOR_WRITE,
		     attempt_to_parse_resume_device2),
	SYSFS_STRING("alt_resume_param", SYSFS_RW, alt_resume_param, 255,
		     SYSFS_NEEDS_SM_FOR_WRITE,
		     attempt_to_parse_alt_resume_param),
	SYSFS_CUSTOM("debug_info", SYSFS_READONLY, get_toi_debug_info, NULL, 0,
		     NULL),
	SYSFS_BIT("ignore_rootfs", SYSFS_RW, &toi_bkd.toi_action,
		  TOI_IGNORE_ROOTFS, 0),
	SYSFS_LONG("image_size_limit", SYSFS_RW, &image_size_limit, -2,
		   INT_MAX, 0),
	SYSFS_UL("last_result", SYSFS_RW, &toi_result, 0, 0, 0),
	SYSFS_BIT("no_multithreaded_io", SYSFS_RW, &toi_bkd.toi_action,
		  TOI_NO_MULTITHREADED_IO, 0),
	SYSFS_BIT("no_flusher_thread", SYSFS_RW, &toi_bkd.toi_action,
		  TOI_NO_FLUSHER_THREAD, 0),
	SYSFS_BIT("full_pageset2", SYSFS_RW, &toi_bkd.toi_action,
		  TOI_PAGESET2_FULL, 0),
	SYSFS_BIT("reboot", SYSFS_RW, &toi_bkd.toi_action, TOI_REBOOT, 0),
	SYSFS_BIT("replace_swsusp", SYSFS_RW, &toi_bkd.toi_action,
		  TOI_REPLACE_SWSUSP, 0),
	SYSFS_STRING("resume_commandline", SYSFS_RW,
		     toi_bkd.toi_nosave_commandline, COMMAND_LINE_SIZE, 0,
		     NULL),
	SYSFS_STRING("version", SYSFS_READONLY, TOI_CORE_VERSION, 0, 0, NULL),
	SYSFS_BIT("freezer_test", SYSFS_RW, &toi_bkd.toi_action,
		  TOI_FREEZER_TEST, 0),
	SYSFS_BIT("test_bio", SYSFS_RW, &toi_bkd.toi_action, TOI_TEST_BIO, 0),
	SYSFS_BIT("test_filter_speed", SYSFS_RW, &toi_bkd.toi_action,
		  TOI_TEST_FILTER_SPEED, 0),
	SYSFS_BIT("no_pageset2", SYSFS_RW, &toi_bkd.toi_action,
		  TOI_NO_PAGESET2, 0),
	SYSFS_BIT("no_pageset2_if_unneeded", SYSFS_RW, &toi_bkd.toi_action,
		  TOI_NO_PS2_IF_UNNEEDED, 0),
	SYSFS_BIT("late_cpu_hotplug", SYSFS_RW, &toi_bkd.toi_action,
		  TOI_LATE_CPU_HOTPLUG, 0),
	SYSFS_STRING("binary_signature", SYSFS_READONLY,
		     tuxonice_signature, 9, 0, NULL),
	SYSFS_INT("max_workers", SYSFS_RW, &toi_max_workers, 0, NR_CPUS, 0,
		  NULL),
#ifdef CONFIG_KGDB
	SYSFS_BIT("post_resume_breakpoint", SYSFS_RW, &toi_bkd.toi_action,
		  TOI_POST_RESUME_BREAKPOINT, 0),
#endif
	SYSFS_BIT("no_readahead", SYSFS_RW, &toi_bkd.toi_action,
		  TOI_NO_READAHEAD, 0),
#ifdef CONFIG_TOI_KEEP_IMAGE
	SYSFS_BIT("keep_image", SYSFS_RW, &toi_bkd.toi_action, TOI_KEEP_IMAGE,
		  0),
#endif
};

static struct toi_core_fns my_fns = {
	.get_nonconflicting_page = __toi_get_nonconflicting_page,
	.post_context_save = __toi_post_context_save,
	.try_hibernate = toi_try_hibernate,
	.try_resume = toi_sys_power_disk_try_resume,
};

/* 
                                              
  
                                                                            
                                                                           
                                                                    
  */
static __init int core_load(void)
{
	int i, numfiles = sizeof(sysfs_params) / sizeof(struct toi_sysfs_data);

	printk(KERN_INFO "TuxOnIce " TOI_CORE_VERSION " (http://tuxonice.net)\n");

	if (toi_sysfs_init())
		return 1;

	for (i = 0; i < numfiles; i++)
		toi_register_sysfs_file(tuxonice_kobj, &sysfs_params[i]);

	toi_core_fns = &my_fns;

	if (toi_alloc_init())
		return 1;
	if (toi_checksum_init())
		return 1;
	if (toi_usm_init())
		return 1;
	if (toi_ui_init())
		return 1;
	if (toi_poweroff_init())
		return 1;
	if (toi_cluster_init())
		return 1;

	return 0;
}

#ifdef MODULE
/* 
                                                
  */
static __exit void core_unload(void)
{
	int i, numfiles = sizeof(sysfs_params) / sizeof(struct toi_sysfs_data);

	toi_alloc_exit();
	toi_checksum_exit();
	toi_poweroff_exit();
	toi_ui_exit();
	toi_usm_exit();
	toi_cluster_exit();

	for (i = 0; i < numfiles; i++)
		toi_unregister_sysfs_file(tuxonice_kobj, &sysfs_params[i]);

	toi_core_fns = NULL;

	toi_sysfs_exit();
}

MODULE_LICENSE("GPL");
module_init(core_load);
module_exit(core_unload);
#else
late_initcall(core_load);
#endif
