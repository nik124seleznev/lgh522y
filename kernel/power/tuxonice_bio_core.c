/*
 * kernel/power/tuxonice_bio.c
 *
 * Copyright (C) 2004-2010 Nigel Cunningham (nigel at tuxonice net)
 *
 * Distributed under GPLv2.
 *
 * This file contains block io functions for TuxOnIce. These are
 * used by the swapwriter and it is planned that they will also
 * be used by the NFSwriter.
 *
 */

#include <linux/blkdev.h>
#include <linux/syscalls.h>
#include <linux/suspend.h>
#include <linux/ctype.h>
#include <linux/fs_uuid.h>

#include "tuxonice.h"
#include "tuxonice_sysfs.h"
#include "tuxonice_modules.h"
#include "tuxonice_prepare_image.h"
#include "tuxonice_bio.h"
#include "tuxonice_ui.h"
#include "tuxonice_alloc.h"
#include "tuxonice_io.h"
#include "tuxonice_builtin.h"
#include "tuxonice_bio_internal.h"

#define MEMORY_ONLY 1
#define THROTTLE_WAIT 2

/*                                  */
#ifndef MEASURE_MUTEX_CONTENTION
#define my_mutex_lock(index, the_lock) mutex_lock(the_lock)
#define my_mutex_unlock(index, the_lock) mutex_unlock(the_lock)
#else
unsigned long mutex_times[2][2][NR_CPUS];
#define my_mutex_lock(index, the_lock) do { \
	int have_mutex; \
	have_mutex = mutex_trylock(the_lock); \
	if (!have_mutex) { \
		mutex_lock(the_lock); \
		mutex_times[index][0][smp_processor_id()]++; \
	} else { \
		mutex_times[index][1][smp_processor_id()]++; \
	}

#define my_mutex_unlock(index, the_lock) \
	mutex_unlock(the_lock); \
} while (0)
#endif

static int page_idx, reset_idx;

static int target_outstanding_io = 1024;
static int max_outstanding_writes, max_outstanding_reads;

static struct page *bio_queue_head, *bio_queue_tail;
static atomic_t toi_bio_queue_size;
static DEFINE_SPINLOCK(bio_queue_lock);

static int free_mem_throttle, throughput_throttle;
int more_readahead = 1;
static struct page *readahead_list_head, *readahead_list_tail;

static struct page *waiting_on;

static atomic_t toi_io_in_progress, toi_io_done;
static DECLARE_WAIT_QUEUE_HEAD(num_in_progress_wait);

int current_stream;
/*                                                          
                      */
char *toi_writer_buffer;
int toi_writer_buffer_posn;

static DEFINE_MUTEX(toi_bio_mutex);
static DEFINE_MUTEX(toi_bio_readahead_mutex);

static struct task_struct *toi_queue_flusher;
static int toi_bio_queue_flush_pages(int dedicated_thread);

struct toi_module_ops toi_blockwriter_ops;

#define TOTAL_OUTSTANDING_IO (atomic_read(&toi_io_in_progress) + \
	       atomic_read(&toi_bio_queue_size))

unsigned long raw_pages_allocd, header_pages_reserved;

/* 
                                                                     
  
                                                                            
                                                                           
           
  */
static void set_free_mem_throttle(void)
{
	int new_throttle = nr_unallocated_buffer_pages() + 256;

	if (new_throttle > free_mem_throttle)
		free_mem_throttle = new_throttle;
}

#define NUM_REASONS 7
static atomic_t reasons[NUM_REASONS];
static char *reason_name[NUM_REASONS] = {
	"readahead not ready",
	"bio allocation",
	"synchronous I/O",
	"toi_bio_get_new_page",
	"memory low",
	"readahead buffer allocation",
	"throughput_throttle",
};

/*                            */
unsigned long resume_firstblock;
dev_t resume_dev_t;
struct block_device *resume_block_device;
static atomic_t resume_bdev_open_count;

struct block_device *header_block_device;

/* 
                                             
  
                                                                   
                                                                          
                                                                          
                                                                                
                                                                   
                                 
                                                   
  
                                                                          
                                                                        
                                                
 */
struct block_device *toi_open_bdev(char *uuid, dev_t default_device, int display_errs)
{
	struct block_device *bdev;
	dev_t device = default_device;
	char buf[32];
	int retried = 0;

 retry:
	if (uuid) {
		struct fs_info seek;
		strncpy((char *)&seek.uuid, uuid, 16);
		seek.dev_t = 0;
		seek.last_mount_size = 0;
		device = blk_lookup_fs_info(&seek);
		if (!device) {
			device = default_device;
			printk(KERN_DEBUG "Unable to resolve uuid. Falling back" " to dev_t.\n");
		} else
			printk(KERN_DEBUG "Resolved uuid to device %s.\n",
			       format_dev_t(buf, device));
	}

	if (!device) {
		printk(KERN_ERR "TuxOnIce attempting to open a " "blank dev_t!\n");
		dump_stack();
		return NULL;
	}
	bdev = toi_open_by_devnum(device);

	if (IS_ERR(bdev) || !bdev) {
		if (!retried) {
			retried = 1;
			wait_for_device_probe();
			goto retry;
		}
		if (display_errs)
			toi_early_boot_message(1, TOI_CONTINUE_REQ,
					       "Failed to get access to block device "
					       "\"%x\" (error %d).\n Maybe you need "
					       "to run mknod and/or lvmsetup in an "
					       "initrd/ramfs?", device, bdev);
		return ERR_PTR(-EINVAL);
	}
	toi_message(TOI_BIO, TOI_VERBOSE, 0, "TuxOnIce got bdev %p for dev_t %x.", bdev, device);

	return bdev;
}

static void toi_bio_reserve_header_space(unsigned long request)
{
	header_pages_reserved = request;
}

/* 
                                                       
                                                        
  
                                                                      
                                                                       
                                                                      
                      
  
                                                                     
  */
static void do_bio_wait(int reason)
{
	struct page *was_waiting_on = waiting_on;

	/*                                                    */
	if (was_waiting_on) {
		wait_on_page_locked(was_waiting_on);
		atomic_inc(&reasons[reason]);
	} else {
		atomic_inc(&reasons[reason]);

		wait_event(num_in_progress_wait,
			!atomic_read(&toi_io_in_progress) ||
			nr_unallocated_buffer_pages() > free_mem_throttle);
	}
}

