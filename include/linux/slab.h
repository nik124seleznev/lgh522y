/*
                                                            
  
                                  
                                                                   
                                       
 */

#ifndef _LINUX_SLAB_H
#define	_LINUX_SLAB_H

#include <linux/gfp.h>
#include <linux/types.h>
#include <linux/workqueue.h>


/*
                                        
                                                                    
 */
#define SLAB_DEBUG_FREE		0x00000100UL	/*                                           */
#define SLAB_RED_ZONE		0x00000400UL	/*                                 */
#define SLAB_POISON		0x00000800UL	/*                       */
#define SLAB_HWCACHE_ALIGN	0x00002000UL	/*                           */
#define SLAB_CACHE_DMA		0x00004000UL	/*                    */
#define SLAB_STORE_USER		0x00010000UL	/*                                             */
#define SLAB_PANIC		0x00040000UL	/*                                    */
#define SLAB_NO_DEBUG		0x10000000UL    /*                                      */
/*
                                               
  
                                                                     
                                                                    
                                                                     
                                                                        
  
                                                                   
                                                                    
                                          
  
                   
         
                               
              
                                                           
                   
  
                                                          
                     
                   
       
     
                      
  
                                                        
 */
#define SLAB_DESTROY_BY_RCU	0x00080000UL	/*                            */
#define SLAB_MEM_SPREAD		0x00100000UL	/*                                */
#define SLAB_TRACE		0x00200000UL	/*                             */

/*                                */
#ifdef CONFIG_DEBUG_OBJECTS
# define SLAB_DEBUG_OBJECTS	0x00400000UL
#else
# define SLAB_DEBUG_OBJECTS	0x00000000UL
#endif

#define SLAB_NOLEAKTRACE	0x00800000UL	/*                        */

/*                                         */
#ifdef CONFIG_KMEMCHECK
# define SLAB_NOTRACK		0x01000000UL
#else
# define SLAB_NOTRACK		0x00000000UL
#endif
#ifdef CONFIG_FAILSLAB
# define SLAB_FAILSLAB		0x02000000UL	/*                      */
#else
# define SLAB_FAILSLAB		0x00000000UL
#endif

/*                                                                          */
#define SLAB_RECLAIM_ACCOUNT	0x00020000UL		/*                         */
#define SLAB_TEMPORARY		SLAB_RECLAIM_ACCOUNT	/*                         */
/*
                                                                  
  
                                                                    
  
                                                                             
                           
 */
#define ZERO_SIZE_PTR ((void *)16)

#define ZERO_OR_NULL_PTR(x) ((unsigned long)(x) <= \
				(unsigned long)ZERO_SIZE_PTR)


struct mem_cgroup;
/*
                                       
 */
void __init kmem_cache_init(void);
int slab_is_available(void);

struct kmem_cache *kmem_cache_create(const char *, size_t, size_t,
			unsigned long,
			void (*)(void *));
struct kmem_cache *
kmem_cache_create_memcg(struct mem_cgroup *, const char *, size_t, size_t,
			unsigned long, void (*)(void *), struct kmem_cache *);
void kmem_cache_destroy(struct kmem_cache *);
int kmem_cache_shrink(struct kmem_cache *);
void kmem_cache_free(struct kmem_cache *, void *);

/*
                                                                  
                                                                    
  
                                                                  
                                                                  
                                                                   
 */
#define KMEM_CACHE(__struct, __flags) kmem_cache_create(#__struct,\
		sizeof(struct __struct), __alignof__(struct __struct),\
		(__flags), NULL)

/*
                                                      
 */
void * __must_check __krealloc(const void *, size_t, gfp_t);
void * __must_check krealloc(const void *, size_t, gfp_t);
void kfree(const void *);
void kzfree(const void *);
size_t ksize(const void *);

/*
                                                                           
                                                           
                                                             
 */
#if defined(ARCH_DMA_MINALIGN) && ARCH_DMA_MINALIGN > 8
#define ARCH_KMALLOC_MINALIGN ARCH_DMA_MINALIGN
#define KMALLOC_MIN_SIZE ARCH_DMA_MINALIGN
#define KMALLOC_SHIFT_LOW ilog2(ARCH_DMA_MINALIGN)
#else
#define ARCH_KMALLOC_MINALIGN __alignof__(unsigned long long)
#endif

#ifdef CONFIG_SLOB
/*
                                                              
                                                              
                                                           
                                                                   
  
                                                                 
                                                              
                                                               
                            
 */
struct kmem_cache {
	unsigned int object_size;/*                                 */
	unsigned int size;	/*                                   */
	unsigned int align;	/*                         */
	unsigned long flags;	/*                          */
	const char *name;	/*                     */
	int refcount;		/*             */
	void (*ctor)(void *);	/*                                */
	struct list_head list;	/*                                       */
};

