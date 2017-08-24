/*
                    
 */

#define pr_fmt(fmt) "["KBUILD_MODNAME"]" fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/buffer_head.h>
#include <linux/device.h>
#include <linux/genhd.h>
#include <linux/highmem.h>
#include <linux/slab.h>
#include <linux/lzo.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <linux/swap.h>
#include <linux/suspend.h>
#include <linux/earlysuspend.h>
#include <linux/migrate.h>
#include "mtkpasr_drv.h"

/*                         */

/*                     */
static struct zs_pool *mtkpasr_mem_pool;
static struct table *mtkpasr_table;
static u64 mtkpasr_disksize;		/*       */
static u32 mtkpasr_total_slots;
static u32 mtkpasr_free_slots;

/*                         */
static struct mtkpasr_bank *mtkpasr_banks;
static struct mtkpasr_rank *mtkpasr_ranks;
static int num_banks;
static int num_ranks;
static unsigned long pages_per_bank;
static unsigned long mtkpasr_start_pfn;
static unsigned long mtkpasr_end_pfn;
static unsigned long mtkpasr_total_pfns;

/*                                        */
#ifdef CONFIG_MTKPASR_MAFL
static unsigned int mtkpasr_ops_invariant;
static unsigned int prev_mafl_count;
static unsigned int before_mafl_count;
#endif

/*                           */
static struct nopasr_bank *nopasr_banks;
static int num_nopasr_banks;

/*                              */
static LIST_HEAD(fromlist);
static LIST_HEAD(tolist);
static int fromlist_count;
static int tolist_count;
static unsigned long mtkpasr_migration_end;
static unsigned long mtkpasr_last_scan;
static int mtkpasr_admit_order;

/*                             */
int mtkpasr_enable = 1;
int mtkpasr_enable_sr = 1;

/*              */
#ifdef CONFIG_MT_ENG_BUILD
int mtkpasr_debug_level = 3;
#else
int mtkpasr_debug_level = 1;
#endif

/*         */
struct mtkpasr *mtkpasr_device;

/*                  */
/*                  */
/*                  */

/*                         */
extern int pasr_find_free_page(struct page *page, struct list_head *freelist);
/*                                         */
extern int pasr_compute_safe_order(void);

/*          */
extern unsigned long pasrbank_pfns;

/*              */
/*              */
/*              */

/*               */
#ifdef CONFIG_MTKPASR_ALLEXTCOMP
extern int mtkpasr_isolate_page(struct page *page, int check_swap);
#else
extern int mtkpasr_isolate_page(struct page *page);
#endif
/*                               */
extern int mtkpasr_drop_page(struct page *page);

#ifdef NO_UART_CONSOLE
extern unsigned char mtkpasr_log_buf[4096];
#endif

#define MTKPASR_EXHAUSTED	(low_wmark_pages(MTKPASR_ZONE) + pageblock_nr_pages)
/*                */
int mtkpasr_show_banks(char *buf)
{
	int i, j, len = 0, tmp;

	if (mtkpasr_device->init_done == 0)
		return sprintf(buf, "MTKPASR is not initialized!\n");

	/*          */
	tmp = sprintf(buf, "num_banks[%d] num_ranks[%d] mtkpasr_start_pfn[%ld] mtkpasr_end_pfn[%ld] mtkpasr_total_pfns[%ld]\n",
			num_banks, num_ranks, mtkpasr_start_pfn, mtkpasr_end_pfn, mtkpasr_total_pfns);
	buf += tmp;
	len += tmp;

	/*                    */
	for (i = 0; i < num_ranks; i++) {
		tmp = sprintf(buf, "Rank[%d] - start_bank[%d] end_bank[%d]\n", i, mtkpasr_ranks[i].start_bank, mtkpasr_ranks[i].end_bank);
		buf += tmp;
		len += tmp;
		for (j = mtkpasr_ranks[i].start_bank; j <= mtkpasr_ranks[i].end_bank; j++) {
			tmp = sprintf(buf, "  Bank[%d] - start_pfn[0x%lx] end_pfn[0x%lx] inmafl[%d] segment[%d]\n", j, mtkpasr_banks[j].start_pfn, mtkpasr_banks[j].end_pfn-1, mtkpasr_banks[j].inmafl, mtkpasr_banks[j].segment);
			buf += tmp;
			len += tmp;
		}
	}

	/*                      */
	for (i = 0; i < num_banks; i++) {
		if (mtkpasr_banks[i].rank == NULL) {
			tmp = sprintf(buf, "Bank[%d] - start_pfn[0x%lx] end_pfn[0x%lx] inmafl[%d] segment[%d]\n", i, mtkpasr_banks[i].start_pfn, mtkpasr_banks[i].end_pfn-1, mtkpasr_banks[i].inmafl, mtkpasr_banks[i].segment);
			buf += tmp;
			len += tmp;
		}
	}

	/*        */
	tmp = sprintf(buf, "Exhausted level[%ld]\n", (unsigned long)MTKPASR_EXHAUSTED);
	buf += tmp;
	len += tmp;

#ifdef NO_UART_CONSOLE
	memcpy(buf, mtkpasr_log_buf, 1024);
	len += 1024;
#endif

	return len;
}

static void mtkpasr_free_page(struct mtkpasr *mtkpasr, size_t index)
{
	unsigned long handle = mtkpasr_table[index].handle;

	if (unlikely(!handle)) {
		/*
                                                  
                                 
   */
		return;
	}

	zs_free(mtkpasr_mem_pool, handle);

	/*                                              */
	mtkpasr_table[index].handle = 0;
	mtkpasr_table[index].size = 0;
	mtkpasr_table[index].obj = NULL;
}

/*            */
static void handle_zero_page(struct page *page)
{
	void *user_mem;

	user_mem = kmap_atomic(page);
	memset(user_mem, 0x5D, PAGE_SIZE);
	kunmap_atomic(user_mem);

	flush_dcache_page(page);
}

static int mtkpasr_read(struct mtkpasr *mtkpasr, u32 index, struct page *page)
{
	int ret;
	size_t clen;
	unsigned char *user_mem, *cmem;

	/*                                                  */
	page = mtkpasr_table[index].obj;
	if (page == NULL) {
		mtkpasr_err("\n\n\n\nNull Page!\n\n\n\n");
		return 0;
	}

	/*                                                  */
	if (unlikely(!mtkpasr_table[index].handle)) {
		mtkpasr_err("Not present : page pfn[%ld]\n", page_to_pfn(page));
		handle_zero_page(page);
		return 0;
	}

	user_mem = kmap_atomic(page);
	clen = PAGE_SIZE;
	cmem = zs_map_object(mtkpasr_mem_pool, mtkpasr_table[index].handle, ZS_MM_RO);

	if (mtkpasr_table[index].size == PAGE_SIZE) {
		memcpy(user_mem, cmem, PAGE_SIZE);
		ret = LZO_E_OK;
	} else {
		ret = lzo1x_decompress_safe(cmem, mtkpasr_table[index].size, user_mem, &clen);
	}

	zs_unmap_object(mtkpasr_mem_pool, mtkpasr_table[index].handle);

	kunmap_atomic(user_mem);

	/*                                                   */
	if (unlikely(ret != LZO_E_OK)) {
		mtkpasr_err("Decompression failed! err=%d, page=%u, pfn=%lu\n", ret, index, page_to_pfn(page));
		/*                 */
		/*           */	/*                   */
	}

	/*                                                         
                          */

	/*                  */
	mtkpasr_free_page(mtkpasr, index);

	return 0;
}

static int mtkpasr_write(struct mtkpasr *mtkpasr, u32 index, struct page *page)
{
	int ret;
	size_t clen;
	unsigned long handle;
	unsigned char *user_mem, *cmem, *src;

	src = mtkpasr->compress_buffer;

	/*
                                                            
                         
  */

	user_mem = kmap_atomic(page);

	ret = lzo1x_1_compress(user_mem, PAGE_SIZE, src, &clen, mtkpasr->compress_workmem);

	kunmap_atomic(user_mem);

	if (unlikely(ret != LZO_E_OK)) {
		mtkpasr_err("Compression failed! err=%d\n", ret);
		ret = -EIO;
		goto out;
	}

	if (unlikely(clen > max_cmpr_size)) {
		src = NULL;
		clen = PAGE_SIZE;
	}

	handle = zs_malloc(mtkpasr_mem_pool, clen);
	if (!handle) {
		mtkpasr_err("Error allocating memory for compressed "
			"page: %u, size=%zu\n", index, clen);
		ret = -ENOMEM;
		goto out;
	}
	cmem = zs_map_object(mtkpasr_mem_pool, handle, ZS_MM_WO);

	if (clen == PAGE_SIZE)
		src = kmap_atomic(page);
	memcpy(cmem, src, clen);
	if (clen == PAGE_SIZE)
		kunmap_atomic(src);

	zs_unmap_object(mtkpasr_mem_pool, handle);

	/*                             */
	mtkpasr_table[index].handle = handle;
	mtkpasr_table[index].size = clen;
	mtkpasr_table[index].obj = page;

	return 0;

out:
	return ret;
}

