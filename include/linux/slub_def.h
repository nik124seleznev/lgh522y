#ifndef _LINUX_SLUB_DEF_H
#define _LINUX_SLUB_DEF_H

/*
                                                 
  
                                  
 */
#include <linux/types.h>
#include <linux/gfp.h>
#include <linux/bug.h>
#include <linux/workqueue.h>
#include <linux/kobject.h>

#include <linux/kmemleak.h>

enum stat_item {
	ALLOC_FASTPATH,		/*                          */
	ALLOC_SLOWPATH,		/*                                      */
	FREE_FASTPATH,		/*                  */
	FREE_SLOWPATH,		/*                         */
	FREE_FROZEN,		/*                        */
	FREE_ADD_PARTIAL,	/*                                    */
	FREE_REMOVE_PARTIAL,	/*                             */
	ALLOC_FROM_PARTIAL,	/*                                          */
	ALLOC_SLAB,		/*                                       */
	ALLOC_REFILL,		/*                                    */
	ALLOC_NODE_MISMATCH,	/*                    */
	FREE_SLAB,		/*                                  */
	CPUSLAB_FLUSH,		/*                            */
	DEACTIVATE_FULL,	/*                                    */
	DEACTIVATE_EMPTY,	/*                                     */
	DEACTIVATE_TO_HEAD,	/*                                            */
	DEACTIVATE_TO_TAIL,	/*                                            */
	DEACTIVATE_REMOTE_FREES,/*                                       */
	DEACTIVATE_BYPASS,	/*                       */
	ORDER_FALLBACK,		/*                                        */
	CMPXCHG_DOUBLE_CPU_FAIL,/*                                    */
	CMPXCHG_DOUBLE_FAIL,	/*                                                   */
	CPU_PARTIAL_ALLOC,	/*                           */
	CPU_PARTIAL_FREE,	/*                            */
	CPU_PARTIAL_NODE,	/*                                      */
	CPU_PARTIAL_DRAIN,	/*                                   */
	NR_SLUB_STAT_ITEMS };

struct kmem_cache_cpu {
	void **freelist;	/*                                  */
	unsigned long tid;	/*                                */
	struct page *page;	/*                                       */
	struct page *partial;	/*                                  */
#ifdef CONFIG_SLUB_STATS
	unsigned stat[NR_SLUB_STAT_ITEMS];
#endif
};

/*
                                                                      
                                                                       
                             
 */
struct kmem_cache_order_objects {
	unsigned long x;
};

/*
                         
 */
struct kmem_cache {
	struct kmem_cache_cpu __percpu *cpu_slab;
	/*                                      */
	unsigned long flags;
	unsigned long min_partial;
	int size;		/*                                           */
	int object_size;	/*                                         */
	int offset;		/*                      */
	int cpu_partial;	/*                                                  */
	struct kmem_cache_order_objects oo;

	/*                                 */
	struct kmem_cache_order_objects max;
	struct kmem_cache_order_objects min;
	gfp_t allocflags;	/*                                */
	int refcount;		/*                                 */
	void (*ctor)(void *);
	int inuse;		/*                    */
	int align;		/*           */
	int reserved;		/*                                    */
	const char *name;	/*                          */
	struct list_head list;	/*                     */
#ifdef CONFIG_SYSFS
	struct kobject kobj;	/*           */
#endif
#ifdef CONFIG_MEMCG_KMEM
	struct memcg_cache_params *memcg_params;
	int max_attr_size; /*                                                */
#endif

#ifdef CONFIG_NUMA
	/*
                                                     
  */
	int remote_node_defrag_ratio;
#endif
	struct kmem_cache_node *node[MAX_NUMNODES];
};

void *kmem_cache_alloc(struct kmem_cache *, gfp_t);
void *__kmalloc(size_t size, gfp_t flags);

static __always_inline void *
kmalloc_order(size_t size, gfp_t flags, unsigned int order)
{
	void *ret;

	flags |= (__GFP_COMP | __GFP_KMEMCG);
	ret = (void *) __get_free_pages(flags, order);
	kmemleak_alloc(ret, size, 1, flags);
	return ret;
}

/* 
                                                              
                                                       
 */
#ifdef CONFIG_SLUB_DEBUG
extern bool verify_mem_not_deleted(const void *x);
#else
static inline bool verify_mem_not_deleted(const void *x)
{
	return true;
}
#endif

#ifdef CONFIG_TRACING
extern void *
kmem_cache_alloc_trace(struct kmem_cache *s, gfp_t gfpflags, size_t size);
extern void *kmalloc_order_trace(size_t size, gfp_t flags, unsigned int order);
#else
static __always_inline void *
kmem_cache_alloc_trace(struct kmem_cache *s, gfp_t gfpflags, size_t size)
{
	return kmem_cache_alloc(s, gfpflags);
}

static __always_inline void *
kmalloc_order_trace(size_t size, gfp_t flags, unsigned int order)
{
	return kmalloc_order(size, flags, order);
}
#endif

static __always_inline void *kmalloc_large(size_t size, gfp_t flags)
{
	unsigned int order = get_order(size);
	return kmalloc_order_trace(size, flags, order);
}

static __always_inline void *kmalloc(size_t size, gfp_t flags)
{
	if (__builtin_constant_p(size)) {
		if (size > KMALLOC_MAX_CACHE_SIZE)
			return kmalloc_large(size, flags);

		if (!(flags & GFP_DMA)) {
			int index = kmalloc_index(size);

			if (!index)
				return ZERO_SIZE_PTR;

			return kmem_cache_alloc_trace(kmalloc_caches[index],
					flags, size);
		}
	}
	return __kmalloc(size, flags);
}

#ifdef CONFIG_NUMA
void *__kmalloc_node(size_t size, gfp_t flags, int node);
void *kmem_cache_alloc_node(struct kmem_cache *, gfp_t flags, int node);

#ifdef CONFIG_TRACING
extern void *kmem_cache_alloc_node_trace(struct kmem_cache *s,
					   gfp_t gfpflags,
					   int node, size_t size);
#else
static __always_inline void *
kmem_cache_alloc_node_trace(struct kmem_cache *s,
			      gfp_t gfpflags,
			      int node, size_t size)
{
	return kmem_cache_alloc_node(s, gfpflags, node);
}
#endif

static __always_inline void *kmalloc_node(size_t size, gfp_t flags, int node)
{
	if (__builtin_constant_p(size) &&
		size <= KMALLOC_MAX_CACHE_SIZE && !(flags & GFP_DMA)) {
		int index = kmalloc_index(size);

		if (!index)
			return ZERO_SIZE_PTR;

		return kmem_cache_alloc_node_trace(kmalloc_caches[index],
			       flags, node, size);
	}
	return __kmalloc_node(size, flags, node);
}
#endif

#endif /*                   */
