#ifndef __LINUX_MEMORY_HOTPLUG_H
#define __LINUX_MEMORY_HOTPLUG_H

#include <linux/mmzone.h>
#include <linux/spinlock.h>
#include <linux/notifier.h>
#include <linux/bug.h>

struct page;
struct zone;
struct pglist_data;
struct mem_section;
struct memory_block;

#ifdef CONFIG_MEMORY_HOTPLUG

/*
                                                                       
                                                                   
 */
enum {
	MEMORY_HOTPLUG_MIN_BOOTMEM_TYPE = 12,
	SECTION_INFO = MEMORY_HOTPLUG_MIN_BOOTMEM_TYPE,
	MIX_SECTION_INFO,
	NODE_INFO,
	MEMORY_HOTPLUG_MAX_BOOTMEM_TYPE = NODE_INFO,
};

/*                                                   */
enum {
	ONLINE_KEEP,
	ONLINE_KERNEL,
	ONLINE_MOVABLE,
};

/*
                           
 */
static inline
void pgdat_resize_lock(struct pglist_data *pgdat, unsigned long *flags)
{
	spin_lock_irqsave(&pgdat->node_size_lock, *flags);
}
static inline
void pgdat_resize_unlock(struct pglist_data *pgdat, unsigned long *flags)
{
	spin_unlock_irqrestore(&pgdat->node_size_lock, *flags);
}
static inline
void pgdat_resize_init(struct pglist_data *pgdat)
{
	spin_lock_init(&pgdat->node_size_lock);
}
/*
                          
  
                                                                    
                                                                  
                                                   
 */
static inline unsigned zone_span_seqbegin(struct zone *zone)
{
	return read_seqbegin(&zone->span_seqlock);
}
static inline int zone_span_seqretry(struct zone *zone, unsigned iv)
{
	return read_seqretry(&zone->span_seqlock, iv);
}
static inline void zone_span_writelock(struct zone *zone)
{
	write_seqlock(&zone->span_seqlock);
}
static inline void zone_span_writeunlock(struct zone *zone)
{
	write_sequnlock(&zone->span_seqlock);
}
static inline void zone_seqlock_init(struct zone *zone)
{
	seqlock_init(&zone->span_seqlock);
}
extern int zone_grow_free_lists(struct zone *zone, unsigned long new_nr_pages);
extern int zone_grow_waitqueues(struct zone *zone, unsigned long nr_pages);
extern int add_one_highpage(struct page *page, int pfn, int bad_ppro);
/*                                                     */
extern int online_pages(unsigned long, unsigned long, int);
extern void __offline_isolated_pages(unsigned long, unsigned long);

typedef void (*online_page_callback_t)(struct page *page);

extern int set_online_page_callback(online_page_callback_t callback);
extern int restore_online_page_callback(online_page_callback_t callback);

extern void __online_page_set_limits(struct page *page);
extern void __online_page_increment_counters(struct page *page);
extern void __online_page_free(struct page *page);

#ifdef CONFIG_MEMORY_HOTREMOVE
extern bool is_pageblock_removable_nolock(struct page *page);
extern int arch_remove_memory(u64 start, u64 size);
extern int __remove_pages(struct zone *zone, unsigned long start_pfn,
	unsigned long nr_pages);
#endif /*                         */

/*                                                                      */
extern int __add_pages(int nid, struct zone *zone, unsigned long start_pfn,
	unsigned long nr_pages);

#ifdef CONFIG_NUMA
extern int memory_add_physaddr_to_nid(u64 start);
#else
static inline int memory_add_physaddr_to_nid(u64 start)
{
	return 0;
}
#endif

#ifdef CONFIG_HAVE_ARCH_NODEDATA_EXTENSION
/*
                                                               
  
                                            
                                                                             
  
                                                
                                                                            
  
 */
extern pg_data_t *arch_alloc_nodedata(int nid);
extern void arch_free_nodedata(pg_data_t *pgdat);
extern void arch_refresh_nodedata(int nid, pg_data_t *pgdat);