/*                                                               */
/*
                         
 */
int mtkpasr_forward_rw(struct mtkpasr *mtkpasr, u32 index, struct page *page, int rw)
{
	int ret = -ENOMEM;

	if (rw == READ) {
		ret = mtkpasr_read(mtkpasr, index, page);
		mtkpasr_free_slots = (!!ret) ? mtkpasr_free_slots : (mtkpasr_free_slots + 1);
	} else {
		/*               */
		if (mtkpasr_free_slots == 0) {
			mtkpasr_log("No free slots!\n");
			return ret;
		}
		ret = mtkpasr_write(mtkpasr, index, page);
		mtkpasr_free_slots = (!!ret) ? mtkpasr_free_slots : (mtkpasr_free_slots - 1);
	}

	return ret;
}
EXPORT_SYMBOL(mtkpasr_forward_rw);

/*                                  */
int mtkpasr_acquire_frees(void)
{
	return mtkpasr_free_slots;
}
EXPORT_SYMBOL(mtkpasr_acquire_frees);

/*                                   */
int mtkpasr_acquire_total(void)
{
	return mtkpasr_total_slots;
}
EXPORT_SYMBOL(mtkpasr_acquire_total);

/*                                                 */
void mtkpasr_reset_slots(void)
{
	size_t index;

	/*                                                      */
	for (index = 0; index < mtkpasr_total_slots; index++) {
		unsigned long handle = mtkpasr_table[index].handle;
		if (!handle)
			continue;

		zs_free(mtkpasr_mem_pool, handle);

		/*                                              */
		mtkpasr_table[index].handle = 0;
		mtkpasr_table[index].size = 0;
		mtkpasr_table[index].obj = NULL;

		/*        */
		mtkpasr_free_slots++;
	}

#ifdef CONFIG_MTKPASR_DEBUG
	if (mtkpasr_free_slots != mtkpasr_total_slots) {
		BUG();
	}
#endif
}

/*                             */
/*                             */
/*                             */

/*                                                                              */
/*                                                    */
static struct page *mtkpasr_alloc(struct page *migratepage, unsigned long data, int **result)
{
#if 1	/*           */
	struct page *page = NULL, *end_page;
	struct zone *z;
	/*                    */
	int found;
	int order;

	/*                                 */
	if (unlikely(mtkpasr_admit_order < 0)) {
		return NULL;
	}
retry:
	/*                               */
	if (!list_empty(&tolist)) {
		page = list_entry(tolist.next, struct page, lru);
		list_del(&page->lru);
#ifdef CONFIG_MTKPASR_DEBUG
		--tolist_count;
#endif
	} else {
		/*                                               */
		if (mtkpasr_last_scan < mtkpasr_migration_end) {
			mtkpasr_last_scan = mtkpasr_start_pfn - pageblock_nr_pages;
			/*                                               */
			mtkpasr_admit_order = -1;
			return NULL;
		}
		/*                       */
		page = pfn_to_page(mtkpasr_last_scan);
		end_page = pfn_to_page(mtkpasr_last_scan + pageblock_nr_pages);
		z = page_zone(page);
		while (page < end_page) {
			/*                */
			/*                                         */
			/*                 */
			if (!PageBuddy(page)) {
				/*                                              */
				++page;
				continue;
			}
			/*             */
			order = PAGE_ORDER(page);
			if (order > mtkpasr_admit_order) {
				/*                                              */
				page += (1 << order);
				continue;
			}
			/*        */
			found = pasr_find_free_page(page, &tolist);
			/*                  */
			/*                                              */
			/*              */
#ifdef CONFIG_MTKPASR_DEBUG
			tolist_count += found;
#endif
			page += found;
		}
		/*                         */
		mtkpasr_last_scan -= pageblock_nr_pages;

		/*       */
		goto retry;
	}

	return page;
#else
	/*                     */
	return alloc_pages(__GFP_HIGHMEM|GFP_ATOMIC, 0);
#endif
}

/*                                                                                 */
#define SAFE_ORDER	(THREAD_SIZE_ORDER + 1)
static struct zone *nz;
static unsigned long safe_level;
static int pasr_check_free_safe(void)
{
	unsigned long free = zone_page_state(nz, NR_FREE_PAGES);

	/*                                                     */
	free = free - MTKPASR_ZONE->free_area[0].nr_free - (MTKPASR_ZONE->free_area[1].nr_free << 1);

	/*           */
	if (free > safe_level) {
		return 0;
	}

	return -1;
}

/*                                      */

/*                                                               */
static void *src_pgmap;
/*                                        */
static unsigned long *extcomp;
/*                                  */
static unsigned long *sorted_for_extcomp;
/*               */
static enum mtkpasr_phase mtkpasr_status = MTKPASR_OFF;
/*                                          */
static atomic_t sloti;

#ifdef CONFIG_MTKPASR_MAFL
static unsigned long mafl_total_count;
unsigned long mtkpasr_show_page_reserved(void)
{
	return mafl_total_count;
}
bool mtkpasr_no_phaseone_ops(void)
{
	int safe_mtkpasr_pfns;

	safe_mtkpasr_pfns = mtkpasr_total_pfns >> 1;
	return ((prev_mafl_count == mafl_total_count) || (mafl_total_count > safe_mtkpasr_pfns));
}
bool mtkpasr_no_ops(void)
{
	return ((mafl_total_count == mtkpasr_total_pfns) || (mtkpasr_ops_invariant > MAX_OPS_INVARIANT));
}
#endif

/*                            */
void mtkpasr_reset_state(void)
{
	mtkpasr_reset_slots();
	mtkpasr_status = MTKPASR_OFF;
}

/*                                                                         
                                                                   
  
                                                                     
                      
 */
enum mtkpasr_phase mtkpasr_enablingSR(void)
{
	enum mtkpasr_phase result = MTKPASR_SUCCESS;
	int check_dpd = 0;
	int i, j;

	/*              */
	if (mtkpasr_status != MTKPASR_ON && mtkpasr_status != MTKPASR_DPD_ON) {
		mtkpasr_err("Error Current State [%d]!\n", mtkpasr_status);
		return MTKPASR_WRONG_STATE;
	} else {
		check_dpd = (mtkpasr_status == MTKPASR_DPD_ON) ? 1 : 0;
		/*                                */
		mtkpasr_status = MTKPASR_ENABLINGSR;
	}

	/*                  */
	if (check_dpd) {
		for (i = 0; i < num_ranks; i++) {
			if (mtkpasr_ranks[i].inused == MTKPASR_DPDON) {
				/*            */
				mtkpasr_ranks[i].inused = 0;
				/*                         */
				for (j = mtkpasr_ranks[i].start_bank; j <= mtkpasr_ranks[i].end_bank; j++) {
					mtkpasr_banks[j].inused = 0;
				}
				mtkpasr_info("Call DPDOFF API!\n");
			}
		}
	}

	/*                 */
	for (i = 0; i < num_banks; i++) {
		if (mtkpasr_banks[i].inused == MTKPASR_SROFF) {
			/*            */
			mtkpasr_banks[i].inused = 0;
			mtkpasr_info("Call SPM SR/ON API on bank[%d]!\n", i);
		} else {
			mtkpasr_info("Bank[%d] free[%d]!\n", i, mtkpasr_banks[i].inused);
		}
	}

	/*                                                               */
	if (result == MTKPASR_SUCCESS) {
		mtkpasr_status = MTKPASR_EXITING;
	}

	return result;
}

/*                                                                                  
  
                               
                           
                                            
 */
enum mtkpasr_phase mtkpasr_exiting(void)
{
	enum mtkpasr_phase result = MTKPASR_SUCCESS;
	int current_index;
	int ret = 0;
	struct mtkpasr *mtkpasr;
	int should_flush_cache = 0;
#ifdef CONFIG_MTKPASR_DEBUG
	int decompressed = 0;
#endif

	mtkpasr_info("\n");

	/*              */
	if (mtkpasr_status != MTKPASR_EXITING) {
		mtkpasr_err("Error Current State [%d]!\n", mtkpasr_status);
		/*
                                                                                   
                                                                
   */
		return MTKPASR_WRONG_STATE;
	}

	/*                     */
	mtkpasr = &mtkpasr_device[0];

	/*                  */
	current_index = atomic_dec_return(&sloti);
	should_flush_cache = current_index;
	while (current_index >= 0) {
		ret = mtkpasr_forward_rw(mtkpasr, current_index, NULL, READ);
		/*                            */
		if (unlikely(ret)) {
			break;
		}
#ifdef CONFIG_MTKPASR_DEBUG
		++decompressed;
#endif
		/*      */
		current_index = atomic_dec_return(&sloti);
	}

