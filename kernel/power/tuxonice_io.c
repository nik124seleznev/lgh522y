/*
 * kernel/power/tuxonice_io.c
 *
 * Copyright (C) 1998-2001 Gabor Kuti <seasons@fornax.hu>
 * Copyright (C) 1998,2001,2002 Pavel Machek <pavel@suse.cz>
 * Copyright (C) 2002-2003 Florent Chabaud <fchabaud@free.fr>
 * Copyright (C) 2002-2010 Nigel Cunningham (nigel at tuxonice net)
 *
 * This file is released under the GPLv2.
 *
 * It contains high level IO routines for hibernating.
 *
 */

#include <linux/suspend.h>
#include <linux/version.h>
#include <linux/utsname.h>
#include <linux/mount.h>
#include <linux/highmem.h>
#include <linux/kthread.h>
#include <linux/cpu.h>
#include <linux/fs_struct.h>
#include <linux/bio.h>
#include <linux/fs_uuid.h>
#include <asm/tlbflush.h>

#include "tuxonice.h"
#include "tuxonice_modules.h"
#include "tuxonice_pageflags.h"
#include "tuxonice_io.h"
#include "tuxonice_ui.h"
#include "tuxonice_storage.h"
#include "tuxonice_prepare_image.h"
#include "tuxonice_extent.h"
#include "tuxonice_sysfs.h"
#include "tuxonice_builtin.h"
#include "tuxonice_checksum.h"
#include "tuxonice_alloc.h"
char alt_resume_param[256];

/*                                          */
static int toi_image_header_version;

#define read_if_version(VERS, VAR, DESC, ERR_ACT) do {					\
	if (likely(toi_image_header_version >= VERS))				\
		if (toiActiveAllocator->rw_header_chunk(READ, NULL,		\
					(char *) &VAR, sizeof(VAR))) {		\
			abort_hibernate(TOI_FAILED_IO, "Failed to read DESC.");	\
			ERR_ACT;					\
		}								\
} while (0)									\

/*                                                              */
static int io_write, io_finish_at, io_base, io_barmax, io_pageset, io_result;
static int io_index, io_nextupdate, io_pc, io_pc_step;
static DEFINE_MUTEX(io_mutex);
static DEFINE_PER_CPU(struct page *, last_sought);
static DEFINE_PER_CPU(struct page *, last_high_page);
static DEFINE_PER_CPU(char *, checksum_locn);
static DEFINE_PER_CPU(struct pbe *, last_low_page);
static atomic_t io_count;
atomic_t toi_io_workers;
EXPORT_SYMBOL_GPL(toi_io_workers);

static int using_flusher;

DECLARE_WAIT_QUEUE_HEAD(toi_io_queue_flusher);
EXPORT_SYMBOL_GPL(toi_io_queue_flusher);

int toi_bio_queue_flusher_should_finish;
EXPORT_SYMBOL_GPL(toi_bio_queue_flusher_should_finish);

int toi_max_workers;

static char *image_version_error = "The image header version is newer than "
    "this kernel supports.";

struct toi_module_ops *first_filter;

static atomic_t toi_num_other_threads;
static DECLARE_WAIT_QUEUE_HEAD(toi_worker_wait_queue);
enum toi_worker_commands {
	TOI_IO_WORKER_STOP,
	TOI_IO_WORKER_RUN,
	TOI_IO_WORKER_EXIT
};
static enum toi_worker_commands toi_worker_command;

/* 
                                                                     
  
                                                         
  */
int toi_attempt_to_parse_resume_device(int quiet)
{
	struct list_head *Allocator;
	struct toi_module_ops *thisAllocator;
	int result, returning = 0;

	if (toi_activate_storage(0))
		return 0;

	toiActiveAllocator = NULL;
	clear_toi_state(TOI_RESUME_DEVICE_OK);
	clear_toi_state(TOI_CAN_RESUME);
	clear_result_state(TOI_ABORTED);

	if (!toiNumAllocators) {
		if (!quiet)
			printk(KERN_INFO "TuxOnIce: No storage allocators have "
			       "been registered. Hibernating will be " "disabled.\n");
		goto cleanup;
	}

	list_for_each(Allocator, &toiAllocators) {
		thisAllocator = list_entry(Allocator, struct toi_module_ops, type_list);

		/*
                                                         
                                                    
   */
		if (!thisAllocator->enabled)
			continue;

		result = thisAllocator->parse_sig_location(resume_file, (toiNumAllocators == 1),
							   quiet);

		switch (result) {
		case -EINVAL:
			/*                                    
                                            */
			goto cleanup;

		case 0:
			/*                               */
			toiActiveAllocator = thisAllocator;

			set_toi_state(TOI_RESUME_DEVICE_OK);
			set_toi_state(TOI_CAN_RESUME);
			returning = 1;
			goto cleanup;
		}
	}
	if (!quiet)
		printk(KERN_INFO "TuxOnIce: No matching enabled allocator "
		       "found. Resuming disabled.\n");
 cleanup:
	toi_deactivate_storage(0);
	return returning;
}
EXPORT_SYMBOL_GPL(toi_attempt_to_parse_resume_device);

void attempt_to_parse_resume_device2(void)
{
	toi_prepare_usm();
	toi_attempt_to_parse_resume_device(0);
	toi_cleanup_usm();
}
EXPORT_SYMBOL_GPL(attempt_to_parse_resume_device2);

void save_restore_alt_param(int replace, int quiet)
{
	static char resume_param_save[255];
	static unsigned long toi_state_save;

	if (replace) {
		toi_state_save = toi_state;
		strncpy(resume_param_save, resume_file, sizeof(resume_param_save) - 1);
		strcpy(resume_file, alt_resume_param);
	} else {
		strcpy(resume_file, resume_param_save);
		toi_state = toi_state_save;
	}
	toi_attempt_to_parse_resume_device(quiet);
}

void attempt_to_parse_alt_resume_param(void)
{
	int ok = 0;

	/*                                                    */
	if (!strlen(alt_resume_param))
		return;

	printk(KERN_INFO "=== Trying Poweroff Resume2 ===\n");
	save_restore_alt_param(SAVE, NOQUIET);
	if (test_toi_state(TOI_CAN_RESUME))
		ok = 1;

	printk(KERN_INFO "=== Done ===\n");
	save_restore_alt_param(RESTORE, QUIET);

	/*                             */
	if (ok)
		return;

	printk(KERN_INFO "Can't resume from that location; clearing " "alt_resume_param.\n");
	alt_resume_param[0] = '\0';
}

/* 
                                                                         
  
                                                                  
                                                              
                                                           
  */
static void noresume_reset_modules(void)
{
	struct toi_module_ops *this_filter;

	list_for_each_entry(this_filter, &toi_filters, type_list)
	    if (this_filter->noresume_reset)
		this_filter->noresume_reset();

	if (toiActiveAllocator && toiActiveAllocator->noresume_reset)
		toiActiveAllocator->noresume_reset();
}

/* 
                                                        
                                                          
  */