/* 
                                                                              
                                        
  
                                                                          
                                                                        
                                                                    
  */
static int throttle_if_needed(int flags)
{
	int free_pages = nr_unallocated_buffer_pages();

	/*                                               */
	while (unlikely(free_pages < free_mem_throttle) &&
	       atomic_read(&toi_io_in_progress) && !test_result_state(TOI_ABORTED)) {
		if (!(flags & THROTTLE_WAIT))
			return -ENOMEM;
		do_bio_wait(4);
		free_pages = nr_unallocated_buffer_pages();
	}

	while (!(flags & MEMORY_ONLY) && throughput_throttle &&
	       TOTAL_OUTSTANDING_IO >= throughput_throttle && !test_result_state(TOI_ABORTED)) {
		int result = toi_bio_queue_flush_pages(0);
		if (result)
			return result;
		atomic_inc(&reasons[6]);
		wait_event(num_in_progress_wait,
			   !atomic_read(&toi_io_in_progress) ||
			   TOTAL_OUTSTANDING_IO < throughput_throttle);
	}

	return 0;
}

/* 
                                                                  
                                                                 
  
                                                                               
                                                                             
                                                                  
  
                                                                   
                                                                    
                                                                     
  
                                     
  */
static void update_throughput_throttle(int jif_index)
{
	int done = atomic_read(&toi_io_done);
	throughput_throttle = done * 2 / 5 / jif_index;
}

/* 
                                                               
  
                                                                        
  */
static int toi_finish_all_io(void)
{
	int result = toi_bio_queue_flush_pages(0);
	toi_bio_queue_flusher_should_finish = 1;
	wake_up(&toi_io_queue_flusher);
	wait_event(num_in_progress_wait, !TOTAL_OUTSTANDING_IO);
	return result;
}

/* 
                                         
                                
                                                                
  
                                                                         
                                                                           
                                                                         
                                                                       
                                                                   
                          
  */
static void toi_end_bio(struct bio *bio, int err)
{
	struct page *page = bio->bi_io_vec[0].bv_page;

	/*                                                       */
	if (!err)
		BUG_ON(!test_bit(BIO_UPTODATE, &bio->bi_flags));

	unlock_page(page);
	bio_put(bio);

	if (waiting_on == page)
		waiting_on = NULL;

	put_page(page);

	if (bio->bi_private)
		toi__free_page((int)((unsigned long)bio->bi_private), page);

	bio_put(bio);

	atomic_dec(&toi_io_in_progress);
	atomic_inc(&toi_io_done);

	wake_up(&num_in_progress_wait);
}

/* 
                              
                           
                                      
                                              
                                      
                                                                          
                                                                 
           
  
                                                                           
                                                                          
                                                             
  
                                                                           
                                                 
  */
static int submit(int writing, struct block_device *dev, sector_t first_block,
		  struct page *page, int free_group)
{
	struct bio *bio = NULL;
	int cur_outstanding_io, result;

	/*
                                                              
                                                         
              
  */
	if (writing) {
		result = throttle_if_needed(MEMORY_ONLY | THROTTLE_WAIT);
		if (result)
			return result;
	}

	while (!bio) {
		bio = bio_alloc(TOI_ATOMIC_GFP, 1);
		if (!bio) {
			set_free_mem_throttle();
			do_bio_wait(1);
		}
	}

	bio->bi_bdev = dev;
	bio->bi_sector = first_block;
	bio->bi_private = (void *)((unsigned long)free_group);
	bio->bi_end_io = toi_end_bio;
	bio->bi_flags |= (1 << BIO_TOI);

	if (bio_add_page(bio, page, PAGE_SIZE, 0) < PAGE_SIZE) {
		printk(KERN_DEBUG "ERROR: adding page to bio at %lld\n",
		       (unsigned long long)first_block);
		bio_put(bio);
		return -EFAULT;
	}

	bio_get(bio);

	cur_outstanding_io = atomic_add_return(1, &toi_io_in_progress);
	if (writing) {
		if (cur_outstanding_io > max_outstanding_writes)
			max_outstanding_writes = cur_outstanding_io;
	} else {
		if (cur_outstanding_io > max_outstanding_reads)
			max_outstanding_reads = cur_outstanding_io;
	}


	/*                        */
	if (unlikely(test_action_state(TOI_TEST_BIO) && writing)) {
		/*                                */
		set_bit(BIO_UPTODATE, &bio->bi_flags);
		toi_end_bio(bio, 0);
	} else
		submit_bio(writing | REQ_SYNC, bio);

	return 0;
}

/* 
                                                                      
  
                                        
                                             
                                                      
                                              
                                                                               
                                                        
  
                                               
  
                                                                              
                                                                            
                                                                             
                                      
  */
int toi_do_io(int writing, struct block_device *bdev, long block0,
	      struct page *page, int is_readahead, int syncio, int free_group)
{
	page->private = 0;

	/*                                                             */
	lock_page(page);

	if (is_readahead) {
		if (readahead_list_head)
			readahead_list_tail->private = (unsigned long)page;
		else
			readahead_list_head = page;

		readahead_list_tail = page;
	}

	/*                                        */
	if (syncio)
		waiting_on = page;

	/*                 */
	get_page(page);

	if (submit(writing, bdev, block0, page, free_group))
		return -EFAULT;

	if (syncio)
		do_bio_wait(2);

	return 0;
}

/* 
                                                                           
                                        
                                                
                                                      
                                  
  
                                                                          
                                      
  */
static int toi_bdev_page_io(int writing, struct block_device *bdev, long pos, struct page *page)
{
	return toi_do_io(writing, bdev, pos, page, 0, 1, 0);
}

/* 
                                                                           
  
                                                                             
                                                                    
  */
static int toi_bio_memory_needed(void)
{
	return target_outstanding_io * (PAGE_SIZE + sizeof(struct request) + sizeof(struct bio));
}

/* 
                                                                            
                                                                    
                              
  
                                                                                
                                                  
  */