	/*                            */
	if (ret) {
		mtkpasr_err("Failed Decompression!\n");
		/*
                                                                                   
                                                                
   */
		result = MTKPASR_FAIL;
	}

	/*                                    */
	if (result == MTKPASR_SUCCESS) {
		mtkpasr_status = MTKPASR_OFF;
	}

	/*                                     */
	if (should_flush_cache >= 0)
		flush_cache_all();

#ifdef CONFIG_MTKPASR_DEBUG
	mtkpasr_info("Decompressed pages [%d]\n", decompressed);
#endif
	return result;
}

/*                                                                                  */
void mtkpasr_restoring(void)
{
	mtkpasr_info("\n");

	/*              */
	if (mtkpasr_status != MTKPASR_ENTERING && mtkpasr_status != MTKPASR_DISABLINGSR) {
		mtkpasr_err("Error Current State [%d]!\n", mtkpasr_status);
		return;
	} else {
		/*                               */
		mtkpasr_status = MTKPASR_RESTORING;
	}

	/*                                                                                                                  */
	mtkpasr_reset_slots();

	/*                         */
	mtkpasr_status = MTKPASR_OFF;

	mtkpasr_info("(END)\n");
}

/*
                                           
                       
                           
 */
static int check_if_compressed(long start, long end, int pfn)
{
	long mid;
	int found = 0;

	/*         */
	end = end - 1;

	/*                 */
	while (start <= end) {
		mid = (start + end) >> 1;
		if (pfn == extcomp[mid]) {
			found = 1;
			break;
		} else if (pfn > extcomp[mid]) {
			start = mid + 1;
		} else {
			end = mid - 1;
		}
	}

	return found;
}

/*                                  */
static u32 check_inused(unsigned long start, unsigned long end, long comp_start, long comp_end)
{
	int inused = 0;
	struct page *page;

	for (; start < end; start++) {
		page = pfn_to_page(start);
		if (page_count(page) != 0) {
			if (check_if_compressed(comp_start, comp_end, start) == 0) {
				++inused;
			}
		}
	}

	return inused;
}

/*                      */
static void compute_bank_inused(int all)
{
	int i;

#ifdef CONFIG_MTKPASR_MAFL
	/*           */
	if (mtkpasr_no_ops()) {
		for (i = 0; i < num_banks; i++) {
			if (mtkpasr_banks[i].inmafl == mtkpasr_banks[i].valid_pages) {
				mtkpasr_banks[i].inused = 0;
			} else {
				/*                  */
				mtkpasr_banks[i].inused = mtkpasr_banks[i].valid_pages - mtkpasr_banks[i].inmafl;
			}
		}
		goto fast_path;
	}
#endif

	/*
                                                    
                                                                                         
                                
   
                                                                
                              
  */
	MTKPASR_FLUSH();

	/*            */
	for (i = 0; i < num_banks; i++) {
#ifdef CONFIG_MTKPASR_MAFL
		mtkpasr_banks[i].inused = check_inused(mtkpasr_banks[i].start_pfn + mtkpasr_banks[i].inmafl, mtkpasr_banks[i].end_pfn,
				mtkpasr_banks[i].comp_start, mtkpasr_banks[i].comp_end);
#else
		mtkpasr_banks[i].inused = check_inused(mtkpasr_banks[i].start_pfn, mtkpasr_banks[i].end_pfn,
				mtkpasr_banks[i].comp_start, mtkpasr_banks[i].comp_end);
#endif
	}

#ifdef CONFIG_MTKPASR_MAFL
fast_path:
#endif
	/*                                          */
	if (all != 0) {
		/*                                                  */
		for (i = 1; i < num_nopasr_banks; i++) {
			nopasr_banks[i].inused = check_inused(nopasr_banks[i].start_pfn, nopasr_banks[i].end_pfn, 0, 0);
		}
	}
}

#ifdef CONFIG_MTKPASR_MAFL
/*                                                       */
static void remove_bank_from_buddy(int bank)
{
	int has_extcomp = mtkpasr_banks[bank].comp_start - mtkpasr_banks[bank].comp_end;
	struct page *spage, *epage;
	struct zone *z;
	unsigned long flags;
	unsigned int order;
	int free_count;
	struct list_head *mafl;
	int *inmafl;

	/*              */
	inmafl = &mtkpasr_banks[bank].inmafl;
	if (*inmafl == mtkpasr_banks[bank].valid_pages) {
		return;
	}

	/*                                                                             */
	if (has_extcomp != 0) {
		return;
	}

	spage = pfn_to_page(mtkpasr_banks[bank].start_pfn);
	epage = pfn_to_page(mtkpasr_banks[bank].end_pfn);
	z = page_zone(spage);

	/*                */
	spin_lock_irqsave(&z->lock, flags);

	/*                                            */
	spage += *inmafl;
	while (spage < epage) {
		/*                    */
		if (!PageBuddy(spage)) {
			spin_unlock_irqrestore(&z->lock, flags);
			return;
		}
		/*                       */
		free_count = 1 << PAGE_ORDER(spage);
		spage += free_count;
	}

	/*                                     */
	mafl = &mtkpasr_banks[bank].mafl;
	spage = pfn_to_page(mtkpasr_banks[bank].start_pfn) + *inmafl;
	while (spage < epage) {
		/*                      */
		list_del(&spage->lru);
		order = PAGE_ORDER(spage);
		z->free_area[order].nr_free--;
		/*                                                           */
		__mod_zone_page_state(z, NR_FREE_PAGES, -(1UL << order));
		/*                */
		list_add_tail(&spage->lru, mafl);
		/*                       */
		free_count = 1 << order;
		spage += free_count;
		/*                   */
		*inmafl += free_count;
		mafl_total_count += free_count;
	}

	/*                  */
	spin_unlock_irqrestore(&z->lock, flags);

#ifdef CONFIG_MTKPASR_DEBUG
	if (mtkpasr_banks[bank].inmafl != mtkpasr_banks[bank].valid_pages) {
		BUG();
	}
#endif
}

static bool mtkpasr_no_exhausted(int request_order)
{
	long free, exhausted_level = MTKPASR_EXHAUSTED;
	int order;

	free = zone_page_state(MTKPASR_ZONE, NR_FREE_PAGES);
	if (request_order > 0) {
		for (order = 0; order < request_order; ++order) {
			free -= MTKPASR_ZONE->free_area[order].nr_free << order;
		}
		exhausted_level >>= order;
	}

	return (free >= exhausted_level);
}

/*                                              */
void try_to_release_mtkpasr_page(int request_order)
{
	int current_bank = 0;
	struct list_head *mafl = NULL;
	struct page *page;
	struct zone *z;
	unsigned long flags;
	unsigned int order;
	int free_count;

	/*                          */
	if (unlikely(current->flags & PF_MTKPASR)) {
		return;
	}

	/*                           */
	if (mafl_total_count <= 0) {
		return;
	}

	/*                                              */
	if (mtkpasr_no_exhausted(request_order)) {
		return;
	}

	/*                               */
	while (current_bank < num_banks) {
		mafl = &mtkpasr_banks[current_bank].mafl;
		if (!list_empty(mafl)) {
			break;
		}
		++current_bank;
		mafl = NULL;
	}

	/*                     */
	if (mafl == NULL) {
		return;
	}

	/*                */
	z = page_zone(pfn_to_page(mtkpasr_banks[current_bank].start_pfn));
	spin_lock_irqsave(&z->lock, flags);

	/*                                                        */
	if (list_empty(mafl)) {
		spin_unlock_irqrestore(&z->lock, flags);
		return;
	}

	/*                                                                               */
	page = list_entry(mafl->prev, struct page, lru);
	list_del(&page->lru);
	order = PAGE_ORDER(page);

	/*               */
	list_add_tail(&page->lru, &z->free_area[order].free_list[MIGRATE_MTKPASR]);
	__mod_zone_page_state(z, NR_FREE_PAGES, 1UL << order);
	z->free_area[order].nr_free++;

	/*                   */
	free_count = 1 << order;
	mtkpasr_banks[current_bank].inmafl -= free_count;
	mafl_total_count -= free_count;

	/*                  */
	spin_unlock_irqrestore(&z->lock, flags);

	/*              */
	if (mtkpasr_banks[current_bank].inmafl < 0) {
		mtkpasr_info("BUG: Negative inmafl in bank[%d] Remaining MAFL [%ld]!\n", current_bank, mafl_total_count);
	}
}

