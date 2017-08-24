#define pr_fmt(fmt) "["KBUILD_MODNAME"]" fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/memory.h>
#include <linux/memblock.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <mach/emi_mpu.h>

#ifdef CONFIG_ARM_LPAE
#include <mach/mt_lpae.h>
#endif

#define CONFIG_MTKPASR_MINDIESIZE_PFN		(0x20000)	/*       */
#define MTKPASR_1GB_PFNS			(0x40000)	/*     */
#define MTKPASR_2GB_PFNS			(0x80000)	/*     */
#define MTKPASR_3GB_PFNS			(0xC0000)	/*     */
#define MTKPASR_4GB_PFNS			(0x100000)	/*     */
#define MTKPASR_DRAM_MINSIZE			MTKPASR_2GB_PFNS

/*                         */
#ifndef NO_UART_CONSOLE
#define PRINT(len, string, args...)	printk(KERN_ALERT string, ##args)
#else
unsigned char mtkpasr_log_buf[4096];
static int log_stored;
#define PRINT(len, string, args...)	do { sprintf(mtkpasr_log_buf + log_stored, string, ##args); log_stored += len; } while (0)
#endif

/*                                               */
struct view_rank {
	unsigned long start_pfn;	/*             */
	unsigned long end_pfn;		/*                                  */
	unsigned long bank_pfn_size;	/*                  */
	unsigned long valid_channel;	/*                                                                                     */
};
static struct view_rank rank_info[MAX_RANKS];

/*                          */
static struct basic_dram_setting pasrdpd;
/*                             */
static int mtkpasr_start_rank;
/*                                     */
unsigned long pasrbank_pfns = 0;
/*
                                                                                                  
                                                          
 */
static unsigned long mtkpasr_pfn_start;
static unsigned long mtkpasr_pfn_end;

/*              */
static unsigned long valid_segment = 0x0;

/*                          */
extern void set_pageblock_mobility(struct page *page, int mobility);

/*                       */
extern bool pasr_is_valid(void)__attribute__((weak));
/*                          */
static inline bool could_do_mtkpasr(void)
{
	return false;
#if 0
	if (pasr_is_valid) {
		return pasr_is_valid();
	}

	return false;
#endif
}

#ifdef CONFIG_ARM_LPAE
#define MAX_RANK_PFN	(0x1FFFFF)
#define MAX_KERNEL_PFN	(0x13FFFF)
#define MAX_KPFN_MASK	(0x0FFFFF)
#define KPFN_TO_VIRT	(0x100000)
static unsigned long __init virt_to_kernel_pfn(unsigned long virt)
{
	unsigned long ret = virt;

	if (enable_4G()) {
		if (virt > MAX_RANK_PFN) {
			ret = virt - KPFN_TO_VIRT;
		} else if (virt > MAX_KERNEL_PFN) {
			ret = virt & MAX_KPFN_MASK;
		}
	}

	return ret;
}
static unsigned long __init kernel_pfn_to_virt(unsigned long kpfn, bool is_end)
{
	unsigned long ret = kpfn;

	if (enable_4G()) {
		if (is_end) {
			ret = kpfn + KPFN_TO_VIRT;
		} else {
			ret = kpfn | KPFN_TO_VIRT;
		}
	}

	return ret;
}
static unsigned long __init rank_pfn_offset(void)
{
	unsigned long ret = ARCH_PFN_OFFSET;

	if (enable_4G()) {
		ret = KPFN_TO_VIRT;
	}

	return ret;
}
#else
#define virt_to_kernel_pfn(x)		(x)
#define kernel_pfn_to_virt(x, y)	(x)
#define rank_pfn_offset()		((unsigned long)ARCH_PFN_OFFSET)
#endif

/*                                  */
static unsigned long __init round_up_base_offset(unsigned long input, unsigned long base, unsigned long offset)
{
	return ((input - offset + base - 1) / base) * base + offset;
}

/*                                    */
static unsigned long __init round_down_base_offset(unsigned long input, unsigned long base, unsigned long offset)
{
	return ((input - offset) / base) * base + offset;
}

/*
                                                                           
 */