static int toi_bio_print_debug_stats(char *buffer, int size)
{
	int len = 0;

	if (toiActiveAllocator != &toi_blockwriter_ops) {
		len = scnprintf(buffer, size, "- Block I/O inactive.\n");
		return len;
	}

	len = scnprintf(buffer, size, "- Block I/O active.\n");

	len += toi_bio_chains_debug_info(buffer + len, size - len);

	len += scnprintf(buffer + len, size - len,
			 "- Max outstanding reads %d. Max writes %d.\n",
			 max_outstanding_reads, max_outstanding_writes);

	len += scnprintf(buffer + len, size - len,
			 "  Memory_needed: %d x (%lu + %u + %u) = %d bytes.\n",
			 target_outstanding_io,
			 PAGE_SIZE, (unsigned int)sizeof(struct request),
			 (unsigned int)sizeof(struct bio), toi_bio_memory_needed());

#ifdef MEASURE_MUTEX_CONTENTION
	{
		int i;

		len += scnprintf(buffer + len, size - len,
				 "  Mutex contention while reading:\n  Contended      Free\n");

		for_each_online_cpu(i)
		    len += scnprintf(buffer + len, size - len,
				     "  %9lu %9lu\n", mutex_times[0][0][i], mutex_times[0][1][i]);

		len += scnprintf(buffer + len, size - len,
				 "  Mutex contention while writing:\n  Contended      Free\n");

		for_each_online_cpu(i)
		    len += scnprintf(buffer + len, size - len,
				     "  %9lu %9lu\n", mutex_times[1][0][i], mutex_times[1][1][i]);

	}
#endif

	return len + scnprintf(buffer + len, size - len,
			       "  Free mem throttle point reached %d.\n", free_mem_throttle);
}

static int total_header_bytes;
static int unowned;

void debug_broken_header(void)
{
	printk(KERN_DEBUG "Image header too big for size allocated!\n");
	print_toi_header_storage_for_modules();
	printk(KERN_DEBUG "Page flags : %d.\n", toi_pageflags_space_needed());
	printk(KERN_DEBUG "toi_header : %zu.\n", sizeof(struct toi_header));
	printk(KERN_DEBUG "Total unowned : %d.\n", unowned);
	printk(KERN_DEBUG "Total used : %d (%ld pages).\n", total_header_bytes,
	       DIV_ROUND_UP(total_header_bytes, PAGE_SIZE));
	printk(KERN_DEBUG "Space needed now : %ld.\n", get_header_storage_needed());
	dump_block_chains();
	abort_hibernate(TOI_HEADER_TOO_BIG, "Header reservation too small.");
}

/* 
                                                               
                                        
                                                        
  
                                                              
  */
static int toi_rw_init(int writing, int stream_number)
{
	if (stream_number)
		toi_extent_state_restore(stream_number);
	else
		toi_extent_state_goto_start();

	if (writing) {
		reset_idx = 0;
		if (!current_stream)
			page_idx = 0;
	} else {
		reset_idx = 1;
	}

	atomic_set(&toi_io_done, 0);
	if (!toi_writer_buffer)
		toi_writer_buffer = (char *)toi_get_zeroed_page(11, TOI_ATOMIC_GFP);
	toi_writer_buffer_posn = writing ? 0 : PAGE_SIZE;

	current_stream = stream_number;

	more_readahead = 1;

	return toi_writer_buffer ? 0 : -ENOMEM;
}

/* 
                                                 
                                               
  
                                                                       
                                                                       
                                                                       
              
  */
static void toi_bio_queue_write(char **full_buffer)
{
	struct page *page = virt_to_page(*full_buffer);
	unsigned long flags;

	*full_buffer = NULL;
	page->private = 0;

	spin_lock_irqsave(&bio_queue_lock, flags);
	if (!bio_queue_head)
		bio_queue_head = page;
	else
		bio_queue_tail->private = (unsigned long)page;

	bio_queue_tail = page;
	atomic_inc(&toi_bio_queue_size);

	spin_unlock_irqrestore(&bio_queue_lock, flags);
	wake_up(&toi_io_queue_flusher);
}

/* 
                                      
                                                
  
                                                                   
                        
  */
static int toi_rw_cleanup(int writing)
{
	int i, result = 0;

	toi_message(TOI_BIO, TOI_VERBOSE, 0, "toi_rw_cleanup.");
	if (writing) {
		if (toi_writer_buffer_posn && !test_result_state(TOI_ABORTED))
			toi_bio_queue_write(&toi_writer_buffer);

		while (bio_queue_head && !result)
			result = toi_bio_queue_flush_pages(0);

		if (result)
			return result;

		if (current_stream == 2)
			toi_extent_state_save(1);
		else if (current_stream == 1)
			toi_extent_state_save(3);
	}

	result = toi_finish_all_io();

	while (readahead_list_head) {
		void *next = (void *)readahead_list_head->private;
		toi__free_page(12, readahead_list_head);
		readahead_list_head = next;
	}

	readahead_list_tail = NULL;

	if (!current_stream)
		return result;

	for (i = 0; i < NUM_REASONS; i++) {
		if (!atomic_read(&reasons[i]))
			continue;
		printk(KERN_DEBUG "Waited for i/o due to %s %d times.\n",
		       reason_name[i], atomic_read(&reasons[i]));
		atomic_set(&reasons[i], 0);
	}

	current_stream = 0;
	return result;
}

#ifdef CONFIG_TOI_FIXUP
static void check_mem_status(void)
{
	return; //                                      
	hib_warn("%d: free/throttle: %lu/%d, no_readahead/target_oustanding_io:%d/%d io_in/io_done:%d/%d\n", __LINE__,
			nr_unallocated_buffer_pages(), free_mem_throttle ,
			test_action_state(TOI_NO_READAHEAD), target_outstanding_io,
			atomic_read(&toi_io_in_progress), atomic_read(&toi_io_done));
	HIB_SHOW_MEMINFO();
}

static int hit_lowmem_barrier(void)
{
	unsigned long free_pages;
    struct zone *zone;

    for_each_populated_zone(zone) {
        if (!strcmp(zone->name, "Normal")) {
            free_pages = zone_page_state(zone, NR_FREE_PAGES);
			return (free_pages < min_wmark_pages(zone)) ? 1 : 0;
		}
    }
	return 0;
}
#endif