/*                                              */
static void shrink_mafl_all(int bank)
{
	struct list_head *mafl = NULL;
	int *inmafl;
	struct page *page, *next;
	struct zone *z;
	unsigned long flags;
	unsigned int order;
#ifdef CONFIG_MTKPASR_DEBUG
	int free_count = 0;
#endif

	/*              */
	if (bank >= num_banks || bank < 0) {
		return;
	}

	mafl = &mtkpasr_banks[bank].mafl;
	if (list_empty(mafl)) {
		return;
	}

	z = page_zone(pfn_to_page(mtkpasr_banks[bank].start_pfn));

	/*                */
	spin_lock_irqsave(&z->lock, flags);

	/*                        */
	if (list_empty(mafl)) {
		spin_unlock_irqrestore(&z->lock, flags);
		return;
	}
	inmafl = &mtkpasr_banks[bank].inmafl;

	/*               */
	list_for_each_entry_safe(page, next, mafl, lru) {
		list_del(&page->lru);
		order = PAGE_ORDER(page);
		/*               */
		list_add_tail(&page->lru, &z->free_area[order].free_list[MIGRATE_MTKPASR]);
		__mod_zone_page_state(z, NR_FREE_PAGES, 1UL << order);
		z->free_area[order].nr_free++;
#ifdef CONFIG_MTKPASR_DEBUG
		free_count += (1 << order);
#endif
	}

#ifdef CONFIG_MTKPASR_DEBUG
	/*                             */
	if (free_count != *inmafl) {
		mtkpasr_err("\n\n BANK[%d] free_count[%d] inmafl[%d]\n\n\n", bank, free_count, *inmafl);
		BUG();
	}
#endif

	/*                   */
	mafl_total_count -= *inmafl;
	*inmafl = 0;

	/*                  */
	spin_unlock_irqrestore(&z->lock, flags);
}

static int collect_free_pages_for_compacting_banks(struct mtkpasr_bank_cc *bcc);
static struct page *compacting_alloc(struct page *migratepage, unsigned long data, int **result);
static unsigned long putback_free_pages(struct list_head *freelist);
/*                           */
void shrink_mtkpasr_all(void)
{
	int i;
	struct page *page, *end_page;
	int to_be_migrated = 1;
	struct mtkpasr_bank_cc bank_cc;

	/*                     */
	if (num_banks <= 0) {
		return;
	}

	/*                      */
	for (i = 0; i < num_banks; i++) {
		shrink_mafl_all(i);
	}

	/*                                        */
	bank_cc.to_bank = i = 0;
	bank_cc.to_cursor = mtkpasr_banks[i].start_pfn;
	page = pfn_to_page(mtkpasr_start_pfn);
	end_page = pfn_to_page(mtkpasr_migration_end);
	while (page > end_page) {
		/*            */
		if (page_count(page) != 0) {
			if (!mtkpasr_isolate_page(page)) {
				list_add(&page->lru, &fromlist);
				++fromlist_count;
				++to_be_migrated;
			}
			/*            */
			if ((to_be_migrated % MTKPASR_CHECK_MIGRATE) == 0) {
				/*                    */
				while (collect_free_pages_for_compacting_banks(&bank_cc) < fromlist_count) {
					if (bank_cc.to_cursor == 0) {
						if (++i == num_banks) {
							goto done;
						}
						bank_cc.to_bank = i;
						bank_cc.to_cursor = mtkpasr_banks[i].start_pfn;
					} else {
						mtkpasr_err("Failed to collect free pages!\n");
						BUG();
					}
				}
				/*                 */
				if (migrate_pages(&fromlist, compacting_alloc, 0, false, MIGRATE_ASYNC) != 0) {
					putback_lru_pages(&fromlist);
				}
				fromlist_count = 0;
			}
		}
		/*          */
		page--;
	}

done:
	/*                */
	if (fromlist_count != 0) {
		putback_lru_pages(&fromlist);
		fromlist_count = 0;
	}

	/*              */
	if (tolist_count != 0) {
		if (putback_free_pages(&tolist) != tolist_count) {
			mtkpasr_err("Should be the same!\n");
		}
		tolist_count = 0;
	}
}

void shrink_mtkpasr_late_resume(void)
{
	/*                                                               */
	if (!is_mtkpasr_triggered()) {
		return;
	}

	/*                     */
	mtkpasr_ops_invariant = 0;

	/*                 */
	clear_mtkpasr_triggered();
}
#else /*                     */

void shrink_mtkpasr_all(void) { do {} while (0); }
void shrink_mtkpasr_late_resume(void) { do {} while (0); }

#endif /*                     */

/*
                                                                                                                 
                                                  
                                             
  
                               
                              
                      
 */
enum mtkpasr_phase mtkpasr_disablingSR(u32 *sr, u32 *dpd)
{
	enum mtkpasr_phase result = MTKPASR_SUCCESS;
	int i, j;
	int enter_dpd = 0;
	u32 banksr = 0x0;	/*                                                         */
	bool keep_ops = true;

	/*          */
	*sr = banksr;

	/*              */
	if (mtkpasr_status != MTKPASR_DISABLINGSR) {
		mtkpasr_err("Error Current State [%d]!\n", mtkpasr_status);
		return MTKPASR_WRONG_STATE;
	}

	/*                              */
	if (CHECK_PENDING_WAKEUP) {
		mtkpasr_log("Pending Wakeup Sources!\n");
		return MTKPASR_GET_WAKEUP;
	}

	/*            */
	compute_bank_inused(0);

	for (i = 0; i < num_ranks; i++) {
		mtkpasr_ranks[i].inused = 0;
		for (j = mtkpasr_ranks[i].start_bank; j <= mtkpasr_ranks[i].end_bank; j++) {
			if (mtkpasr_banks[j].inused != 0) {
				++mtkpasr_ranks[i].inused;
			}
		}
	}

	/*                                      */
	for (i = 0; i < num_ranks; i++) {
		if (mtkpasr_ranks[i].inused == 0) {
			mtkpasr_info("DPD!\n");
			/*                   */
			mtkpasr_ranks[i].inused = MTKPASR_DPDON;
			/*                                         */
			for (j = mtkpasr_ranks[i].start_bank; j <= mtkpasr_ranks[i].end_bank; j++) {
#ifdef CONFIG_MTKPASR_MAFL
				/*                                                       */
				remove_bank_from_buddy(j);
#endif
				mtkpasr_banks[j].inused = MTKPASR_RDPDON;
				/*                */
				banksr = banksr | (0x1 << (mtkpasr_banks[j].segment /*                     */));
			}
			enter_dpd = 1;
		}
	}

	/*                                         */
	for (i = 0; i < num_banks; i++) {
		if (mtkpasr_banks[i].inused == 0) {
			/*                   */
			mtkpasr_banks[i].inused = MTKPASR_SROFF;
			/*                */
			banksr = banksr | (0x1 << (mtkpasr_banks[i].segment /*                     */));
#ifdef CONFIG_MTKPASR_MAFL
			/*                                                       */
			remove_bank_from_buddy(i);
#endif
			mtkpasr_info("SPM SR/OFF[%d]!\n", i);
		} else {
			mtkpasr_log("Bank[%d] %s[%d]!\n", i, (mtkpasr_banks[i].inused == MTKPASR_RDPDON) ? "RDPDON":"inused", mtkpasr_banks[i].inused);
		}

		/*                                                                                         */
		if (keep_ops) {
			if (mtkpasr_banks[i].comp_pos != 0) {
				keep_ops = false;
			}
		}
	}

	/*                                   */
	if (result == MTKPASR_SUCCESS) {
		mtkpasr_status = enter_dpd ? MTKPASR_DPD_ON : MTKPASR_ON;
		*sr = banksr;
	}

#ifdef CONFIG_MTKPASR_MAFL
	/*                         */
	if (before_mafl_count == mafl_total_count) {	/*               */
		/*                        */
		if (!keep_ops) {
			mtkpasr_ops_invariant = KEEP_NO_OPS;
		}
		/*                                           */
		if (mtkpasr_ops_invariant != KEEP_NO_OPS) {
			++mtkpasr_ops_invariant;
			if (mtkpasr_ops_invariant > MAX_NO_OPS_INVARIANT) {
				mtkpasr_ops_invariant = MAX_OPS_INVARIANT;
			}
		}
	} else {
		mtkpasr_ops_invariant = 0;
	}
	prev_mafl_count = mafl_total_count;
#endif

	mtkpasr_log("Ops_invariant[%u] result [%s] mtkpasr_status [%s]\n", mtkpasr_ops_invariant,
			(result == MTKPASR_SUCCESS) ? "MTKPASR_SUCCESS" : "MTKPASR_FAIL",
			(enter_dpd == 1) ? "MTKPASR_DPD_ON" : "MTKPASR_ON");

	return result;
}

static struct page *compacting_alloc(struct page *migratepage, unsigned long data, int **result)
{
	struct page *page = NULL;

	if (!list_empty(&tolist)) {
		page = list_entry(tolist.next, struct page, lru);
		list_del(&page->lru);
		--tolist_count;
	}

	return page;
}