extern void acquire_dram_setting(struct basic_dram_setting *pasrdpd)__attribute__((weak));
static bool __init parse_dram_setting(unsigned long hint)
{
	int channel_num, chan, rank, check_segment_num;
	unsigned long valid_channel;
	unsigned long check_rank_size, rank_pfn, start_pfn = rank_pfn_offset();

	PRINT(29, "rank_pfn_offset() [0x%8lx]\n", rank_pfn_offset());

	if (acquire_dram_setting) {
		hint = 0;
		acquire_dram_setting(&pasrdpd);
		channel_num = pasrdpd.channel_nr;
		/*          */
		for (rank = 0; rank < MAX_RANKS; ++rank) {
			rank_pfn = 0;
			rank_info[rank].valid_channel = 0x0;
			valid_channel = 0x1;
			check_rank_size = 0x0;
			check_segment_num = 0x0;
			for (chan = 0; chan < channel_num; ++chan) {
				if (pasrdpd.channel[chan].rank[rank].valid_rank) {
					rank_pfn += (pasrdpd.channel[chan].rank[rank].rank_size << (27 - PAGE_SHIFT));
					rank_info[rank].valid_channel |= valid_channel;
					/*                            */
					if (!check_rank_size) {
						check_rank_size = pasrdpd.channel[chan].rank[rank].rank_size;
					} else {
						/*                                       */
						if (check_rank_size != pasrdpd.channel[chan].rank[rank].rank_size) {
							return false;
						}
					}
					/*                                 */
					if (!check_segment_num) {
						check_segment_num = pasrdpd.channel[chan].rank[rank].segment_nr;
					} else {
						/*                                                 */
						if (check_segment_num != pasrdpd.channel[chan].rank[rank].segment_nr) {
							return false;
						}
					}
				}
				valid_channel <<= 8;
			}
			/*                            */
			if (check_rank_size != 0 && check_segment_num != 0) {
				rank_info[rank].start_pfn = virt_to_kernel_pfn(start_pfn);
				rank_info[rank].end_pfn = virt_to_kernel_pfn(start_pfn + rank_pfn);
				rank_info[rank].bank_pfn_size = rank_pfn/check_segment_num;
				start_pfn = kernel_pfn_to_virt(rank_info[rank].end_pfn, true);
				PRINT(96, "Rank[%d] start_pfn[%8lu] end_pfn[%8lu] bank_pfn_size[%8lu] valid_channel[0x%-8lx]\n",
						rank, rank_info[rank].start_pfn, rank_info[rank].end_pfn,
						rank_info[rank].bank_pfn_size, rank_info[rank].valid_channel);
			} else {
				rank_info[rank].start_pfn = virt_to_kernel_pfn(rank_pfn_offset());
				rank_info[rank].end_pfn = virt_to_kernel_pfn(rank_pfn_offset());
				rank_info[rank].bank_pfn_size = 0;
				rank_info[rank].valid_channel = 0x0;
			}
			/*                      */
			hint += rank_pfn;
		}
	} else {
		/*                                                                          */
		rank_pfn = (hint + CONFIG_MTKPASR_MINDIESIZE_PFN - 1) & ~(CONFIG_MTKPASR_MINDIESIZE_PFN - 1);
		rank_pfn >>= 1;
		for (rank = 0; rank < 2; ++rank) {
			rank_info[rank].start_pfn = virt_to_kernel_pfn(start_pfn);
			rank_info[rank].end_pfn = virt_to_kernel_pfn(start_pfn + rank_pfn);
			rank_info[rank].bank_pfn_size = rank_pfn >> 3;
			rank_info[rank].valid_channel = 0x1;
			start_pfn = kernel_pfn_to_virt(rank_info[rank].end_pfn, true);
			PRINT(96, "Rank[%d] start_pfn[%8lu] end_pfn[%8lu] bank_pfn_size[%8lu] valid_channel[0x%-8lx]\n",
					rank, rank_info[rank].start_pfn, rank_info[rank].end_pfn,
					rank_info[rank].bank_pfn_size, rank_info[rank].valid_channel);
		}
		/*                       */
		for (; rank < MAX_RANKS; ++rank) {
			rank_info[rank].start_pfn = virt_to_kernel_pfn(rank_pfn_offset());
			rank_info[rank].end_pfn = virt_to_kernel_pfn(rank_pfn_offset());
			rank_info[rank].bank_pfn_size = 0;
			rank_info[rank].valid_channel = 0x0;
		}
	}

	/*                                             */
	if (hint < MTKPASR_DRAM_MINSIZE) {
		printk(KERN_ALERT "[MTKPASR] Total memory: %lu < 1GB\n", (hint << PAGE_SHIFT));
		return false;
	}

	return true;
}

