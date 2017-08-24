#ifndef _LINUX_SLAB_DEF_H
#define	_LINUX_SLAB_DEF_H

/*
                                                           
  
                                                                 
                                                                 
                                                               
                
 */

#include <linux/init.h>
#include <linux/compiler.h>

/*
                    
  
                   
 */

struct kmem_cache {
/*                                                   */
	unsigned int batchcount;
	unsigned int limit;
	unsigned int shared;

	unsigned int size;
	u32 reciprocal_buffer_size;
/*                                                   */

	unsigned int flags;		/*                */
	unsigned int num;		/*                    */

/*                      */
	/*                             */
	unsigned int gfporder;

	/*                               */
	gfp_t allocflags;

	size_t colour;			/*                       */
	unsigned int colour_off;	/*               */
	struct kmem_cache *slabp_cache;
	unsigned int slab_size;

	/*                  */
	void (*ctor)(void *obj);

/*                           */
	const char *name;
	struct list_head list;
	int refcount;
	int object_size;
	int align;

/*               */
#ifdef CONFIG_DEBUG_SLAB
	unsigned long num_active;
	unsigned long num_allocations;
	unsigned long high_mark;
	unsigned long grown;
	unsigned long reaped;
	unsigned long errors;
	unsigned long max_freeable;
	unsigned long node_allocs;
	unsigned long node_frees;
	unsigned long node_overflow;
	atomic_t allochit;
	atomic_t allocmiss;
	atomic_t freehit;
	atomic_t freemiss;

	/*
                                                                  
                                                                  
                                                                  
                                                                 
  */
	int obj_offset;
#endif /*                   */
#ifdef CONFIG_MEMCG_KMEM
	struct memcg_cache_params *memcg_params;
#endif

/*                                                           */
	/*
                                                                    
                                                     
                           
                                                                 
                                                                
   
                                                                   
                                                                 
                       
  */
	struct kmem_cache_node **node;
	struct array_cache *array[NR_CPUS + MAX_NUMNODES];
	/*
                                   
  */
};

void *kmem_cache_alloc(struct kmem_cache *, gfp_t);
void *__kmalloc(size_t size, gfp_t flags);

#ifdef CONFIG_TRACING
extern void *kmem_cache_alloc_trace(struct kmem_cache *, gfp_t, size_t);
#else
static __always_inline void *
kmem_cache_alloc_trace(struct kmem_cache *cachep, gfp_t flags, size_t size)
{
	return kmem_cache_alloc(cachep, flags);
}
#endif

static __always_inline void *kmalloc(size_t size, gfp_t flags)
{
	struct kmem_cache *cachep;
	void *ret;

	if (__builtin_constant_p(size)) {
		int i;

		if (!size)
			return ZERO_SIZE_PTR;

		if (WARN_ON_ONCE(size > KMALLOC_MAX_SIZE))
			return NULL;

		i = kmalloc_index(size);

#ifdef CONFIG_ZONE_DMA
		if (flags & GFP_DMA)
			cachep = kmalloc_dma_caches[i];
		else
#endif
			cachep = kmalloc_caches[i];

		ret = kmem_cache_alloc_trace(cachep, flags, size);

		return ret;
	}
	return __kmalloc(size, flags);
}

#ifdef CONFIG_NUMA
extern void *__kmalloc_node(size_t size, gfp_t flags, int node);
extern void *kmem_cache_alloc_node(struct kmem_cache *, gfp_t flags, int node);

#ifdef CONFIG_TRACING
extern void *kmem_cache_alloc_node_trace(struct kmem_cache *cachep,
					 gfp_t flags,
					 int nodeid,
					 size_t size);
#else
static __always_inline void *
kmem_cache_alloc_node_trace(struct kmem_cache *cachep,
			    gfp_t flags,
			    int nodeid,
			    size_t size)
{
	return kmem_cache_alloc_node(cachep, flags, nodeid);
}
#endif

static __always_inline void *kmalloc_node(size_t size, gfp_t flags, int node)
{
	struct kmem_cache *cachep;

	if (__builtin_constant_p(size)) {
		int i;

		if (!size)
			return ZERO_SIZE_PTR;

		if (WARN_ON_ONCE(size > KMALLOC_MAX_SIZE))
			return NULL;

		i = kmalloc_index(size);

#ifdef CONFIG_ZONE_DMA
		if (flags & GFP_DMA)
			cachep = kmalloc_dma_caches[i];
		else
#endif
			cachep = kmalloc_caches[i];

		return kmem_cache_alloc_node_trace(cachep, flags, node, size);
	}
	return __kmalloc_node(size, flags, node);
}

#endif	/*             */

#endif	/*                   */