#define KMALLOC_MAX_SIZE (1UL << 30)

#include <linux/slob_def.h>

#else /*             */

/*
                                    
 */

#ifdef CONFIG_SLAB
/*
                                                               
                                                                      
                   
  
                                                                         
                                                                       
                                  
 */
#define KMALLOC_SHIFT_HIGH	((MAX_ORDER + PAGE_SHIFT - 1) <= 25 ? \
				(MAX_ORDER + PAGE_SHIFT - 1) : 25)
#define KMALLOC_SHIFT_MAX	KMALLOC_SHIFT_HIGH
#ifndef KMALLOC_SHIFT_LOW
#define KMALLOC_SHIFT_LOW	5
#endif
#else
/*
                                                            
                                            
 */
#define KMALLOC_SHIFT_HIGH	(PAGE_SHIFT + 1)
#define KMALLOC_SHIFT_MAX	(MAX_ORDER + PAGE_SHIFT)
#ifndef KMALLOC_SHIFT_LOW
#define KMALLOC_SHIFT_LOW	3
#endif
#endif

/*                          */
#define KMALLOC_MAX_SIZE	(1UL << KMALLOC_SHIFT_MAX)
/*                                                     */
#define KMALLOC_MAX_CACHE_SIZE	(1UL << KMALLOC_SHIFT_HIGH)
/*                                                   */
#define KMALLOC_MAX_ORDER	(KMALLOC_SHIFT_MAX - PAGE_SHIFT)

/*
                     
 */
#ifndef KMALLOC_MIN_SIZE
#define KMALLOC_MIN_SIZE (1 << KMALLOC_SHIFT_LOW)
#endif

extern struct kmem_cache *kmalloc_caches[KMALLOC_SHIFT_HIGH + 1];
#ifdef CONFIG_ZONE_DMA
extern struct kmem_cache *kmalloc_dma_caches[KMALLOC_SHIFT_HIGH + 1];
#endif

/*
                                                                
              
                 
                      
                       
                        
 */
static __always_inline int kmalloc_index(size_t size)
{
	if (!size)
		return 0;

	if (size <= KMALLOC_MIN_SIZE)
		return KMALLOC_SHIFT_LOW;

	if (KMALLOC_MIN_SIZE <= 32 && size > 64 && size <= 96)
		return 1;
	if (KMALLOC_MIN_SIZE <= 64 && size > 128 && size <= 192)
		return 2;
	if (size <=          8) return 3;
	if (size <=         16) return 4;
	if (size <=         32) return 5;
	if (size <=         64) return 6;
	if (size <=        128) return 7;
	if (size <=        256) return 8;
	if (size <=        512) return 9;
	if (size <=       1024) return 10;
	if (size <=   2 * 1024) return 11;
	if (size <=   4 * 1024) return 12;
	if (size <=   8 * 1024) return 13;
	if (size <=  16 * 1024) return 14;
	if (size <=  32 * 1024) return 15;
	if (size <=  64 * 1024) return 16;
	if (size <= 128 * 1024) return 17;
	if (size <= 256 * 1024) return 18;
	if (size <= 512 * 1024) return 19;
	if (size <= 1024 * 1024) return 20;
	if (size <=  2 * 1024 * 1024) return 21;
	if (size <=  4 * 1024 * 1024) return 22;
	if (size <=  8 * 1024 * 1024) return 23;
	if (size <=  16 * 1024 * 1024) return 24;
	if (size <=  32 * 1024 * 1024) return 25;
	if (size <=  64 * 1024 * 1024) return 26;
	BUG();

	/*                                                                 */
	return -1;
}

#ifdef CONFIG_SLAB
#include <linux/slab_def.h>
#elif defined(CONFIG_SLUB)
#include <linux/slub_def.h>
#else
#error "Unknown slab allocator"
#endif

/*
                                                 
                                               
                      
 */
static __always_inline int kmalloc_size(int n)
{
	if (n > 2)
		return 1 << n;

	if (n == 1 && KMALLOC_MIN_SIZE <= 32)
		return 96;

	if (n == 2 && KMALLOC_MIN_SIZE <= 64)
		return 192;

	return 0;
}
#endif /*              */

/*
                                                                           
                                                                           
                   
 */
#ifndef ARCH_SLAB_MINALIGN
#define ARCH_SLAB_MINALIGN __alignof__(unsigned long long)
#endif
/*
                                                                             
                                                                        
                                                                             
                                                                           
                                                                      
                        
  
                                                                             
                                                                    
                                                                
  
                                                                              
  
                                                     
                                                            
                                                                              
                                                                          
                                                         
                                                                            
                                          
 */