/* 
                                                        
                                                                    
  
                                                                       
                                                                       
                      
  */
static int toi_start_one_readahead(int dedicated_thread)
{
	char *buffer = NULL;
	int oom = 0, result;

	result = throttle_if_needed(dedicated_thread ? THROTTLE_WAIT : 0);
	if (result)
		return result;

	mutex_lock(&toi_bio_readahead_mutex);

	while (!buffer) {
#ifdef CONFIG_TOI_FIXUP
		buffer = likely(hit_lowmem_barrier()) ? NULL : (char *) toi_get_zeroed_page(12, TOI_ATOMIC_GFP);
#else
		buffer = (char *) toi_get_zeroed_page(12, TOI_ATOMIC_GFP);
#endif
		if (!buffer) {
			if (oom && !dedicated_thread) {
				mutex_unlock(&toi_bio_readahead_mutex);
				return -ENOMEM;
			}

			oom = 1;
#ifdef CONFIG_TOI_FIXUP
			check_mem_status();
#endif
			set_free_mem_throttle();
			do_bio_wait(5);
		}
	}

	result = toi_bio_rw_page(READ, virt_to_page(buffer), 1, 0);
	if (result == -ENOSPC)
		toi__free_page(12, virt_to_page(buffer));
	mutex_unlock(&toi_bio_readahead_mutex);
	if (result) {
		if (result == -ENOSPC)
			toi_message(TOI_BIO, TOI_VERBOSE, 0, "Last readahead page submitted.");
		else
			printk(KERN_DEBUG "toi_bio_rw_page returned %d.\n", result);
	}
	return result;
}

/* 
                                                
                                                    
  
                                  
  
                                                                         
                                                                           
                  
  
                                                                         
                                                                        
                                                                        
                         
  
                                                                     
  */
static int toi_start_new_readahead(int dedicated_thread)
{
	int last_result, num_submitted = 0;

	/*                        */
	if (!more_readahead)
		return 0;

	do {
		last_result = toi_start_one_readahead(dedicated_thread);

		if (last_result) {
			if (last_result == -ENOMEM || last_result == -ENOSPC)
				return 0;

			printk(KERN_DEBUG "Begin read chunk returned %d.\n", last_result);
		} else
			num_submitted++;

	} while (more_readahead && !last_result &&
		 (dedicated_thread ||
		  (num_submitted < target_outstanding_io &&
		   atomic_read(&toi_io_in_progress) < target_outstanding_io)));

	return last_result;
}

/* 
                                                            
                                             
  */
static int bio_io_flusher(int writing)
{

	if (writing)
		return toi_bio_queue_flush_pages(1);
	else
		return toi_start_new_readahead(1);
}

/* 
                                                                        
                                              
  
                                                                           
                                          
  */
static int toi_bio_get_next_page_read(int no_readahead)
{
	char *virt;
	struct page *old_readahead_list_head;

	/*
                                                          
                                                          
                                  
  */
	if (unlikely(no_readahead && toi_start_one_readahead(0))) {
		printk(KERN_EMERG "%d:No readahead and toi_start_one_readahead "
		       "returned non-zero.\n", __LINE__);
		return -EIO;
	}

	if (unlikely(!readahead_list_head)) {
		/*
                                                  
                                                   
                                                    
                                
   */
		if (!more_readahead) {
			printk(KERN_EMERG "No more readahead.\n");
			return -ENOSPC;
		}
		if (unlikely(toi_start_one_readahead(0))) {
			printk(KERN_EMERG "%d:No readahead and "
			       "toi_start_one_readahead returned non-zero.\n", __LINE__);
			return -EIO;
		}
	}

	if (PageLocked(readahead_list_head)) {
		waiting_on = readahead_list_head;
		do_bio_wait(0);
	}

	virt = page_address(readahead_list_head);
	memcpy(toi_writer_buffer, virt, PAGE_SIZE);

	mutex_lock(&toi_bio_readahead_mutex);
	old_readahead_list_head = readahead_list_head;
	readahead_list_head = (struct page *)readahead_list_head->private;
	mutex_unlock(&toi_bio_readahead_mutex);
	toi__free_page(12, old_readahead_list_head);
	return 0;
}

/* 
                                                                          
                                                      
  
                                                        
  
                                                                 
                          
  
                                                                     
                                                                     
                                                                    
                                                                     
                      
  
                                                                       
           
  */

int toi_bio_queue_flush_pages(int dedicated_thread)
{
	unsigned long flags;
	int result = 0;
	static DEFINE_MUTEX(busy);

	if (!mutex_trylock(&busy))
		return 0;

 top:
	spin_lock_irqsave(&bio_queue_lock, flags);
	while (bio_queue_head) {
		struct page *page = bio_queue_head;
		bio_queue_head = (struct page *)page->private;
		if (bio_queue_tail == page)
			bio_queue_tail = NULL;
		atomic_dec(&toi_bio_queue_size);
		spin_unlock_irqrestore(&bio_queue_lock, flags);

		/*                                                       */
		if (!result)
			result = toi_bio_rw_page(WRITE, page, 0, 11);
		/*
                                                
                                     
   */
		if (result)
			toi__free_page(11, page);
		spin_lock_irqsave(&bio_queue_lock, flags);
	}
	spin_unlock_irqrestore(&bio_queue_lock, flags);

	if (dedicated_thread) {
		wait_event(toi_io_queue_flusher, bio_queue_head ||
			   toi_bio_queue_flusher_should_finish);
		if (likely(!toi_bio_queue_flusher_should_finish))
			goto top;
		toi_bio_queue_flusher_should_finish = 0;
	}

	mutex_unlock(&busy);
	return result;
}

/* 
                                                
                                               
  */
static int toi_bio_get_new_page(char **full_buffer)
{
	int result = throttle_if_needed(THROTTLE_WAIT);
	if (result)
		return result;

	while (!*full_buffer) {
		*full_buffer = (char *)toi_get_zeroed_page(11, TOI_ATOMIC_GFP);
		if (!*full_buffer) {
			set_free_mem_throttle();
			do_bio_wait(3);
		}
	}

	return 0;
}