/*                                  */
static bool __init is_valid_rank(int rank)
{
	/*                     */
	if (rank_info[rank].start_pfn == rank_info[rank].end_pfn) {
		return false;
	}

	/*                     */
	if (rank_info[rank].valid_channel == 0x0) {
		return false;
	}

	return true;
}

#if 0
/*               */
void show_memblock(void)
{
	struct memblock_region *reg;
	phys_addr_t start;
	phys_addr_t end;

	for_each_memblock(memory, reg) {
		start = reg->base;
		end = start + reg->size;
		printk(KERN_EMERG"[PHY layout]kernel   :   0x%08llx - 0x%08llx (0x%08llx)\n",
				(unsigned long long)start,
				(unsigned long long)end - 1,
				(unsigned long long)reg->size);
	}

	for_each_memblock(reserved, reg) {
		start = reg->base;
		end = start + reg->size;
		printk(KERN_EMERG"[PHY layout]reserved   :   0x%08llx - 0x%08llx (0x%08llx)\n",
				(unsigned long long)start,
				(unsigned long long)end - 1,
				(unsigned long long)reg->size);

	}
}
#endif

#define PHYS_TO_PFN(x)	__phys_to_pfn(x)
/*                    */
static void __init mark_valid_segment(unsigned long start, unsigned long end, bool last)
{
	int num_segment, rank;
	unsigned long spfn, epfn;
	unsigned long rspfn, repfn;

	num_segment = 0;
	for (rank = 0; rank < MAX_RANKS; ++rank) {
		spfn = kernel_pfn_to_virt(start, false);
		epfn = kernel_pfn_to_virt(end, true);
		rspfn = kernel_pfn_to_virt(rank_info[rank].start_pfn, false);
		repfn = kernel_pfn_to_virt(rank_info[rank].end_pfn, true);
		if (is_valid_rank(rank)) {
			spfn = max(spfn, rspfn);
			if (repfn > spfn) {
				if (last) {
					spfn = round_down_base_offset(spfn, rank_info[rank].bank_pfn_size, rank_pfn_offset());		/*            */
				} else {
					spfn = round_up_base_offset(spfn, rank_info[rank].bank_pfn_size, rank_pfn_offset());		/*          */
				}
				epfn = min(epfn, repfn);
				while (epfn >= (spfn + rank_info[rank].bank_pfn_size)) {
					valid_segment |= (1 << ((spfn - rspfn) / rank_info[rank].bank_pfn_size + num_segment));
					spfn += rank_info[rank].bank_pfn_size;
				}
			}
			num_segment += (repfn - rspfn) / rank_info[rank].bank_pfn_size;
		}
	}
}

/*                                      */
static void __init set_page_mobility_mtkpasr(unsigned long start, unsigned long end, bool last)
{
	int rank;
	unsigned long spfn, epfn, espfn, vpfn, pfn;
	unsigned long rspfn, repfn;
	struct page *page;

	for (rank = 0; rank < MAX_RANKS; ++rank) {
		spfn = kernel_pfn_to_virt(start, false);
		epfn = kernel_pfn_to_virt(end, true);
		rspfn = kernel_pfn_to_virt(rank_info[rank].start_pfn, false);
		repfn = kernel_pfn_to_virt(rank_info[rank].end_pfn, true);
		if (is_valid_rank(rank)) {
			spfn = max(spfn, rspfn);
			if (repfn > spfn) {
				if (last) {
					spfn = round_down_base_offset(spfn, rank_info[rank].bank_pfn_size, rank_pfn_offset());		/*            */
				} else {
					spfn = round_up_base_offset(spfn, rank_info[rank].bank_pfn_size, rank_pfn_offset());		/*          */
				}
				epfn = min(epfn, repfn);
				espfn = spfn + rank_info[rank].bank_pfn_size;
				while (epfn >= espfn) {
					/*                                      */
					for (vpfn = spfn; vpfn < espfn; vpfn++) {
						pfn = virt_to_kernel_pfn(vpfn);
						/*                                                                                          */
						if (!pfn_valid(pfn))
							continue;
						/*                           */
						page = pfn_to_page(pfn);
						if (!(pfn & (pageblock_nr_pages - 1)))
							set_pageblock_mobility(page, MIGRATE_MTKPASR);
					}
					spfn += rank_info[rank].bank_pfn_size;
					espfn = spfn + rank_info[rank].bank_pfn_size;
				}
			}
		}
	}
}