static int fill_toi_header(struct toi_header *sh)
{
	int i, error;

	error = init_header((struct swsusp_info *)sh);
	if (error)
		return error;

	sh->pagedir = pagedir1;
	sh->pageset_2_size = pagedir2.size;
	sh->param0 = toi_result;
	sh->param1 = toi_bkd.toi_action;
	sh->param2 = toi_bkd.toi_debug_state;
	sh->param3 = toi_bkd.toi_default_console_level;
	sh->root_fs = current->fs->root.mnt->mnt_sb->s_dev;
	for (i = 0; i < 4; i++)
		sh->io_time[i / 2][i % 2] = toi_bkd.toi_io_time[i / 2][i % 2];
	sh->bkd = boot_kernel_data_buffer;
	return 0;
}

/* 
                                       
                                                    
                                                
  
                                                                              
        
  */
static int rw_init_modules(int rw, int which)
{
	struct toi_module_ops *this_module;
	/*                              */
	list_for_each_entry(this_module, &toi_filters, type_list) {
		if (!this_module->enabled)
			continue;
		if (this_module->rw_init && this_module->rw_init(rw, which)) {
			abort_hibernate(TOI_FAILED_MODULE_INIT,
					"Failed to initialize the %s filter.", this_module->name);
			return 1;
		}
	}

	/*                      */
	if (toiActiveAllocator->rw_init(rw, which)) {
		abort_hibernate(TOI_FAILED_MODULE_INIT, "Failed to initialise the allocator.");
		return 1;
	}

	/*                          */
	list_for_each_entry(this_module, &toi_modules, module_list) {
		if (!this_module->enabled ||
		    this_module->type == FILTER_MODULE || this_module->type == WRITER_MODULE)
			continue;
		if (this_module->rw_init && this_module->rw_init(rw, which)) {
			set_abort_result(TOI_FAILED_MODULE_INIT);
			printk(KERN_INFO "Setting aborted flag due to module " "init failure.\n");
			return 1;
		}
	}

	return 0;
}

/* 
                                       
                                                   
  
                                                              
                               
  */
static int rw_cleanup_modules(int rw)
{
	struct toi_module_ops *this_module;
	int result = 0;

	/*                       */
	list_for_each_entry(this_module, &toi_modules, module_list) {
		if (!this_module->enabled ||
		    this_module->type == FILTER_MODULE || this_module->type == WRITER_MODULE)
			continue;
		if (this_module->rw_cleanup)
			result |= this_module->rw_cleanup(rw);
	}

	/*                        */
	list_for_each_entry(this_module, &toi_filters, type_list) {
		if (!this_module->enabled)
			continue;
		if (this_module->rw_cleanup)
			result |= this_module->rw_cleanup(rw);
	}

	result |= toiActiveAllocator->rw_cleanup(rw);

	return result;
}

static struct page *copy_page_from_orig_page(struct page *orig_page, int is_high)
{
	int index, min, max;
	struct page *high_page = NULL,
	    **my_last_high_page = &__get_cpu_var(last_high_page),
	    **my_last_sought = &__get_cpu_var(last_sought);
	struct pbe *this, **my_last_low_page = &__get_cpu_var(last_low_page);
	void *compare;

	if (is_high) {
		if (*my_last_sought && *my_last_high_page && *my_last_sought < orig_page)
			high_page = *my_last_high_page;
		else
			high_page = (struct page *)restore_highmem_pblist;
		this = (struct pbe *)kmap(high_page);
		compare = orig_page;
	} else {
		if (*my_last_sought && *my_last_low_page && *my_last_sought < orig_page)
			this = *my_last_low_page;
		else
			this = restore_pblist;
		compare = page_address(orig_page);
	}

	*my_last_sought = orig_page;

	/*                            */
	while (this[PBES_PER_PAGE - 1].next && this[PBES_PER_PAGE - 1].orig_address < compare) {
		if (is_high) {
			struct page *next_high_page = (struct page *)
			    this[PBES_PER_PAGE - 1].next;
			kunmap(high_page);
			this = kmap(next_high_page);
			high_page = next_high_page;
		} else
			this = this[PBES_PER_PAGE - 1].next;
	}

	/*                                    */
	min = 0;
	max = PBES_PER_PAGE;
	index = PBES_PER_PAGE / 2;
	while (max - min) {
		if (!this[index].orig_address || this[index].orig_address > compare)
			max = index;
		else if (this[index].orig_address == compare) {
			if (is_high) {
				struct page *page = this[index].address;
				*my_last_high_page = high_page;
				kunmap(high_page);
				return page;
			}
			*my_last_low_page = this;
			return virt_to_page(this[index].address);
		} else
			min = index;
		index = ((max + min) / 2);
	};

	if (is_high)
		kunmap(high_page);

	abort_hibernate(TOI_FAILED_IO, "Failed to get destination page for"
			" orig page %p. This[min].orig_address=%p.\n", orig_page,
			this[index].orig_address);
	return NULL;
}

/* 
                                                     
                                                              
                                                      
                                                                             
  
                                                                              
         
  */
static int write_next_page(unsigned long *data_pfn, int *my_io_index, unsigned long *write_pfn)
{
	struct page *page;
	char **my_checksum_locn = &__get_cpu_var(checksum_locn);
	int result = 0, was_present;

	*data_pfn = memory_bm_next_pfn(io_map);

	/*                                            */
	if (*data_pfn == BM_END_OF_MAP) {
		if (atomic_read(&io_count)) {
			printk(KERN_INFO "Ran out of pfns but io_count is "
			       "still %d.\n", atomic_read(&io_count));
			BUG();
		}
		mutex_unlock(&io_mutex);
		return -ENODATA;
	}

	*my_io_index = io_finish_at - atomic_sub_return(1, &io_count);

	memory_bm_clear_bit(io_map, *data_pfn);
	page = pfn_to_page(*data_pfn);

	was_present = kernel_page_present(page);
	if (!was_present)
		kernel_map_pages(page, 1, 1);

	if (io_pageset == 1)
		*write_pfn = memory_bm_next_pfn(pageset1_map);
	else {
		*write_pfn = *data_pfn;
		*my_checksum_locn = tuxonice_get_next_checksum();
	}

	toi_message(TOI_IO, TOI_VERBOSE, 0, "Write %d:%ld.", *my_io_index, *write_pfn);

	mutex_unlock(&io_mutex);

	if (io_pageset == 2 && tuxonice_calc_checksum(page, *my_checksum_locn))
		return 1;

	result = first_filter->write_page(*write_pfn, TOI_PAGE, page, PAGE_SIZE);

	if (!was_present)
		kernel_map_pages(page, 1, 0);

	return result;
}

/* 
                                                   
                                                      
                                                 
  
                                                                           
                                                                             
                                                                   
                                                                           
                                                                           
                
  */