struct memcg_cache_params {
	bool is_root_cache;
	union {
		struct kmem_cache *memcg_caches[0];
		struct {
			struct mem_cgroup *memcg;
			struct list_head list;
			struct kmem_cache *root_cache;
			bool dead;
			atomic_t nr_pages;
			struct work_struct destroy;
		};
	};
};

int memcg_update_all_caches(int num_memcgs);

struct seq_file;
int cache_show(struct kmem_cache *s, struct seq_file *m);
void print_slabinfo_header(struct seq_file *m);

/* 
                                                
                          
                       
                                          
  
                                     
  
                                                             
  
                                                        
  
                                                                     
                                                     
  
                                                   
  
                                                                   
  
                                                                   
  
                                           
  
                                                   
  
                                          
                                                               
                                
  
                                                       
                                                     
  
                                                    
                                       
  
                                                                               
  
                                                                             
                                
  
                                                           
                          
  
                                                                 
  
                                                                               
  
                                                                      
                                                                      
                                                
 */
static inline void *kmalloc_array(size_t n, size_t size, gfp_t flags)
{
	if (size != 0 && n > SIZE_MAX / size)
		return NULL;
	return __kmalloc(n * size, flags);
}

/* 
                                                                     
                          
                       
                                                        
 */
static inline void *kcalloc(size_t n, size_t size, gfp_t flags)
{
	return kmalloc_array(n, size, flags | __GFP_ZERO);
}

#if !defined(CONFIG_NUMA) && !defined(CONFIG_SLOB)
/* 
                                                      
                                                
                                                        
                                
  
                                                                       
                                                                    
        
 */
static inline void *kmalloc_node(size_t size, gfp_t flags, int node)
{
	return kmalloc(size, flags);
}

static inline void *__kmalloc_node(size_t size, gfp_t flags, int node)
{
	return __kmalloc(size, flags);
}

void *kmem_cache_alloc(struct kmem_cache *, gfp_t);

static inline void *kmem_cache_alloc_node(struct kmem_cache *cachep,
					gfp_t flags, int node)
{
	return kmem_cache_alloc(cachep, flags);
}
#endif /*                              */

/*
                                                                        
                                                                            
                                                 
                                                                         
                                                                     
                      
 */
#if defined(CONFIG_DEBUG_SLAB) || defined(CONFIG_SLUB) || \
	(defined(CONFIG_SLAB) && defined(CONFIG_TRACING)) || \
	(defined(CONFIG_SLOB) && defined(CONFIG_TRACING))
extern void *__kmalloc_track_caller(size_t, gfp_t, unsigned long);
#define kmalloc_track_caller(size, flags) \
	__kmalloc_track_caller(size, flags, _RET_IP_)
#else
#define kmalloc_track_caller(size, flags) \
	__kmalloc(size, flags)
#endif /*            */

#ifdef CONFIG_NUMA
/*
                                                                      
                                                                       
                                                                  
                                                                     
                                                                   
                                 
 */
#if defined(CONFIG_DEBUG_SLAB) || defined(CONFIG_SLUB) || \
	(defined(CONFIG_SLAB) && defined(CONFIG_TRACING)) || \
	(defined(CONFIG_SLOB) && defined(CONFIG_TRACING))
extern void *__kmalloc_node_track_caller(size_t, gfp_t, int, unsigned long);
#define kmalloc_node_track_caller(size, flags, node) \
	__kmalloc_node_track_caller(size, flags, node, \
			_RET_IP_)
#else
#define kmalloc_node_track_caller(size, flags, node) \
	__kmalloc_node(size, flags, node)
#endif

#else /*             */

#define kmalloc_node_track_caller(size, flags, node) \
	kmalloc_track_caller(size, flags)

#endif /*             */

/*
            
 */
static inline void *kmem_cache_zalloc(struct kmem_cache *k, gfp_t flags)
{
	return kmem_cache_alloc(k, flags | __GFP_ZERO);
}

/* 
                                                        
                                                
                                                        
 */
static inline void *kzalloc(size_t size, gfp_t flags)
{
	return kmalloc(size, flags | __GFP_ZERO);
}

/* 
                                                                       
                                                
                                                        
                                            
 */
static inline void *kzalloc_node(size_t size, gfp_t flags, int node)
{
	return kmalloc_node(size, flags | __GFP_ZERO, node);
}

/*
                                      
 */
static inline unsigned int kmem_cache_size(struct kmem_cache *s)
{
	return s->object_size;
}

void __init kmem_cache_init_late(void);

#endif	/*               */