/*                                                    */
static void __init fix_memblock_region(unsigned long *start, unsigned long *end)
{
	int rank;
	unsigned long total_pfn_size = 0;
	unsigned long rspfn, repfn, spfn, epfn;

	spfn = *start;
	epfn = *end;

	/*               */
	for (rank = 0; rank < MAX_RANKS; ++rank) {
		if (is_valid_rank(rank)) {
			rspfn = kernel_pfn_to_virt(rank_info[rank].start_pfn, false);
			repfn = kernel_pfn_to_virt(rank_info[rank].end_pfn, true);
			total_pfn_size += (repfn - rspfn);
		}
	}
	repfn = kernel_pfn_to_virt(rank_info[0].start_pfn, false) + (total_pfn_size >> 1);
	repfn = virt_to_kernel_pfn(repfn);
	if (spfn == (repfn + 1)) {
		spfn = repfn;
	}
	if (epfn == (repfn - 1)) {
		epfn = repfn;
	}

	/*               */
	*start = spfn;
	*end = epfn;
}

/*                                        */
static void __init construct_mtkpasr_range(void)
{
	unsigned long vstart, vend;
	struct memblock_region *reg;
	unsigned long start = 0;
	unsigned long end = ~(unsigned long)0;

	/*                            */
	for_each_memblock(memory, reg) {
		vstart = mtkpasr_pfn_start;
		vend = mtkpasr_pfn_end;
		start = PHYS_TO_PFN(reg->base);
		end = PHYS_TO_PFN(reg->base + reg->size);
		/*                     */
		fix_memblock_region(&start, &end);
		/*           */
		if (end > vstart && start < vend) {
			vstart = max(start, vstart);
			vend = min(end, vend);
			/*                    */
			mark_valid_segment(vstart, vend, false);
			/*                   */
			set_page_mobility_mtkpasr(vstart, vend, false);
		}
	}

	/*                  */

	/*                  
                      
                                                                                      
                                                
                                                                                          
  */

#ifndef CONFIG_MTKPASR_NO_LASTBANK
	vend = mtkpasr_pfn_end;
	/*                                                */
	if (end < vend) {
		vstart = max(end, mtkpasr_pfn_start);
		/*                    */
		mark_valid_segment(vstart, vend, true);
		/*                   */
		set_page_mobility_mtkpasr(vstart, vend, true);
	}
#endif
}

/*
                                                              
                                                               
                                            
 */
