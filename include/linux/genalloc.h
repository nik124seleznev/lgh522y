/*
 * Basic general purpose allocator for managing special purpose
 * memory, for example, memory that is not managed by the regular
 * kmalloc/kfree interface.  Uses for this includes on-device special
 * memory, uncached memory etc.
 *
 * It is safe to use the allocator in NMI handlers and other special
 * unblockable contexts that could otherwise deadlock on locks.  This
 * is implemented by using atomic operations and retries on any
 * conflicts.  The disadvantage is that there may be livelocks in
 * extreme cases.  For better scalability, one allocator can be used
 * for each CPU.
 *
 * The lockless operation only works if there is enough memory
 * available.  If new memory is added to the pool a lock has to be
 * still taken.  So any user relying on locklessness has to ensure
 * that sufficient memory is preallocated.
 *
 * The basic atomic operation of this allocator is cmpxchg on long.
 * On architectures that don't have NMI-safe cmpxchg implementation,
 * the allocator can NOT be used in NMI handler.  So code uses the
 * allocator in NMI handler should depend on
 * CONFIG_ARCH_HAVE_NMI_SAFE_CMPXCHG.
 *
 * This source code is licensed under the GNU General Public License,
 * Version 2.  See the file COPYING for more details.
 */


#ifndef __GENALLOC_H__
#define __GENALLOC_H__

struct device;
struct device_node;

/* 
                                               
                          
                                 
                                              
                                                   
                                                          
 */
typedef unsigned long (*genpool_algo_t)(unsigned long *map,
			unsigned long size,
			unsigned long start,
			unsigned int nr,
			void *data);

/*
                                                   
 */
struct gen_pool {
	spinlock_t lock;
	struct list_head chunks;	/*                             */
	int min_alloc_order;		/*                          */

	genpool_algo_t algo;		/*                     */
	void *data;
};

/*
                                                         
 */
struct gen_pool_chunk {
	struct list_head next_chunk;	/*                    */
	atomic_t avail;
	phys_addr_t phys_addr;		/*                                           */
	unsigned long start_addr;	/*                               */
	unsigned long end_addr;		/*                                         */
	unsigned long bits[0];		/*                                    */
};

extern struct gen_pool *gen_pool_create(int, int);
extern phys_addr_t gen_pool_virt_to_phys(struct gen_pool *pool, unsigned long);
extern int gen_pool_add_virt(struct gen_pool *, unsigned long, phys_addr_t,
			     size_t, int);
/* 
                                                               
                                         
                                                         
                                                          
                                                                     
                            
  
                                                           
  
                                                  
 */
static inline int gen_pool_add(struct gen_pool *pool, unsigned long addr,
			       size_t size, int nid)
{
	return gen_pool_add_virt(pool, addr, -1, size, nid);
}
extern void gen_pool_destroy(struct gen_pool *);
extern unsigned long gen_pool_alloc(struct gen_pool *, size_t);
extern void gen_pool_free(struct gen_pool *, unsigned long, size_t);
extern void gen_pool_for_each_chunk(struct gen_pool *,
	void (*)(struct gen_pool *, struct gen_pool_chunk *, void *), void *);
extern size_t gen_pool_avail(struct gen_pool *);
extern size_t gen_pool_size(struct gen_pool *);

extern void gen_pool_set_algo(struct gen_pool *pool, genpool_algo_t algo,
		void *data);

extern unsigned long gen_pool_first_fit(unsigned long *map, unsigned long size,
		unsigned long start, unsigned int nr, void *data);

extern unsigned long gen_pool_best_fit(unsigned long *map, unsigned long size,
		unsigned long start, unsigned int nr, void *data);

extern struct gen_pool *devm_gen_pool_create(struct device *dev,
		int min_alloc_order, int nid);
extern struct gen_pool *dev_get_gen_pool(struct device *dev);

#ifdef CONFIG_OF
extern struct gen_pool *of_get_named_gen_pool(struct device_node *np,
	const char *propname, int index);
#else
static inline struct gen_pool *of_get_named_gen_pool(struct device_node *np,
	const char *propname, int index)
{
	return NULL;
}
#endif
#endif /*                */