#else /*                                     */

#define arch_alloc_nodedata(nid)	generic_alloc_nodedata(nid)
#define arch_free_nodedata(pgdat)	generic_free_nodedata(pgdat)

#ifdef CONFIG_NUMA
/*
                                                                         
                                                                             
                                                                           
                                                                  
 */
#define generic_alloc_nodedata(nid)				\
({								\
	kzalloc(sizeof(pg_data_t), GFP_KERNEL);			\
})
/*
                                                         
                                               
 */
#define generic_free_nodedata(pgdat)	kfree(pgdat)

extern pg_data_t *node_data[];
static inline void arch_refresh_nodedata(int nid, pg_data_t *pgdat)
{
	node_data[nid] = pgdat;
}

#else /*              */

/*              */
static inline pg_data_t *generic_alloc_nodedata(int nid)
{
	BUG();
	return NULL;
}
static inline void generic_free_nodedata(pg_data_t *pgdat)
{
}
static inline void arch_refresh_nodedata(int nid, pg_data_t *pgdat)
{
}
#endif /*             */
#endif /*                                     */

#ifdef CONFIG_HAVE_BOOTMEM_INFO_NODE
extern void register_page_bootmem_info_node(struct pglist_data *pgdat);
#else
static inline void register_page_bootmem_info_node(struct pglist_data *pgdat)
{
}
#endif
extern void put_page_bootmem(struct page *page);
extern void get_page_bootmem(unsigned long ingo, struct page *page,
			     unsigned long type);

/*
                                                                         
                                                                          
                               
 */

void lock_memory_hotplug(void);
void unlock_memory_hotplug(void);

#else /*                         */
/*
                                         
 */
static inline void pgdat_resize_lock(struct pglist_data *p, unsigned long *f) {}
static inline void pgdat_resize_unlock(struct pglist_data *p, unsigned long *f) {}
static inline void pgdat_resize_init(struct pglist_data *pgdat) {}

static inline unsigned zone_span_seqbegin(struct zone *zone)
{
	return 0;
}
static inline int zone_span_seqretry(struct zone *zone, unsigned iv)
{
	return 0;
}
static inline void zone_span_writelock(struct zone *zone) {}
static inline void zone_span_writeunlock(struct zone *zone) {}
static inline void zone_seqlock_init(struct zone *zone) {}

static inline int mhp_notimplemented(const char *func)
{
	printk(KERN_WARNING "%s() called, with CONFIG_MEMORY_HOTPLUG disabled\n", func);
	dump_stack();
	return -ENOSYS;
}

static inline void register_page_bootmem_info_node(struct pglist_data *pgdat)
{
}

static inline void lock_memory_hotplug(void) {}
static inline void unlock_memory_hotplug(void) {}

#endif /*                         */

#ifdef CONFIG_MEMORY_HOTREMOVE

extern int is_mem_section_removable(unsigned long pfn, unsigned long nr_pages);
extern void try_offline_node(int nid);

#else
static inline int is_mem_section_removable(unsigned long pfn,
					unsigned long nr_pages)
{
	return 0;
}

static inline void try_offline_node(int nid) {}
#endif /*                         */

extern int mem_online_node(int nid);
extern int add_memory(int nid, u64 start, u64 size);
extern int arch_add_memory(int nid, u64 start, u64 size);
extern int offline_pages(unsigned long start_pfn, unsigned long nr_pages);
extern int offline_memory_block(struct memory_block *mem);
extern bool is_memblock_offlined(struct memory_block *mem);
extern int remove_memory(int nid, u64 start, u64 size);
extern int sparse_add_one_section(struct zone *zone, unsigned long start_pfn,
								int nr_pages);
extern void sparse_remove_one_section(struct zone *zone, struct mem_section *ms);
extern struct page *sparse_decode_mem_map(unsigned long coded_mem_map,
					  unsigned long pnum);

#endif /*                          */