/*                                       */
static int collect_free_pages_for_compacting_banks(struct mtkpasr_bank_cc *bcc)
{
	struct page *page, *end_page;
	struct zone *z;
	unsigned long flags;
	int found;

	/*                         */
	if (bcc->to_cursor == 0) {
		return tolist_count;
	}

	/*                           */
	if (tolist_count >= fromlist_count) {
		return tolist_count;
	}

	/*                      */
	page = pfn_to_page(bcc->to_cursor);
	z = page_zone(page);
	end_page = pfn_to_page(mtkpasr_banks[bcc->to_bank].end_pfn);
	while (page < end_page) {
		/*                */
		spin_lock_irqsave(&z->lock, flags);
		/*                 */
		if (!PageBuddy(page)) {
			spin_unlock_irqrestore(&z->lock, flags);
			++page;
			continue;
		}
		found = pasr_find_free_page(page, &tolist);
		/*                  */
		spin_unlock_irqrestore(&z->lock, flags);
		/*              */
		tolist_count += found;
		page += found;
		/*                           */
		bcc->to_cursor += (page - pfn_to_page(bcc->to_cursor));
		BANK_INUSED(bcc->to_bank) += found;
		/*                    */
		if (tolist_count >= fromlist_count) {
			break;
		}
		/*                  */
		if (BANK_INUSED(bcc->to_bank) == mtkpasr_banks[bcc->to_bank].valid_pages) {
			bcc->to_cursor = 0;
			break;
		}
	}

	if (page == end_page) {
		mtkpasr_info("\"To\" bank[%d] is full!\n", bcc->to_bank);
		bcc->to_cursor = 0;
	}

	return tolist_count;
}

/*                             */
static unsigned long putback_free_pages(struct list_head *freelist)
{
	struct page *page, *next;
	unsigned long count = 0;

	list_for_each_entry_safe(page, next, freelist, lru) {
		list_del(&page->lru);
		__free_page(page);
		count++;
	}

	return count;
}

#define COMPACTING_COLLECT()										\
	{												\
		if (collect_free_pages_for_compacting_banks(&bank_cc) >= fromlist_count) {		\
			if (migrate_pages(&fromlist, compacting_alloc, 0, false, MIGRATE_ASYNC) != 0) {	\
				mtkpasr_log("Bank[%d] can't be cleared!\n", from);			\
				ret = -1;								\
				goto next;								\
			}										\
			/*                                  */						\
			fromlist_count = 0;								\
		} else {										\
			ret = 1;									\
			goto next;									\
		}											\
	}

/*
                                    
              
                                             
                                                  
                                             
 */