static int read_next_page(int *my_io_index, unsigned long *write_pfn, struct page *buffer)
{
	unsigned int buf_size = PAGE_SIZE;
	unsigned long left = atomic_read(&io_count);

	if (!left)
		return -ENODATA;

	/*                                                   */
	*my_io_index = io_finish_at - atomic_sub_return(1, &io_count);

	mutex_unlock(&io_mutex);

	/*
                                                        
                                                        
                                                  
  */
	if (unlikely(test_toi_state(TOI_STOP_RESUME))) {
		atomic_dec(&toi_io_workers);
		if (!atomic_read(&toi_io_workers)) {
			/*
                                             
                                      
    */
			rw_cleanup_modules(READ);
			set_toi_state(TOI_IO_STOPPED);
		}
		while (1)
			schedule();
	}

	/*
                                            
                                    
  */
	return first_filter->read_page(write_pfn, TOI_PAGE, buffer, &buf_size);
}

static void use_read_page(unsigned long write_pfn, struct page *buffer)
{
	struct page *final_page = pfn_to_page(write_pfn), *copy_page = final_page;
	char *virt, *buffer_virt;
	int was_present, cpu = smp_processor_id();
	unsigned long idx = 0;

	if (io_pageset == 1 && (!pageset1_copy_map ||
				!memory_bm_test_bit_index(pageset1_copy_map, write_pfn, cpu))) {
		int is_high = PageHighMem(final_page);
		copy_page =
		    copy_page_from_orig_page(is_high ? (void *)write_pfn : final_page, is_high);
	}

	if (!memory_bm_test_bit_index(io_map, write_pfn, cpu)) {
		toi_message(TOI_IO, TOI_VERBOSE, 0, "Discard %ld.", write_pfn);
		mutex_lock(&io_mutex);
		idx = atomic_add_return(1, &io_count);
		mutex_unlock(&io_mutex);
		return;
	}

	virt = kmap(copy_page);
	buffer_virt = kmap(buffer);
	was_present = kernel_page_present(copy_page);
	if (!was_present)
		kernel_map_pages(copy_page, 1, 1);
	memcpy(virt, buffer_virt, PAGE_SIZE);
	if (!was_present)
		kernel_map_pages(copy_page, 1, 0);
	kunmap(copy_page);
	kunmap(buffer);
	memory_bm_clear_bit_index(io_map, write_pfn, cpu);
	toi_message(TOI_IO, TOI_VERBOSE, 0, "Read %d:%ld", idx, write_pfn);
}

static unsigned long status_update(int writing, unsigned long done, unsigned long ticks)
{
	int cs_index = writing ? 0 : 1;
	unsigned long ticks_so_far = toi_bkd.toi_io_time[cs_index][1] + ticks;
	unsigned long msec = jiffies_to_msecs(abs(ticks_so_far));
	unsigned long pgs_per_s, estimate = 0, pages_left;

	if (msec) {
		pages_left = io_barmax - done;
		pgs_per_s = 1000 * done / msec;
		if (pgs_per_s)
			estimate = DIV_ROUND_UP(pages_left, pgs_per_s);
	}

	if (estimate && ticks > HZ / 2)
		return toi_update_status(done, io_barmax,
					 " %d/%d MB (%lu sec left)",
					 MB(done + 1), MB(io_barmax), estimate);

	return toi_update_status(done, io_barmax, " %d/%d MB", MB(done + 1), MB(io_barmax));
}

/* 
                                                 
  
                                                                               
                                 
                                                                       
                                                                         
                                                    
  */
static int worker_rw_loop(void *data)
{
	unsigned long data_pfn, write_pfn, next_jiffies = jiffies + HZ / 4,
	    jif_index = 1, start_time = jiffies, thread_num;
	int result = 0, my_io_index = 0, last_worker;
	struct page *buffer = toi_alloc_page(28, TOI_ATOMIC_GFP);
	cpumask_var_t orig_mask;

	if (!alloc_cpumask_var(&orig_mask, GFP_KERNEL)) {
		printk(KERN_EMERG "Failed to allocate cpumask for TuxOnIce I/O thread %ld.\n",
		       (unsigned long)data);
		return -ENOMEM;
	}

	cpumask_copy(orig_mask, tsk_cpus_allowed(current));

	current->flags |= PF_NOFREEZE;

 top:
	mutex_lock(&io_mutex);
	thread_num = atomic_read(&toi_io_workers);

	cpumask_copy(tsk_cpus_allowed(current), orig_mask);
	schedule();

	atomic_inc(&toi_io_workers);

	while (atomic_read(&io_count) >= atomic_read(&toi_io_workers) &&
	       !(io_write && test_result_state(TOI_ABORTED)) &&
	       toi_worker_command == TOI_IO_WORKER_RUN) {
		if (!thread_num && jiffies > next_jiffies) {
			next_jiffies += HZ / 4;
			if (toiActiveAllocator->update_throughput_throttle)
				toiActiveAllocator->update_throughput_throttle(jif_index);
			jif_index++;
		}

		/*
                                                              
                                                             
                                                             
                                               
   */
		if (io_write)
			result = write_next_page(&data_pfn, &my_io_index, &write_pfn);
		else		/*         */
			result = read_next_page(&my_io_index, &write_pfn, buffer);

		if (result) {
			mutex_lock(&io_mutex);
			/*                */
			if (result == -ENODATA) {
				toi_message(TOI_IO, TOI_VERBOSE, 0,
					    "Thread %d has no more work.", smp_processor_id());
				break;
			}

			io_result = result;

			if (io_write) {
				printk(KERN_INFO "Write chunk returned %d.\n", result);
				abort_hibernate(TOI_FAILED_IO,
						"Failed to write a chunk of the " "image.");
				break;
			}

			if (io_pageset == 1) {
				printk(KERN_ERR "\nBreaking out of I/O loop "
				       "because of result code %d.\n", result);
				break;
			}
			panic("Read chunk returned (%d)", result);
		}

		/*
                                                     
                                                          
   */
		if (!io_write) {
			if (!PageResave(pfn_to_page(write_pfn)))
				use_read_page(write_pfn, buffer);
			else {
				mutex_lock(&io_mutex);
				toi_message(TOI_IO, TOI_VERBOSE, 0, "Resaved %ld.", write_pfn);
				atomic_inc(&io_count);
				mutex_unlock(&io_mutex);
			}
		}

		if (!thread_num) {
			if (my_io_index + io_base > io_nextupdate)
				io_nextupdate = status_update(io_write,
							      my_io_index + io_base,
							      jiffies - start_time);

			if (my_io_index > io_pc) {
				printk(KERN_CONT "...%d%%", 20 * io_pc_step);
				io_pc_step++;
				io_pc = io_finish_at * io_pc_step / 5;
			}
		}

		toi_cond_pause(0, NULL);

		/*
                                                              
                                                             
                            
    
                                                              
                                                            
                                                              
   */

		mutex_lock(&io_mutex);
	}

	last_worker = atomic_dec_and_test(&toi_io_workers);
	toi_message(TOI_IO, TOI_VERBOSE, 0, "%d workers left.", atomic_read(&toi_io_workers));
	mutex_unlock(&io_mutex);

	if ((unsigned long)data && toi_worker_command != TOI_IO_WORKER_EXIT) {
		/*                                                           */
		if (last_worker && using_flusher) {
			toiActiveAllocator->finish_all_io();
		}
		/*                                                  */
		wait_event(toi_worker_wait_queue, toi_worker_command != TOI_IO_WORKER_RUN);
		/*                                      */
		wait_event(toi_worker_wait_queue, toi_worker_command != TOI_IO_WORKER_STOP);
		if (toi_worker_command == TOI_IO_WORKER_RUN)
			goto top;
	}

	if (thread_num)
		atomic_dec(&toi_num_other_threads);

	toi_message(TOI_IO, TOI_LOW, 0, "Thread %d exiting.", thread_num);
	toi__free_page(28, buffer);
	free_cpumask_var(orig_mask);

	return result;
}