void __init init_mtkpasr_range(struct zone *zone)
{
	struct pglist_data *pgdat;
	int rank;
	unsigned long start_pfn;
	unsigned long end_pfn;
	unsigned long pfn_bank_alignment = 0;
	unsigned long shift_size = 0;

	/*                                          */
	if (!could_do_mtkpasr()) {
		/*                    */
		return;
	}

	/*               */
	pgdat = zone->zone_pgdat;

	/*                      */
	if (parse_dram_setting(pgdat->node_spanned_pages) == false) {
		/*                    */
		return;
	}

#ifdef CONFIG_HIGHMEM
	/*                                                            */
	zone = zone + ZONE_HIGHMEM;
#else
	zone = zone + ZONE_NORMAL;
#endif

	/*                                    */
	if (!populated_zone(zone)) {
		/*                    */
		return;
	}

	/*                  */
	end_pfn = zone->zone_start_pfn + zone->spanned_pages;

	/*                                                    */
	for (rank = MAX_RANKS - 1; rank >= 0; --rank) {
		if (is_valid_rank(rank)) {
			end_pfn = min(end_pfn, rank_info[rank].end_pfn);
			break;
		}
	}

	/*                                        */
	for (rank = 0; rank < MAX_RANKS; ++rank) {
		if (is_valid_rank(rank)) {
			shift_size += (kernel_pfn_to_virt(rank_info[rank].end_pfn, true) - kernel_pfn_to_virt(rank_info[rank].start_pfn, false));
		}
	}

	/*                                     */
	start_pfn = rank_pfn_offset() + (shift_size >> 1);
	if (shift_size <= MTKPASR_1GB_PFNS) {
		start_pfn += (shift_size >> 4);
	}
	if (shift_size >= MTKPASR_2GB_PFNS) {
		start_pfn -= (shift_size >> 4);
	}
	if (shift_size >= MTKPASR_3GB_PFNS) {
		start_pfn -= (shift_size >> 4);
	}
	if (shift_size >= MTKPASR_4GB_PFNS) {
		start_pfn -= (shift_size >> 4);
	}

	/*               */
	start_pfn = max(start_pfn, kernel_pfn_to_virt(zone->zone_start_pfn, false));
	start_pfn = virt_to_kernel_pfn(start_pfn);

	/*                                            */
	for (rank = 0; rank < MAX_RANKS; ++rank) {
		if (kernel_pfn_to_virt(start_pfn, false) < kernel_pfn_to_virt(rank_info[rank].end_pfn, true) &&
				kernel_pfn_to_virt(start_pfn, false) >= kernel_pfn_to_virt(rank_info[rank].start_pfn, false)) {
			mtkpasr_start_rank = rank;
			pfn_bank_alignment = rank_info[rank].bank_pfn_size;
			break;
		}
	}

	/*              */
	if (!pfn_bank_alignment) {
		/*                    */
		return;
	}

	/*                         */
	pasrbank_pfns = pfn_bank_alignment;

	/*                            */
	start_pfn = round_up_base_offset(start_pfn, pfn_bank_alignment, ARCH_PFN_OFFSET);

	/*                                          */
	for (rank = 0; rank < MAX_RANKS; ++rank) {
		if (kernel_pfn_to_virt(end_pfn, true) <= kernel_pfn_to_virt(rank_info[rank].end_pfn, true) &&
				kernel_pfn_to_virt(end_pfn, true) > kernel_pfn_to_virt(rank_info[rank].start_pfn, false)) {
			pfn_bank_alignment = rank_info[rank].bank_pfn_size;
			break;
		}
	}

	/*                               */
	if (pasrbank_pfns < pfn_bank_alignment) {
		pasrbank_pfns = pfn_bank_alignment;
	}

	/*                                */
	mtkpasr_pfn_start = start_pfn;
	mtkpasr_pfn_end	= end_pfn;

	/*                                                                                    */
	mtkpasr_pfn_end = round_up_base_offset(mtkpasr_pfn_end, pfn_bank_alignment, ARCH_PFN_OFFSET);

	/*                                                 */
	construct_mtkpasr_range();

	PRINT(138, "[MTKPASR] @@@@@@ Start_pfn[%8lu] End_pfn[%8lu] (MTKPASR) start_pfn[%8lu] end_pfn[%8lu] Valid_segment[0x%8lx] @@@@@@\n",
			start_pfn, end_pfn, mtkpasr_pfn_start, mtkpasr_pfn_end, valid_segment);
}

/*                                                 */
static void mtkpasr_reserve_reserved(void)
{
	int order = MAX_ORDER - 1, t;
	struct list_head *curr, *tmp;
	struct page *spage;
	unsigned long flags;
	unsigned long pfn;

	/*            */
	for_each_migratetype_order(order, t) {
		if (t != MIGRATE_MTKPASR) {
			list_for_each_safe(curr, tmp, &MTKPASR_ZONE->free_area[order].free_list[t]) {
				spage = list_entry(curr, struct page, lru);
				pfn = page_to_pfn(spage);
				/*                           */
				if (pfn >= mtkpasr_pfn_start && pfn < mtkpasr_pfn_end) {
					printk(KERN_ALERT "\norder[%d] t[%d] pfn[%lu]\n", order, t, pfn);
					spin_lock_irqsave(&MTKPASR_ZONE->lock, flags);
					/*                                                   */
					list_move(&spage->lru, &MTKPASR_ZONE->free_area[order].free_list[MIGRATE_MTKPASR]);
					/*                           */
					set_pageblock_mobility(spage, MIGRATE_MTKPASR);
					spin_unlock_irqrestore(&MTKPASR_ZONE->lock, flags);
				}
			}
		}
	}
}

/*
                                                                    
  
                                                                                
                                                                                             
                                                                         
                                             
  
                                                                                     
 */