/* 
                                                             
                                                  
                                                      
                                                         
                                                                     
  */
static int toi_rw_buffer(int writing, char *buffer, int buffer_size, int no_readahead)
{
	int bytes_left = buffer_size, result = 0;

	while (bytes_left) {
		char *source_start = buffer + buffer_size - bytes_left;
		char *dest_start = toi_writer_buffer + toi_writer_buffer_posn;
		int capacity = PAGE_SIZE - toi_writer_buffer_posn;
		char *to = writing ? dest_start : source_start;
		char *from = writing ? source_start : dest_start;

		if (bytes_left <= capacity) {
			memcpy(to, from, bytes_left);
			toi_writer_buffer_posn += bytes_left;
			return 0;
		}

		/*                                        */
		memcpy(to, from, capacity);
		bytes_left -= capacity;

		if (!writing) {
			/*
                         
                                                     
    */
			int result = toi_bio_get_next_page_read(no_readahead);
			if (result) {
				printk("toi_bio_get_next_page_read " "returned %d.\n", result);
				return result;
			}
		} else {
			toi_bio_queue_write(&toi_writer_buffer);
			result = toi_bio_get_new_page(&toi_writer_buffer);
			if (result) {
				printk(KERN_ERR "toi_bio_get_new_page returned " "%d.\n", result);
				return result;
			}
		}

		toi_writer_buffer_posn = 0;
		toi_cond_pause(0, NULL);
	}

	return 0;
}

/* 
                                               
                                         
                                                                    
                                                                    
  
                                                                      
                                         
  */
static int toi_bio_read_page(unsigned long *pfn, int buf_type,
			     void *buffer_page, unsigned int *buf_size)
{
	int result = 0;
	int this_idx;
	char *buffer_virt = TOI_MAP(buf_type, buffer_page);

	/*
                                                                     
                                
  */
	if (current == toi_queue_flusher && more_readahead && !test_action_state(TOI_NO_READAHEAD)) {
		int result2 = toi_start_new_readahead(0);
		if (result2) {
			printk(KERN_DEBUG "Queue flusher and "
			       "toi_start_one_readahead returned non-zero.\n");
			result = -EIO;
			goto out;
		}
	}

	my_mutex_lock(0, &toi_bio_mutex);

	/*
                           
                                              
                         
                                                                  
              
  */
	if (toi_rw_buffer(READ, (char *)&this_idx, sizeof(int), 0) ||
	    toi_rw_buffer(READ, (char *)pfn, sizeof(unsigned long), 0) ||
	    toi_rw_buffer(READ, (char *)buf_size, sizeof(int), 0) ||
	    toi_rw_buffer(READ, buffer_virt, *buf_size, 0)) {
		result = -ENODATA;
		goto out_unlock;
	}

	if (reset_idx) {
		page_idx = this_idx;
		reset_idx = 0;
	} else {
		page_idx++;
		if (!this_idx)
			result = -ENODATA;
		else if (page_idx != this_idx)
			printk(KERN_ERR "Got page index %d, expected %d.\n", this_idx, page_idx);
	}

 out_unlock:
	my_mutex_unlock(0, &toi_bio_mutex);
 out:
	TOI_UNMAP(buf_type, buffer_page);
	return result;
}

/* 
                                                 
                                         
                                                                    
                                                       
  
                                                                            
                                   
  */
static int toi_bio_write_page(unsigned long pfn, int buf_type,
			      void *buffer_page, unsigned int buf_size)
{
	char *buffer_virt;
	int result = 0, result2 = 0;

	if (unlikely(test_action_state(TOI_TEST_FILTER_SPEED)))
		return 0;

	my_mutex_lock(1, &toi_bio_mutex);

	if (test_result_state(TOI_ABORTED)) {
		my_mutex_unlock(1, &toi_bio_mutex);
		return 0;
	}

	buffer_virt = TOI_MAP(buf_type, buffer_page);
	page_idx++;

	/*
                           
                                              
                         
  */
	if (toi_rw_buffer(WRITE, (char *)&page_idx, sizeof(int), 0) ||
	    toi_rw_buffer(WRITE, (char *)&pfn, sizeof(unsigned long), 0) ||
	    toi_rw_buffer(WRITE, (char *)&buf_size, sizeof(int), 0) ||
	    toi_rw_buffer(WRITE, buffer_virt, buf_size, 0)) {
		printk(KERN_DEBUG "toi_rw_buffer returned non-zero to " "toi_bio_write_page.\n");
		result = -EIO;
	}

	TOI_UNMAP(buf_type, buffer_page);
	my_mutex_unlock(1, &toi_bio_mutex);

	if (current == toi_queue_flusher)
		result2 = toi_bio_queue_flush_pages(0);

	return result ? result : result2;
}

/* 
                                                                     
                                         
                                               
                                     
                                                     
                                          
                                         
                                                                     
  
                                                    
  */
static int _toi_rw_header_chunk(int writing, struct toi_module_ops *owner,
				char *buffer, int buffer_size, int no_readahead)
{
	int result = 0;

	if (owner) {
		owner->header_used += buffer_size;
		toi_message(TOI_HEADER, TOI_LOW, 1,
			    "Header: %s : %d bytes (%d/%d) from offset %d.",
			    owner->name,
			    buffer_size, owner->header_used,
			    owner->header_requested, toi_writer_buffer_posn);
		if (owner->header_used > owner->header_requested && writing) {
			printk(KERN_EMERG "TuxOnIce module %s is using more "
			       "header space (%u) than it requested (%u).\n",
			       owner->name, owner->header_used, owner->header_requested);
			return buffer_size;
		}
	} else {
		unowned += buffer_size;
		toi_message(TOI_HEADER, TOI_LOW, 1,
			    "Header: (No owner): %d bytes (%d total so far) from "
			    "offset %d.", buffer_size, unowned, toi_writer_buffer_posn);
	}

	if (!writing && !no_readahead && more_readahead) {
		result = toi_start_new_readahead(0);
		toi_message(TOI_BIO, TOI_VERBOSE, 0, "Start new readahead " "returned %d.", result);
	}

	if (!result) {
		result = toi_rw_buffer(writing, buffer, buffer_size, no_readahead);
		toi_message(TOI_BIO, TOI_VERBOSE, 0, "rw_buffer returned " "%d.", result);
	}

	total_header_bytes += buffer_size;
	toi_message(TOI_BIO, TOI_VERBOSE, 0, "_toi_rw_header_chunk returning " "%d.", result);
	return result;
}