int toi_start_other_threads(void)
{
	int cpu;
	struct task_struct *p;
	int to_start = (toi_max_workers ? toi_max_workers : num_online_cpus()) - 1;
	unsigned long num_started = 0;

	if (test_action_state(TOI_NO_MULTITHREADED_IO))
		return 0;

	toi_worker_command = TOI_IO_WORKER_STOP;

	for_each_online_cpu(cpu) {
		if (num_started == to_start)
			break;

		if (cpu == smp_processor_id())
			continue;

		p = kthread_create_on_node(worker_rw_loop, (void *)num_started + 1,
					   cpu_to_node(cpu), "ktoi_io/%d", cpu);
		if (IS_ERR(p)) {
			printk(KERN_ERR "ktoi_io for %i failed\n", cpu);
			continue;
		}
		kthread_bind(p, cpu);
		p->flags |= PF_MEMALLOC;
		wake_up_process(p);
		num_started++;
		atomic_inc(&toi_num_other_threads);
	}

	hib_warn("Started %ld threads.", num_started);

	toi_message(TOI_IO, TOI_LOW, 0, "Started %d threads.", num_started);
	return num_started;
}

void toi_stop_other_threads(void)
{
	toi_message(TOI_IO, TOI_LOW, 0, "Stopping other threads.");
	toi_worker_command = TOI_IO_WORKER_EXIT;
	wake_up(&toi_worker_wait_queue);
}

/* 
                                                                    
  
                                                                              
  */
static int do_rw_loop(int write, int finish_at, struct memory_bitmap *pageflags,
		      int base, int barmax, int pageset)
{
	int index = 0, cpu, result = 0, workers_started;
	unsigned long pfn;

	first_filter = toi_get_next_filter(NULL);

	if (!finish_at)
		return 0;

	io_write = write;
	io_finish_at = finish_at;
	io_base = base;
	io_barmax = barmax;
	io_pageset = pageset;
	io_index = 0;
	io_pc = io_finish_at / 5;
	io_pc_step = 1;
	io_result = 0;
	io_nextupdate = base + 1;
	toi_bio_queue_flusher_should_finish = 0;

	for_each_online_cpu(cpu) {
		per_cpu(last_sought, cpu) = NULL;
		per_cpu(last_low_page, cpu) = NULL;
		per_cpu(last_high_page, cpu) = NULL;
	}

	/*                       */
	memory_bm_clear(io_map);

	/*                                     */
	memory_bm_position_reset(pageflags);

	pfn = memory_bm_next_pfn(pageflags);

	while (pfn != BM_END_OF_MAP && index < finish_at) {
		memory_bm_set_bit(io_map, pfn);
		pfn = memory_bm_next_pfn(pageflags);
		index++;
	}

	BUG_ON(index < finish_at);

	atomic_set(&io_count, finish_at);

	memory_bm_position_reset(pageset1_map);

	mutex_lock(&io_mutex);

	clear_toi_state(TOI_IO_STOPPED);

	using_flusher = (atomic_read(&toi_num_other_threads) &&
			 toiActiveAllocator->io_flusher &&
			 !test_action_state(TOI_NO_FLUSHER_THREAD));

	workers_started = atomic_read(&toi_num_other_threads);

	memory_bm_set_iterators(io_map, atomic_read(&toi_num_other_threads) + 1);
	memory_bm_position_reset(io_map);

	memory_bm_set_iterators(pageset1_copy_map, atomic_read(&toi_num_other_threads) + 1);
	memory_bm_position_reset(pageset1_copy_map);

	toi_worker_command = TOI_IO_WORKER_RUN;
	wake_up(&toi_worker_wait_queue);

	mutex_unlock(&io_mutex);

	if (using_flusher)
		result = toiActiveAllocator->io_flusher(write);
	else
		worker_rw_loop(NULL);

	while (atomic_read(&toi_io_workers))
		schedule();

	printk(KERN_CONT "\n");

	toi_worker_command = TOI_IO_WORKER_STOP;
	wake_up(&toi_worker_wait_queue);

	if (unlikely(test_toi_state(TOI_STOP_RESUME))) {
		if (!atomic_read(&toi_io_workers)) {
			rw_cleanup_modules(READ);
			set_toi_state(TOI_IO_STOPPED);
		}
		while (1)
			schedule();
	}
	set_toi_state(TOI_IO_STOPPED);

	if (!io_result && !result && !test_result_state(TOI_ABORTED)) {
		unsigned long next;

		toi_update_status(io_base + io_finish_at, io_barmax,
				  " %d/%d MB ", MB(io_base + io_finish_at), MB(io_barmax));

		memory_bm_position_reset(io_map);
		next = memory_bm_next_pfn(io_map);
		if (next != BM_END_OF_MAP) {
			printk(KERN_INFO "Finished I/O loop but still work to "
			       "do?\nFinish at = %d. io_count = %d.\n",
			       finish_at, atomic_read(&io_count));
			printk(KERN_INFO "I/O bitmap still records work to do." "%ld.\n", next);
			hib_err("Finish at = %d. io_count = %d. next: %lu\n", finish_at, atomic_read(&io_count), next);
#ifdef CONFIG_TOI_FIXUP
			HIB_SHOW_MEMINFO();
#endif
			BUG();
			do {
				cpu_relax();
			} while (0);
		}
	}

	return io_result ? io_result : result;
}

/* 
                                           
                                    
  
           
                                    
  */
int write_pageset(struct pagedir *pagedir)
{
	int finish_at, base = 0;
	int barmax = pagedir1.size + pagedir2.size;
	long error = 0;
	struct memory_bitmap *pageflags;
	unsigned long start_time, end_time;

	/*
                                                            
                                                          
                                                         
  */
	finish_at = pagedir->size;

	if (pagedir->id == 1) {
		hib_log("start to writing kernel & process data...\n");
		toi_prepare_status(DONT_CLEAR_BAR, "Writing kernel & process data...");
		base = pagedir2.size;
		if (test_action_state(TOI_TEST_FILTER_SPEED) || test_action_state(TOI_TEST_BIO))
			pageflags = pageset1_map;
		else
			pageflags = pageset1_copy_map;
	} else {
		hib_log("start to writing caches...\n");
		toi_prepare_status(DONT_CLEAR_BAR, "Writing caches...");
		pageflags = pageset2_map;
	}

	start_time = jiffies;

	if (rw_init_modules(1, pagedir->id)) {
		abort_hibernate(TOI_FAILED_MODULE_INIT,
				"Failed to initialise modules for writing.");
		error = 1;
	}

	if (!error)
		error = do_rw_loop(1, finish_at, pageflags, base, barmax, pagedir->id);

	if (rw_cleanup_modules(WRITE) && !error) {
		abort_hibernate(TOI_FAILED_MODULE_CLEANUP, "Failed to cleanup after writing.");
		error = 1;
	}

	end_time = jiffies;

	if ((end_time - start_time) && (!test_result_state(TOI_ABORTED))) {
		toi_bkd.toi_io_time[0][0] += finish_at,
		    toi_bkd.toi_io_time[0][1] += (end_time - start_time);
	}

	hib_log("@line:%d return value(%ld)\n", __LINE__, error);
	return error;
}

