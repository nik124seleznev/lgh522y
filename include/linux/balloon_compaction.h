/*
 * include/linux/balloon_compaction.h
 *
 * Common interface definitions for making balloon pages movable by compaction.
 *
 * Despite being perfectly possible to perform ballooned pages migration, they
 * make a special corner case to compaction scans because balloon pages are not
 * enlisted at any LRU list like the other pages we do compact / migrate.
 *
 * As the page isolation scanning step a compaction thread does is a lockless
 * procedure (from a page standpoint), it might bring some racy situations while
 * performing balloon page compaction. In order to sort out these racy scenarios
 * and safely perform balloon's page compaction and migration we must, always,
 * ensure following these three simple rules:
 *
 *   i. when updating a balloon's page ->mapping element, strictly do it under
 *      the following lock order, independently of the far superior
 *      locking scheme (lru_lock, balloon_lock):
 *	    +-page_lock(page);
 *	      +--spin_lock_irq(&b_dev_info->pages_lock);
 *	            ... page->mapping updates here ...
 *
 *  ii. before isolating or dequeueing a balloon page from the balloon device
 *      pages list, the page reference counter must be raised by one and the
 *      extra refcount must be dropped when the page is enqueued back into
 *      the balloon device page list, thus a balloon page keeps its reference
 *      counter raised only while it is under our special handling;
 *
 * iii. after the lockless scan step have selected a potential balloon page for
 *      isolation, re-test the page->mapping flags and the page ref counter
 *      under the proper page lock, to ensure isolating a valid balloon page
 *      (not yet isolated, nor under release procedure)
 *
 * The functions provided by this interface are placed to help on coping with
 * the aforementioned balloon page corner case, as well as to ensure the simple
 * set of exposed rules are satisfied while we are dealing with balloon pages
 * compaction / migration.
 *
 * Copyright (C) 2012, Red Hat, Inc.  Rafael Aquini <aquini@redhat.com>
 */
#ifndef _LINUX_BALLOON_COMPACTION_H
#define _LINUX_BALLOON_COMPACTION_H
#include <linux/pagemap.h>
#include <linux/page-flags.h>
#include <linux/migrate.h>
#include <linux/gfp.h>
#include <linux/err.h>

/*
                                         
                                                                       
                                                                            
                                                                         
                                                                           
 */
struct balloon_dev_info {
	void *balloon_device;		/*                           */
	struct address_space *mapping;	/*                               */
	unsigned long isolated_pages;	/*                                   */
	spinlock_t pages_lock;		/*                          */
	struct list_head pages;		/*                                  */
};

extern struct page *balloon_page_enqueue(struct balloon_dev_info *b_dev_info);
extern struct page *balloon_page_dequeue(struct balloon_dev_info *b_dev_info);
extern struct balloon_dev_info *balloon_devinfo_alloc(
						void *balloon_dev_descriptor);

static inline void balloon_devinfo_free(struct balloon_dev_info *b_dev_info)
{
	kfree(b_dev_info);
}

/*
                                                                         
                                       
  
                                                                               
                                                                            
                          
 */
static inline void balloon_page_free(struct page *page)
{
	/*
                                                                    
                                                                       
                                                                        
                                          
   
                                                                        
  */
	put_page(page);
	__free_page(page);
}

#ifdef CONFIG_BALLOON_COMPACTION
extern bool balloon_page_isolate(struct page *page);
extern void balloon_page_putback(struct page *page);
extern int balloon_page_migrate(struct page *newpage,
				struct page *page, enum migrate_mode mode);
extern struct address_space
*balloon_mapping_alloc(struct balloon_dev_info *b_dev_info,
			const struct address_space_operations *a_ops);

static inline void balloon_mapping_free(struct address_space *balloon_mapping)
{
	kfree(balloon_mapping);
}

/*
                                                                      
  
                                                                               
                                                                             
                                                             
                                                                            
                                                                                
                                                        
 */
static inline bool page_flags_cleared(struct page *page)
{
	return !(page->flags & PAGE_FLAGS_CHECK_AT_PREP);
}

/*
                                                                           
 */
static inline bool __is_movable_balloon_page(struct page *page)
{
	struct address_space *mapping = page->mapping;
	return mapping_balloon(mapping);
}

/*
                                                                             
                                                 
  
                                                                              
                                                                              
                                                                              
                                                                   
  
                                                                              
                                                                          
                                                                            
                     
 */
static inline bool balloon_page_movable(struct page *page)
{
	/*
                                                                    
                                                             
  */
	if (page_flags_cleared(page) && !page_mapped(page) &&
	    page_count(page) == 1)
		return __is_movable_balloon_page(page);

	return false;
}

/*
                                                                       
                                        
  
                                                                             
                                                                               
                                                                            
                                                                            
                                                         
 */
static inline bool isolated_balloon_page(struct page *page)
{
	/*                                                                    */
	if (page_flags_cleared(page) && !page_mapped(page) &&
	    page_count(page) >= 2)
		return __is_movable_balloon_page(page);

	return false;
}

/*
                                                                            
                                                      
                                                     
                                                 
                                             
  
                                                                             
                                                                      
 */
static inline void balloon_page_insert(struct page *page,
				       struct address_space *mapping,
				       struct list_head *head)
{
	page->mapping = mapping;
	list_add(&page->lru, head);
}

/*
                                                                         
                                                
                                                          
  
                                                                             
                                                                     
 */
static inline void balloon_page_delete(struct page *page)
{
	page->mapping = NULL;
	list_del(&page->lru);
}

/*
                                                                             
                                   
 */
static inline struct balloon_dev_info *balloon_page_device(struct page *page)
{
	struct address_space *mapping = page->mapping;
	if (likely(mapping))
		return mapping->private_data;

	return NULL;
}

static inline gfp_t balloon_mapping_gfp_mask(void)
{
	return GFP_HIGHUSER_MOVABLE;
}

static inline bool balloon_compaction_check(void)
{
	return true;
}

#else /*                            */

static inline void *balloon_mapping_alloc(void *balloon_device,
				const struct address_space_operations *a_ops)
{
	return ERR_PTR(-EOPNOTSUPP);
}

static inline void balloon_mapping_free(struct address_space *balloon_mapping)
{
	return;
}

static inline void balloon_page_insert(struct page *page,
				       struct address_space *mapping,
				       struct list_head *head)
{
	list_add(&page->lru, head);
}

static inline void balloon_page_delete(struct page *page)
{
	list_del(&page->lru);
}

static inline bool balloon_page_movable(struct page *page)
{
	return false;
}

static inline bool isolated_balloon_page(struct page *page)
{
	return false;
}

static inline bool balloon_page_isolate(struct page *page)
{
	return false;
}

static inline void balloon_page_putback(struct page *page)
{
	return;
}

static inline int balloon_page_migrate(struct page *newpage,
				struct page *page, enum migrate_mode mode)
{
	return 0;
}

static inline gfp_t balloon_mapping_gfp_mask(void)
{
	return GFP_HIGHUSER;
}

static inline bool balloon_compaction_check(void)
{
	return false;
}
#endif /*                           */
#endif /*                             */
