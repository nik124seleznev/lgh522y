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

#ifndef _MTKPASR_DRV_H_
#define _MTKPASR_DRV_H_

#include <linux/version.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/swap.h>
#ifdef CONFIG_MTKPASR_MAFL
#include <linux/shrinker.h>
#endif

#include <mach/mt_spm_sleep.h>

#include "../zsmalloc/zsmalloc.h"

/*                  */
#define IS_MTKPASR_ENABLED		\
	do {				\
		if (!mtkpasr_enable)	\
			return 0;	\
	} while (0)

#define IS_MTKPASR_ENABLED_NORV		\
	do {				\
		if (!mtkpasr_enable)	\
			return;		\
	} while (0)

/*                      */
#define mtkpasr_print(level, x...)			\
	do {						\
		if (mtkpasr_debug_level >= level)	\
			printk(KERN_CRIT x);		\
	} while (0)

#define MTKPASR		"MTKPASR"
#ifdef CONFIG_MTKPASR_DEBUG
#define	mtkpasr_info(string, args...)	mtkpasr_print(3, "[%s]:[%s][%d] "string, MTKPASR, __func__, __LINE__, ##args)
#define mtkpasr_log(string, args...)    mtkpasr_print(2, "[%s]:[%s][%d] "string, MTKPASR, __func__, __LINE__, ##args)
#define	mtkpasr_err(string, args...)	mtkpasr_print(1, "[%s]:[%s][%d] "string, MTKPASR, __func__, __LINE__, ##args)
#else
#define	mtkpasr_info(string, args...)
#define mtkpasr_log(string, args...)    mtkpasr_print(2, "[%s]:[%s][%d] "string, MTKPASR, __func__, __LINE__, ##args)
#define	mtkpasr_err(string, args...)	mtkpasr_print(1, "[%s]:[%s][%d] "string, MTKPASR, __func__, __LINE__, ##args)
#endif

/*                                                                                                                               */
/*                                    */
/*                                                                                                                               */
#ifndef CONFIG_64BIT
#define MTKPASR_MAX_EXTCOMP	0x801
#else
#define MTKPASR_MAX_EXTCOMP	0x0
#endif

#define MTKPASR_FLUSH() do {				\
				lru_add_drain_all();	\
				drain_all_pages();	\
			} while (0)			\

/*                                                                                      */
#define MTKPASR_CHECK_ABORTED		(SWAP_CLUSTER_MAX >> 1)
/*                                 */
#define MTKPASR_CHECK_MIGRATE		SWAP_CLUSTER_MAX

#ifdef CONFIG_MTKPASR_MAFL
/*                                                     */
#define PAGE_ORDER(page)	page_private(page)
#endif

/*                      */
#define BANK_INUSED(i)		(mtkpasr_banks[i].inused)
/*              */
#define BANK_RANK(i)		(mtkpasr_banks[i].rank)

/*                                                   */
#define CHECK_PENDING_WAKEUP	spm_check_wakeup_src()	/*                   */

/*             */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0)
#define PAGE_EVICTABLE(page, vma)	page_evictable(page, vma)
#define ZS_CREATE_POOL(name, flags)	zs_create_pool(name, flags)
#else
#define PAGE_EVICTABLE(page, vma)	page_evictable(page)
#define ZS_CREATE_POOL(name, flags)	zs_create_pool(flags)
#endif

/*                           */

/*                                             */
static const unsigned default_disksize_perc_ram = 50;

/*
                                                           
                          
 */
static const size_t max_cmpr_size = PAGE_SIZE / 4 * 3;

/*                              */

/*                                                */
enum mtkpasr_pageflags {
	/*                                 */
	MTKPASR_ZERO,

	__NR_MTKPASR_PAGEFLAGS,
};

/*                   */

/*                              */
struct table {
	unsigned long handle;
	u16 size;		/*                                */
	u8 count;		/*                                 */
	u8 flags;
	void *obj;
} __aligned(4);

struct mtkpasr_stats {
	u64 compr_size;		/*                                 */
	u64 failed_reads;	/*                      */
	u64 failed_writes;	/*                                   */
	u32 pages_zero;		/*                          */
	u32 pages_stored;	/*                               */
	u32 good_compress;	/*                                        */
	u32 bad_compress;	/*                                        */
};

struct mtkpasr {

	void *compress_workmem;
	void *compress_buffer;

	spinlock_t stat64_lock;	/*                      */
	int init_done;
	/*                                                                    */
	struct rw_semaphore init_lock;

	/*            */
	unsigned long index;

	struct mtkpasr_stats stats;
};

/*
                               
                                   
                                     
                                     
                                      
                                                         
 */