/* 
                                                                
                              
                                                              
                      
  
           
                                    
  */
static int read_pageset(struct pagedir *pagedir, int overwrittenpagesonly)
{
	int result = 0, base = 0;
	int finish_at = pagedir->size;
	int barmax = pagedir1.size + pagedir2.size;
	struct memory_bitmap *pageflags;
	unsigned long start_time, end_time;

	if (pagedir->id == 1) {
		toi_prepare_status(DONT_CLEAR_BAR, "Reading kernel & process data...");
		pageflags = pageset1_map;
	} else {
		toi_prepare_status(DONT_CLEAR_BAR, "Reading caches...");
		if (overwrittenpagesonly) {
			barmax = min(pagedir1.size, pagedir2.size);
			finish_at = min(pagedir1.size, pagedir2.size);
		} else
			base = pagedir1.size;
		pageflags = pageset2_map;
	}

	start_time = jiffies;

	if (rw_init_modules(0, pagedir->id)) {
		toiActiveAllocator->remove_image();
		result = 1;
	} else
		result = do_rw_loop(0, finish_at, pageflags, base, barmax, pagedir->id);

	if (rw_cleanup_modules(READ) && !result) {
		abort_hibernate(TOI_FAILED_MODULE_CLEANUP, "Failed to cleanup after reading.");
		result = 1;
	}

	/*            */
	end_time = jiffies;

	if ((end_time - start_time) && (!test_result_state(TOI_ABORTED))) {
		toi_bkd.toi_io_time[1][0] += finish_at,
		    toi_bkd.toi_io_time[1][1] += (end_time - start_time);
	}

	return result;
}

/* 
                                                         
  
                                                                   
               
                                          
  */
static int write_module_configs(void)
{
	struct toi_module_ops *this_module;
	char *buffer = (char *)toi_get_zeroed_page(22, TOI_ATOMIC_GFP);
	int len, index = 1;
	struct toi_module_header toi_module_header;

	if (!buffer) {
		printk(KERN_INFO "Failed to allocate a buffer for saving "
		       "module configuration info.\n");
		return -ENOMEM;
	}

	/*
                                                               
                                                               
                                                                
  */

	/*                                         */
	list_for_each_entry(this_module, &toi_modules, module_list) {
		if (!this_module->enabled || !this_module->storage_needed ||
		    (this_module->type == WRITER_MODULE && toiActiveAllocator != this_module))
			continue;

		/*                              */
		len = 0;
		if (this_module->save_config_info)
			len = this_module->save_config_info(buffer);

		/*                                */
		toi_module_header.enabled = this_module->enabled;
		toi_module_header.type = this_module->type;
		toi_module_header.index = index++;
		strncpy(toi_module_header.name, this_module->name,
			sizeof(toi_module_header.name) - 1);
		toiActiveAllocator->rw_header_chunk(WRITE,
						    this_module,
						    (char *)&toi_module_header,
						    sizeof(toi_module_header));

		/*                                                 */
		toiActiveAllocator->rw_header_chunk(WRITE, this_module, (char *)&len, sizeof(int));
		if (len)
			toiActiveAllocator->rw_header_chunk(WRITE, this_module, buffer, len);
	}

	/*                                            */
	toi_module_header.name[0] = '\0';
	toiActiveAllocator->rw_header_chunk(WRITE, NULL,
					    (char *)&toi_module_header, sizeof(toi_module_header));

	toi_free_page(22, (unsigned long)buffer);
	return 0;
}

/* 
                                                         
  
                                                                  
                            
  
               
                                          
  */
static int read_one_module_config(struct toi_module_header *header)
{
	struct toi_module_ops *this_module;
	int result, len;
	char *buffer;

	/*                 */
	this_module = toi_find_module_given_name(header->name);

	if (!this_module) {
		if (header->enabled) {
			toi_early_boot_message(1, TOI_CONTINUE_REQ,
					       "It looks like we need module %s for reading "
					       "the image but it hasn't been registered.\n",
					       header->name);
			if (!(test_toi_state(TOI_CONTINUE_REQ)))
				return -EINVAL;
		} else
			printk(KERN_INFO "Module %s configuration data found, "
			       "but the module hasn't registered. Looks like "
			       "it was disabled, so we're ignoring its data.", header->name);
	}

	/*                                     */
	result = toiActiveAllocator->rw_header_chunk(READ, NULL, (char *)&len, sizeof(int));
	if (result) {
		printk(KERN_ERR "Failed to read the length of the module %s's"
		       " configuration data.\n", header->name);
		return -EINVAL;
	}

	/*                                                        */
	if (!len)
		return 0;

	buffer = (char *)toi_get_zeroed_page(23, TOI_ATOMIC_GFP);

	if (!buffer) {
		printk(KERN_ERR "Failed to allocate a buffer for reloading "
		       "module configuration info.\n");
		return -ENOMEM;
	}

	toiActiveAllocator->rw_header_chunk(READ, NULL, buffer, len);

	if (!this_module)
		goto out;

	if (!this_module->save_config_info)
		printk(KERN_ERR "Huh? Module %s appears to have a "
		       "save_config_info, but not a load_config_info "
		       "function!\n", this_module->name);
	else
		this_module->load_config_info(buffer, len);

	/*
                                                                      
                                                                    
                                                             
                                                                       
                       
  */

	toi_move_module_tail(this_module);

	this_module->enabled = header->enabled;

 out:
	toi_free_page(23, (unsigned long)buffer);
	return 0;
}

/* 
                                                                            
  
               
                                    
  */
static int read_module_configs(void)
{
	int result = 0;
	struct toi_module_header toi_module_header;
	struct toi_module_ops *this_module;

	/*                                                                  
                                                                      
                               
  */
	list_for_each_entry(this_module, &toi_modules, module_list)
	    this_module->enabled = 0;

	/*                             */
	result = toiActiveAllocator->rw_header_chunk(READ, NULL,
						     (char *)&toi_module_header,
						     sizeof(toi_module_header));
	if (result) {
		printk(KERN_ERR "Failed to read the next module header.\n");
		return -EINVAL;
	}

	/*                                         */
	while (toi_module_header.name[0]) {
		result = read_one_module_config(&toi_module_header);

		if (result)
			return -EINVAL;

		/*                            */
		result = toiActiveAllocator->rw_header_chunk(READ, NULL,
							     (char *)&toi_module_header,
							     sizeof(toi_module_header));

		if (result) {
			printk(KERN_ERR "Failed to read the next module " "header.\n");
			return -EINVAL;
		}
	}

	return 0;
}