static int toi_rw_header_chunk(int writing, struct toi_module_ops *owner, char *buffer, int size)
{
	return _toi_rw_header_chunk(writing, owner, buffer, size, 1);
}

static int toi_rw_header_chunk_noreadahead(int writing,
					   struct toi_module_ops *owner, char *buffer, int size)
{
	return _toi_rw_header_chunk(writing, owner, buffer, size, 1);
}

/* 
                                                                       
  */
static int toi_bio_storage_needed(void)
{
	return sizeof(int) + PAGE_SIZE + toi_bio_devinfo_storage_needed();
}

/* 
                                                                   
                                                            
  */
static int toi_bio_save_config_info(char *buf)
{
	int *ints = (int *)buf;
	ints[0] = target_outstanding_io;
	return sizeof(int);
}

/* 
                                                      
                             
                                   
  */
static void toi_bio_load_config_info(char *buf, int size)
{
	int *ints = (int *)buf;
	target_outstanding_io = ints[0];
}

void close_resume_dev_t(int force)
{
	if (!resume_block_device)
		return;

	if (force)
		atomic_set(&resume_bdev_open_count, 0);
	else
		atomic_dec(&resume_bdev_open_count);

	if (!atomic_read(&resume_bdev_open_count)) {
		toi_close_bdev(resume_block_device);
		resume_block_device = NULL;
	}
}

int open_resume_dev_t(int force, int quiet)
{
	if (force) {
		close_resume_dev_t(1);
		atomic_set(&resume_bdev_open_count, 1);
	} else
		atomic_inc(&resume_bdev_open_count);

	if (resume_block_device)
		return 0;

	resume_block_device = toi_open_bdev(NULL, resume_dev_t, 0);
	if (IS_ERR(resume_block_device)) {
		if (!quiet)
			toi_early_boot_message(1, TOI_CONTINUE_REQ,
					       "Failed to open device %x, where"
					       " the header should be found.", resume_dev_t);
		resume_block_device = NULL;
		atomic_set(&resume_bdev_open_count, 0);
		return 1;
	}

	return 0;
}

/* 
                                                                   
                                                                            
                           
  */
static int toi_bio_initialise(int starting_cycle)
{
	int result;

	if (!starting_cycle || !resume_dev_t)
		return 0;

	max_outstanding_writes = 0;
	max_outstanding_reads = 0;
	current_stream = 0;
	toi_queue_flusher = current;
#ifdef MEASURE_MUTEX_CONTENTION
	{
		int i, j, k;

		for (i = 0; i < 2; i++)
			for (j = 0; j < 2; j++)
				for_each_online_cpu(k)
				    mutex_times[i][j][k] = 0;
	}
#endif
	result = open_resume_dev_t(0, 1);

	if (result)
		return result;

	return get_signature_page();
}

static unsigned long raw_to_real(unsigned long raw)
{
	unsigned long extra;

	extra = (raw * (sizeof(unsigned long) + sizeof(int)) +
		 (PAGE_SIZE + sizeof(unsigned long) + sizeof(int) + 1)) /
	    (PAGE_SIZE + sizeof(unsigned long) + sizeof(int));

	return raw > extra ? raw - extra : 0;
}

static unsigned long toi_bio_storage_available(void)
{
	unsigned long sum = 0;
	struct toi_module_ops *this_module;

	list_for_each_entry(this_module, &toi_modules, module_list) {
		if (!this_module->enabled || this_module->type != BIO_ALLOCATOR_MODULE)
			continue;
		toi_message(TOI_BIO, TOI_VERBOSE, 0, "Seeking storage "
			    "available from %s.", this_module->name);
		sum += this_module->bio_allocator_ops->storage_available();
	}

	toi_message(TOI_BIO, TOI_VERBOSE, 0, "Total storage available is %lu "
		    "pages (%d header pages).", sum, header_pages_reserved);

	return sum > header_pages_reserved ? raw_to_real(sum - header_pages_reserved) : 0;

}

static unsigned long toi_bio_storage_allocated(void)
{
	return raw_pages_allocd > header_pages_reserved ?
	    raw_to_real(raw_pages_allocd - header_pages_reserved) : 0;
}

/*
                                                                       
                                  
 */
static void toi_bio_noresume_reset(void)
{
	toi_message(TOI_BIO, TOI_VERBOSE, 0, "toi_bio_noresume_reset.");
	toi_rw_cleanup(READ);
	free_all_bdev_info();
}

/* 
                                              
                                                
  */
static void toi_bio_cleanup(int finishing_cycle)
{
	if (!finishing_cycle)
		return;

	if (toi_writer_buffer) {
		toi_free_page(11, (unsigned long)toi_writer_buffer);
		toi_writer_buffer = NULL;
	}

	forget_signature_page();

	if (header_block_device && toi_sig_data && toi_sig_data->header_dev_t != resume_dev_t)
		toi_close_bdev(header_block_device);

	header_block_device = NULL;

	close_resume_dev_t(0);
}

static int toi_bio_write_header_init(void)
{
	int result;

	toi_message(TOI_BIO, TOI_VERBOSE, 0, "toi_bio_write_header_init");
	toi_rw_init(WRITE, 0);
	toi_writer_buffer_posn = 0;

	/*                                                          
                                                                 
                                                                    
                                      
                                                                   
                                                                 
                                                 
  */

	toi_message(TOI_BIO, TOI_VERBOSE, 0, "serialise extent chains.");
	result = toi_serialise_extent_chains();

	if (result)
		return result;

	/*
                                                                  
                                          
  */
	toi_message(TOI_BIO, TOI_VERBOSE, 0, "serialise signature page.");
	return toi_rw_header_chunk_noreadahead(WRITE, &toi_blockwriter_ops,
					       (char *)toi_cur_sig_page, PAGE_SIZE);
}

