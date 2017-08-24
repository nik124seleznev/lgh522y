/*
 * Compressed RAM block device
 *
 * Copyright (C) 2008, 2009, 2010  Nitin Gupta
 *
 * This code is released using a dual license strategy: BSD/GPL
 * You can choose the licence that better fits your requirements.
 *
 * Released under the terms of 3-clause BSD License
 * Released under the terms of GNU General Public License Version 2.0
 *
 * Project home: http://compcache.googlecode.com
 */

#ifndef _ZRAM_DRV_H_
#define _ZRAM_DRV_H_

#include <linux/spinlock.h>
#include <linux/mutex.h>

#include "../zsmalloc/zsmalloc.h"

/*
                                              
                                                  
 */
static const unsigned max_num_devices = 32;

/*                           */

/*                                          */
static const unsigned default_disksize_perc_ram = 50;	/*    */
/*                                     */
#define	DISKSIZE_ALIGNMENT	0x2000000		/*      */
/*                                                */
#define SUPPOSED_TOTALRAM	0x20000			/*       */

/*
                                                           
                          
 */
static const size_t max_zpage_size = PAGE_SIZE / 4 * 3;

/*
                                                      
                                                    
                         
 */

/*                              */

#define SECTOR_SHIFT		9
#define SECTOR_SIZE		(1 << SECTOR_SHIFT)
#define SECTORS_PER_PAGE_SHIFT	(PAGE_SHIFT - SECTOR_SHIFT)
#define SECTORS_PER_PAGE	(1 << SECTORS_PER_PAGE_SHIFT)
#define ZRAM_LOGICAL_BLOCK_SHIFT 12
#define ZRAM_LOGICAL_BLOCK_SIZE	(1 << ZRAM_LOGICAL_BLOCK_SHIFT)
#define ZRAM_SECTOR_PER_LOGICAL_BLOCK	\
	(1 << (ZRAM_LOGICAL_BLOCK_SHIFT - SECTOR_SHIFT))

/*                                             */
enum zram_pageflags {
	/*                                 */
	ZRAM_ZERO,

	__NR_ZRAM_PAGEFLAGS,
#ifdef CONFIG_ZSM
	ZRAM_FIRST_NODE ,
	ZRAM_RB_NODE 
#endif
};

/*                   */

/*                              */
struct table {
	unsigned long handle;
	u16 size;	/*                                */
	u8 count;	/*                                 */
	u8 flags;
#if CONFIG_ZSM
	struct rb_node node;
	struct list_head head;
	u32 copy_count;
	u32 next_index;
	u32 copy_index;
	u32 checksum;	
#endif
} __aligned(4);

struct zram_stats {
	u64 compr_size;		/*                                 */
	u64 num_reads;		/*                     */
	u64 num_writes;		/*        */
	u64 failed_reads;	/*                      */
	u64 failed_writes;	/*                                   */
	u64 invalid_io;		/*                               */
	u64 notify_free;	/*                                     */
#ifdef CONFIG_ZSM
	u64 zsm_saved;          /*                    */
	u64 zsm_saved4k;
#endif
	u32 pages_zero;		/*                          */
	u32 pages_stored;	/*                               */
	u32 good_compress;	/*                                        */
	u32 bad_compress;	/*                                        */
};

struct zram_meta {
	void *compress_workmem;
	void *compress_buffer;
	struct table *table;
	struct zs_pool *mem_pool;
};

struct zram {
	struct zram_meta *meta;
	spinlock_t stat64_lock;	/*                      */
	struct rw_semaphore lock; /*                                    
                                               
                                         */
	struct request_queue *queue;
	struct gendisk *disk;
	int init_done;
	/*                                                                    */
	struct rw_semaphore init_lock;
	/*
                                                               
                           
  */
	u64 disksize;	/*       */

	struct zram_stats stats;
};

extern struct zram *zram_devices;
unsigned int zram_get_num_devices(void);
#ifdef CONFIG_SYSFS
extern struct attribute_group zram_disk_attr_group;
#endif

extern void zram_reset_device(struct zram *zram);
extern struct zram_meta *zram_meta_alloc(u64 disksize);
extern void zram_meta_free(struct zram_meta *meta);
extern void zram_init_device(struct zram *zram, struct zram_meta *meta);

/*                                               */
#ifdef CONFIG_ZSM
typedef int (*comp_hook) (const unsigned char *, size_t , unsigned char *, size_t *, void *, int *);
#else
typedef int (*comp_hook) (const unsigned char *, size_t , unsigned char *, size_t *, void *);
#endif
typedef int (*decomp_hook) (const unsigned char *, size_t , unsigned char *, size_t *);

#endif