static inline int save_fs_info(struct fs_info *fs, struct block_device *bdev)
{
	return (!fs || IS_ERR(fs) || !fs->last_mount_size) ? 0 : 1;
}

int fs_info_space_needed(void)
{
	const struct super_block *sb;
	int result = sizeof(int);

	list_for_each_entry(sb, &super_blocks, s_list) {
		struct fs_info *fs;

		if (!sb->s_bdev)
			continue;

		fs = fs_info_from_block_dev(sb->s_bdev);
		if (save_fs_info(fs, sb->s_bdev))
			result += 16 + sizeof(dev_t) + sizeof(int) + fs->last_mount_size;
		free_fs_info(fs);
	}
	return result;
}

static int fs_info_num_to_save(void)
{
	const struct super_block *sb;
	int to_save = 0;

	list_for_each_entry(sb, &super_blocks, s_list) {
		struct fs_info *fs;

		if (!sb->s_bdev)
			continue;

		fs = fs_info_from_block_dev(sb->s_bdev);
		if (save_fs_info(fs, sb->s_bdev))
			to_save++;
		free_fs_info(fs);
	}

	return to_save;
}

static int fs_info_save(void)
{
	const struct super_block *sb;
	int to_save = fs_info_num_to_save();

	if (toiActiveAllocator->rw_header_chunk(WRITE, NULL, (char *)&to_save, sizeof(int))) {
		abort_hibernate(TOI_FAILED_IO, "Failed to write num fs_info" " to save.");
		return -EIO;
	}

	list_for_each_entry(sb, &super_blocks, s_list) {
		struct fs_info *fs;

		if (!sb->s_bdev)
			continue;

		fs = fs_info_from_block_dev(sb->s_bdev);
		if (save_fs_info(fs, sb->s_bdev)) {
			if (toiActiveAllocator->rw_header_chunk(WRITE, NULL, &fs->uuid[0], 16)) {
				abort_hibernate(TOI_FAILED_IO, "Failed to " "write uuid.");
				return -EIO;
			}
			if (toiActiveAllocator->rw_header_chunk(WRITE, NULL,
								(char *)&fs->dev_t,
								sizeof(dev_t))) {
				abort_hibernate(TOI_FAILED_IO, "Failed to " "write dev_t.");
				return -EIO;
			}
			if (toiActiveAllocator->rw_header_chunk(WRITE, NULL,
								(char *)&fs->last_mount_size,
								sizeof(int))) {
				abort_hibernate(TOI_FAILED_IO,
						"Failed to " "write last mount length.");
				return -EIO;
			}
			if (toiActiveAllocator->rw_header_chunk(WRITE, NULL,
								fs->last_mount,
								fs->last_mount_size)) {
				abort_hibernate(TOI_FAILED_IO, "Failed to " "write uuid.");
				return -EIO;
			}
		}
		free_fs_info(fs);
	}
	return 0;
}

static int fs_info_load_and_check_one(void)
{
	char uuid[16], *last_mount;
	int result = 0, ln;
	dev_t dev_t;
	struct block_device *dev;
	struct fs_info *fs_info, seek;

	if (toiActiveAllocator->rw_header_chunk(READ, NULL, uuid, 16)) {
		abort_hibernate(TOI_FAILED_IO, "Failed to read uuid.");
		return -EIO;
	}

	read_if_version(3, dev_t, "uuid dev_t field", return -EIO);

	if (toiActiveAllocator->rw_header_chunk(READ, NULL, (char *)&ln, sizeof(int))) {
		abort_hibernate(TOI_FAILED_IO, "Failed to read last mount size.");
		return -EIO;
	}

	last_mount = kzalloc(ln, GFP_KERNEL);

	if (!last_mount)
		return -ENOMEM;

	if (toiActiveAllocator->rw_header_chunk(READ, NULL, last_mount, ln)) {
		abort_hibernate(TOI_FAILED_IO, "Failed to read last mount timestamp.");
		result = -EIO;
		goto out_lmt;
	}

	strncpy((char *)&seek.uuid, uuid, 16);
	seek.dev_t = dev_t;
	seek.last_mount_size = ln;
	seek.last_mount = last_mount;
	dev_t = blk_lookup_fs_info(&seek);
	if (!dev_t)
		goto out_lmt;

	dev = toi_open_by_devnum(dev_t);

	fs_info = fs_info_from_block_dev(dev);
	if (fs_info && !IS_ERR(fs_info)) {
		if (ln != fs_info->last_mount_size) {
			printk(KERN_EMERG "Found matching uuid but last mount "
			       "time lengths differ?! "
			       "(%d vs %d).\n", ln, fs_info->last_mount_size);
			result = -EINVAL;
		} else {
			char buf[BDEVNAME_SIZE];
			result = !!memcmp(fs_info->last_mount, last_mount, ln);
			if (result)
				printk(KERN_EMERG "Last mount time for %s has "
				       "changed!\n", bdevname(dev, buf));
		}
	}
	toi_close_bdev(dev);
	free_fs_info(fs_info);
 out_lmt:
	kfree(last_mount);
	return result;
}

static int fs_info_load_and_check(void)
{
	int to_do, result = 0;

	if (toiActiveAllocator->rw_header_chunk(READ, NULL, (char *)&to_do, sizeof(int))) {
		abort_hibernate(TOI_FAILED_IO, "Failed to read num fs_info " "to load.");
		return -EIO;
	}

	while (to_do--)
		result |= fs_info_load_and_check_one();

	return result;
}

/* 
                                                                           
  
               
                                          
  */
int write_image_header(void)
{
	int ret;
	int total = pagedir1.size + pagedir2.size + 2;
	char *header_buffer = NULL;

	/*                                 */
	ret = toiActiveAllocator->write_header_init();
	if (ret) {
		abort_hibernate(TOI_FAILED_MODULE_INIT,
				"Active allocator's write_header_init" " function failed.");
		goto write_image_header_abort;
	}

	/*              */
	header_buffer = (char *)toi_get_zeroed_page(24, TOI_ATOMIC_GFP);
	if (!header_buffer) {
		abort_hibernate(TOI_OUT_OF_MEMORY,
				"Out of memory when trying to get page for header!");
		goto write_image_header_abort;
	}

	/*                        */
	if (fill_toi_header((struct toi_header *)header_buffer)) {
		abort_hibernate(TOI_OUT_OF_MEMORY, "Failure to fill header information!");
		goto write_image_header_abort;
	}

	if (toiActiveAllocator->rw_header_chunk(WRITE, NULL,
						header_buffer, sizeof(struct toi_header))) {
		abort_hibernate(TOI_OUT_OF_MEMORY, "Failure to write header info.");
		goto write_image_header_abort;
	}

	if (toiActiveAllocator->rw_header_chunk(WRITE, NULL,
						(char *)&toi_max_workers,
						sizeof(toi_max_workers))) {
		abort_hibernate(TOI_OUT_OF_MEMORY, "Failure to number of workers to use.");
		goto write_image_header_abort;
	}

	/*                       */
	if (fs_info_save())
		goto write_image_header_abort;

	/*                             */
	ret = write_module_configs();
	if (ret) {
		abort_hibernate(TOI_FAILED_IO, "Failed to write module configs.");
		goto write_image_header_abort;
	}

	if (memory_bm_write(pageset1_map, toiActiveAllocator->rw_header_chunk)) {
		abort_hibernate(TOI_FAILED_IO, "Failed to write bitmaps.");
		goto write_image_header_abort;
	}

	/*                                      */
	if (toiActiveAllocator->write_header_cleanup()) {
		abort_hibernate(TOI_FAILED_IO, "Failed to cleanup writing header.");
		goto write_image_header_abort_no_cleanup;
	}

	if (test_result_state(TOI_ABORTED))
		goto write_image_header_abort_no_cleanup;

	toi_update_status(total, total, NULL);

 out:
	if (header_buffer)
		toi_free_page(24, (unsigned long)header_buffer);
	return ret;

 write_image_header_abort:
	toiActiveAllocator->write_header_cleanup();
 write_image_header_abort_no_cleanup:
	ret = -1;
	goto out;
}