static int compacting_banks(int from, int to, unsigned long *from_cursor, unsigned long *to_cursor)
{
	struct page *page;
	unsigned long fc = *from_cursor;
	unsigned long ec = mtkpasr_banks[from].end_pfn;
	int to_be_migrated = 0;
	int ret = 0;
	struct mtkpasr_bank_cc bank_cc = {
		.to_bank = to,
		.to_cursor = *to_cursor,
	};

	/*                              */
	if (CHECK_PENDING_WAKEUP) {
		mtkpasr_log("Pending Wakeup Sources!\n");
		return -EBUSY;
	}

	/*          */
	if (list_empty(&fromlist)) {
		fromlist_count = 0;
	}
	if (list_empty(&tolist)) {
		tolist_count = 0;
	}

	/*                                               */
	while (fc < ec) {
		/*                              */
		if ((fc % MTKPASR_CHECK_ABORTED) == 0) {
			if (CHECK_PENDING_WAKEUP) {
				mtkpasr_log("Pending Wakeup Sources!\n");
				ret = -EBUSY;
				break;
			}
		}
		/*                  */
		page = pfn_to_page(fc);
		if (page_count(page) != 0) {
			/*                                       */
			if (check_if_compressed(mtkpasr_banks[from].comp_start, mtkpasr_banks[from].comp_end, fc)) {
				++fc;
				continue;
			}
			/*               */
#ifdef CONFIG_MTKPASR_ALLEXTCOMP
			if (!mtkpasr_isolate_page(page, 0x0)) {
#else
			if (!mtkpasr_isolate_page(page)) {
#endif
				list_add(&page->lru, &fromlist);
				++fromlist_count;
				++to_be_migrated;
				--BANK_INUSED(from);
			} else {
				/*                             */
				mtkpasr_log("Bank[%d] can't be cleared!\n", from);
				ret = -1;
				break;
			}
		} else {
			++fc;
			continue;
		}
		/*            */
		if ((to_be_migrated % MTKPASR_CHECK_MIGRATE) == 0) {
			if (!list_empty(&fromlist)) {
				COMPACTING_COLLECT();
			}
		}
		/*                                                                */
		if (BANK_INUSED(from) == 0) {
			fc = 0;
			if (!list_empty(&fromlist)) {
				ret = 1;
			} else {
				/*                               */
				ret = 0;
			}
			break;
		}
		/*           */
		++fc;
	}

	/*                                                           */
	if (fc == ec) {
		mtkpasr_err("Should always be false!\n");
		fc = 0;
		if (!list_empty(&fromlist)) {
			ret = 1;
		} else {
			/*                               */
			ret = 0;
		}
	}

	/*                               */
	if (ret > 0 && !list_empty(&fromlist)) {
		COMPACTING_COLLECT();
	}

next:
	/*                                          */
	if (ret == -1) {
		/*                                            */
		putback_lru_pages(&fromlist);
		fromlist_count = 0;								\
		/*                                               */
		fc = 0;
		ret = 1;
	}

	/*                */
	*from_cursor = fc;
	*to_cursor = bank_cc.to_cursor;

	return ret;
}

/*                                                                          */
static enum mtkpasr_phase mtkpasr_compact_banks(int toget)
{
	int to_be_sorted = num_banks;
	int dsort_banks[to_be_sorted];
	int i, j, tmp, ret;
	unsigned long from_cursor = 0, to_cursor = 0;
	enum mtkpasr_phase result = MTKPASR_SUCCESS;

	/*                              */
	if (CHECK_PENDING_WAKEUP) {
		mtkpasr_log("Pending Wakeup Sources!\n");
		return MTKPASR_GET_WAKEUP;
	}

	/*                */
	for (i = 0; i < to_be_sorted; ++i) {
		dsort_banks[i] = i;
	}

	/*               */
	for (i = to_be_sorted; i > 1; --i) {
		for (j = 0; j < i-1; ++j) {
			/*                      */
			if (BANK_RANK(dsort_banks[j]) < BANK_RANK(dsort_banks[j+1])) {
				continue;
			}
			/*                        */
			if (BANK_INUSED(dsort_banks[j]) < BANK_INUSED(dsort_banks[j+1])) {
				tmp = dsort_banks[j];
				dsort_banks[j] = dsort_banks[j+1];
				dsort_banks[j+1] = tmp;
			}
		}
	}

#ifdef CONFIG_MTKPASR_DEBUG
	for (i = 0; i < to_be_sorted; ++i) {
		mtkpasr_info("[%d] - (%d) - inused(%d) - rank(%p)\n", i, dsort_banks[i], BANK_INUSED(dsort_banks[i]), BANK_RANK(dsort_banks[i]));
	}
#endif

	/*                  */
	i = to_be_sorted - 1;	/*           */
	j = 0;			/*         */
	while (i > j) {
		/*                            */
		if (BANK_INUSED(dsort_banks[i]) == 0) {
			--i;
			continue;
		}
		/*                         */
		if (BANK_INUSED(dsort_banks[j]) == mtkpasr_banks[dsort_banks[j]].valid_pages) {
			++j;
			continue;
		}
		/*                                   */
		if (!from_cursor) {
			from_cursor = mtkpasr_banks[dsort_banks[i]].start_pfn;
#ifdef CONFIG_MTKPASR_MAFL
			from_cursor += mtkpasr_banks[dsort_banks[i]].inmafl;
#endif
		}
		if (!to_cursor) {
#ifdef CONFIG_MTKPASR_MAFL
			/*                                    */
			shrink_mafl_all(dsort_banks[j]);
#endif
			to_cursor = mtkpasr_banks[dsort_banks[j]].start_pfn;
		}
		/*                           */
		ret = compacting_banks(dsort_banks[i], dsort_banks[j], &from_cursor, &to_cursor);
		if (ret >= 0) {
			if (!from_cursor) {
				--i;
			}
			if (!to_cursor) {
				++j;
			}
			if (!ret) {
				--toget;
			} else {
				continue;
			}
		} else {
			/*                 */
			mtkpasr_err("Error occurred during the compacting on banks!\n");
			if (ret == -EBUSY) {
				result = MTKPASR_GET_WAKEUP;
			} else {
				result = MTKPASR_FAIL;
#ifdef CONFIG_MTKPASR_DEBUG
				BUG();
#endif
			}
			break;
		}
		/*                                */
		if (!toget) {
			break;
		}
	}

	/*                                   */
	putback_lru_pages(&fromlist);
	fromlist_count = 0;
	if (putback_free_pages(&tolist) != tolist_count) {
		mtkpasr_err("Should be the same!\n");
	}
	tolist_count = 0;

	return result;
}

/*                           */
static enum mtkpasr_phase mtkpasr_compact(void)
{
	int i;
	int no_compact = 0;
	int total_free = 0;
	int free_banks;

	/*                       */
	compute_bank_inused(0);

	/*                              */
	if (CHECK_PENDING_WAKEUP) {
		mtkpasr_log("Pending Wakeup Sources!\n");
		return MTKPASR_GET_WAKEUP;
	}

	/*                                                */
	for (i = 0; i < num_banks; ++i) {
		total_free += (mtkpasr_banks[i].valid_pages - mtkpasr_banks[i].inused);
		if (mtkpasr_banks[i].inused == 0) {
			++no_compact;
		}
	}

	/*                                  */
	free_banks = total_free / pages_per_bank;
	if (no_compact >= free_banks) {
		mtkpasr_info("No need to do compaction on banks!\n");
		/*                                   */
		return MTKPASR_SUCCESS;
	}

	/*                            */
	return mtkpasr_compact_banks(free_banks - no_compact);
}

/*
                                                                     
  
                                                               
                                            
                                         
                                                                                
 */
static int pasr_scan_memory(void *src_map, unsigned long start, unsigned long end, unsigned long *sorted)
{
	unsigned long start_pfn;
	unsigned long end_pfn;
	struct page *page;
	unsigned long *pgmap = (unsigned long *)src_map;
	int need_compressed = 0;

	/*                      */
	start_pfn = start;
	end_pfn = end;

	/*                                                                                      */
	if (start == end) {
		return need_compressed;
	}

	/*                           */
	do {
		page = pfn_to_page(start_pfn);
		if (page_count(page) != 0) {
			*pgmap++ = start_pfn;
			++need_compressed;
		}
	} while (++start_pfn < end_pfn);

	/*                                                          */
	memset(sorted, 0, (end-start)*sizeof(unsigned long));

	mtkpasr_info("@@@ start_pfn[0x%lx] end_pfn[0x%lx] - to process[%d] @@@\n", start, end, need_compressed);

	return need_compressed;
}

/*                                                                 */
#define MTKPASR_DIRECT_COMPRESSION()							\
	{										\
		ret = 0;								\
		if (!trylock_page(page)) {						\
			mtkpasr_err("FL!\n");						\
			ret = -1;							\
			break;								\
		}									\
		if (likely(compressed < MTKPASR_MAX_EXTCOMP)) {				\
			/*                   */						\
			current_index = atomic_inc_return(&sloti);			\
			/*                              */				\
			ret = mtkpasr_forward_rw(mtkpasr, current_index, page , WRITE);	\
			/*                           */					\
			if (unlikely(ret)) {						\
				unlock_page(page);					\
				atomic_dec(&sloti);					\
				mtkpasr_err("FFRW!\n");					\
				break;							\
			}								\
			/*                                         */			\
			sorted_for_extcomp[page_to_pfn(page)-bank_start_pfn] = 1;	\
			++compressed;							\
		}									\
		unlock_page(page);							\
	}

/*
                                        
  
                               
                              
                      
 */
enum mtkpasr_phase mtkpasr_entering(void)
{
#define SAFE_CONDITION_CHECK() {										\
		/*                                                                         */			\
		if (CHECK_PENDING_WAKEUP) {									\
			mtkpasr_log("Pending Wakeup Sources!\n");						\
			ret = -EBUSY;										\
			break;											\
		}												\
		/*                                       */							\
		if (unlikely(pasr_check_free_safe())) {								\
			mtkpasr_log("Unsafe System Status!\n");							\
			ret = -1;										\
			break;											\
		}												\
		/*                                                    */					\
		if (mtkpasr_admit_order < 0) {									\
			mtkpasr_log("mtkpasr_admit_order is negative!\n");					\
			ret = -1;										\
			break;											\
		}												\
	}

	int ret = 0;
	struct mtkpasr *mtkpasr;
	struct page *page;
	int current_bank, current_pos, current_index;
	unsigned long which_pfn, bank_start_pfn, bank_end_pfn;
#ifdef CONFIG_MTKPASR_DEBUG
	int drop_cnt, to_be_migrated, splitting, no_migrated;
#endif
	int compressed = 0, val;
	unsigned long *start;
	int pgmi;
	LIST_HEAD(to_migrate);
	LIST_HEAD(batch_to_migrate);
	enum mtkpasr_phase result = MTKPASR_SUCCESS;
	struct zone *zone;
	unsigned long isolated_file, isolated_anon;
	long bias_file, bias_anon;

	/*              */
	if (mtkpasr_status != MTKPASR_OFF) {
		mtkpasr_err("Error Current State [%d]!\n", mtkpasr_status);
		return MTKPASR_WRONG_STATE;
	} else {
		/*                              */
		mtkpasr_status = MTKPASR_ENTERING;
	}

	/*                              */
	if (CHECK_PENDING_WAKEUP) {
		mtkpasr_log("Pending Wakeup Sources!\n");
		return MTKPASR_GET_WAKEUP;
	}

	/*                                                                                    */
	atomic_set(&sloti, -1);
	for (current_bank = 0; current_bank < num_banks; ++current_bank) {
		mtkpasr_banks[current_bank].comp_pos = 0;
	}

#ifdef CONFIG_MTKPASR_MAFL
	/*                                       */
	before_mafl_count = mafl_total_count;

	/*                      */
	if (prev_mafl_count == mafl_total_count) {
		if (mtkpasr_no_ops()) {
			/*                      */
			mtkpasr_status = MTKPASR_DISABLINGSR;
			goto fast_path;
		}
	} else {
		/*                                                          */
		mtkpasr_ops_invariant = 0;
	}
#endif

	/*                                                  */
	MTKPASR_FLUSH();

	/*                           */
	/*                           */
	/*                           */

#ifdef CONFIG_MTKPASR_DEBUG
	drop_cnt = to_be_migrated = splitting = no_migrated = 0;
#endif

	/*                                                           */
	zone = MTKPASR_ZONE;
	isolated_file = zone_page_state(zone, NR_ISOLATED_FILE);
	isolated_anon = zone_page_state(zone, NR_ISOLATED_ANON);

	/*                     */
#ifndef CONFIG_MTKPASR_RDIRECT
	current_bank = 0;
#else
	current_bank = num_banks - 1;
#endif

	/*                                       */
	if (unlikely(pasr_check_free_safe())) {
		mtkpasr_log("Unsafe System Status!\n");
		goto no_safe;
	}

	/*                     */
	mtkpasr_admit_order = pasr_compute_safe_order();
	mtkpasr_info("mtkpasr_admit_order is [%d]\n", mtkpasr_admit_order);

	/*                     */
	mtkpasr = &mtkpasr_device[0];

	/*                            */
	mtkpasr_last_scan = mtkpasr_start_pfn - pageblock_nr_pages;

next_bank:
	/*                          */
	mtkpasr_banks[current_bank].comp_start = (s16)compressed;

	/*                            */
#ifdef CONFIG_MTKPASR_MAFL
	bank_start_pfn = mtkpasr_banks[current_bank].start_pfn + mtkpasr_banks[current_bank].inmafl;
#else
	bank_start_pfn = mtkpasr_banks[current_bank].start_pfn;
#endif
	bank_end_pfn = mtkpasr_banks[current_bank].end_pfn;
	val = pasr_scan_memory(src_pgmap, bank_start_pfn, bank_end_pfn, sorted_for_extcomp);
	start = src_pgmap;

	/*                  */
	pgmi = -1;

	/*                       */
	ret = 0;

	/*                                         */
	current_pos = ++pgmi;
	while (current_pos < val) {
		/*                                                                            */
		/*                                                                            */
		if ((current_pos % MTKPASR_CHECK_MIGRATE) == 0) {
			SAFE_CONDITION_CHECK();
		}
		/*                 */
		page = pfn_to_page(start[current_pos]);
		if (page != NULL) {
			/*                                                         */
			if (!PageLRU(page) || PageUnevictable(page) || !PAGE_EVICTABLE(page, NULL)) {
				MTKPASR_DIRECT_COMPRESSION();
				goto next_page;
			}
			/*         */
			ret = mtkpasr_drop_page(page);
			if (ret == 0) {
#ifdef CONFIG_MTKPASR_DEBUG
				++drop_cnt;
#endif
			} else if (ret == -EAGAIN) {
				ret = -1;
				break;
			} else {
				if (unlikely(ret == -EACCES)) {
					/*                                                   */
					MTKPASR_DIRECT_COMPRESSION();
				} else {
					/*                                                                                                    */
					list_add_tail(&page->lru, &to_migrate);
#ifdef CONFIG_MTKPASR_DEBUG
					++to_be_migrated;
#endif
					/*                       */
					ret = 0;
				}
			}
		}
next_page:
		current_pos = ++pgmi;
	}

	/*            */
	while (!ret && !list_empty(&to_migrate)) {
		struct list_head *this, *split_start;
		/*                                    */
		current_pos = 0;
		split_start = to_migrate.next;
		list_for_each(this, &to_migrate) {
			++current_pos;
			if (current_pos == MTKPASR_CHECK_MIGRATE) {
				break;
			}
		}
		/*                         */
		SAFE_CONDITION_CHECK();
		/*             */
		if (current_pos == MTKPASR_CHECK_MIGRATE) {
			to_migrate.next = this->next;
			this->next->prev = &to_migrate;
			batch_to_migrate.next = split_start;
			split_start->prev = &batch_to_migrate;
			batch_to_migrate.prev = this;
			this->next = &batch_to_migrate;
			this = &batch_to_migrate;
		} else {
			this = &to_migrate;
		}
#ifdef CONFIG_MTKPASR_DEBUG
		splitting += current_pos;
#endif
		/*               */
		if (migrate_pages(this, mtkpasr_alloc, 0, false, MIGRATE_ASYNC)) {
			/*                                                                     */
			list_for_each_entry(page, this, lru) {
#ifdef CONFIG_MTKPASR_DEBUG
				++no_migrated;
#endif
				MTKPASR_DIRECT_COMPRESSION();
				/*                                                                     */
				ret = 0;
			}
			putback_lru_pages(this);
		}
		/*                         */
		SAFE_CONDITION_CHECK();
	}

	/*                           */
	putback_lru_pages(&batch_to_migrate);
	putback_lru_pages(&to_migrate);

	/*                                         */
	mtkpasr_banks[current_bank].comp_end = (s16)compressed;
	mtkpasr_info("bank[%d] - comp_start[%d] comp_end[%d]\n",
			current_bank, mtkpasr_banks[current_bank].comp_start, mtkpasr_banks[current_bank].comp_end);

	/*                          */
	if (mtkpasr_banks[current_bank].comp_start < mtkpasr_banks[current_bank].comp_end) {
		which_pfn = 0;
		bank_end_pfn = bank_end_pfn - bank_start_pfn;
		start = extcomp + mtkpasr_banks[current_bank].comp_start;
		do {
			/*                         */
			if (sorted_for_extcomp[which_pfn] == 1) {
				*start++ = which_pfn + bank_start_pfn;
			}
		} while (++which_pfn < bank_end_pfn);

#ifdef CONFIG_MTKPASR_DEBUG
		/*              */
		if (start != (extcomp + compressed)) {
			mtkpasr_err("\n\n\n\n\n\n Oh no!\n\n\n\n\n\n");
		}
#endif
	}

	/*                                             */
	/*                                                                                                          */
#ifndef CONFIG_MTKPASR_RDIRECT
	if (!ret && (++current_bank) < num_banks)
#else
	if (!ret && (--current_bank) >= 0)
#endif
		goto next_bank;

	/*                                                           */
	atomic_inc(&sloti);

	/*                                       */
#ifdef CONFIG_MTKPASR_DEBUG
	if (putback_free_pages(&tolist) != tolist_count) {
		mtkpasr_err("Should be the same!\n");
	}
	tolist_count = 0;
#else
	putback_free_pages(&tolist);
#endif

	/*                      */
	if (ret == -EBUSY) {
		mtkpasr_log("Failed MTKPASR due to pending wakeup source!\n");
		/*                                                                                       */
		result = MTKPASR_GET_WAKEUP;
	} else if (ret == -1) {
		mtkpasr_log("Failed compression or no safe amount of free space! Go ahead to SPM!\n");
	}

no_safe:
	/*                                            */
	if (result == MTKPASR_SUCCESS) {
		/*                     */
#ifndef CONFIG_MTKPASR_RDIRECT
		if (current_bank < (num_banks - 1))
#else
		if (current_bank > 0)
#endif
			result = mtkpasr_compact();
		/*                     */
		if (result == MTKPASR_SUCCESS) {
			/*                      */
			mtkpasr_status = MTKPASR_DISABLINGSR;
		}
		/*                                                                    */
		flush_cache_all();
	}

	/*                                                              */
	bias_file = isolated_file - zone_page_state(zone, NR_ISOLATED_FILE);
	bias_anon = isolated_anon - zone_page_state(zone, NR_ISOLATED_ANON);
	mod_zone_page_state(zone, NR_ISOLATED_FILE, bias_file);
	mod_zone_page_state(zone, NR_ISOLATED_ANON, bias_anon);

#ifdef CONFIG_MTKPASR_DEBUG
	mtkpasr_info("dropped [%d] - compressed [%d] - to_be_migrated [%d] - splitting [%d] - no_migrated [%d]\n"
			, drop_cnt, compressed, to_be_migrated, splitting, no_migrated);
#endif

#ifdef CONFIG_MTKPASR_MAFL
fast_path:
#endif

	mtkpasr_log("result [%s]\n\n", (result == MTKPASR_SUCCESS) ? "MTKPASR_SUCCESS" : ((result == MTKPASR_FAIL) ? "MTKPASR_FAIL" : "MTKPASR_GET_WAKEUP"));

	return result;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
/*                                             */
static void mtkpasr_early_suspend(struct early_suspend *h)
{
	mtkpasr_info("\n");
}

static void mtkpasr_late_resume(struct early_suspend *h)
{
	mtkpasr_info("\n");
	shrink_mtkpasr_late_resume();
}

static struct early_suspend mtkpasr_early_suspend_desc = {
	.level = EARLY_SUSPEND_LEVEL_DISABLE_FB + 1,
	.suspend = mtkpasr_early_suspend,
	.resume = mtkpasr_late_resume,
};
#endif

/*       */
void __mtkpasr_reset_device(struct mtkpasr *mtkpasr)
{
	mtkpasr->init_done = 0;

	/*                                 */
	if (mtkpasr->compress_workmem != NULL) {
		kfree(mtkpasr->compress_workmem);
		mtkpasr->compress_workmem = NULL;
	}
	if (mtkpasr->compress_buffer != NULL) {
		free_pages((unsigned long)mtkpasr->compress_buffer, 1);
		mtkpasr->compress_buffer = NULL;
	}
}

void mtkpasr_reset_device(struct mtkpasr *mtkpasr)
{
	down_write(&mtkpasr->init_lock);
	__mtkpasr_reset_device(mtkpasr);
	up_write(&mtkpasr->init_lock);
}

void mtkpasr_reset_global(void)
{
	mtkpasr_reset_slots();

	/*            */
	kfree(mtkpasr_table);
	mtkpasr_table = NULL;
	mtkpasr_disksize = 0;

	/*              */
	zs_destroy_pool(mtkpasr_mem_pool);
	mtkpasr_mem_pool = NULL;
}

int mtkpasr_init_device(struct mtkpasr *mtkpasr)
{
	int ret;

	down_write(&mtkpasr->init_lock);

	if (mtkpasr->init_done) {
		up_write(&mtkpasr->init_lock);
		return 0;
	}

	mtkpasr->compress_workmem = kzalloc(LZO1X_MEM_COMPRESS, GFP_KERNEL);
	if (!mtkpasr->compress_workmem) {
		pr_err("Error allocating compressor working memory!\n");
		ret = -ENOMEM;
		goto fail;
	}

	mtkpasr->compress_buffer = (void *)__get_free_pages(GFP_KERNEL | __GFP_ZERO, 1);
	if (!mtkpasr->compress_buffer) {
		pr_err("Error allocating compressor buffer space\n");
		ret = -ENOMEM;
		goto fail;
	}

	mtkpasr->init_done = 1;
	up_write(&mtkpasr->init_lock);

	pr_debug("Initialization done!\n");
	return 0;

fail:
	__mtkpasr_reset_device(mtkpasr);
	up_write(&mtkpasr->init_lock);
	pr_err("Initialization failed: err=%d\n", ret);
	return ret;
}

/*                                         */
static void __init mtkpasr_construct_bankrank(void)
{
	int bank, rank, hwrank, prev;
	unsigned long spfn, epfn;

	/*                            */
	/*                            */
	/*                            */

	/*                  */
	mtkpasr_total_pfns = 0;

	/*                                      */
	for (bank = 0, rank = -1, prev = -1; bank < num_banks; ++bank) {
		/*                                       */
		hwrank = query_bank_information(bank, &spfn, &epfn, true);
		mtkpasr_banks[bank].start_pfn = spfn;
		mtkpasr_banks[bank].end_pfn = epfn;
		mtkpasr_banks[bank].inused = 0;
		mtkpasr_banks[bank].segment = pasr_bank_to_segment(spfn, epfn);
		mtkpasr_banks[bank].comp_pos = 0;
		if (hwrank >= 0) {
			if (prev != hwrank) {
				++rank;
				/*             */
				mtkpasr_ranks[rank].start_bank = (u16)bank;
				mtkpasr_ranks[rank].hw_rank = hwrank;
				mtkpasr_ranks[rank].inused = 0;
			}
			mtkpasr_banks[bank].rank = &mtkpasr_ranks[rank];
			mtkpasr_ranks[rank].end_bank = (u16)bank;		/*             */
			prev = hwrank;
		} else {
			mtkpasr_banks[bank].rank = NULL;			/*                  */
		}

#ifdef CONFIG_MTKPASR_MAFL
		/*                                                                    */
		INIT_LIST_HEAD(&mtkpasr_banks[bank].mafl);
		mtkpasr_banks[bank].inmafl = 0;
#endif
		/*
                                                         
   */
		spfn = mtkpasr_banks[bank].start_pfn;
		epfn = mtkpasr_banks[bank].end_pfn;

		/*                            */
		while (spfn < epfn) {
			if (pfn_valid(spfn)) {
				mtkpasr_banks[bank].start_pfn = spfn++;
				break;
			}
			++spfn;
		}

		/*                                            */
		while (spfn < epfn) {
			if (!pfn_valid(spfn)) {
				mtkpasr_banks[bank].end_pfn = spfn;
				break;
			}
			++spfn;
		}

		/*                    */
		mtkpasr_banks[bank].valid_pages = mtkpasr_banks[bank].end_pfn - mtkpasr_banks[bank].start_pfn;

		/*                                                     */
		mtkpasr_total_pfns += mtkpasr_banks[bank].valid_pages;

		mtkpasr_info("Bank[%d] - start_pfn[0x%lx] end_pfn[0x%lx] valid_pages[%u] rank[%p]\n",
				bank, mtkpasr_banks[bank].start_pfn, mtkpasr_banks[bank].end_pfn,
				mtkpasr_banks[bank].valid_pages, mtkpasr_banks[bank].rank);
	}

	/*                                   */
	pages_per_bank = mtkpasr_total_pfns / num_banks;

	/*                        */
	/*                        */
	/*                        */

#ifdef CONFIG_MTKPASR_MAFL
	/*                                                                     */
	compute_bank_inused(0);

	/*                   */
	for (bank = 0; bank < num_banks; bank++) {
		if (mtkpasr_banks[bank].inused == 0) {
			remove_bank_from_buddy(bank);
		}
	}

	prev_mafl_count = mafl_total_count;
#endif
}

/*                  */
static int __init mtkpasr_init(void)
{
	int ret;

	/*                                                                                  */
	mtkpasr_total_slots = MTKPASR_MAX_EXTCOMP + 1;	/*                            */
	mtkpasr_free_slots = mtkpasr_total_slots;
	mtkpasr_disksize = mtkpasr_total_slots << PAGE_SHIFT;
	mtkpasr_table = kzalloc(mtkpasr_total_slots * sizeof(*mtkpasr_table), GFP_KERNEL);
	if (!mtkpasr_table) {
		mtkpasr_err("Error allocating mtkpasr address table\n");
		ret = -ENOMEM;
		goto fail;
	}

	/*                         */
	mtkpasr_mem_pool = ZS_CREATE_POOL("mtkpasr", GFP_ATOMIC|__GFP_HIGHMEM);
	if (!mtkpasr_mem_pool) {
		mtkpasr_err("Error creating memory pool\n");
		ret = -ENOMEM;
		goto no_mem_pool;
	}

	/*                                */
	mtkpasr_device = kzalloc(sizeof(struct mtkpasr), GFP_KERNEL);
	if (!mtkpasr_device) {
		mtkpasr_err("Failed to create mtkpasr_device\n");
		ret = -ENOMEM;
		goto out;
	}

	/*                                                                             */
	if (src_pgmap == NULL) {
		src_pgmap = (void *)__get_free_pages(GFP_KERNEL, get_order(pasrbank_pfns * sizeof(unsigned long)));
		if (src_pgmap == NULL) {
			mtkpasr_err("Failed to allocate (order:%d)(%ld) memory!\n", get_order(pasrbank_pfns * sizeof(unsigned long)), pasrbank_pfns);
			ret = -ENOMEM;
			goto free_devices;
		}
	}

	/*                                                                 */
	if (extcomp == NULL) {
		extcomp = (unsigned long *)__get_free_pages(GFP_KERNEL, get_order(MTKPASR_MAX_EXTCOMP * sizeof(unsigned long)));
		if (extcomp == NULL) {
			mtkpasr_err("Failed to allocate memory for extcomp!\n");
			ret = -ENOMEM;
			goto no_memory;
		}
		sorted_for_extcomp = (unsigned long *)__get_free_pages(GFP_KERNEL, get_order(pasrbank_pfns * sizeof(unsigned long)));
		if (sorted_for_extcomp == NULL) {
			free_pages((unsigned long)extcomp, get_order(MTKPASR_MAX_EXTCOMP * sizeof(unsigned long)));
			mtkpasr_err("Failed to allocate memory for extcomp!\n");
			ret = -ENOMEM;
			goto no_memory;
		}
	}

	/*                      */
	init_rwsem(&mtkpasr_device->init_lock);
	spin_lock_init(&mtkpasr_device->stat64_lock);
	mtkpasr_device->init_done = 0;

	/*                        */
	ret = mtkpasr_init_device(mtkpasr_device);
	if (ret < 0) {
		mtkpasr_err("Failed to initialize mtkpasr device\n");
		goto reset_devices;
	}

	/*                        */
	ret = sysfs_create_group(power_kobj, &mtkpasr_attr_group);
	if (ret < 0) {
		mtkpasr_err("Error creating sysfs group\n");
		goto reset_devices;
	}

	/*                                          */
	num_banks = compute_valid_pasr_range(&mtkpasr_start_pfn, &mtkpasr_end_pfn, &num_ranks);
	if (num_banks < 0) {
		mtkpasr_err("No valid PASR range!\n");
		ret = -EINVAL;
		goto free_banks_ranks;
	}
	/*                                                           */
	mtkpasr_banks = kzalloc(num_banks * sizeof(struct mtkpasr_bank), GFP_KERNEL);
	if (!mtkpasr_banks) {
		mtkpasr_err("Error allocating mtkpasr banks information!\n");
		ret = -ENOMEM;
		goto free_banks_ranks;
	}
	mtkpasr_ranks = kzalloc(num_ranks * sizeof(struct mtkpasr_rank), GFP_KERNEL);
	if (!mtkpasr_ranks) {
		mtkpasr_err("Error allocating mtkpasr ranks information!\n");
		ret = -ENOMEM;
		goto free_banks_ranks;
	}
	mtkpasr_construct_bankrank();

	/*                        */
	mtkpasr_migration_end = NODE_DATA(0)->node_start_pfn + pasrbank_pfns;

	mtkpasr_info("num_banks[%d] num_ranks[%d] mtkpasr_start_pfn[%ld] mtkpasr_end_pfn[%ld] mtkpasr_total_pfns[%ld]\n",
			num_banks, num_ranks, mtkpasr_start_pfn, mtkpasr_end_pfn, mtkpasr_total_pfns);

	/*                                    */
	register_early_suspend(&mtkpasr_early_suspend_desc);

	/*              */
	nz = &NODE_DATA(0)->node_zones[ZONE_NORMAL];
	safe_level = low_wmark_pages(nz);
#ifdef CONFIG_HIGHMEM
	safe_level += nz->lowmem_reserve[ZONE_HIGHMEM];
#endif
	safe_level = safe_level >> 2;

	return 0;

free_banks_ranks:
	if (mtkpasr_banks != NULL) {
		kfree(mtkpasr_banks);
		mtkpasr_banks = NULL;
	}
	if (mtkpasr_ranks != NULL) {
		kfree(mtkpasr_ranks);
		mtkpasr_ranks = NULL;
	}
	sysfs_remove_group(power_kobj, &mtkpasr_attr_group);

reset_devices:
	mtkpasr_reset_device(mtkpasr_device);
	free_pages((unsigned long)extcomp, get_order(MTKPASR_MAX_EXTCOMP * sizeof(unsigned long)));
	free_pages((unsigned long)sorted_for_extcomp, get_order(pasrbank_pfns * sizeof(unsigned long)));

no_memory:
	free_pages((unsigned long)src_pgmap, get_order(pasrbank_pfns * sizeof(unsigned long)));

free_devices:
	kfree(mtkpasr_device);

out:
	zs_destroy_pool(mtkpasr_mem_pool);

no_mem_pool:
	kfree(mtkpasr_table);
	mtkpasr_table = NULL;
	mtkpasr_disksize = 0;

fail:
	/*                 */
	mtkpasr_enable = 0;
	mtkpasr_enable_sr = 0;
	num_banks = 0;

	return ret;
}

static void __exit mtkpasr_exit(void)
{
	sysfs_remove_group(power_kobj, &mtkpasr_attr_group);
	if (mtkpasr_device->init_done)
		mtkpasr_reset_device(mtkpasr_device);

	mtkpasr_reset_global();

	kfree(mtkpasr_device);

	unregister_early_suspend(&mtkpasr_early_suspend_desc);

	pr_debug("Cleanup done!\n");
}
subsys_initcall_sync(mtkpasr_init);
module_exit(mtkpasr_exit);

MODULE_AUTHOR("MTK");
MODULE_DESCRIPTION("MTK proprietary PASR driver");