static int toi_bio_write_header_cleanup(void)
{
	int result = 0;

	if (toi_writer_buffer_posn)
		toi_bio_queue_write(&toi_writer_buffer);

	result = toi_finish_all_io();

	unowned = 0;
	total_header_bytes = 0;

	/*                                        */
	if (!result)
		result = toi_bio_mark_have_image();

	return result;
}

/*
                             
  
               
                                                        
                                                              
                                                         
                                                            
                                                               
                                     
  
           
                                                        
                                                                 
            
 */

static int toi_bio_read_header_init(void)
{
	int result = 0;
#if 0				/*                                                                           */
	char buf[32];
#endif

	toi_writer_buffer_posn = 0;

	toi_message(TOI_BIO, TOI_VERBOSE, 0, "toi_bio_read_header_init");

	if (!toi_sig_data) {
		printk(KERN_INFO "toi_bio_read_header_init called when we "
		       "haven't verified there is an image!\n");
		return -EINVAL;
	}

	/*
                                                                        
          
  */
	toi_message(TOI_BIO, TOI_VERBOSE, 0, "Header dev_t is %lx.", toi_sig_data->header_dev_t);
#if 0				/*                                                                           */
	if (toi_sig_data->have_uuid) {
		struct fs_info seek;
		dev_t device;

		strncpy((char *)seek.uuid, toi_sig_data->header_uuid, 16);
		seek.dev_t = toi_sig_data->header_dev_t;
		seek.last_mount_size = 0;
		device = blk_lookup_fs_info(&seek);
		if (device) {
			printk("Using dev_t %s, returned by blk_lookup_fs_info.\n",
			       format_dev_t(buf, device));
			toi_sig_data->header_dev_t = device;
		}
	}
#endif
	if (toi_sig_data->header_dev_t != resume_dev_t) {
		header_block_device = toi_open_bdev(NULL, toi_sig_data->header_dev_t, 1);

		if (IS_ERR(header_block_device))
			return PTR_ERR(header_block_device);
	} else
		header_block_device = resume_block_device;

	if (!toi_writer_buffer)
		toi_writer_buffer = (char *)toi_get_zeroed_page(11, TOI_ATOMIC_GFP);
	more_readahead = 1;

	/*
                                
                                                
  */
	result = toi_bio_ops.bdev_page_io(READ, header_block_device,
					  toi_sig_data->first_header_block,
					  virt_to_page((unsigned long)toi_writer_buffer));
	if (result)
		return result;

	toi_message(TOI_BIO, TOI_VERBOSE, 0, "load extent chains.");
	result = toi_load_extent_chains();

	toi_message(TOI_BIO, TOI_VERBOSE, 0, "load original signature page.");
	toi_orig_sig_page = (char *)toi_get_zeroed_page(38, TOI_ATOMIC_GFP);
	if (!toi_orig_sig_page) {
		printk(KERN_ERR "Failed to allocate memory for the current" " image signature.\n");
		return -ENOMEM;
	}

	return toi_rw_header_chunk_noreadahead(READ, &toi_blockwriter_ops,
					       (char *)toi_orig_sig_page, PAGE_SIZE);
}

static int toi_bio_read_header_cleanup(void)
{
	toi_message(TOI_BIO, TOI_VERBOSE, 0, "toi_bio_read_header_cleanup.");
	return toi_rw_cleanup(READ);
}

/*                                                       */
#define TOLOWER(x) ((x) | 0x20)

/*
                                                              
        
 */
char *uuid_from_commandline(char *commandline)
{
	int low = 0;
	char *result = NULL, *output, *ptr;

	if (strncmp(commandline, "UUID=", 5))
		return NULL;

	result = kzalloc(17, GFP_KERNEL);
	if (!result) {
		printk("Failed to kzalloc UUID text memory.\n");
		return NULL;
	}

	ptr = commandline + 5;
	output = result;

	while (*ptr && (output - result) < 16) {
		if (isxdigit(*ptr)) {
			int value = isdigit(*ptr) ? *ptr - '0' : TOLOWER(*ptr) - 'a' + 10;
			if (low) {
				*output += value;
				output++;
			} else {
				*output = value << 4;
			}
			low = !low;
		} else if (*ptr != '-')
			break;
		ptr++;
	}

	if ((output - result) < 16 || *ptr) {
		printk(KERN_DEBUG "Found resume=UUID=, but the value looks " "invalid.\n");
		kfree(result);
		result = NULL;
	}

	return result;
}

#define retry_if_fails(command) \
do { \
	command; \
	if (!resume_dev_t && !waited_for_device_probe) { \
		wait_for_device_probe(); \
		command; \
		waited_for_device_probe = 1; \
	} \
} while (0)

/* 
                                                           
  
                                                                             
                                                                            
                                                
 */
static int try_to_open_resume_device(char *commandline, int quiet)
{
	struct kstat stat;
	int error = 0;
	char *uuid = uuid_from_commandline(commandline);
	int waited_for_device_probe = 0;

	hib_log("commandline=\"%s\"\n", commandline);
	resume_dev_t = MKDEV(0, 0);

	if (!strlen(commandline)) {
#ifdef CONFIG_TOI_FIXUP
		hib_warn("skip scanning for image...\n");
#else
		retry_if_fails(toi_bio_scan_for_image(quiet));
#endif
	}

	if (uuid) {
		struct fs_info seek;
		strncpy((char *)&seek.uuid, uuid, 16);
		seek.dev_t = resume_dev_t;
		seek.last_mount_size = 0;
		retry_if_fails(resume_dev_t = blk_lookup_fs_info(&seek));
		kfree(uuid);
	}

	if (!resume_dev_t)
		retry_if_fails(resume_dev_t = name_to_dev_t(commandline));

	if (!resume_dev_t) {
		struct file *file = filp_open(commandline,
					      O_RDONLY | O_LARGEFILE, 0);

		if (!IS_ERR(file) && file) {
			vfs_getattr(&file->f_path, &stat);
			filp_close(file, NULL);
		} else
			error = vfs_stat(commandline, &stat);
		if (!error)
			resume_dev_t = stat.rdev;
	}

	if (!resume_dev_t) {
		if (quiet)
			return 1;

		if (test_toi_state(TOI_TRYING_TO_RESUME))
			toi_early_boot_message(1, toi_translate_err_default,
					       "Failed to translate \"%s\" into a device id.\n",
					       commandline);
		else if (strlen(commandline))
			printk("TuxOnIce: Can't translate \"%s\" into a device "
			       "id yet.\n", commandline);
		return 1;
	}

	return open_resume_dev_t(1, quiet);
}