/* 
                                  
                                                     
  
                                                                
                                                                  
                                                                  
                                                              
  */
static char *sanity_check(struct toi_header *sh)
{
	char *reason = check_image_kernel((struct swsusp_info *)sh);

	if (reason)
		return reason;

	if (!test_action_state(TOI_IGNORE_ROOTFS)) {
		const struct super_block *sb;
		list_for_each_entry(sb, &super_blocks, s_list) {
			if ((!(sb->s_flags & MS_RDONLY)) &&
			    (sb->s_type->fs_flags & FS_REQUIRES_DEV))
				return "Device backed fs has been mounted "
				    "rw prior to resume or initrd/ramfs " "is mounted rw.";
		}
	}

	return NULL;
}

static DECLARE_WAIT_QUEUE_HEAD(freeze_wait);

#define FREEZE_IN_PROGRESS (~0)

static int freeze_result;

#ifdef CONFIG_MTK_HIBERNATION
static DECLARE_WAIT_QUEUE_HEAD(mmc_rescan_wait);
static bool mmc_rescan_done;

void mmc_rescan_wait_finish(void)
{
	if (mmc_rescan_done == true)
		return;
	mmc_rescan_done = true;
	wake_up(&mmc_rescan_wait);
	pr_warn("[%s] done\n", __func__);
}
EXPORT_SYMBOL_GPL(mmc_rescan_wait_finish);

static int mmc_rescan_ready(void)
{
	int result = 0;

	/*                                                                                       */
	/*                                                                                                                   */
	wait_event_timeout(mmc_rescan_wait, mmc_rescan_done == true, 5 * HZ);
	if (mmc_rescan_done == false) {
		pr_warn("[%s] wait mmc rescan partition timeout !!\n", __func__);
		abort_hibernate(TOI_FAILED_IO, "Failed to wait mmc rescan ready.");
		return -EIO;
	}
	/*           */
	return result;
}
#endif				/*                        */

static void do_freeze(struct work_struct *dummy)
{
	freeze_result = freeze_processes();
	wake_up(&freeze_wait);
	trap_non_toi_io = 1;
}

static DECLARE_WORK(freeze_work, do_freeze);

/* 
                                                                              
  
               
                                                        
                                     
  */