#define MTKPASR_SROFF	0xFFFFFFFF
#define MTKPASR_RDPDON	0xFFFFFFFE
#define MTKPASR_SEGMENT_CH0	0x000000FF
#define MTKPASR_SEGMENT_CH1	0x0000FF00
#define MTKPASR_SEGMENT_CH2	0x00FF0000
#define MTKPASR_SEGMENT_CH3	0xFF000000

/*                           */
struct nopasr_bank {
	unsigned long start_pfn;	/*             */
	unsigned long end_pfn;		/*                                  */
	u32 inused;			/*                                                */
	u32 segment;			/*                                */
};

/*                                */
struct mtkpasr_bank {
	unsigned long start_pfn;	/*             */
	unsigned long end_pfn;		/*                                  */
	u32 inused;			/*                                                */
	u32 valid_pages;		/*                          */
	void *rank;			/*                 */
	u32 segment;			/*                                */
#ifdef CONFIG_MTKPASR_MAFL
	struct list_head mafl;		/*                                                                          */
	int inmafl;			/*                                   */
#endif
	union {
		u32 comp_pos;
		struct {
			s16 comp_start;	/*                               */
			s16 comp_end;	/*                                              */
		};
	};
};

#define MTKPASR_DPDON	0xFFFF		/*                                                            */
/*                               */
struct mtkpasr_rank {
	u16 start_bank;			/*              */
	u16 end_bank;			/*               */
	u16 hw_rank;			/*                                */
	u16 inused;			/*                                                */
};

/*                */
enum mtkpasr_phase {
	MTKPASR_OFF,			/*             */
	MTKPASR_ENTERING,		/*                                                            */
	MTKPASR_DISABLINGSR,		/*                                   */
	MTKPASR_RESTORING,		/*                                                                                           */
	MTKPASR_EXITING,		/*                                                             */
	MTKPASR_ENABLINGSR,		/*                                  */
	MTKPASR_ON,			/*            */
	MTKPASR_DPD_OFF,		/*            */
	MTKPASR_DPD_ON,			/*           */
	MTKPASR_PHASE_TOTAL,
	MTKPASR_SUCCESS,
	MTKPASR_FAIL,
	MTKPASR_WRONG_STATE,		/*              */
	MTKPASR_GET_WAKEUP,		/*                                                                                    */
	MTKPASR_PHASE_ONE,
	MTKPASR_PHASE_TWO,
};

/*                     */
struct mtkpasr_bank_cc {
	int to_bank;
	unsigned long to_cursor;
};

extern struct mtkpasr *mtkpasr_device;
#ifdef CONFIG_SYSFS
extern struct attribute_group mtkpasr_attr_group;
#endif

/*                */
extern int mtkpasr_enable;
extern int mtkpasr_enable_sr;

/*              */
extern int mtkpasr_debug_level;

/*                 */
/*                 */
/*                 */
/*                                                                 */
extern int compute_valid_pasr_range(unsigned long *start_pfn, unsigned long *end_pfn, int *num_ranks);
/*                                                                                      */
extern int __init query_bank_information(int bank, unsigned long *spfn, unsigned long *epfn, bool fully);
/*                                            */
extern u32 pasr_bank_to_segment(unsigned long start_pfn, unsigned long end_pfn);

/*                */
extern int mtkpasr_show_banks(char *);

#ifdef CONFIG_MTKPASR_MAFL
#define MAX_OPS_INVARIANT	(3)
#define MAX_NO_OPS_INVARIANT	(MAX_OPS_INVARIANT << 2)
#define KEEP_NO_OPS		(0x7FFFFFFF)
extern unsigned long mtkpasr_show_page_reserved(void);
extern bool mtkpasr_no_phaseone_ops(void);
extern bool mtkpasr_no_ops(void);
#endif

extern enum mtkpasr_phase mtkpasr_entering(void);
extern enum mtkpasr_phase mtkpasr_disablingSR(u32 *sr, u32 *dpd);
extern enum mtkpasr_phase mtkpasr_enablingSR(void);
extern enum mtkpasr_phase mtkpasr_exiting(void);
extern void mtkpasr_restoring(void);

extern void mtkpasr_reset_slots(void);
extern int mtkpasr_init_device(struct mtkpasr *mtkpasr);
extern void __mtkpasr_reset_device(struct mtkpasr *mtkpasr);
extern int mtkpasr_forward_rw(struct mtkpasr *mtkpasr, u32 index, struct page *page, int rw);

extern int mtkpasr_acquire_total(void);
extern int mtkpasr_acquire_frees(void);

extern void set_mtkpasr_triggered(void);
extern void clear_mtkpasr_triggered(void);
extern bool is_mtkpasr_triggered(void);

#endif