int __init compute_valid_pasr_range(unsigned long *start_pfn, unsigned long *end_pfn, int *num_ranks)
{
	int num_banks, rank, seg_num;
	unsigned long vseg = valid_segment;
	bool contain_rank;

	/*                                          */
	if (!could_do_mtkpasr()) {
		/*                    */
		return -1;
	}

	/*                                          */
	if (mtkpasr_pfn_start == mtkpasr_pfn_end) {
		/*                    */
		return -1;
	}

	/*                    */
	*start_pfn = mtkpasr_pfn_start;
	*end_pfn = mtkpasr_pfn_end;

	/*                                */
	num_banks = 0;
	*num_ranks = 0;
	for (rank = 0; rank < MAX_RANKS; ++rank) {
		if (is_valid_rank(rank)) {
			contain_rank = true;
			seg_num = (kernel_pfn_to_virt(rank_info[rank].end_pfn, true) - kernel_pfn_to_virt(rank_info[rank].start_pfn, false)) /
				rank_info[rank].bank_pfn_size;
			while (seg_num--) {
				if (vseg & 0x1) {
					num_banks++;
				} else {
					contain_rank = false;
				}
				vseg >>= 1;
			}
			if (contain_rank) {
				*num_ranks += 1;
			}
		}
	}

	/*                */
	if (num_banks == 0) {
		return -1;
	}

	/*                                                 */
	mtkpasr_reserve_reserved();

	return num_banks;
}

/*
                                                                                       
                                                                                                     
 */
int __init query_bank_information(int bank, unsigned long *spfn, unsigned long *epfn, bool fully)
{
	int seg_num = 0, rank, num_segment = 0;
	unsigned long vseg = valid_segment, valid_mask;

	/*       */
	*spfn = 0;
	*epfn = 0;

	/*               */
	do {
		if (vseg & 0x1) {
			if (!bank) {
				/*        */
				break;
			}
			bank--;
		}
		vseg >>= 1;
		seg_num++;
	} while (seg_num < BITS_PER_LONG);

	/*              */
	if (seg_num == BITS_PER_LONG) {
		return -1;
	}

	/*            */
	vseg = valid_segment;
	for (rank = 0; rank < MAX_RANKS; ++rank) {
		if (is_valid_rank(rank)) {
			num_segment = (kernel_pfn_to_virt(rank_info[rank].end_pfn, true) - kernel_pfn_to_virt(rank_info[rank].start_pfn, false)) /
				rank_info[rank].bank_pfn_size;
			if (seg_num < num_segment) {
				*spfn = virt_to_kernel_pfn(kernel_pfn_to_virt(rank_info[rank].start_pfn, false) + seg_num * rank_info[rank].bank_pfn_size);
				*epfn = virt_to_kernel_pfn(kernel_pfn_to_virt(*spfn, false) + rank_info[rank].bank_pfn_size);
				/*                                     */
				if (*epfn <= *spfn) {
					*epfn = kernel_pfn_to_virt(*epfn, true);
				}
				break;
			}
			seg_num -= num_segment;
			vseg >>= num_segment;
		}
	}

	/*              */
	if (rank == MAX_RANKS) {
		return -1;
	}

	/*                                                     */
	if (fully) {
		valid_mask = (1 << num_segment) - 1;
		if ((vseg & valid_mask) == valid_mask) {
			return rank;
		}
	}

	return -1;
}

/*
                                              
                                                                                               
                                                                 
  
                                                      
 */
u32 __init pasr_bank_to_segment(unsigned long start_pfn, unsigned long end_pfn)
{
	int num_segment, rank;
	unsigned long spfn, epfn;
	unsigned long rspfn, repfn;

	spfn = kernel_pfn_to_virt(start_pfn, false);
	epfn = kernel_pfn_to_virt(end_pfn, true);
	rspfn = kernel_pfn_to_virt(rank_info[0].start_pfn, false);

	num_segment = 0;
	for (rank = 0; rank < MAX_RANKS; ++rank) {
		if (is_valid_rank(rank)) {
			rspfn = kernel_pfn_to_virt(rank_info[rank].start_pfn, false);
			repfn = kernel_pfn_to_virt(rank_info[rank].end_pfn, true);
			if (rspfn <= spfn && repfn >= epfn) {
				break;
			}
			num_segment += (repfn - rspfn) / rank_info[rank].bank_pfn_size;
		}
	}

	/*              */
	if (rank == MAX_RANKS) {
		return (0x1F);
	}

	return ((spfn - rspfn) / rank_info[rank].bank_pfn_size + num_segment);

	/*
                           
                                                                                             
                            
                                     
                              
                                                       
                                
                                                                         
  */
}