/*
                       
  
                                        
                       
                                                       
  
         
                                                     
                                                                 
                                                                       
                                                                 
                                                              
                                                              
             
 */
static int toi_bio_parse_sig_location(char *commandline, int only_allocator, int quiet)
{
	char *thischar, *devstart, *colon = NULL;
	int signature_found, result = -EINVAL, temp_result = 0;

	if (strncmp(commandline, "swap:", 5) && strncmp(commandline, "file:", 5)) {
		/*
                                                              
                                                            
                                     
   */
		if (strncmp(commandline, "/dev/", 5) &&
		    strncmp(commandline, "UUID=", 5) && strlen(commandline))
			return 1;
	} else
		commandline += 5;

	devstart = commandline;
	thischar = commandline;
	while ((*thischar != ':') && (*thischar != '@') &&
	       ((thischar - commandline) < 250) && (*thischar))
		thischar++;

	if (*thischar == ':') {
		colon = thischar;
		*colon = 0;
		thischar++;
	}

	while ((thischar - commandline) < 250 && *thischar)
		thischar++;

	if (colon) {
		unsigned long block;
		temp_result = strict_strtoul(colon + 1, 0, &block);
		if (!temp_result)
			resume_firstblock = (int)block;
	} else if (swsusp_resume_block) {
		resume_firstblock = swsusp_resume_block;
	} else
		resume_firstblock = 0;

	clear_toi_state(TOI_CAN_HIBERNATE);
	clear_toi_state(TOI_CAN_RESUME);

	if (!temp_result)
		temp_result = try_to_open_resume_device(devstart, quiet);

	if (colon)
		*colon = ':';

	/*                             */
	if (temp_result)
		return strlen(commandline) ? -EINVAL : 1;

	signature_found = toi_bio_image_exists(quiet);

	if (signature_found != -1) {
		result = 0;
		/*
                                                             
                                             
   */
		set_toi_state(TOI_CAN_HIBERNATE);
		set_toi_state(TOI_CAN_RESUME);
	} else if (!quiet)
		printk(KERN_ERR "TuxOnIce: Block I/O: No " "signature found at %s.\n", devstart);

	return result;
}

static void toi_bio_release_storage(void)
{
	header_pages_reserved = 0;
	raw_pages_allocd = 0;

	free_all_bdev_info();
}

/*                      
  
 */
static int toi_bio_remove_image(void)
{
	int result;

	toi_message(TOI_BIO, TOI_VERBOSE, 0, "toi_bio_remove_image.");

	result = toi_bio_restore_original_signature();

	/*
                                                                
                                                        
   
                                                         
                                
   
                                                         
                                                   
  */

	toi_bio_release_storage();

	return result;
}

struct toi_bio_ops toi_bio_ops = {
	.bdev_page_io = toi_bdev_page_io,
	.register_storage = toi_register_storage_chain,
	.free_storage = toi_bio_release_storage,
};
EXPORT_SYMBOL_GPL(toi_bio_ops);

static struct toi_sysfs_data sysfs_params[] = {
	SYSFS_INT("target_outstanding_io", SYSFS_RW, &target_outstanding_io,
		  0, 16384, 0, NULL),
};

struct toi_module_ops toi_blockwriter_ops = {
	.type = WRITER_MODULE,
	.name = "block i/o",
	.directory = "block_io",
	.module = THIS_MODULE,
	.memory_needed = toi_bio_memory_needed,
	.print_debug_info = toi_bio_print_debug_stats,
	.storage_needed = toi_bio_storage_needed,
	.save_config_info = toi_bio_save_config_info,
	.load_config_info = toi_bio_load_config_info,
	.initialise = toi_bio_initialise,
	.cleanup = toi_bio_cleanup,
	.post_atomic_restore = toi_bio_chains_post_atomic,

	.rw_init = toi_rw_init,
	.rw_cleanup = toi_rw_cleanup,
	.read_page = toi_bio_read_page,
	.write_page = toi_bio_write_page,
	.rw_header_chunk = toi_rw_header_chunk,
	.rw_header_chunk_noreadahead = toi_rw_header_chunk_noreadahead,
	.io_flusher = bio_io_flusher,
	.update_throughput_throttle = update_throughput_throttle,
	.finish_all_io = toi_finish_all_io,

	.noresume_reset = toi_bio_noresume_reset,
	.storage_available = toi_bio_storage_available,
	.storage_allocated = toi_bio_storage_allocated,
	.reserve_header_space = toi_bio_reserve_header_space,
	.allocate_storage = toi_bio_allocate_storage,
	.image_exists = toi_bio_image_exists,
	.mark_resume_attempted = toi_bio_mark_resume_attempted,
	.write_header_init = toi_bio_write_header_init,
	.write_header_cleanup = toi_bio_write_header_cleanup,
	.read_header_init = toi_bio_read_header_init,
	.read_header_cleanup = toi_bio_read_header_cleanup,
	.get_header_version = toi_bio_get_header_version,
	.remove_image = toi_bio_remove_image,
	.parse_sig_location = toi_bio_parse_sig_location,

	.sysfs_data = sysfs_params,
	.num_sysfs_entries = sizeof(sysfs_params) / sizeof(struct toi_sysfs_data),
};

/* 
                                                             
  
                                            
  */
static __init int toi_block_io_load(void)
{
	return toi_register_module(&toi_blockwriter_ops);
}

#ifdef MODULE
static __exit void toi_block_io_unload(void)
{
	toi_unregister_module(&toi_blockwriter_ops);
}
module_init(toi_block_io_load);
module_exit(toi_block_io_unload);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nigel Cunningham");
MODULE_DESCRIPTION("TuxOnIce block io functions");
#else
late_initcall(toi_block_io_load);
#endif