static int __read_pageset1(void)
{
	int i, result = 0;
	char *header_buffer = (char *)toi_get_zeroed_page(25, TOI_ATOMIC_GFP), *sanity_error = NULL;
	struct toi_header *toi_header;

	if (!header_buffer) {
		printk(KERN_INFO "Unable to allocate a page for reading the " "signature.\n");
		return -ENOMEM;
	}

	/*                    */
	result = toiActiveAllocator->image_exists(1);
	if (result == 3) {
		result = -ENODATA;
		toi_early_boot_message(1, 0, "The signature from an older "
				       "version of TuxOnIce has been detected.");
		goto out_remove_image;
	}

	if (result != 1) {
		result = -ENODATA;
		noresume_reset_modules();
		printk(KERN_INFO "TuxOnIce: No image found.\n");
		goto out;
	}

	/*
                                                                  
                                                        
   
                                                                     
                                                                    
                                                                    
                                                                    
                        
  */

	result = toiActiveAllocator->read_header_init();
	if (result) {
		printk(KERN_INFO "TuxOnIce: Failed to initialise, reading the " "image header.\n");
		goto out_remove_image;
	}

	/*                                        */
	if (test_toi_state(TOI_NORESUME_SPECIFIED)) {
		printk(KERN_INFO "TuxOnIce: Noresume on command line. Removed " "image.\n");
		goto out_remove_image;
	}

	/*                                    */
	if (test_toi_state(TOI_RESUMED_BEFORE)) {
		toi_early_boot_message(1, 0, NULL);
		if (!(test_toi_state(TOI_CONTINUE_REQ))) {
			printk(KERN_INFO "TuxOnIce: Tried to resume before: "
			       "Invalidated image.\n");
			goto out_remove_image;
		}
	}

	clear_toi_state(TOI_CONTINUE_REQ);

	toi_image_header_version = toiActiveAllocator->get_header_version();

	if (unlikely(toi_image_header_version > TOI_HEADER_VERSION)) {
		toi_early_boot_message(1, 0, image_version_error);
		if (!(test_toi_state(TOI_CONTINUE_REQ))) {
			printk(KERN_INFO "TuxOnIce: Header version too new: "
			       "Invalidated image.\n");
			goto out_remove_image;
		}
	}

	/*                       */
	result = toiActiveAllocator->rw_header_chunk(READ, NULL,
						     header_buffer, sizeof(struct toi_header));
	if (result < 0) {
		printk(KERN_ERR "TuxOnIce: Failed to read the image " "signature.\n");
		goto out_remove_image;
	}

	toi_header = (struct toi_header *)header_buffer;

	/*
                                                                    
  */

	sanity_error = sanity_check(toi_header);
	if (sanity_error) {
		toi_early_boot_message(1, TOI_CONTINUE_REQ, sanity_error);
		printk(KERN_INFO "TuxOnIce: Sanity check failed.\n");
		goto out_remove_image;
	}

	/*
                                                         
   
                                                                    
   
                                                                    
                                                             
  */

	memcpy((char *)&pagedir1, (char *)&toi_header->pagedir, sizeof(pagedir1));
	toi_result = toi_header->param0;
	if (!toi_bkd.toi_debug_state) {
		toi_bkd.toi_action =
		    (toi_header->param1 & ~toi_bootflags_mask) |
		    (toi_bkd.toi_action & toi_bootflags_mask);
		toi_bkd.toi_debug_state = toi_header->param2;
		toi_bkd.toi_default_console_level = toi_header->param3;
	}
	clear_toi_state(TOI_IGNORE_LOGLEVEL);
	pagedir2.size = toi_header->pageset_2_size;
	for (i = 0; i < 4; i++)
		toi_bkd.toi_io_time[i / 2][i % 2] = toi_header->io_time[i / 2][i % 2];

	set_toi_state(TOI_BOOT_KERNEL);
	boot_kernel_data_buffer = toi_header->bkd;

	read_if_version(1, toi_max_workers, "TuxOnIce max workers", goto out_remove_image);

	/*                      */
	if (fs_info_load_and_check()) {
		printk(KERN_EMERG "TuxOnIce: File system mount time checks "
		       "failed. Refusing to corrupt your filesystems!\n");
		goto out_remove_image;
	}
#ifdef CONFIG_MTK_HIBERNATION
	if (mmc_rescan_ready()) {
		printk(KERN_EMERG "TuxOnIce: MMC rescan partition is not "
		       "ready. Refusing to corrupt your filesystems!\n");
		goto out_remove_image;
	}
#endif				/*                        */

	/*                            */
	result = read_module_configs();
	if (result) {
		pagedir1.size = 0;
		pagedir2.size = 0;
		printk(KERN_INFO "TuxOnIce: Failed to read TuxOnIce module " "configurations.\n");
		clear_action_state(TOI_KEEP_IMAGE);
		goto out_remove_image;
	}

	toi_prepare_console();

	set_toi_state(TOI_NOW_RESUMING);

	if (!test_action_state(TOI_LATE_CPU_HOTPLUG)) {
		toi_prepare_status(DONT_CLEAR_BAR, "Disable nonboot cpus.");
		if (disable_nonboot_cpus()) {
			set_abort_result(TOI_CPU_HOTPLUG_FAILED);
			goto out_reset_console;
		}
	}

	result = pm_notifier_call_chain(PM_RESTORE_PREPARE);
	if (result)
		goto out_notifier_call_chain;

	if (usermodehelper_disable())
		goto out_enable_nonboot_cpus;

	current->flags |= PF_NOFREEZE;
	freeze_result = FREEZE_IN_PROGRESS;

	schedule_work_on(cpumask_first(cpu_online_mask), &freeze_work);

	toi_cond_pause(1, "About to read original pageset1 locations.");

	/*
                                               
                                                              
  */
	if (memory_bm_read(pageset1_map, toiActiveAllocator->rw_header_chunk))
		goto out_thaw;

	/*
                                         
                                      
  */
	result = toiActiveAllocator->read_header_cleanup();
	if (result) {
		printk(KERN_ERR "TuxOnIce: Failed to cleanup after reading the " "image header.\n");
		goto out_thaw;
	}

	toi_cond_pause(1, "About to read pagedir.");

	/*
                                                                    
                                                                         
  */
	if (toi_get_pageset1_load_addresses()) {
		printk(KERN_INFO "TuxOnIce: Failed to get load addresses for " "pageset1.\n");
		goto out_thaw;
	}

	/*                               */
	toi_cond_pause(1, "About to read pageset 1.");

	/*                                                 */
	if (read_pageset(&pagedir1, 0)) {
		toi_prepare_status(DONT_CLEAR_BAR, "Failed to read pageset 1.");
		result = -EIO;
		goto out_thaw;
	}

	toi_cond_pause(1, "About to restore original kernel.");
	result = 0;

	if (!test_action_state(TOI_KEEP_IMAGE) && toiActiveAllocator->mark_resume_attempted)
		toiActiveAllocator->mark_resume_attempted(1);

	wait_event(freeze_wait, freeze_result != FREEZE_IN_PROGRESS);
 out:
	current->flags &= ~PF_NOFREEZE;
	toi_free_page(25, (unsigned long)header_buffer);
	return result;

 out_thaw:
	wait_event(freeze_wait, freeze_result != FREEZE_IN_PROGRESS);
	trap_non_toi_io = 0;
	thaw_processes();
	usermodehelper_enable();
 out_enable_nonboot_cpus:
	enable_nonboot_cpus();
 out_notifier_call_chain:
	pm_notifier_call_chain(PM_POST_RESTORE);
 out_reset_console:
	toi_cleanup_console();
 out_remove_image:
	result = -EINVAL;
	if (!test_action_state(TOI_KEEP_IMAGE))
		toiActiveAllocator->remove_image();
	toiActiveAllocator->read_header_cleanup();
	noresume_reset_modules();
	goto out;
}

/* 
                                                             
  
                                                                
                                                     
  */
int read_pageset1(void)
{
	int error;

	error = __read_pageset1();

	if (error && error != -ENODATA && error != -EINVAL && !test_result_state(TOI_ABORTED))
		abort_hibernate(TOI_IMAGE_ERROR, "TuxOnIce: Error %d resuming\n", error);

	return error;
}

/* 
                                               
  */
static char *get_have_image_data(void)
{
	char *output_buffer = (char *)toi_get_zeroed_page(26, TOI_ATOMIC_GFP);
	struct toi_header *toi_header;

	if (!output_buffer) {
		printk(KERN_INFO "Output buffer null.\n");
		return NULL;
	}

	/*                    */
	if (!toiActiveAllocator->image_exists(1) ||
	    toiActiveAllocator->read_header_init() ||
	    toiActiveAllocator->rw_header_chunk(READ, NULL,
						output_buffer, sizeof(struct toi_header))) {
		sprintf(output_buffer, "0\n");
		/*
                                                 
                                         
   */
		clear_toi_state(TOI_BOOT_TIME);
		goto out;
	}

	toi_header = (struct toi_header *)output_buffer;

	sprintf(output_buffer, "1\n%s\n%s\n", toi_header->uts.machine, toi_header->uts.version);

	/*                                    */
	if (test_toi_state(TOI_RESUMED_BEFORE))
		strcat(output_buffer, "Resumed before.\n");

 out:
	noresume_reset_modules();
	return output_buffer;
}

/* 
                                                
                                                               
                             
  
                                                              
                                                                 
                                                             
       
  
               
                                               
  */
int read_pageset2(int overwrittenpagesonly)
{
	int result = 0;

	if (!pagedir2.size)
		return 0;

	result = read_pageset(&pagedir2, overwrittenpagesonly);

	toi_cond_pause(1, "Pagedir 2 read.");

	return result;
}

/* 
                                               
                       
  
                                                                
                                                        
                                                    
            
  */
int image_exists_read(const char *page, int count)
{
	int len = 0;
	char *result;

	if (toi_activate_storage(0))
		return count;

	if (!test_toi_state(TOI_RESUME_DEVICE_OK))
		toi_attempt_to_parse_resume_device(0);

	if (!toiActiveAllocator) {
		len = sprintf((char *)page, "-1\n");
	} else {
		result = get_have_image_data();
		if (result) {
			len = sprintf((char *)page, "%s", result);
			toi_free_page(26, (unsigned long)result);
		}
	}

	toi_deactivate_storage(0);

	return len;
}

/* 
                                                         
  */
int image_exists_write(const char *buffer, int count)
{
	if (toi_activate_storage(0))
		return count;

	if (toiActiveAllocator && toiActiveAllocator->image_exists(1))
		toiActiveAllocator->remove_image();

	toi_deactivate_storage(0);

	clear_result_state(TOI_KEPT_IMAGE);

	return count;
}
